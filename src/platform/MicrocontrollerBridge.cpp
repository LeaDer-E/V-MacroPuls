#include "MicrocontrollerBridge.h"
#include "../core/I18n.h"
#include "../core/Paths.h"
#include <QDateTime>
#include <QDir>
#include <QFile>
#include <QThread>

#ifdef _WIN32
#include <windows.h>
#include <winreg.h>
#else
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#endif

namespace vmacro {

static bool isPicoVid(const QString& vid) { return vid == "2E8A" || vid == "239A"; }
static bool isArduinoVid(const QString& vid) { return vid == "2341" || vid == "2A03" || vid == "1B4F"; }

MicrocontrollerBridge& MicrocontrollerBridge::instance() {
    static MicrocontrollerBridge inst;
    return inst;
}

void MicrocontrollerBridge::setPreferredPort(const QString& port) {
    QString clean = port.trimmed().isEmpty() ? QStringLiteral("auto") : port.trimmed();
    if (clean != m_preferredPort) {
        m_preferredPort = clean;
        m_lastFailedProbeMs.clear();
        disconnect();
    }
}

void MicrocontrollerBridge::disconnect() {
    QMutexLocker lock(&m_mutex);
#ifdef _WIN32
    if (m_handle && m_handle != INVALID_HANDLE_VALUE) {
        CloseHandle(static_cast<HANDLE>(m_handle));
    }
    m_handle = nullptr;
#else
    if (m_fd >= 0) {
        ::close(m_fd);
    }
    m_fd = -1;
#endif
    m_activePort.clear();
    m_activeBoard.clear();
    m_activeBanner.clear();
}

bool MicrocontrollerBridge::openSerial(const QString& port, int timeoutMs) {
#ifdef _WIN32
    QString devPath = "\\\\.\\" + port;
    HANDLE h = CreateFileW(
        reinterpret_cast<const wchar_t*>(devPath.utf16()),
        GENERIC_READ | GENERIC_WRITE,
        0, nullptr, OPEN_EXISTING, 0, nullptr
    );
    if (h == INVALID_HANDLE_VALUE || !h) return false;

    DCB dcb{};
    dcb.DCBlength = sizeof(DCB);
    if (GetCommState(h, &dcb)) {
        dcb.BaudRate = CBR_115200;
        dcb.ByteSize = 8;
        dcb.Parity = NOPARITY;
        dcb.StopBits = ONESTOPBIT;
        dcb.fBinary = TRUE;
        dcb.fDtrControl = DTR_CONTROL_ENABLE;
        dcb.fRtsControl = RTS_CONTROL_ENABLE;
        SetCommState(h, &dcb);
    }
    COMMTIMEOUTS timeouts{};
    timeouts.ReadIntervalTimeout = 15;
    timeouts.ReadTotalTimeoutMultiplier = 1;
    timeouts.ReadTotalTimeoutConstant = static_cast<DWORD>(timeoutMs);
    timeouts.WriteTotalTimeoutMultiplier = 1;
    timeouts.WriteTotalTimeoutConstant = static_cast<DWORD>(timeoutMs);
    SetCommTimeouts(h, &timeouts);
    PurgeComm(h, PURGE_RXCLEAR | PURGE_TXCLEAR);
    m_handle = h;
    return true;
#else
    Q_UNUSED(timeoutMs);
    int fd = ::open(port.toUtf8().constData(), O_RDWR | O_NOCTTY | O_NONBLOCK);
    if (fd < 0) return false;
    struct termios tty{};
    if (tcgetattr(fd, &tty) == 0) {
        cfsetospeed(&tty, B115200);
        cfsetispeed(&tty, B115200);
        tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS8;
        tty.c_cflag |= (CLOCAL | CREAD);
        tty.c_cflag &= ~(PARENB | CSTOPB | CRTSCTS);
        tty.c_lflag = 0;
        tty.c_iflag = 0;
        tty.c_oflag = 0;
        tcsetattr(fd, TCSANOW, &tty);
    }
    m_fd = fd;
    return true;
#endif
}

bool MicrocontrollerBridge::writeSerialLine(const QString& cmd, bool expectReply, QString& outReply) {
    QByteArray payload = (cmd.trimmed() + "\n").toUtf8();
#ifdef _WIN32
    if (!m_handle || m_handle == INVALID_HANDLE_VALUE) return false;
    DWORD written = 0;
    if (!WriteFile(static_cast<HANDLE>(m_handle), payload.constData(), static_cast<DWORD>(payload.size()), &written, nullptr) || written == 0) {
        CloseHandle(static_cast<HANDLE>(m_handle));
        m_handle = nullptr;
        return false;
    }
    if (!expectReply) return true;
    char buf[128] = {0};
    DWORD readBytes = 0;
    ReadFile(static_cast<HANDLE>(m_handle), buf, 127, &readBytes, nullptr);
    outReply = QString::fromUtf8(buf, static_cast<int>(readBytes)).trimmed();
    return true;
#else
    if (m_fd < 0) return false;
    ssize_t w = ::write(m_fd, payload.constData(), static_cast<size_t>(payload.size()));
    if (w <= 0) {
        ::close(m_fd);
        m_fd = -1;
        return false;
    }
    if (!expectReply) return true;
    QThread::msleep(20);
    char buf[128] = {0};
    ssize_t r = ::read(m_fd, buf, 127);
    if (r > 0) outReply = QString::fromUtf8(buf, static_cast<int>(r)).trimmed();
    return true;
#endif
}

QVector<SerialPortInfo> MicrocontrollerBridge::listSerialPorts() const {
    QVector<SerialPortInfo> out;
#ifdef _WIN32
    // Map USB VID/PID -> COMx from HKLM\SYSTEM\CurrentControlSet\Enum\USB (non-blocking)
    QMap<QString, QStringList> usbVidMap;
    HKEY hUsb = nullptr;
    if (RegOpenKeyExW(HKEY_LOCAL_MACHINE, L"SYSTEM\\CurrentControlSet\\Enum\\USB", 0, KEY_READ, &hUsb) == ERROR_SUCCESS) {
        wchar_t devBuf[256];
        for (DWORD i = 0;; ++i) {
            DWORD sz = 255;
            if (RegEnumKeyExW(hUsb, i, devBuf, &sz, nullptr, nullptr, nullptr, nullptr) != ERROR_SUCCESS) break;
            QString devId = QString::fromWCharArray(devBuf).toUpper();
            int vp = devId.indexOf("VID_");
            if (vp < 0) continue;
            QString vid = devId.mid(vp + 4, 4);
            int pp = devId.indexOf("PID_");
            QString pid = (pp >= 0) ? devId.mid(pp + 4, 4) : QString();
            QString bType;
            if (isPicoVid(vid)) bType = "pico";
            else if (isArduinoVid(vid)) bType = "arduino";
            else if (vid == "1A86" || vid == "10C4" || vid == "0403") bType = "usb_uart";
            else continue;

            HKEY hDev = nullptr;
            if (RegOpenKeyExW(hUsb, devBuf, 0, KEY_READ, &hDev) == ERROR_SUCCESS) {
                wchar_t instBuf[256];
                for (DWORD j = 0;; ++j) {
                    DWORD isz = 255;
                    if (RegEnumKeyExW(hDev, j, instBuf, &isz, nullptr, nullptr, nullptr, nullptr) != ERROR_SUCCESS) break;
                    QString dpSub = QString::fromWCharArray(instBuf) + "\\Device Parameters";
                    HKEY hDp = nullptr;
                    if (RegOpenKeyExW(hDev, reinterpret_cast<const wchar_t*>(dpSub.utf16()), 0, KEY_READ, &hDp) == ERROR_SUCCESS) {
                        wchar_t portBuf[64];
                        DWORD pSz = sizeof(portBuf);
                        if (RegQueryValueExW(hDp, L"PortName", nullptr, nullptr, reinterpret_cast<LPBYTE>(portBuf), &pSz) == ERROR_SUCCESS) {
                            QString pStr = QString::fromWCharArray(portBuf).trimmed().toUpper();
                            if (pStr.startsWith("COM")) usbVidMap[pStr] = {vid, pid, bType};
                        }
                        RegCloseKey(hDp);
                    }
                }
                RegCloseKey(hDev);
            }
        }
        RegCloseKey(hUsb);
    }

    HKEY hSer = nullptr;
    if (RegOpenKeyExW(HKEY_LOCAL_MACHINE, L"HARDWARE\\DEVICEMAP\\SERIALCOMM", 0, KEY_READ, &hSer) == ERROR_SUCCESS) {
        wchar_t valName[256], portName[64];
        for (DWORD idx = 0;; ++idx) {
            DWORD vnSz = 255, pnSz = sizeof(portName), type = 0;
            if (RegEnumValueW(hSer, idx, valName, &vnSz, nullptr, &type, reinterpret_cast<LPBYTE>(portName), &pnSz) != ERROR_SUCCESS) break;
            QString vn = QString::fromWCharArray(valName);
            QString pn = QString::fromWCharArray(portName).trimmed();
            if (vn.contains("bth", Qt::CaseInsensitive) || vn.contains("bluetooth", Qt::CaseInsensitive)) continue;
            QString pUp = pn.toUpper();
            SerialPortInfo info;
            info.portName = info.port = pn;
            if (usbVidMap.contains(pUp)) {
                auto lst = usbVidMap[pUp];
                info.vid = lst[0];
                info.pid = lst[1];
                info.boardType = lst[2];
                if (info.boardType == "pico") info.description = pn + " - Raspberry Pi Pico USB (VID:" + info.vid + ")";
                else if (info.boardType == "arduino") info.description = pn + " - Arduino USB HID (VID:" + info.vid + ")";
                else info.description = pn + " - USB Serial (VID:" + info.vid + ")";
            } else {
                info.description = pn + " (" + vn.split('\\').last() + ")";
            }
            out.append(info);
        }
        RegCloseKey(hSer);
    }
#else
    QDir devDir("/dev");
    for (const QString& name : devDir.entryList({"ttyACM*", "ttyUSB*"}, QDir::System)) {
        SerialPortInfo info;
        info.portName = info.port = "/dev/" + name;
        info.description = info.port;
        info.boardType = name.startsWith("ttyACM") ? "usb_uart" : "";
        out.append(info);
    }
#endif
    return out;
}

bool MicrocontrollerBridge::probeAndConnect(const QString& targetBoard, const QString& preferredPort, bool forceScan) {
    QMutexLocker lock(&m_mutex);
    QString pref = preferredPort.trimmed().isEmpty() ? m_preferredPort : preferredPort.trimmed();
    if (pref.isEmpty()) pref = "auto";

#ifdef _WIN32
    bool isOpen = (m_handle && m_handle != INVALID_HANDLE_VALUE);
#else
    bool isOpen = (m_fd >= 0);
#endif
    if (isOpen && (pref == "auto" || pref.compare(m_activePort, Qt::CaseInsensitive) == 0)) {
        if (targetBoard == "any" || targetBoard.isEmpty() || targetBoard == m_activeBoard) return true;
    }

    QString cacheKey = targetBoard + ":" + pref;
    qint64 now = QDateTime::currentMSecsSinceEpoch();
    if (!forceScan && (now - m_lastFailedProbeMs.value(cacheKey, -999999)) < 4000) {
        return false;
    }

    auto ports = listSerialPorts();
    QVector<SerialPortInfo> candidates;
    if (pref.compare("auto", Qt::CaseInsensitive) != 0) {
        for (const auto& p : ports) {
            if (p.port.compare(pref, Qt::CaseInsensitive) == 0) candidates.append(p);
        }
        if (candidates.isEmpty()) candidates.append(SerialPortInfo{pref, pref, pref, "", "", "manual"});
    } else {
        for (const auto& p : ports) {
            if (p.boardType == "pico" || p.boardType == "arduino" || p.boardType == "usb_uart") {
                if (targetBoard == "any" || targetBoard.isEmpty() || p.boardType == targetBoard || p.boardType == "usb_uart") {
                    candidates.append(p);
                }
            }
        }
    }

    if (candidates.isEmpty()) {
        m_lastFailedProbeMs[cacheKey] = now;
        return false;
    }

    for (const auto& cand : candidates) {
        if ((targetBoard == "pico" || targetBoard == "arduino") &&
            (cand.boardType == "pico" || cand.boardType == "arduino") &&
            cand.boardType != targetBoard) {
            continue;
        }
        if (!openSerial(cand.port, 25)) continue;
        QString reply;
        bool ok = writeSerialLine("PING", true, reply);
        if (ok && reply.startsWith("PONG:")) {
            QString bType = reply.toUpper().contains("PICO") ? "pico" : (reply.toUpper().contains("ARDUINO") ? "arduino" : cand.boardType);
            if (targetBoard != "any" && !targetBoard.isEmpty() && bType != targetBoard) {
                lock.unlock();
                disconnect();
                lock.relock();
                continue;
            }
            m_activePort = cand.port;
            m_activeBoard = bType;
            m_activeBanner = reply;
            m_lastFailedProbeMs.clear();
            return true;
        }
        if ((cand.boardType == "pico" || cand.boardType == "arduino") &&
            (targetBoard == "any" || targetBoard.isEmpty() || cand.boardType == targetBoard)) {
            m_activePort = cand.port;
            m_activeBoard = cand.boardType;
            m_activeBanner = "USB-VID:" + cand.vid;
            m_lastFailedProbeMs.clear();
            return true;
        }
        lock.unlock();
        disconnect();
        lock.relock();
    }

    m_lastFailedProbeMs[cacheKey] = QDateTime::currentMSecsSinceEpoch();
    return false;
}

bool MicrocontrollerBridge::isBoardReady(const QString& boardType, bool allowProbe) {
#ifdef _WIN32
    bool isOpen = (m_handle && m_handle != INVALID_HANDLE_VALUE);
#else
    bool isOpen = (m_fd >= 0);
#endif
    if (isOpen && (boardType == "any" || boardType.isEmpty() || boardType == m_activeBoard)) return true;
    return probeAndConnect(boardType, QString(), allowProbe);
}

bool MicrocontrollerBridge::sendKeyEvent(int usbHidUsage, bool isDown, int modBit, const QString& boardType) {
    if (!isBoardReady(boardType)) return false;
    QMutexLocker lock(&m_mutex);
    QString dummy;
    return writeSerialLine(QString("%1:%2,%3").arg(isDown ? "KD" : "KU").arg(usbHidUsage).arg(modBit), false, dummy);
}

bool MicrocontrollerBridge::sendKeyTap(int usbHidUsage, int holdMs, int modBit, const QString& boardType) {
    if (!isBoardReady(boardType)) return false;
    {
        QMutexLocker lock(&m_mutex);
        QString dummy;
        if (!writeSerialLine(QString("KT:%1,%2,%3").arg(usbHidUsage).arg(qMax(5, holdMs)).arg(modBit), false, dummy)) return false;
    }
    if (holdMs > 0) QThread::msleep(static_cast<unsigned long>(qBound(5, holdMs, 2000)));
    return true;
}

bool MicrocontrollerBridge::sendMouseAction(const QString& button, const QString& action, int holdMs, const QString& boardType) {
    if (!isBoardReady(boardType)) return false;
    QString b = button.trimmed().toLower();
    if (b.isEmpty()) b = "left";
    QString dummy;
    if (action == "down") {
        QMutexLocker lock(&m_mutex);
        return writeSerialLine("MD:" + b, false, dummy);
    }
    if (action == "up") {
        QMutexLocker lock(&m_mutex);
        return writeSerialLine("MU:" + b, false, dummy);
    }
    {
        QMutexLocker lock(&m_mutex);
        if (!writeSerialLine(QString("MC:%1,%2").arg(b).arg(qMax(5, holdMs)), false, dummy)) return false;
    }
    if (holdMs > 0) QThread::msleep(static_cast<unsigned long>(qBound(5, holdMs, 2000)));
    return true;
}

bool MicrocontrollerBridge::sendMouseMoveRel(int dx, int dy, const QString& boardType) {
    if (!isBoardReady(boardType)) return false;
    QMutexLocker lock(&m_mutex);
    QString dummy;
    return writeSerialLine(QString("MM:%1,%2").arg(dx).arg(dy), false, dummy);
}

bool MicrocontrollerBridge::sendMouseWheel(int wheelDelta, const QString& boardType) {
    if (!isBoardReady(boardType)) return false;
    QMutexLocker lock(&m_mutex);
    QString dummy;
    return writeSerialLine(QString("MS:%1").arg(wheelDelta), false, dummy);
}

QString MicrocontrollerBridge::findCircuitPyDrive() const {
#ifdef _WIN32
    for (char ch = 'A'; ch <= 'Z'; ++ch) {
        QString root = QString("%1:\\").arg(ch);
        wchar_t vol[256] = {0};
        if (GetVolumeInformationW(reinterpret_cast<const wchar_t*>(root.utf16()), vol, 255, nullptr, nullptr, nullptr, nullptr, 0)) {
            if (QString::fromWCharArray(vol).trimmed().compare("CIRCUITPY", Qt::CaseInsensitive) == 0) {
                return root;
            }
        }
    }
#else
    for (const QString& base : {"/media", "/run/media", "/mnt"}) {
        QDir bd(base);
        if (!bd.exists()) continue;
        if (QDir(base + "/CIRCUITPY").exists()) return base + "/CIRCUITPY";
        for (const QString& sub : bd.entryList(QDir::Dirs | QDir::NoDotAndDotDot)) {
            QString cand = base + "/" + sub + "/CIRCUITPY";
            if (QDir(cand).exists()) return cand;
        }
    }
#endif
    return {};
}

static void copyDirRecursive(const QString& src, const QString& dst) {
    QDir sDir(src);
    QDir dDir(dst);
    if (!dDir.exists()) dDir.mkpath(".");
    for (const QString& f : sDir.entryList(QDir::Files)) {
        QFile::remove(dst + "/" + f);
        QFile::copy(src + "/" + f, dst + "/" + f);
    }
    for (const QString& sub : sDir.entryList(QDir::Dirs | QDir::NoDotAndDotDot)) {
        copyDirRecursive(src + "/" + sub, dst + "/" + sub);
    }
}

bool MicrocontrollerBridge::autoInstallCircuitPythonToPico(QString& outMsg) const {
    QString drv = findCircuitPyDrive();
    if (drv.isEmpty()) {
        outMsg = trDual(
            "لم يتم العثور على فلاشة CIRCUITPY متصلة — تأكد من تثبيت CircuitPython على الـ Pico وتوصيلها بالـ USB أولاً.",
            "No CIRCUITPY USB drive detected — Make sure CircuitPython is flashed on your Pico and plugged in."
        );
        return false;
    }
    QString fwCp = Paths::firmwareDir() + "/raspberry_pi_pico_circuitpython";
    if (!QDir(fwCp).exists()) {
        outMsg = trDual("مجلد firmware غير موجود!", "Firmware folder missing!");
        return false;
    }
    copyDirRecursive(fwCp, drv);
    outMsg = trDual(
        QString("✓ تم نسخ boot.py و code.py تلقائياً إلى فلاشة الـ Pico (%1)! افصل الـ Pico وأعد توصيلها لتفعيل الـ USB HID.").arg(drv),
        QString("✓ Automatically installed boot.py & code.py onto Pico drive (%1)! Unplug and replug your Pico once to activate USB HID.").arg(drv)
    );
    return true;
}

bool MicrocontrollerBridge::extractAllFirmwareFiles(const QString& targetDir, QString& outMsg) const {
    QString srcFw = Paths::firmwareDir();
    QString dest = targetDir + "/V-MacroPlus_MCU_Firmwares";
    copyDirRecursive(srcFw, dest);
    outMsg = trDual(
        QString("✓ تم استخراج جميع ملفات السوفتوير الجاهزة (Pico CircuitPython + Pico C++ + Arduino) في: %1").arg(dest),
        QString("✓ Extracted all ready-to-flash firmware files (Pico CircuitPython + Pico C++ + Arduino) to: %1").arg(dest)
    );
    return true;
}

McuDiagnostics MicrocontrollerBridge::diagnose(const QString& preferredPort, bool runLivePing) {
    setPreferredPort(preferredPort);
    McuDiagnostics d;
    d.preferredPort = preferredPort;
    d.ports = listSerialPorts();
    d.circuitpyDrive = findCircuitPyDrive();
    d.firmwareDir = Paths::firmwareDir();
    if (runLivePing) {
        d.connected = probeAndConnect("any", preferredPort, true);
    } else {
#ifdef _WIN32
        d.connected = (m_handle && m_handle != INVALID_HANDLE_VALUE);
#else
        d.connected = (m_fd >= 0);
#endif
    }
    d.activePort = m_activePort;
    d.boardType = m_activeBoard;
    d.firmwareBanner = m_activeBanner;
    return d;
}

void MicrocontrollerBridge::formatDiagnosticReport(const McuDiagnostics& diag, bool isArabic, QString& outHeadline, QString& outDetails, QString& outKind) const {
    QStringList pLabels;
    for (const auto& p : diag.ports) {
        pLabels.append(p.boardType.isEmpty() ? p.port : (p.port + " [" + p.boardType.toUpper() + "]"));
    }
    QString pSummary = pLabels.isEmpty()
        ? (isArabic ? "لا توجد منافذ COM متصلة حالياً" : "No active COM ports detected")
        : pLabels.join(", ");

    if (diag.connected) {
        QString bName = (diag.boardType == "pico") ? "Raspberry Pi Pico" : ((diag.boardType == "arduino") ? "Arduino Leonardo / Pro Micro" : "MCU USB HID");
        outHeadline = isArabic
            ? QString("🟢 بوردة %1 متصلة ونشطة فعلياً على المنفذ [%2]! (%3)").arg(bName, diag.activePort, diag.firmwareBanner)
            : QString("🟢 %1 Hardware USB HID Board ACTIVE & Verified on [%2]! (%3)").arg(bName, diag.activePort, diag.firmwareBanner);
        outKind = "ok";
    } else if (!diag.circuitpyDrive.isEmpty()) {
        outHeadline = isArabic
            ? QString("🟡 تم اكتشاف فلاشة Raspberry Pi Pico (%1) — اضغط زر «تثبيت تلقائي على فلاشة الـ Pico» بالأسفل!").arg(diag.circuitpyDrive)
            : QString("🟡 Detected Raspberry Pi Pico drive (%1) — Click '1-Click Install to Pico Drive' below!").arg(diag.circuitpyDrive);
        outKind = "warn";
    } else {
        outHeadline = isArabic
            ? "🔵 جاهز للكشف التلقائي عن Raspberry Pi Pico (الطريقة 21) و Arduino Leonardo/Pro Micro (الطريقة 22) بمجرد توصيل القطعة بالـ USB"
            : "🔵 Ready to Auto-Detect Raspberry Pi Pico (Method 21) & Arduino Leonardo/Pro Micro (Method 22) as soon as plugged into USB";
        outKind = "info";
    }

    if (isArabic) {
        outDetails = QString("• المنافذ المتاحة (COM Ports): %1\n• حالة الاتصال المباشر (PING/PONG @ 115200): %2\n• السوفتوير المدمج (3 نسخ جاهزة): Pico CircuitPython | Pico C++ (.ino) | Arduino Leonardo/Pro Micro (.ino)")
            .arg(pSummary, diag.connected ? ("✓ متصل على " + diag.activePort) : "في انتظار توصيل القطعة (الكشف التلقائي يعمل)");
    } else {
        outDetails = QString("• Available Serial Ports (COM): %1\n• Live Hardware Bridge Status (115200 baud): %2\n• Bundled Ready Firmwares (3 Editions): Pico CircuitPython | Pico C++ (.ino) | Arduino Leonardo/Pro Micro (.ino)")
            .arg(pSummary, diag.connected ? ("✓ Connected on " + diag.activePort) : "Waiting for board connection (Auto-Detect active)");
    }
}

} // namespace vmacro
