#pragma once
#include <QMutex>
#include <QString>
#include <QStringList>
#include <QMap>
#include <QVector>

namespace vmacro {

struct SerialPortInfo {
    QString port;
    QString portName;
    QString description;
    QString vid;
    QString pid;
    QString boardType; // "pico", "arduino", "usb_uart", ""
};

struct McuDiagnostics {
    bool connected = false;
    QString activePort;
    QString boardType;
    QString firmwareBanner;
    QString preferredPort = "auto";
    QVector<SerialPortInfo> ports;
    QString circuitpyDrive;
    QString firmwareDir;
};

struct McuConnectionTestResult {
    bool ok = false;
    QString message;
};

class MicrocontrollerBridge {
public:
    static MicrocontrollerBridge& instance();

    void setPreferredPort(const QString& port);
    QString preferredPort() const { return m_preferredPort; }
    QString activePort() const { return m_activePort; }
    QString activeBoard() const { return m_activeBoard; }

    void disconnect();
    QVector<SerialPortInfo> listSerialPorts() const;
    static QVector<SerialPortInfo> listPorts() { return instance().listSerialPorts(); }
    bool probeAndConnect(const QString& targetBoard = "any", const QString& preferredPort = QString(), bool forceScan = false);
    bool isBoardReady(const QString& boardType = "any", bool allowProbe = false);

    bool sendKeyEvent(int usbHidUsage, bool isDown, int modBit = 0, const QString& boardType = "any");
    bool sendKeyTap(int usbHidUsage, int holdMs = 25, int modBit = 0, const QString& boardType = "any");
    bool sendMouseAction(const QString& button = "left", const QString& action = "click", int holdMs = 25, const QString& boardType = "any");
    bool sendMouseMoveRel(int dx, int dy, const QString& boardType = "any");
    bool sendMouseWheel(int wheelDelta, const QString& boardType = "any");

    QString findCircuitPyDrive() const;
    bool autoInstallCircuitPythonToPico(QString& outMsg) const;
    bool installToCircuitpyDrive(QString& outMsg) const { return autoInstallCircuitPythonToPico(outMsg); }
    bool extractAllFirmwareFiles(const QString& targetDir, QString& outMsg) const;
    bool extractAllFirmware(const QString& targetDir, QString& outMsg) const { return extractAllFirmwareFiles(targetDir, outMsg); }
    McuDiagnostics diagnose(const QString& preferredPort = "auto", bool runLivePing = true);
    void formatDiagnosticReport(const McuDiagnostics& diag, bool isArabic, QString& outHeadline, QString& outDetails, QString& outKind) const;

    McuConnectionTestResult testConnection(const QString& preferredPort = "auto", const QString& board = "any") {
        Q_UNUSED(board);
        auto d = diagnose(preferredPort, true);
        QString head, det, kind;
        formatDiagnosticReport(d, true, head, det, kind);
        return {d.connected, head + QStringLiteral(" — ") + det};
    }

private:
    MicrocontrollerBridge() = default;
    ~MicrocontrollerBridge() { disconnect(); }
    bool openSerial(const QString& port, int timeoutMs = 25);
    bool writeSerialLine(const QString& cmd, bool expectReply, QString& outReply);

#ifdef _WIN32
    void* m_handle = nullptr;
#else
    int m_fd = -1;
#endif
    QString m_activePort;
    QString m_activeBoard;
    QString m_activeBanner;
    QString m_preferredPort = "auto";
    QMap<QString, qint64> m_lastFailedProbeMs;
    mutable QMutex m_mutex;
};

} // namespace vmacro
