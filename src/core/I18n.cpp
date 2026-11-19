#include "I18n.h"

namespace vmacro {

I18n& I18n::instance() {
    static I18n inst;
    return inst;
}

I18n::I18n() {
    m_langs.append(LangInfo{QStringLiteral("ar"), QStringLiteral("العربية"), true});
    m_langs.append(LangInfo{QStringLiteral("en"), QStringLiteral("English"), false});
    m_langs.append(LangInfo{QStringLiteral("es"), QStringLiteral("Español"), false});
    m_langs.append(LangInfo{QStringLiteral("fr"), QStringLiteral("Français"), false});
    m_langs.append(LangInfo{QStringLiteral("de"), QStringLiteral("Deutsch"), false});
    m_langs.append(LangInfo{QStringLiteral("tr"), QStringLiteral("Türkçe"), false});
    m_langs.append(LangInfo{QStringLiteral("ru"), QStringLiteral("Русский"), false});
    m_langs.append(LangInfo{QStringLiteral("id"), QStringLiteral("Bahasa Indonesia"), false});
    m_langs.append(LangInfo{QStringLiteral("fa"), QStringLiteral("فارسی"), true});
    m_langs.append(LangInfo{QStringLiteral("ur"), QStringLiteral("اردو"), true});

    { auto& m = m_translations[QStringLiteral("app_name")];
      m.insert(QStringLiteral("ar"), QStringLiteral("V-MacroPlus"));
      m.insert(QStringLiteral("en"), QStringLiteral("V-MacroPlus"));
      m.insert(QStringLiteral("es"), QStringLiteral("V-MacroPlus"));
      m.insert(QStringLiteral("fr"), QStringLiteral("V-MacroPlus"));
      m.insert(QStringLiteral("de"), QStringLiteral("V-MacroPlus"));
      m.insert(QStringLiteral("tr"), QStringLiteral("V-MacroPlus"));
      m.insert(QStringLiteral("ru"), QStringLiteral("V-MacroPlus"));
      m.insert(QStringLiteral("id"), QStringLiteral("V-MacroPlus"));
      m.insert(QStringLiteral("fa"), QStringLiteral("V-MacroPlus"));
      m.insert(QStringLiteral("ur"), QStringLiteral("V-MacroPlus"));
    }
    { auto& m = m_translations[QStringLiteral("search_placeholder")];
      m.insert(QStringLiteral("ar"), QStringLiteral("بحث..."));
      m.insert(QStringLiteral("en"), QStringLiteral("Search..."));
      m.insert(QStringLiteral("es"), QStringLiteral("Buscar..."));
      m.insert(QStringLiteral("fr"), QStringLiteral("Rechercher..."));
      m.insert(QStringLiteral("de"), QStringLiteral("Suchen..."));
      m.insert(QStringLiteral("tr"), QStringLiteral("Ara..."));
      m.insert(QStringLiteral("ru"), QStringLiteral("Поиск..."));
      m.insert(QStringLiteral("id"), QStringLiteral("Cari..."));
      m.insert(QStringLiteral("fa"), QStringLiteral("جستجو..."));
      m.insert(QStringLiteral("ur"), QStringLiteral("تلاش..."));
    }
    { auto& m = m_translations[QStringLiteral("app_Tagline")];
      m.insert(QStringLiteral("ar"), QStringLiteral("استوديو الأتمتة الفائقة، الاختصارات الذكية، وتسجيل الماوس والكيبورد"));
      m.insert(QStringLiteral("en"), QStringLiteral("Precision Smart Hotkeys, Multi-Monitor Mouse & Keyboard Automation Studio"));
      m.insert(QStringLiteral("es"), QStringLiteral("Estudio de automatización de teclado, ratón y atajos inteligentes"));
      m.insert(QStringLiteral("fr"), QStringLiteral("Studio d'automatisation clavier, souris et raccourcis intelligents"));
      m.insert(QStringLiteral("de"), QStringLiteral("Präzisions-Studio für intelligente Hotkeys, Maus- und Tastatur-Automatisierung"));
      m.insert(QStringLiteral("tr"), QStringLiteral("Akıllı Kısayollar, Fare ve Klavye Otomasyon Stüdyosu"));
      m.insert(QStringLiteral("ru"), QStringLiteral("Студия умных клавиш и автоматизации мыши и клавиатуры"));
      m.insert(QStringLiteral("id"), QStringLiteral("Studio Otomatisasi Mouse, Keyboard & Pintasan Cerdas"));
      m.insert(QStringLiteral("fa"), QStringLiteral("استودیوی هوشمند خودکارسازی ماوس، کیبورد و میانبرهای پیشرفته"));
      m.insert(QStringLiteral("ur"), QStringLiteral("سمارٹ شارٹ کٹس، ماؤس اور کی بورڈ آٹومیشن اسٹوڈیو"));
    }
    { auto& m = m_translations[QStringLiteral("nav_combos")];
      m.insert(QStringLiteral("ar"), QStringLiteral("الأزرار الذكية"));
      m.insert(QStringLiteral("en"), QStringLiteral("Smart Keys"));
      m.insert(QStringLiteral("es"), QStringLiteral("Teclas"));
      m.insert(QStringLiteral("fr"), QStringLiteral("Touches"));
      m.insert(QStringLiteral("de"), QStringLiteral("Smart-Tasten"));
      m.insert(QStringLiteral("tr"), QStringLiteral("Akıllı Tuşlar"));
      m.insert(QStringLiteral("ru"), QStringLiteral("Клавиши"));
      m.insert(QStringLiteral("id"), QStringLiteral("Tombol Cerdas"));
      m.insert(QStringLiteral("fa"), QStringLiteral("کلیدهای هوشمند"));
      m.insert(QStringLiteral("ur"), QStringLiteral("سمارٹ بٹن"));
    }
    { auto& m = m_translations[QStringLiteral("nav_recorder")];
      m.insert(QStringLiteral("ar"), QStringLiteral("التسجيلات"));
      m.insert(QStringLiteral("en"), QStringLiteral("Recordings"));
      m.insert(QStringLiteral("es"), QStringLiteral("Grabaciones"));
      m.insert(QStringLiteral("fr"), QStringLiteral("Enregistrements"));
      m.insert(QStringLiteral("de"), QStringLiteral("Aufnahmen"));
      m.insert(QStringLiteral("tr"), QStringLiteral("Kayıtlar"));
      m.insert(QStringLiteral("ru"), QStringLiteral("Записи"));
      m.insert(QStringLiteral("id"), QStringLiteral("Rekaman"));
      m.insert(QStringLiteral("fa"), QStringLiteral("ضبط‌ها"));
      m.insert(QStringLiteral("ur"), QStringLiteral("ریکارڈنگز"));
    }
    { auto& m = m_translations[QStringLiteral("nav_test")];
      m.insert(QStringLiteral("ar"), QStringLiteral("معمل التجربة"));
      m.insert(QStringLiteral("en"), QStringLiteral("Test Lab"));
      m.insert(QStringLiteral("es"), QStringLiteral("Laboratorio"));
      m.insert(QStringLiteral("fr"), QStringLiteral("Test Lab"));
      m.insert(QStringLiteral("de"), QStringLiteral("Testlabor"));
      m.insert(QStringLiteral("tr"), QStringLiteral("Test Merkezi"));
      m.insert(QStringLiteral("ru"), QStringLiteral("Тест-лаб"));
      m.insert(QStringLiteral("id"), QStringLiteral("Lab Uji"));
      m.insert(QStringLiteral("fa"), QStringLiteral("آزمایشگاه تست"));
      m.insert(QStringLiteral("ur"), QStringLiteral("ٹیسٹ لیب"));
    }
    { auto& m = m_translations[QStringLiteral("nav_settings")];
      m.insert(QStringLiteral("ar"), QStringLiteral("الإعدادات"));
      m.insert(QStringLiteral("en"), QStringLiteral("Settings"));
      m.insert(QStringLiteral("es"), QStringLiteral("Ajustes"));
      m.insert(QStringLiteral("fr"), QStringLiteral("Paramètres"));
      m.insert(QStringLiteral("de"), QStringLiteral("Einstellungen"));
      m.insert(QStringLiteral("tr"), QStringLiteral("Ayarlar"));
      m.insert(QStringLiteral("ru"), QStringLiteral("Настройки"));
      m.insert(QStringLiteral("id"), QStringLiteral("Pengaturan"));
      m.insert(QStringLiteral("fa"), QStringLiteral("تنظیمات"));
      m.insert(QStringLiteral("ur"), QStringLiteral("ترتیبات"));
    }
    { auto& m = m_translations[QStringLiteral("nav_about")];
      m.insert(QStringLiteral("ar"), QStringLiteral("عن البرنامج"));
      m.insert(QStringLiteral("en"), QStringLiteral("About"));
      m.insert(QStringLiteral("es"), QStringLiteral("Acerca de"));
      m.insert(QStringLiteral("fr"), QStringLiteral("À propos"));
      m.insert(QStringLiteral("de"), QStringLiteral("Über"));
      m.insert(QStringLiteral("tr"), QStringLiteral("Hakkında"));
      m.insert(QStringLiteral("ru"), QStringLiteral("О программе"));
      m.insert(QStringLiteral("id"), QStringLiteral("Tentang"));
      m.insert(QStringLiteral("fa"), QStringLiteral("درباره برنامه"));
      m.insert(QStringLiteral("ur"), QStringLiteral("پروگرام کے بارے میں"));
    }
    { auto& m = m_translations[QStringLiteral("status_idle")];
      m.insert(QStringLiteral("ar"), QStringLiteral("متوقف مؤقتاً"));
      m.insert(QStringLiteral("en"), QStringLiteral("Paused"));
      m.insert(QStringLiteral("es"), QStringLiteral("Pausado"));
      m.insert(QStringLiteral("fr"), QStringLiteral("En pause"));
      m.insert(QStringLiteral("de"), QStringLiteral("Pausiert"));
      m.insert(QStringLiteral("tr"), QStringLiteral("Duraklatıldı"));
      m.insert(QStringLiteral("ru"), QStringLiteral("Пауза"));
      m.insert(QStringLiteral("id"), QStringLiteral("Dijeda"));
      m.insert(QStringLiteral("fa"), QStringLiteral("متوقف موقت"));
      m.insert(QStringLiteral("ur"), QStringLiteral("موقوف"));
    }
    { auto& m = m_translations[QStringLiteral("status_recording")];
      m.insert(QStringLiteral("ar"), QStringLiteral("جاري التسجيل..."));
      m.insert(QStringLiteral("en"), QStringLiteral("Recording..."));
      m.insert(QStringLiteral("es"), QStringLiteral("Grabando..."));
      m.insert(QStringLiteral("fr"), QStringLiteral("Enregistrement..."));
      m.insert(QStringLiteral("de"), QStringLiteral("Aufnahme..."));
      m.insert(QStringLiteral("tr"), QStringLiteral("Kaydediliyor..."));
      m.insert(QStringLiteral("ru"), QStringLiteral("Запись..."));
      m.insert(QStringLiteral("id"), QStringLiteral("Merekam..."));
      m.insert(QStringLiteral("fa"), QStringLiteral("در حال ضبط..."));
      m.insert(QStringLiteral("ur"), QStringLiteral("ریکارڈنگ جاری..."));
    }
    { auto& m = m_translations[QStringLiteral("status_playing")];
      m.insert(QStringLiteral("ar"), QStringLiteral("جاري التشغيل..."));
      m.insert(QStringLiteral("en"), QStringLiteral("Playing..."));
      m.insert(QStringLiteral("es"), QStringLiteral("Reproduciendo..."));
      m.insert(QStringLiteral("fr"), QStringLiteral("Lecture..."));
      m.insert(QStringLiteral("de"), QStringLiteral("Wiedergabe..."));
      m.insert(QStringLiteral("tr"), QStringLiteral("Oynatılıyor..."));
      m.insert(QStringLiteral("ru"), QStringLiteral("Воспроизведение..."));
      m.insert(QStringLiteral("id"), QStringLiteral("Memutar..."));
      m.insert(QStringLiteral("fa"), QStringLiteral("در حال اجرا..."));
      m.insert(QStringLiteral("ur"), QStringLiteral("چل رہا ہے..."));
    }
    { auto& m = m_translations[QStringLiteral("status_combos_active")];
      m.insert(QStringLiteral("ar"), QStringLiteral("الأزرار الذكية نشطة"));
      m.insert(QStringLiteral("en"), QStringLiteral("Smart Keys Ready"));
      m.insert(QStringLiteral("es"), QStringLiteral("Teclas Listas"));
      m.insert(QStringLiteral("fr"), QStringLiteral("Touches Prêtes"));
      m.insert(QStringLiteral("de"), QStringLiteral("Smart-Tasten Bereit"));
      m.insert(QStringLiteral("tr"), QStringLiteral("Akıllı Tuşlar Hazır"));
      m.insert(QStringLiteral("ru"), QStringLiteral("Клавиши активны"));
      m.insert(QStringLiteral("id"), QStringLiteral("Tombol Siap"));
      m.insert(QStringLiteral("fa"), QStringLiteral("کلیدهای هوشمند فعال"));
      m.insert(QStringLiteral("ur"), QStringLiteral("سمارٹ بٹن تیار"));
    }
    { auto& m = m_translations[QStringLiteral("badge_admin")];
      m.insert(QStringLiteral("ar"), QStringLiteral("مسؤول النظام"));
      m.insert(QStringLiteral("en"), QStringLiteral("ADMIN"));
      m.insert(QStringLiteral("es"), QStringLiteral("ADMIN"));
      m.insert(QStringLiteral("fr"), QStringLiteral("ADMIN"));
      m.insert(QStringLiteral("de"), QStringLiteral("ADMIN"));
      m.insert(QStringLiteral("tr"), QStringLiteral("YÖNETİCİ"));
      m.insert(QStringLiteral("ru"), QStringLiteral("АДМИН"));
      m.insert(QStringLiteral("id"), QStringLiteral("ADMIN"));
      m.insert(QStringLiteral("fa"), QStringLiteral("مدیر سیستم"));
      m.insert(QStringLiteral("ur"), QStringLiteral("ایڈمن"));
    }
    { auto& m = m_translations[QStringLiteral("badge_user")];
      m.insert(QStringLiteral("ar"), QStringLiteral("مستخدم عادي"));
      m.insert(QStringLiteral("en"), QStringLiteral("Standard"));
      m.insert(QStringLiteral("es"), QStringLiteral("Estándar"));
      m.insert(QStringLiteral("fr"), QStringLiteral("Standard"));
      m.insert(QStringLiteral("de"), QStringLiteral("Standard"));
      m.insert(QStringLiteral("tr"), QStringLiteral("Standart"));
      m.insert(QStringLiteral("ru"), QStringLiteral("Обычный"));
      m.insert(QStringLiteral("id"), QStringLiteral("Standar"));
      m.insert(QStringLiteral("fa"), QStringLiteral("کاربر عادی"));
      m.insert(QStringLiteral("ur"), QStringLiteral("عام صارف"));
    }
    { auto& m = m_translations[QStringLiteral("all_apps")];
      m.insert(QStringLiteral("ar"), QStringLiteral("كل البرامج"));
      m.insert(QStringLiteral("en"), QStringLiteral("All Applications"));
      m.insert(QStringLiteral("es"), QStringLiteral("Todas las aplicaciones"));
      m.insert(QStringLiteral("fr"), QStringLiteral("Toutes les applications"));
      m.insert(QStringLiteral("de"), QStringLiteral("Alle Anwendungen"));
      m.insert(QStringLiteral("tr"), QStringLiteral("Tüm Uygulamalar"));
      m.insert(QStringLiteral("ru"), QStringLiteral("Все программы"));
      m.insert(QStringLiteral("id"), QStringLiteral("Semua Aplikasi"));
      m.insert(QStringLiteral("fa"), QStringLiteral("همه برنامه‌ها"));
      m.insert(QStringLiteral("ur"), QStringLiteral("تمام پروگرامز"));
    }
    { auto& m = m_translations[QStringLiteral("combo_title")];
      m.insert(QStringLiteral("ar"), QStringLiteral("الأزرار الذكية والماكرو المخصص (زرار واحد أو عدة أزرار معاً أو تشغيل يدوي)"));
      m.insert(QStringLiteral("en"), QStringLiteral("Smart Keys & Custom Macros (Single Key, Multi-Key Chord, or Manual Run)"));
      m.insert(QStringLiteral("es"), QStringLiteral("Teclas Inteligentes y Macros Personalizadas"));
      m.insert(QStringLiteral("fr"), QStringLiteral("Touches Intelligentes et Macros Personnalisées"));
      m.insert(QStringLiteral("de"), QStringLiteral("Smart-Tasten & Benutzerdefinierte Makros"));
      m.insert(QStringLiteral("tr"), QStringLiteral("Akıllı Tuşlar ve Özel Makrolar"));
      m.insert(QStringLiteral("ru"), QStringLiteral("Умные клавиши и пользовательские макросы"));
      m.insert(QStringLiteral("id"), QStringLiteral("Tombol Cerdas & Makro Kustom"));
      m.insert(QStringLiteral("fa"), QStringLiteral("کلیدهای هوشمند و ماکروهای سفارشی (تک کلید، چند کلید همزمان یا اجرای دستی)"));
      m.insert(QStringLiteral("ur"), QStringLiteral("سمارٹ بٹن اور کسٹم میکروز (ایک بٹن، ملٹی بٹن یا دستی چلائیں)"));
    }
    { auto& m = m_translations[QStringLiteral("combo_subtitle")];
      m.insert(QStringLiteral("ar"), QStringLiteral("يدعم تحديد مدة ضغط الزرار، وجميع أزرار وبكرة الماوس (فوق/تحت/يمين/شمال/رجوع/تقدم)، والتحويل المتطابق ١٠٠٪ من التسجيلات."));
      m.insert(QStringLiteral("en"), QStringLiteral("Supports timed key/mouse holds, all mouse buttons & 4-way scroll wheel, and 100% 1-to-1 conversion from Recordings."));
      m.insert(QStringLiteral("es"), QStringLiteral("Admite pulsación con duración, todos los botones y rueda del ratón, y conversión 1:1 de grabaciones."));
      m.insert(QStringLiteral("fr"), QStringLiteral("Prend en charge la durée de maintien, tous les boutons/molette de souris et la conversion 1:1."));
      m.insert(QStringLiteral("de"), QStringLiteral("Unterstützt Haltedauer, alle Maustasten & 4-Wege-Mausrad sowie 1:1-Umwandlung aus Aufnahmen."));
      m.insert(QStringLiteral("tr"), QStringLiteral("Basılı tutma süresi, tüm fare tuşları/tekerleği ve kayıtlardan %100 birebir dönüştürmeyi destekler."));
      m.insert(QStringLiteral("ru"), QStringLiteral("Поддерживает время удержания клавиш, все кнопки и колесо мыши и 100% конвертацию записей."));
      m.insert(QStringLiteral("id"), QStringLiteral("Mendukung durasi tahan tombol, semua tombol & scroll mouse, serta konversi 1:1 dari rekaman."));
      m.insert(QStringLiteral("fa"), QStringLiteral("پشتیبانی از مدت زمان نگه‌داشتن کلید، تمام دکمه‌ها و اسکرول ماوس و تبدیل ۱۰۰٪ دقیق از ضبط‌ها."));
      m.insert(QStringLiteral("ur"), QStringLiteral("بٹن دبانے کا دورانیہ، ماؤس کے تمام بٹن اور اسکرول وہیل، اور ریکارڈنگز سے ١٠٠٪ تبدیلی۔"));
    }
    { auto& m = m_translations[QStringLiteral("combo_master_switch")];
      m.insert(QStringLiteral("ar"), QStringLiteral("تفعيل الأزرار الذكية"));
      m.insert(QStringLiteral("en"), QStringLiteral("Enable Smart Keys"));
      m.insert(QStringLiteral("es"), QStringLiteral("Activar Teclas"));
      m.insert(QStringLiteral("fr"), QStringLiteral("Activer Touches"));
      m.insert(QStringLiteral("de"), QStringLiteral("Smart-Tasten aktivieren"));
      m.insert(QStringLiteral("tr"), QStringLiteral("Akıllı Tuşları Etkinleştir"));
      m.insert(QStringLiteral("ru"), QStringLiteral("Включить клавиши"));
      m.insert(QStringLiteral("id"), QStringLiteral("Aktifkan Tombol"));
      m.insert(QStringLiteral("fa"), QStringLiteral("فعال‌سازی کلیدهای هوشمند"));
      m.insert(QStringLiteral("ur"), QStringLiteral("سمارٹ بٹن فعال کریں"));
    }
    { auto& m = m_translations[QStringLiteral("btn_new_smart_key")];
      m.insert(QStringLiteral("ar"), QStringLiteral("+ ماكرو جديد"));
      m.insert(QStringLiteral("en"), QStringLiteral("+ New Macro"));
      m.insert(QStringLiteral("es"), QStringLiteral("+ Nueva Macro"));
      m.insert(QStringLiteral("fr"), QStringLiteral("+ Nouvelle Macro"));
      m.insert(QStringLiteral("de"), QStringLiteral("+ Neues Makro"));
      m.insert(QStringLiteral("tr"), QStringLiteral("+ Yeni Makro"));
      m.insert(QStringLiteral("ru"), QStringLiteral("+ Новый макрос"));
      m.insert(QStringLiteral("id"), QStringLiteral("+ Makro Baru"));
      m.insert(QStringLiteral("fa"), QStringLiteral("+ ماکروی جدید"));
      m.insert(QStringLiteral("ur"), QStringLiteral("+ نیا میکرو"));
    }
    { auto& m = m_translations[QStringLiteral("empty_macros_title")];
      m.insert(QStringLiteral("ar"), QStringLiteral("لا يوجد أي ماكرو مضاف حالياً"));
      m.insert(QStringLiteral("en"), QStringLiteral("No Macros Created Yet"));
      m.insert(QStringLiteral("es"), QStringLiteral("Aún no hay macros creadas"));
      m.insert(QStringLiteral("fr"), QStringLiteral("Aucune macro créée pour le moment"));
      m.insert(QStringLiteral("de"), QStringLiteral("Noch keine Makros erstellt"));
      m.insert(QStringLiteral("tr"), QStringLiteral("Henüz Makro Oluşturulmadı"));
      m.insert(QStringLiteral("ru"), QStringLiteral("Макросы еще не созданы"));
      m.insert(QStringLiteral("id"), QStringLiteral("Belum Ada Makro yang Dibuat"));
      m.insert(QStringLiteral("fa"), QStringLiteral("هنوز هیچ ماکرویی ساخته نشده است"));
      m.insert(QStringLiteral("ur"), QStringLiteral("ابھی تک کوئی میکرو نہیں بنایا گیا"));
    }
    { auto& m = m_translations[QStringLiteral("empty_macros_subtitle")];
      m.insert(QStringLiteral("ar"), QStringLiteral("اضغط على زر «+ ماكرو جديد» بالأسفل لإضافة أول ماكرو لك وفتح نافذة إعداد الخطوات والاختصارات."));
      m.insert(QStringLiteral("en"), QStringLiteral("Click '+ New Macro' below to create your first macro and open the step & shortcut editor."));
      m.insert(QStringLiteral("es"), QStringLiteral("Haz clic en '+ Nueva Macro' abajo para crear tu primera macro."));
      m.insert(QStringLiteral("fr"), QStringLiteral("Cliquez sur '+ Nouvelle Macro' ci-dessous pour créer votre première macro."));
      m.insert(QStringLiteral("de"), QStringLiteral("Klicken Sie unten auf '+ Neues Makro', um Ihr erstes Makro zu erstellen."));
      m.insert(QStringLiteral("tr"), QStringLiteral("İlk makronuzu oluşturmak için aşağıdaki '+ Yeni Makro' düğmesine tıklayın."));
      m.insert(QStringLiteral("ru"), QStringLiteral("Нажмите «+ Новый макрос» ниже, чтобы создать свой первый макрос."));
      m.insert(QStringLiteral("id"), QStringLiteral("Klik '+ Makro Baru' di bawah untuk membuat makro pertama Anda."));
      m.insert(QStringLiteral("fa"), QStringLiteral("برای ساخت اولین ماکرو، روی دکمه «+ ماکروی جدید» در پایین کلیک کنید."));
      m.insert(QStringLiteral("ur"), QStringLiteral("اپنا پہلا میکرو بنانے کے لیے نیچے '+ نیا میکرو' پر کلک کریں۔"));
    }
    { auto& m = m_translations[QStringLiteral("step1_header")];
      m.insert(QStringLiteral("ar"), QStringLiteral("① حدد الاختصار (زرار أو 2 أو 3 معاً — أو اتركه بدون زرار للتشغيل اليدوي)"));
      m.insert(QStringLiteral("en"), QStringLiteral("① Set Trigger Shortcut (1, 2, or 3 Keys Together — or Leave Blank for Manual Run)"));
      m.insert(QStringLiteral("es"), QStringLiteral("① Asignar Atajo (1, 2 o 3 teclas juntas — o dejar vacío para modo manual)"));
      m.insert(QStringLiteral("fr"), QStringLiteral("① Définir le Raccourci (1, 2 ou 3 touches — ou laisser vide pour exécution manuelle)"));
      m.insert(QStringLiteral("de"), QStringLiteral("① Tastenkürzel wählen (1, 2 oder 3 Tasten — oder leer lassen für manuellen Start)"));
      m.insert(QStringLiteral("tr"), QStringLiteral("① Kısayol Tuşunu Belirle (1, 2 veya 3 Tuş Birlikte — veya Manuel Çalıştırma İçin Boş Bırak)"));
      m.insert(QStringLiteral("ru"), QStringLiteral("① Задайте горячую клавишу (1, 2 или 3 клавиши — или оставьте пустым для ручного запуска)"));
      m.insert(QStringLiteral("id"), QStringLiteral("① Atur Pintasan Pemicu (1, 2, atau 3 Tombol — atau Kosongkan untuk Manual)"));
      m.insert(QStringLiteral("fa"), QStringLiteral("① تعیین میانبر (۱، ۲ یا ۳ کلید همزمان — یا بدون کلید برای اجرای دستی)"));
      m.insert(QStringLiteral("ur"), QStringLiteral("① شارٹ کٹ منتخب کریں (١، ٢ یا ٣ بٹن ایک ساتھ — یا دستی چلانے کے لیے خالی چھوڑ دیں)"));
    }
    { auto& m = m_translations[QStringLiteral("btn_clear_hotkey")];
      m.insert(QStringLiteral("ar"), QStringLiteral("بدون زرار (تشغيل يدوي)"));
      m.insert(QStringLiteral("en"), QStringLiteral("No Key (Manual Only)"));
      m.insert(QStringLiteral("es"), QStringLiteral("Sin tecla (Manual)"));
      m.insert(QStringLiteral("fr"), QStringLiteral("Sans touche (Manuel)"));
      m.insert(QStringLiteral("de"), QStringLiteral("Ohne Taste (Manuell)"));
      m.insert(QStringLiteral("tr"), QStringLiteral("Tuşsuz (Manuel)"));
      m.insert(QStringLiteral("ru"), QStringLiteral("Без клавиши"));
      m.insert(QStringLiteral("id"), QStringLiteral("Tanpa Tombol"));
      m.insert(QStringLiteral("fa"), QStringLiteral("بدون کلید (اجرای دستی)"));
      m.insert(QStringLiteral("ur"), QStringLiteral("بغیر بٹن (دستی چلائیں)"));
    }
    { auto& m = m_translations[QStringLiteral("ph_macro_title_opt")];
      m.insert(QStringLiteral("ar"), QStringLiteral("اسم الماكرو (اختياري)..."));
      m.insert(QStringLiteral("en"), QStringLiteral("Macro title (optional)..."));
      m.insert(QStringLiteral("es"), QStringLiteral("Título de macro (opcional)..."));
      m.insert(QStringLiteral("fr"), QStringLiteral("Nom de la macro (optionnel)..."));
      m.insert(QStringLiteral("de"), QStringLiteral("Makroname (optional)..."));
      m.insert(QStringLiteral("tr"), QStringLiteral("Makro adı (isteğe bağlı)..."));
      m.insert(QStringLiteral("ru"), QStringLiteral("Название макроса..."));
      m.insert(QStringLiteral("id"), QStringLiteral("Nama makro (opsional)..."));
      m.insert(QStringLiteral("fa"), QStringLiteral("نام ماکرو (اختیاری)..."));
      m.insert(QStringLiteral("ur"), QStringLiteral("میکرو کا نام (اختیاری)..."));
    }
    { auto& m = m_translations[QStringLiteral("step2_header")];
      m.insert(QStringLiteral("ar"), QStringLiteral("② خطوات الماكرو (اسحب أي خطوة بالماوس لترتيبها فوراً، أو اضغط ▶ لتجربتها)"));
      m.insert(QStringLiteral("en"), QStringLiteral("② Macro Steps (Drag any step to reorder live, or click ▶ to test it)"));
      m.insert(QStringLiteral("es"), QStringLiteral("② Pasos de la Macro (Arrastra cualquier paso para reordenar o pulsa ▶ para probar)"));
      m.insert(QStringLiteral("fr"), QStringLiteral("② Étapes de la Macro (Glissez pour réorganiser ou cliquez sur ▶ pour tester)"));
      m.insert(QStringLiteral("de"), QStringLiteral("② Makro-Schritte (Zum Sortieren ziehen oder ▶ zum Testen klicken)"));
      m.insert(QStringLiteral("tr"), QStringLiteral("② Makro Adımları (Sıralamak için sürükleyin veya test etmek için ▶ tuşuna basın)"));
      m.insert(QStringLiteral("ru"), QStringLiteral("② Шаги макроса (Перетащите шаг мышью для сортировки или нажмите ▶ для теста)"));
      m.insert(QStringLiteral("id"), QStringLiteral("② Langkah Makro (Seret langkah untuk mengurutkan, atau klik ▶ untuk menguji)"));
      m.insert(QStringLiteral("fa"), QStringLiteral("② گام‌های ماکرو (هر گام را با ماوس بکشید و جابجا کنید، یا با ▶ تست کنید)"));
      m.insert(QStringLiteral("ur"), QStringLiteral("② میکرو کے مراحل (ترتیب بدلنے کے لیے ماؤس سے کھینچیں یا ٹیسٹ کے لیے ▶ دبائیں)"));
    }
    { auto& m = m_translations[QStringLiteral("step_add_key_tap")];
      m.insert(QStringLiteral("ar"), QStringLiteral("+ ضغطة زرار"));
      m.insert(QStringLiteral("en"), QStringLiteral("+ Key Press"));
      m.insert(QStringLiteral("es"), QStringLiteral("+ Tecla"));
      m.insert(QStringLiteral("fr"), QStringLiteral("+ Touche"));
      m.insert(QStringLiteral("de"), QStringLiteral("+ Taste"));
      m.insert(QStringLiteral("tr"), QStringLiteral("+ Tuş Basımı"));
      m.insert(QStringLiteral("ru"), QStringLiteral("+ Клавиша"));
      m.insert(QStringLiteral("id"), QStringLiteral("+ Tekan Tombol"));
      m.insert(QStringLiteral("fa"), QStringLiteral("+ فشردن کلید"));
      m.insert(QStringLiteral("ur"), QStringLiteral("+ بٹن دبائیں"));
    }
    { auto& m = m_translations[QStringLiteral("step_add_key_hold")];
      m.insert(QStringLiteral("ar"), QStringLiteral("+ ضغطة مطوّلة"));
      m.insert(QStringLiteral("en"), QStringLiteral("+ Key Hold"));
      m.insert(QStringLiteral("es"), QStringLiteral("+ Mantener Tecla"));
      m.insert(QStringLiteral("fr"), QStringLiteral("+ Maintenir Touche"));
      m.insert(QStringLiteral("de"), QStringLiteral("+ Taste Halten"));
      m.insert(QStringLiteral("tr"), QStringLiteral("+ Uzun Basım"));
      m.insert(QStringLiteral("ru"), QStringLiteral("+ Удержание клавиши"));
      m.insert(QStringLiteral("id"), QStringLiteral("+ Tahan Tombol"));
      m.insert(QStringLiteral("fa"), QStringLiteral("+ نگه‌داشتن کلید"));
      m.insert(QStringLiteral("ur"), QStringLiteral("+ لمبا بٹن دبائیں"));
    }
    { auto& m = m_translations[QStringLiteral("step_add_wait")];
      m.insert(QStringLiteral("ar"), QStringLiteral("+ انتظار"));
      m.insert(QStringLiteral("en"), QStringLiteral("+ Wait (ms)"));
      m.insert(QStringLiteral("es"), QStringLiteral("+ Espera"));
      m.insert(QStringLiteral("fr"), QStringLiteral("+ Attente"));
      m.insert(QStringLiteral("de"), QStringLiteral("+ Warten"));
      m.insert(QStringLiteral("tr"), QStringLiteral("+ Bekleme"));
      m.insert(QStringLiteral("ru"), QStringLiteral("+ Ожидание"));
      m.insert(QStringLiteral("id"), QStringLiteral("+ Tunggu"));
      m.insert(QStringLiteral("fa"), QStringLiteral("+ انتظار"));
      m.insert(QStringLiteral("ur"), QStringLiteral("+ انتظار"));
    }
    { auto& m = m_translations[QStringLiteral("step_add_mouse_click")];
      m.insert(QStringLiteral("ar"), QStringLiteral("+ كليك ماوس"));
      m.insert(QStringLiteral("en"), QStringLiteral("+ Mouse Click"));
      m.insert(QStringLiteral("es"), QStringLiteral("+ Clic Ratón"));
      m.insert(QStringLiteral("fr"), QStringLiteral("+ Clic Souris"));
      m.insert(QStringLiteral("de"), QStringLiteral("+ Mausklick"));
      m.insert(QStringLiteral("tr"), QStringLiteral("+ Fare Tıkla"));
      m.insert(QStringLiteral("ru"), QStringLiteral("+ Клик мыши"));
      m.insert(QStringLiteral("id"), QStringLiteral("+ Klik Mouse"));
      m.insert(QStringLiteral("fa"), QStringLiteral("+ کلیک ماوس"));
      m.insert(QStringLiteral("ur"), QStringLiteral("+ ماؤس کلک"));
    }
    { auto& m = m_translations[QStringLiteral("step_add_mouse_scroll")];
      m.insert(QStringLiteral("ar"), QStringLiteral("+ سكرول البكرة"));
      m.insert(QStringLiteral("en"), QStringLiteral("+ Mouse Scroll"));
      m.insert(QStringLiteral("es"), QStringLiteral("+ Rueda Ratón"));
      m.insert(QStringLiteral("fr"), QStringLiteral("+ Molette Souris"));
      m.insert(QStringLiteral("de"), QStringLiteral("+ Mausrad"));
      m.insert(QStringLiteral("tr"), QStringLiteral("+ Fare Tekerleği"));
      m.insert(QStringLiteral("ru"), QStringLiteral("+ Колесо мыши"));
      m.insert(QStringLiteral("id"), QStringLiteral("+ Scroll Mouse"));
      m.insert(QStringLiteral("fa"), QStringLiteral("+ اسکرول ماوس"));
      m.insert(QStringLiteral("ur"), QStringLiteral("+ ماؤس اسکرول"));
    }
    { auto& m = m_translations[QStringLiteral("step_add_folder")];
      m.insert(QStringLiteral("ar"), QStringLiteral("+ فتح فولدر / برنامج"));
      m.insert(QStringLiteral("en"), QStringLiteral("+ Open Folder / App"));
      m.insert(QStringLiteral("es"), QStringLiteral("+ Abrir Carpeta / App"));
      m.insert(QStringLiteral("fr"), QStringLiteral("+ Ouvrir Dossier / App"));
      m.insert(QStringLiteral("de"), QStringLiteral("+ Ordner / App öffnen"));
      m.insert(QStringLiteral("tr"), QStringLiteral("+ Klasör / Program Aç"));
      m.insert(QStringLiteral("ru"), QStringLiteral("+ Открыть папку / программу"));
      m.insert(QStringLiteral("id"), QStringLiteral("+ Buka Folder / Aplikasi"));
      m.insert(QStringLiteral("fa"), QStringLiteral("+ باز کردن پوشه / برنامه"));
      m.insert(QStringLiteral("ur"), QStringLiteral("+ فولڈر / پروگرام کھولیں"));
    }
    { auto& m = m_translations[QStringLiteral("step_add_ps")];
      m.insert(QStringLiteral("ar"), QStringLiteral("+ أمر PowerShell"));
      m.insert(QStringLiteral("en"), QStringLiteral("+ PowerShell"));
      m.insert(QStringLiteral("es"), QStringLiteral("+ PowerShell"));
      m.insert(QStringLiteral("fr"), QStringLiteral("+ PowerShell"));
      m.insert(QStringLiteral("de"), QStringLiteral("+ PowerShell"));
      m.insert(QStringLiteral("tr"), QStringLiteral("+ PowerShell"));
      m.insert(QStringLiteral("ru"), QStringLiteral("+ PowerShell"));
      m.insert(QStringLiteral("id"), QStringLiteral("+ PowerShell"));
      m.insert(QStringLiteral("fa"), QStringLiteral("+ دستور PowerShell"));
      m.insert(QStringLiteral("ur"), QStringLiteral("+ PowerShell کمانڈ"));
    }
    { auto& m = m_translations[QStringLiteral("step_add_cmd")];
      m.insert(QStringLiteral("ar"), QStringLiteral("+ أمر CMD"));
      m.insert(QStringLiteral("en"), QStringLiteral("+ CMD"));
      m.insert(QStringLiteral("es"), QStringLiteral("+ CMD"));
      m.insert(QStringLiteral("fr"), QStringLiteral("+ CMD"));
      m.insert(QStringLiteral("de"), QStringLiteral("+ CMD"));
      m.insert(QStringLiteral("tr"), QStringLiteral("+ CMD"));
      m.insert(QStringLiteral("ru"), QStringLiteral("+ CMD"));
      m.insert(QStringLiteral("id"), QStringLiteral("+ CMD"));
      m.insert(QStringLiteral("fa"), QStringLiteral("+ دستور CMD"));
      m.insert(QStringLiteral("ur"), QStringLiteral("+ CMD کمانڈ"));
    }
    { auto& m = m_translations[QStringLiteral("lbl_hold_duration")];
      m.insert(QStringLiteral("ar"), QStringLiteral("مدة الضغط:"));
      m.insert(QStringLiteral("en"), QStringLiteral("Hold:"));
      m.insert(QStringLiteral("es"), QStringLiteral("Duración:"));
      m.insert(QStringLiteral("fr"), QStringLiteral("Durée :"));
      m.insert(QStringLiteral("de"), QStringLiteral("Haltedauer:"));
      m.insert(QStringLiteral("tr"), QStringLiteral("Basılı Süre:"));
      m.insert(QStringLiteral("ru"), QStringLiteral("Удержание:"));
      m.insert(QStringLiteral("id"), QStringLiteral("Durasi tahan:"));
      m.insert(QStringLiteral("fa"), QStringLiteral("مدت فشار:"));
      m.insert(QStringLiteral("ur"), QStringLiteral("دبانے کا وقت:"));
    }
    { auto& m = m_translations[QStringLiteral("lbl_pre_delay")];
      m.insert(QStringLiteral("ar"), QStringLiteral("التأخير:"));
      m.insert(QStringLiteral("en"), QStringLiteral("Delay:"));
      m.insert(QStringLiteral("es"), QStringLiteral("Retraso:"));
      m.insert(QStringLiteral("fr"), QStringLiteral("Délai :"));
      m.insert(QStringLiteral("de"), QStringLiteral("Verzögerung:"));
      m.insert(QStringLiteral("tr"), QStringLiteral("Gecikme:"));
      m.insert(QStringLiteral("ru"), QStringLiteral("Задержка:"));
      m.insert(QStringLiteral("id"), QStringLiteral("Jeda:"));
      m.insert(QStringLiteral("fa"), QStringLiteral("تأخیر:"));
      m.insert(QStringLiteral("ur"), QStringLiteral("تاخیر:"));
    }
    { auto& m = m_translations[QStringLiteral("lbl_scroll_dir")];
      m.insert(QStringLiteral("ar"), QStringLiteral("سكرول البكرة:"));
      m.insert(QStringLiteral("en"), QStringLiteral("Scroll Wheel:"));
      m.insert(QStringLiteral("es"), QStringLiteral("Rueda:"));
      m.insert(QStringLiteral("fr"), QStringLiteral("Molette :"));
      m.insert(QStringLiteral("de"), QStringLiteral("Mausrad:"));
      m.insert(QStringLiteral("tr"), QStringLiteral("Tekerlek:"));
      m.insert(QStringLiteral("ru"), QStringLiteral("Прокрутка:"));
      m.insert(QStringLiteral("id"), QStringLiteral("Scroll:"));
      m.insert(QStringLiteral("fa"), QStringLiteral("اسکرول ماوس:"));
      m.insert(QStringLiteral("ur"), QStringLiteral("ماؤس اسکرول:"));
    }
    { auto& m = m_translations[QStringLiteral("lbl_scroll_amount")];
      m.insert(QStringLiteral("ar"), QStringLiteral("المقدار:"));
      m.insert(QStringLiteral("en"), QStringLiteral("Amount:"));
      m.insert(QStringLiteral("es"), QStringLiteral("Cantidad:"));
      m.insert(QStringLiteral("fr"), QStringLiteral("Quantité :"));
      m.insert(QStringLiteral("de"), QStringLiteral("Stufen:"));
      m.insert(QStringLiteral("tr"), QStringLiteral("Miktar:"));
      m.insert(QStringLiteral("ru"), QStringLiteral("Шаги:"));
      m.insert(QStringLiteral("id"), QStringLiteral("Jumlah:"));
      m.insert(QStringLiteral("fa"), QStringLiteral("مقدار:"));
      m.insert(QStringLiteral("ur"), QStringLiteral("مقدار:"));
    }
    { auto& m = m_translations[QStringLiteral("scroll_down")];
      m.insert(QStringLiteral("ar"), QStringLiteral("لأسفل ↓"));
      m.insert(QStringLiteral("en"), QStringLiteral("Down ↓"));
      m.insert(QStringLiteral("es"), QStringLiteral("Abajo ↓"));
      m.insert(QStringLiteral("fr"), QStringLiteral("Bas ↓"));
      m.insert(QStringLiteral("de"), QStringLiteral("Runter ↓"));
      m.insert(QStringLiteral("tr"), QStringLiteral("Aşağı ↓"));
      m.insert(QStringLiteral("ru"), QStringLiteral("Вниз ↓"));
      m.insert(QStringLiteral("id"), QStringLiteral("Bawah ↓"));
      m.insert(QStringLiteral("fa"), QStringLiteral("رو به پایین ↓"));
      m.insert(QStringLiteral("ur"), QStringLiteral("نیچے ↓"));
    }
    { auto& m = m_translations[QStringLiteral("scroll_up")];
      m.insert(QStringLiteral("ar"), QStringLiteral("لأعلى ↑"));
      m.insert(QStringLiteral("en"), QStringLiteral("Up ↑"));
      m.insert(QStringLiteral("es"), QStringLiteral("Arriba ↑"));
      m.insert(QStringLiteral("fr"), QStringLiteral("Haut ↑"));
      m.insert(QStringLiteral("de"), QStringLiteral("Hoch ↑"));
      m.insert(QStringLiteral("tr"), QStringLiteral("Yukarı ↑"));
      m.insert(QStringLiteral("ru"), QStringLiteral("Вверх ↑"));
      m.insert(QStringLiteral("id"), QStringLiteral("Atas ↑"));
      m.insert(QStringLiteral("fa"), QStringLiteral("رو به بالا ↑"));
      m.insert(QStringLiteral("ur"), QStringLiteral("اوپر ↑"));
    }
    { auto& m = m_translations[QStringLiteral("scroll_left")];
      m.insert(QStringLiteral("ar"), QStringLiteral("يسار ←"));
      m.insert(QStringLiteral("en"), QStringLiteral("Left ←"));
      m.insert(QStringLiteral("es"), QStringLiteral("Izquierda ←"));
      m.insert(QStringLiteral("fr"), QStringLiteral("Gauche ←"));
      m.insert(QStringLiteral("de"), QStringLiteral("Links ←"));
      m.insert(QStringLiteral("tr"), QStringLiteral("Sol ←"));
      m.insert(QStringLiteral("ru"), QStringLiteral("Влево ←"));
      m.insert(QStringLiteral("id"), QStringLiteral("Kiri ←"));
      m.insert(QStringLiteral("fa"), QStringLiteral("چپ ←"));
      m.insert(QStringLiteral("ur"), QStringLiteral("بائیں ←"));
    }
    { auto& m = m_translations[QStringLiteral("scroll_right")];
      m.insert(QStringLiteral("ar"), QStringLiteral("يمين →"));
      m.insert(QStringLiteral("en"), QStringLiteral("Right →"));
      m.insert(QStringLiteral("es"), QStringLiteral("Derecha →"));
      m.insert(QStringLiteral("fr"), QStringLiteral("Droite →"));
      m.insert(QStringLiteral("de"), QStringLiteral("Rechts →"));
      m.insert(QStringLiteral("tr"), QStringLiteral("Sağ →"));
      m.insert(QStringLiteral("ru"), QStringLiteral("Вправо →"));
      m.insert(QStringLiteral("id"), QStringLiteral("Kanan →"));
      m.insert(QStringLiteral("fa"), QStringLiteral("راست →"));
      m.insert(QStringLiteral("ur"), QStringLiteral("دائیں →"));
    }
    { auto& m = m_translations[QStringLiteral("mouse_btn_left")];
      m.insert(QStringLiteral("ar"), QStringLiteral("كليك شمال"));
      m.insert(QStringLiteral("en"), QStringLiteral("Left Click"));
      m.insert(QStringLiteral("es"), QStringLiteral("Clic Izquierdo"));
      m.insert(QStringLiteral("fr"), QStringLiteral("Clic Gauche"));
      m.insert(QStringLiteral("de"), QStringLiteral("Linksklick"));
      m.insert(QStringLiteral("tr"), QStringLiteral("Sol Tık"));
      m.insert(QStringLiteral("ru"), QStringLiteral("Левый клик"));
      m.insert(QStringLiteral("id"), QStringLiteral("Klik Kiri"));
      m.insert(QStringLiteral("fa"), QStringLiteral("کلیک چپ"));
      m.insert(QStringLiteral("ur"), QStringLiteral("بائیں کلک"));
    }
    { auto& m = m_translations[QStringLiteral("mouse_btn_right")];
      m.insert(QStringLiteral("ar"), QStringLiteral("كليك يمين"));
      m.insert(QStringLiteral("en"), QStringLiteral("Right Click"));
      m.insert(QStringLiteral("es"), QStringLiteral("Clic Derecho"));
      m.insert(QStringLiteral("fr"), QStringLiteral("Clic Droit"));
      m.insert(QStringLiteral("de"), QStringLiteral("Rechtsklick"));
      m.insert(QStringLiteral("tr"), QStringLiteral("Sağ Tık"));
      m.insert(QStringLiteral("ru"), QStringLiteral("Правый клик"));
      m.insert(QStringLiteral("id"), QStringLiteral("Klik Kanan"));
      m.insert(QStringLiteral("fa"), QStringLiteral("کلیک راست"));
      m.insert(QStringLiteral("ur"), QStringLiteral("دائیں کلک"));
    }
    { auto& m = m_translations[QStringLiteral("mouse_btn_middle")];
      m.insert(QStringLiteral("ar"), QStringLiteral("زر البكرة الأوسط"));
      m.insert(QStringLiteral("en"), QStringLiteral("Middle Wheel"));
      m.insert(QStringLiteral("es"), QStringLiteral("Botón Central"));
      m.insert(QStringLiteral("fr"), QStringLiteral("Bouton Milieu"));
      m.insert(QStringLiteral("de"), QStringLiteral("Mittelklick"));
      m.insert(QStringLiteral("tr"), QStringLiteral("Orta Tuş"));
      m.insert(QStringLiteral("ru"), QStringLiteral("Средняя кнопка"));
      m.insert(QStringLiteral("id"), QStringLiteral("Tombol Tengah"));
      m.insert(QStringLiteral("fa"), QStringLiteral("دکمه وسط ماوس"));
      m.insert(QStringLiteral("ur"), QStringLiteral("درمیانی وہیل بٹن"));
    }
    { auto& m = m_translations[QStringLiteral("mouse_btn_x1")];
      m.insert(QStringLiteral("ar"), QStringLiteral("زر الرجوع (Back)"));
      m.insert(QStringLiteral("en"), QStringLiteral("Back Button (X1)"));
      m.insert(QStringLiteral("es"), QStringLiteral("Atrás (X1)"));
      m.insert(QStringLiteral("fr"), QStringLiteral("Retour (X1)"));
      m.insert(QStringLiteral("de"), QStringLiteral("Zurück (X1)"));
      m.insert(QStringLiteral("tr"), QStringLiteral("Geri (X1)"));
      m.insert(QStringLiteral("ru"), QStringLiteral("Назад (X1)"));
      m.insert(QStringLiteral("id"), QStringLiteral("Mundur (X1)"));
      m.insert(QStringLiteral("fa"), QStringLiteral("دکمه برگشت (Back)"));
      m.insert(QStringLiteral("ur"), QStringLiteral("بیک بٹن (Back)"));
    }
    { auto& m = m_translations[QStringLiteral("mouse_btn_x2")];
      m.insert(QStringLiteral("ar"), QStringLiteral("زر التقدم (Forward)"));
      m.insert(QStringLiteral("en"), QStringLiteral("Forward Button (X2)"));
      m.insert(QStringLiteral("es"), QStringLiteral("Adelante (X2)"));
      m.insert(QStringLiteral("fr"), QStringLiteral("Avancer (X2)"));
      m.insert(QStringLiteral("de"), QStringLiteral("Vorwärts (X2)"));
      m.insert(QStringLiteral("tr"), QStringLiteral("İleri (X2)"));
      m.insert(QStringLiteral("ru"), QStringLiteral("Вперёд (X2)"));
      m.insert(QStringLiteral("id"), QStringLiteral("Maju (X2)"));
      m.insert(QStringLiteral("fa"), QStringLiteral("دکمه جلو (Forward)"));
      m.insert(QStringLiteral("ur"), QStringLiteral("فارورڈ بٹن (Forward)"));
    }
    { auto& m = m_translations[QStringLiteral("step_row_key_tap")];
      m.insert(QStringLiteral("ar"), QStringLiteral("ضغطة زرار:"));
      m.insert(QStringLiteral("en"), QStringLiteral("Key Press:"));
      m.insert(QStringLiteral("es"), QStringLiteral("Tecla:"));
      m.insert(QStringLiteral("fr"), QStringLiteral("Touche :"));
      m.insert(QStringLiteral("de"), QStringLiteral("Taste:"));
      m.insert(QStringLiteral("tr"), QStringLiteral("Tuş Basımı:"));
      m.insert(QStringLiteral("ru"), QStringLiteral("Клавиша:"));
      m.insert(QStringLiteral("id"), QStringLiteral("Tekan Tombol:"));
      m.insert(QStringLiteral("fa"), QStringLiteral("فشردن کلید:"));
      m.insert(QStringLiteral("ur"), QStringLiteral("بٹن دبائیں:"));
    }
    { auto& m = m_translations[QStringLiteral("step_row_key_hold")];
      m.insert(QStringLiteral("ar"), QStringLiteral("ضغطة زرار:"));
      m.insert(QStringLiteral("en"), QStringLiteral("Key Press:"));
      m.insert(QStringLiteral("es"), QStringLiteral("Tecla:"));
      m.insert(QStringLiteral("fr"), QStringLiteral("Touche :"));
      m.insert(QStringLiteral("de"), QStringLiteral("Taste:"));
      m.insert(QStringLiteral("tr"), QStringLiteral("Tuş Basımı:"));
      m.insert(QStringLiteral("ru"), QStringLiteral("Клавиша:"));
      m.insert(QStringLiteral("id"), QStringLiteral("Tekan Tombol:"));
      m.insert(QStringLiteral("fa"), QStringLiteral("فشردن کلید:"));
      m.insert(QStringLiteral("ur"), QStringLiteral("بٹن دبائیں:"));
    }
    { auto& m = m_translations[QStringLiteral("step_row_key_down")];
      m.insert(QStringLiteral("ar"), QStringLiteral("ضغط مطوّل ↓:"));
      m.insert(QStringLiteral("en"), QStringLiteral("Key Down ↓:"));
      m.insert(QStringLiteral("es"), QStringLiteral("Tecla Abajo ↓:"));
      m.insert(QStringLiteral("fr"), QStringLiteral("Touche Bas ↓ :"));
      m.insert(QStringLiteral("de"), QStringLiteral("Taste Runter ↓:"));
      m.insert(QStringLiteral("tr"), QStringLiteral("Tuş Basılı ↓:"));
      m.insert(QStringLiteral("ru"), QStringLiteral("Нажатие ↓:"));
      m.insert(QStringLiteral("id"), QStringLiteral("Tekan Bawah ↓:"));
      m.insert(QStringLiteral("fa"), QStringLiteral("نگه‌داشتن کلید ↓:"));
      m.insert(QStringLiteral("ur"), QStringLiteral("بٹن نیچے ↓:"));
    }
    { auto& m = m_translations[QStringLiteral("step_row_key_up")];
      m.insert(QStringLiteral("ar"), QStringLiteral("رفع الزرار ↑:"));
      m.insert(QStringLiteral("en"), QStringLiteral("Key Up ↑:"));
      m.insert(QStringLiteral("es"), QStringLiteral("Soltar Tecla ↑:"));
      m.insert(QStringLiteral("fr"), QStringLiteral("Relâcher ↑ :"));
      m.insert(QStringLiteral("de"), QStringLiteral("Taste Hoch ↑:"));
      m.insert(QStringLiteral("tr"), QStringLiteral("Tuş Bırak ↑:"));
      m.insert(QStringLiteral("ru"), QStringLiteral("Отпускание ↑:"));
      m.insert(QStringLiteral("id"), QStringLiteral("Lepas Tombol ↑:"));
      m.insert(QStringLiteral("fa"), QStringLiteral("رها کردن کلید ↑:"));
      m.insert(QStringLiteral("ur"), QStringLiteral("بٹن اوپر ↑:"));
    }
    { auto& m = m_translations[QStringLiteral("step_row_wait")];
      m.insert(QStringLiteral("ar"), QStringLiteral("انتظار (ms):"));
      m.insert(QStringLiteral("en"), QStringLiteral("Wait (ms):"));
      m.insert(QStringLiteral("es"), QStringLiteral("Espera (ms):"));
      m.insert(QStringLiteral("fr"), QStringLiteral("Attente (ms) :"));
      m.insert(QStringLiteral("de"), QStringLiteral("Warten (ms):"));
      m.insert(QStringLiteral("tr"), QStringLiteral("Bekleme (ms):"));
      m.insert(QStringLiteral("ru"), QStringLiteral("Ожидание (ms):"));
      m.insert(QStringLiteral("id"), QStringLiteral("Tunggu (ms):"));
      m.insert(QStringLiteral("fa"), QStringLiteral("انتظار (ms):"));
      m.insert(QStringLiteral("ur"), QStringLiteral("انتظار (ms):"));
    }
    { auto& m = m_translations[QStringLiteral("step_row_mouse_click")];
      m.insert(QStringLiteral("ar"), QStringLiteral("كليك ماوس:"));
      m.insert(QStringLiteral("en"), QStringLiteral("Mouse Click:"));
      m.insert(QStringLiteral("es"), QStringLiteral("Clic Ratón:"));
      m.insert(QStringLiteral("fr"), QStringLiteral("Clic Souris :"));
      m.insert(QStringLiteral("de"), QStringLiteral("Mausklick:"));
      m.insert(QStringLiteral("tr"), QStringLiteral("Fare Tıkla:"));
      m.insert(QStringLiteral("ru"), QStringLiteral("Клик мыши:"));
      m.insert(QStringLiteral("id"), QStringLiteral("Klik Mouse:"));
      m.insert(QStringLiteral("fa"), QStringLiteral("کلیک ماوس:"));
      m.insert(QStringLiteral("ur"), QStringLiteral("ماؤس کلک:"));
    }
    { auto& m = m_translations[QStringLiteral("step_row_mouse_down")];
      m.insert(QStringLiteral("ar"), QStringLiteral("ضغط ماوس ↓:"));
      m.insert(QStringLiteral("en"), QStringLiteral("Mouse Down ↓:"));
      m.insert(QStringLiteral("es"), QStringLiteral("Ratón Abajo ↓:"));
      m.insert(QStringLiteral("fr"), QStringLiteral("Souris Bas ↓ :"));
      m.insert(QStringLiteral("de"), QStringLiteral("Maus Runter ↓:"));
      m.insert(QStringLiteral("tr"), QStringLiteral("Fare Basılı ↓:"));
      m.insert(QStringLiteral("ru"), QStringLiteral("Мышь вниз ↓:"));
      m.insert(QStringLiteral("id"), QStringLiteral("Mouse Bawah ↓:"));
      m.insert(QStringLiteral("fa"), QStringLiteral("فشردن ماوس ↓:"));
      m.insert(QStringLiteral("ur"), QStringLiteral("ماؤس نیچے ↓:"));
    }
    { auto& m = m_translations[QStringLiteral("step_row_mouse_up")];
      m.insert(QStringLiteral("ar"), QStringLiteral("رفع ماوس ↑:"));
      m.insert(QStringLiteral("en"), QStringLiteral("Mouse Up ↑:"));
      m.insert(QStringLiteral("es"), QStringLiteral("Soltar Ratón ↑:"));
      m.insert(QStringLiteral("fr"), QStringLiteral("Relâcher Souris ↑ :"));
      m.insert(QStringLiteral("de"), QStringLiteral("Maus Hoch ↑:"));
      m.insert(QStringLiteral("tr"), QStringLiteral("Fare Bırak ↑:"));
      m.insert(QStringLiteral("ru"), QStringLiteral("Мышь вверх ↑:"));
      m.insert(QStringLiteral("id"), QStringLiteral("Mouse Atas ↑:"));
      m.insert(QStringLiteral("fa"), QStringLiteral("رها کردن ماوس ↑:"));
      m.insert(QStringLiteral("ur"), QStringLiteral("ماؤس اوپر ↑:"));
    }
    { auto& m = m_translations[QStringLiteral("step_row_mouse_move")];
      m.insert(QStringLiteral("ar"), QStringLiteral("تحريك ماوس:"));
      m.insert(QStringLiteral("en"), QStringLiteral("Move Mouse:"));
      m.insert(QStringLiteral("es"), QStringLiteral("Mover Ratón:"));
      m.insert(QStringLiteral("fr"), QStringLiteral("Déplacer Souris :"));
      m.insert(QStringLiteral("de"), QStringLiteral("Maus bewegen:"));
      m.insert(QStringLiteral("tr"), QStringLiteral("Fare Hareketi:"));
      m.insert(QStringLiteral("ru"), QStringLiteral("Движение мыши:"));
      m.insert(QStringLiteral("id"), QStringLiteral("Gerak Mouse:"));
      m.insert(QStringLiteral("fa"), QStringLiteral("حرکت ماوس:"));
      m.insert(QStringLiteral("ur"), QStringLiteral("ماؤس حرکت:"));
    }
    { auto& m = m_translations[QStringLiteral("step_row_mouse_scroll")];
      m.insert(QStringLiteral("ar"), QStringLiteral("سكرول البكرة:"));
      m.insert(QStringLiteral("en"), QStringLiteral("Scroll Wheel:"));
      m.insert(QStringLiteral("es"), QStringLiteral("Rueda:"));
      m.insert(QStringLiteral("fr"), QStringLiteral("Molette :"));
      m.insert(QStringLiteral("de"), QStringLiteral("Mausrad:"));
      m.insert(QStringLiteral("tr"), QStringLiteral("Tekerlek:"));
      m.insert(QStringLiteral("ru"), QStringLiteral("Колесо мыши:"));
      m.insert(QStringLiteral("id"), QStringLiteral("Scroll Mouse:"));
      m.insert(QStringLiteral("fa"), QStringLiteral("اسکرول ماوس:"));
      m.insert(QStringLiteral("ur"), QStringLiteral("ماؤس اسکرول:"));
    }
    { auto& m = m_translations[QStringLiteral("step_row_folder")];
      m.insert(QStringLiteral("ar"), QStringLiteral("فتح مسار / فولدر:"));
      m.insert(QStringLiteral("en"), QStringLiteral("Open Folder / Path:"));
      m.insert(QStringLiteral("es"), QStringLiteral("Abrir carpeta:"));
      m.insert(QStringLiteral("fr"), QStringLiteral("Ouvrir dossier :"));
      m.insert(QStringLiteral("de"), QStringLiteral("Ordner öffnen:"));
      m.insert(QStringLiteral("tr"), QStringLiteral("Klasör aç:"));
      m.insert(QStringLiteral("ru"), QStringLiteral("Открыть папку:"));
      m.insert(QStringLiteral("id"), QStringLiteral("Buka folder:"));
      m.insert(QStringLiteral("fa"), QStringLiteral("باز کردن پوشه:"));
      m.insert(QStringLiteral("ur"), QStringLiteral("فولڈر کھولیں:"));
    }
    { auto& m = m_translations[QStringLiteral("lbl_open_folder_step")];
      m.insert(QStringLiteral("ar"), QStringLiteral("فتح مسار / فولدر:"));
      m.insert(QStringLiteral("en"), QStringLiteral("Open Folder / Path:"));
      m.insert(QStringLiteral("es"), QStringLiteral("Abrir carpeta:"));
      m.insert(QStringLiteral("fr"), QStringLiteral("Ouvrir dossier :"));
      m.insert(QStringLiteral("de"), QStringLiteral("Ordner öffnen:"));
      m.insert(QStringLiteral("tr"), QStringLiteral("Klasör aç:"));
      m.insert(QStringLiteral("ru"), QStringLiteral("Открыть папку:"));
      m.insert(QStringLiteral("id"), QStringLiteral("Buka folder:"));
      m.insert(QStringLiteral("fa"), QStringLiteral("باز کردن پوشه:"));
      m.insert(QStringLiteral("ur"), QStringLiteral("فولڈر کھولیں:"));
    }
    { auto& m = m_translations[QStringLiteral("step_row_ps")];
      m.insert(QStringLiteral("ar"), QStringLiteral("أمر PowerShell:"));
      m.insert(QStringLiteral("en"), QStringLiteral("PowerShell:"));
      m.insert(QStringLiteral("es"), QStringLiteral("PowerShell:"));
      m.insert(QStringLiteral("fr"), QStringLiteral("PowerShell :"));
      m.insert(QStringLiteral("de"), QStringLiteral("PowerShell:"));
      m.insert(QStringLiteral("tr"), QStringLiteral("PowerShell:"));
      m.insert(QStringLiteral("ru"), QStringLiteral("PowerShell:"));
      m.insert(QStringLiteral("id"), QStringLiteral("PowerShell:"));
      m.insert(QStringLiteral("fa"), QStringLiteral("دستور PowerShell:"));
      m.insert(QStringLiteral("ur"), QStringLiteral("PowerShell کمانڈ:"));
    }
    { auto& m = m_translations[QStringLiteral("step_row_cmd")];
      m.insert(QStringLiteral("ar"), QStringLiteral("أمر CMD:"));
      m.insert(QStringLiteral("en"), QStringLiteral("CMD:"));
      m.insert(QStringLiteral("es"), QStringLiteral("CMD:"));
      m.insert(QStringLiteral("fr"), QStringLiteral("CMD :"));
      m.insert(QStringLiteral("de"), QStringLiteral("CMD:"));
      m.insert(QStringLiteral("tr"), QStringLiteral("CMD:"));
      m.insert(QStringLiteral("ru"), QStringLiteral("CMD:"));
      m.insert(QStringLiteral("id"), QStringLiteral("CMD:"));
      m.insert(QStringLiteral("fa"), QStringLiteral("دستور CMD:"));
      m.insert(QStringLiteral("ur"), QStringLiteral("CMD کمانڈ:"));
    }
    { auto& m = m_translations[QStringLiteral("btn_browse_folder")];
      m.insert(QStringLiteral("ar"), QStringLiteral("فولدر..."));
      m.insert(QStringLiteral("en"), QStringLiteral("Folder..."));
      m.insert(QStringLiteral("es"), QStringLiteral("Carpeta..."));
      m.insert(QStringLiteral("fr"), QStringLiteral("Dossier..."));
      m.insert(QStringLiteral("de"), QStringLiteral("Ordner..."));
      m.insert(QStringLiteral("tr"), QStringLiteral("Klasör..."));
      m.insert(QStringLiteral("ru"), QStringLiteral("Папка..."));
      m.insert(QStringLiteral("id"), QStringLiteral("Folder..."));
      m.insert(QStringLiteral("fa"), QStringLiteral("پوشه..."));
      m.insert(QStringLiteral("ur"), QStringLiteral("فولڈر..."));
    }
    { auto& m = m_translations[QStringLiteral("btn_pick_coord")];
      m.insert(QStringLiteral("ar"), QStringLiteral("التقط (F8)"));
      m.insert(QStringLiteral("en"), QStringLiteral("Pick (F8)"));
      m.insert(QStringLiteral("es"), QStringLiteral("Capturar (F8)"));
      m.insert(QStringLiteral("fr"), QStringLiteral("Capturer (F8)"));
      m.insert(QStringLiteral("de"), QStringLiteral("Erfassen (F8)"));
      m.insert(QStringLiteral("tr"), QStringLiteral("Seç (F8)"));
      m.insert(QStringLiteral("ru"), QStringLiteral("Захват (F8)"));
      m.insert(QStringLiteral("id"), QStringLiteral("Ambil (F8)"));
      m.insert(QStringLiteral("fa"), QStringLiteral("دریافت (F8)"));
      m.insert(QStringLiteral("ur"), QStringLiteral("منتخب کریں (F8)"));
    }
    { auto& m = m_translations[QStringLiteral("btn_pick_live_xy")];
      m.insert(QStringLiteral("ar"), QStringLiteral("التقط (F8)"));
      m.insert(QStringLiteral("en"), QStringLiteral("Pick (F8)"));
      m.insert(QStringLiteral("es"), QStringLiteral("Capturar (F8)"));
      m.insert(QStringLiteral("fr"), QStringLiteral("Capturer (F8)"));
      m.insert(QStringLiteral("de"), QStringLiteral("Erfassen (F8)"));
      m.insert(QStringLiteral("tr"), QStringLiteral("Seç (F8)"));
      m.insert(QStringLiteral("ru"), QStringLiteral("Захват (F8)"));
      m.insert(QStringLiteral("id"), QStringLiteral("Ambil (F8)"));
      m.insert(QStringLiteral("fa"), QStringLiteral("دریافت (F8)"));
      m.insert(QStringLiteral("ur"), QStringLiteral("منتخب کریں (F8)"));
    }
    { auto& m = m_translations[QStringLiteral("btn_press_f8")];
      m.insert(QStringLiteral("ar"), QStringLiteral("اضغط F8"));
      m.insert(QStringLiteral("en"), QStringLiteral("Press F8"));
      m.insert(QStringLiteral("es"), QStringLiteral("Pulsa F8"));
      m.insert(QStringLiteral("fr"), QStringLiteral("Appuyez F8"));
      m.insert(QStringLiteral("de"), QStringLiteral("F8 drücken"));
      m.insert(QStringLiteral("tr"), QStringLiteral("F8'e Basın"));
      m.insert(QStringLiteral("ru"), QStringLiteral("Нажмите F8"));
      m.insert(QStringLiteral("id"), QStringLiteral("Tekan F8"));
      m.insert(QStringLiteral("fa"), QStringLiteral("کلید F8 را بزنید"));
      m.insert(QStringLiteral("ur"), QStringLiteral("F8 دبائیں"));
    }
    { auto& m = m_translations[QStringLiteral("lbl_live_cursor")];
      m.insert(QStringLiteral("ar"), QStringLiteral("موقع الماوس الآن:"));
      m.insert(QStringLiteral("en"), QStringLiteral("Live Cursor:"));
      m.insert(QStringLiteral("es"), QStringLiteral("Cursor actual:"));
      m.insert(QStringLiteral("fr"), QStringLiteral("Curseur actuel :"));
      m.insert(QStringLiteral("de"), QStringLiteral("Mausposition:"));
      m.insert(QStringLiteral("tr"), QStringLiteral("Canlı Konum:"));
      m.insert(QStringLiteral("ru"), QStringLiteral("Курсор:"));
      m.insert(QStringLiteral("id"), QStringLiteral("Kursor Saat Ini:"));
      m.insert(QStringLiteral("fa"), QStringLiteral("مکان فعلی ماوس:"));
      m.insert(QStringLiteral("ur"), QStringLiteral("ماؤس کی موجودہ جگہ:"));
    }
    { auto& m = m_translations[QStringLiteral("step3_header")];
      m.insert(QStringLiteral("ar"), QStringLiteral("③ تفعيل الاختصار أو ضبط مهلة التشغيل اليدوي:"));
      m.insert(QStringLiteral("en"), QStringLiteral("③ Arm Shortcut or Set Start Delay for Manual Run:"));
      m.insert(QStringLiteral("es"), QStringLiteral("③ Activar Atajo o Ajustar Retraso Inicial:"));
      m.insert(QStringLiteral("fr"), QStringLiteral("③ Activer le Raccourci ou Délai de Départ :"));
      m.insert(QStringLiteral("de"), QStringLiteral("③ Kürzel aktivieren oder Startverzögerung:"));
      m.insert(QStringLiteral("tr"), QStringLiteral("③ Kısayolu Etkinleştir veya Başlangıç Gecikmesi:"));
      m.insert(QStringLiteral("ru"), QStringLiteral("③ Включить клавишу или задать задержку старта:"));
      m.insert(QStringLiteral("id"), QStringLiteral("③ Aktifkan Pintasan atau Atur Jeda Mulai:"));
      m.insert(QStringLiteral("fa"), QStringLiteral("③ فعال‌سازی میانبر یا تنظیم تأخیر شروع دستی:"));
      m.insert(QStringLiteral("ur"), QStringLiteral("③ شارٹ کٹ فعال کریں یا دستی چلانے کا وقفہ طے کریں:"));
    }
    { auto& m = m_translations[QStringLiteral("lbl_run_after")];
      m.insert(QStringLiteral("ar"), QStringLiteral("مهلة البدء:"));
      m.insert(QStringLiteral("en"), QStringLiteral("Start delay:"));
      m.insert(QStringLiteral("es"), QStringLiteral("Retraso inicial:"));
      m.insert(QStringLiteral("fr"), QStringLiteral("Délai de départ :"));
      m.insert(QStringLiteral("de"), QStringLiteral("Startverzögerung:"));
      m.insert(QStringLiteral("tr"), QStringLiteral("Başlangıç gecikmesi:"));
      m.insert(QStringLiteral("ru"), QStringLiteral("Задержка старта:"));
      m.insert(QStringLiteral("id"), QStringLiteral("Jeda mulai:"));
      m.insert(QStringLiteral("fa"), QStringLiteral("تأخیر شروع:"));
      m.insert(QStringLiteral("ur"), QStringLiteral("شروع کا وقفہ:"));
    }
    { auto& m = m_translations[QStringLiteral("lbl_target_app")];
      m.insert(QStringLiteral("ar"), QStringLiteral("يعمل داخل:"));
      m.insert(QStringLiteral("en"), QStringLiteral("Active In:"));
      m.insert(QStringLiteral("es"), QStringLiteral("Activo en:"));
      m.insert(QStringLiteral("fr"), QStringLiteral("Actif dans :"));
      m.insert(QStringLiteral("de"), QStringLiteral("Aktiv in:"));
      m.insert(QStringLiteral("tr"), QStringLiteral("Çalışacağı Uygulama:"));
      m.insert(QStringLiteral("ru"), QStringLiteral("Активно в:"));
      m.insert(QStringLiteral("id"), QStringLiteral("Aktif di:"));
      m.insert(QStringLiteral("fa"), QStringLiteral("فعال در برنامه:"));
      m.insert(QStringLiteral("ur"), QStringLiteral("اندر کام کرے:"));
    }
    { auto& m = m_translations[QStringLiteral("lbl_combo_mode")];
      m.insert(QStringLiteral("ar"), QStringLiteral("النمط:"));
      m.insert(QStringLiteral("en"), QStringLiteral("Mode:"));
      m.insert(QStringLiteral("es"), QStringLiteral("Modo:"));
      m.insert(QStringLiteral("fr"), QStringLiteral("Mode :"));
      m.insert(QStringLiteral("de"), QStringLiteral("Modus:"));
      m.insert(QStringLiteral("tr"), QStringLiteral("Mod:"));
      m.insert(QStringLiteral("ru"), QStringLiteral("Режим:"));
      m.insert(QStringLiteral("id"), QStringLiteral("Mode:"));
      m.insert(QStringLiteral("fa"), QStringLiteral("حالت اجرا:"));
      m.insert(QStringLiteral("ur"), QStringLiteral("موڈ:"));
    }
    { auto& m = m_translations[QStringLiteral("lbl_repeat_mode")];
      m.insert(QStringLiteral("ar"), QStringLiteral("النمط:"));
      m.insert(QStringLiteral("en"), QStringLiteral("Mode:"));
      m.insert(QStringLiteral("es"), QStringLiteral("Modo:"));
      m.insert(QStringLiteral("fr"), QStringLiteral("Mode :"));
      m.insert(QStringLiteral("de"), QStringLiteral("Modus:"));
      m.insert(QStringLiteral("tr"), QStringLiteral("Mod:"));
      m.insert(QStringLiteral("ru"), QStringLiteral("Режим:"));
      m.insert(QStringLiteral("id"), QStringLiteral("Mode:"));
      m.insert(QStringLiteral("fa"), QStringLiteral("حالت اجرا:"));
      m.insert(QStringLiteral("ur"), QStringLiteral("موڈ:"));
    }
    { auto& m = m_translations[QStringLiteral("mode_once")];
      m.insert(QStringLiteral("ar"), QStringLiteral("مرة واحدة"));
      m.insert(QStringLiteral("en"), QStringLiteral("Once"));
      m.insert(QStringLiteral("es"), QStringLiteral("Una vez"));
      m.insert(QStringLiteral("fr"), QStringLiteral("Une fois"));
      m.insert(QStringLiteral("de"), QStringLiteral("Einmal"));
      m.insert(QStringLiteral("tr"), QStringLiteral("Bir Kez"));
      m.insert(QStringLiteral("ru"), QStringLiteral("Один раз"));
      m.insert(QStringLiteral("id"), QStringLiteral("Sekali"));
      m.insert(QStringLiteral("fa"), QStringLiteral("یک بار"));
      m.insert(QStringLiteral("ur"), QStringLiteral("ایک بار"));
    }
    { auto& m = m_translations[QStringLiteral("mode_hold")];
      m.insert(QStringLiteral("ar"), QStringLiteral("كرر مع الضغط"));
      m.insert(QStringLiteral("en"), QStringLiteral("Hold"));
      m.insert(QStringLiteral("es"), QStringLiteral("Mantener"));
      m.insert(QStringLiteral("fr"), QStringLiteral("Maintenir"));
      m.insert(QStringLiteral("de"), QStringLiteral("Halten"));
      m.insert(QStringLiteral("tr"), QStringLiteral("Basılı Tut"));
      m.insert(QStringLiteral("ru"), QStringLiteral("Удержание"));
      m.insert(QStringLiteral("id"), QStringLiteral("Tahan"));
      m.insert(QStringLiteral("fa"), QStringLiteral("تکرار با نگه‌داشتن"));
      m.insert(QStringLiteral("ur"), QStringLiteral("دبانے تک تکرار"));
    }
    { auto& m = m_translations[QStringLiteral("mode_repeat_hold")];
      m.insert(QStringLiteral("ar"), QStringLiteral("كرر مع الضغط"));
      m.insert(QStringLiteral("en"), QStringLiteral("Hold"));
      m.insert(QStringLiteral("es"), QStringLiteral("Mantener"));
      m.insert(QStringLiteral("fr"), QStringLiteral("Maintenir"));
      m.insert(QStringLiteral("de"), QStringLiteral("Halten"));
      m.insert(QStringLiteral("tr"), QStringLiteral("Basılı Tut"));
      m.insert(QStringLiteral("ru"), QStringLiteral("Удержание"));
      m.insert(QStringLiteral("id"), QStringLiteral("Tahan"));
      m.insert(QStringLiteral("fa"), QStringLiteral("تکرار با نگه‌داشتن"));
      m.insert(QStringLiteral("ur"), QStringLiteral("دبانے تک تکرار"));
    }
    { auto& m = m_translations[QStringLiteral("mode_toggle")];
      m.insert(QStringLiteral("ar"), QStringLiteral("تكرار مستمر"));
      m.insert(QStringLiteral("en"), QStringLiteral("Toggle Loop"));
      m.insert(QStringLiteral("es"), QStringLiteral("Bucle"));
      m.insert(QStringLiteral("fr"), QStringLiteral("Boucle"));
      m.insert(QStringLiteral("de"), QStringLiteral("Schleife"));
      m.insert(QStringLiteral("tr"), QStringLiteral("Sürekli Döngü"));
      m.insert(QStringLiteral("ru"), QStringLiteral("Цикл"));
      m.insert(QStringLiteral("id"), QStringLiteral("Loop"));
      m.insert(QStringLiteral("fa"), QStringLiteral("تکرار مداوم"));
      m.insert(QStringLiteral("ur"), QStringLiteral("مسلسل تکرار"));
    }
    { auto& m = m_translations[QStringLiteral("mode_toggle_loop")];
      m.insert(QStringLiteral("ar"), QStringLiteral("تكرار مستمر"));
      m.insert(QStringLiteral("en"), QStringLiteral("Toggle Loop"));
      m.insert(QStringLiteral("es"), QStringLiteral("Bucle"));
      m.insert(QStringLiteral("fr"), QStringLiteral("Boucle"));
      m.insert(QStringLiteral("de"), QStringLiteral("Schleife"));
      m.insert(QStringLiteral("tr"), QStringLiteral("Sürekli Döngü"));
      m.insert(QStringLiteral("ru"), QStringLiteral("Цикл"));
      m.insert(QStringLiteral("id"), QStringLiteral("Loop"));
      m.insert(QStringLiteral("fa"), QStringLiteral("تکرار مداوم"));
      m.insert(QStringLiteral("ur"), QStringLiteral("مسلسل تکرار"));
    }
    { auto& m = m_translations[QStringLiteral("btn_delete")];
      m.insert(QStringLiteral("ar"), QStringLiteral("حذف الماكرو"));
      m.insert(QStringLiteral("en"), QStringLiteral("Delete Macro"));
      m.insert(QStringLiteral("es"), QStringLiteral("Eliminar Macro"));
      m.insert(QStringLiteral("fr"), QStringLiteral("Supprimer Macro"));
      m.insert(QStringLiteral("de"), QStringLiteral("Makro löschen"));
      m.insert(QStringLiteral("tr"), QStringLiteral("Makroyu Sil"));
      m.insert(QStringLiteral("ru"), QStringLiteral("Удалить макрос"));
      m.insert(QStringLiteral("id"), QStringLiteral("Hapus Makro"));
      m.insert(QStringLiteral("fa"), QStringLiteral("حذف ماکرو"));
      m.insert(QStringLiteral("ur"), QStringLiteral("میکرو حذف کریں"));
    }
    { auto& m = m_translations[QStringLiteral("btn_delete_combo")];
      m.insert(QStringLiteral("ar"), QStringLiteral("حذف الماكرو"));
      m.insert(QStringLiteral("en"), QStringLiteral("Delete Macro"));
      m.insert(QStringLiteral("es"), QStringLiteral("Eliminar Macro"));
      m.insert(QStringLiteral("fr"), QStringLiteral("Supprimer Macro"));
      m.insert(QStringLiteral("de"), QStringLiteral("Makro löschen"));
      m.insert(QStringLiteral("tr"), QStringLiteral("Makroyu Sil"));
      m.insert(QStringLiteral("ru"), QStringLiteral("Удалить макрос"));
      m.insert(QStringLiteral("id"), QStringLiteral("Hapus Makro"));
      m.insert(QStringLiteral("fa"), QStringLiteral("حذف ماکرو"));
      m.insert(QStringLiteral("ur"), QStringLiteral("میکرو حذف کریں"));
    }
    { auto& m = m_translations[QStringLiteral("btn_delete_rec")];
      m.insert(QStringLiteral("ar"), QStringLiteral("حذف التسجيل"));
      m.insert(QStringLiteral("en"), QStringLiteral("Delete Record"));
      m.insert(QStringLiteral("es"), QStringLiteral("Eliminar Grabación"));
      m.insert(QStringLiteral("fr"), QStringLiteral("Supprimer"));
      m.insert(QStringLiteral("de"), QStringLiteral("Aufnahme löschen"));
      m.insert(QStringLiteral("tr"), QStringLiteral("Kaydı Sil"));
      m.insert(QStringLiteral("ru"), QStringLiteral("Удалить запись"));
      m.insert(QStringLiteral("id"), QStringLiteral("Hapus Rekaman"));
      m.insert(QStringLiteral("fa"), QStringLiteral("حذف ضبط"));
      m.insert(QStringLiteral("ur"), QStringLiteral("ریکارڈنگ حذف کریں"));
    }
    { auto& m = m_translations[QStringLiteral("btn_undo_delete")];
      m.insert(QStringLiteral("ar"), QStringLiteral("تراجع"));
      m.insert(QStringLiteral("en"), QStringLiteral("Undo"));
      m.insert(QStringLiteral("es"), QStringLiteral("Deshacer"));
      m.insert(QStringLiteral("fr"), QStringLiteral("Annuler"));
      m.insert(QStringLiteral("de"), QStringLiteral("Rückgängig"));
      m.insert(QStringLiteral("tr"), QStringLiteral("Geri Al"));
      m.insert(QStringLiteral("ru"), QStringLiteral("Отменить"));
      m.insert(QStringLiteral("id"), QStringLiteral("Urungkan"));
      m.insert(QStringLiteral("fa"), QStringLiteral("واگرد"));
      m.insert(QStringLiteral("ur"), QStringLiteral("واپس لائیں"));
    }
    { auto& m = m_translations[QStringLiteral("press_any_key")];
      m.insert(QStringLiteral("ar"), QStringLiteral("اضغط 1 أو 2 أو 3 أزرار معاً... (Esc للإلغاء)"));
      m.insert(QStringLiteral("en"), QStringLiteral("Press 1, 2, or 3 keys together... (Esc to cancel)"));
      m.insert(QStringLiteral("es"), QStringLiteral("Presiona 1, 2 o 3 teclas... (Esc para cancelar)"));
      m.insert(QStringLiteral("fr"), QStringLiteral("Appuyez sur 1, 2 ou 3 touches... (Échap pour annuler)"));
      m.insert(QStringLiteral("de"), QStringLiteral("1, 2 oder 3 Tasten drücken... (Esc zum Abbrechen)"));
      m.insert(QStringLiteral("tr"), QStringLiteral("1, 2 veya 3 tuşa basın... (İptal için Esc)"));
      m.insert(QStringLiteral("ru"), QStringLiteral("Нажмите 1, 2 или 3 клавиши... (Esc для отмены)"));
      m.insert(QStringLiteral("id"), QStringLiteral("Tekan 1, 2, atau 3 tombol... (Esc untuk batal)"));
      m.insert(QStringLiteral("fa"), QStringLiteral("۱، ۲ یا ۳ کلید را همزمان فشار دهید... (Esc برای لغو)"));
      m.insert(QStringLiteral("ur"), QStringLiteral("١، ٢ یا ٣ بٹن ایک ساتھ دبائیں... (منسوخ کرنے کے لیے Esc)"));
    }
    { auto& m = m_translations[QStringLiteral("rec_title")];
      m.insert(QStringLiteral("ar"), QStringLiteral("مسجل الماوس والكيبورد والتسجيلات المحفوظة"));
      m.insert(QStringLiteral("en"), QStringLiteral("Mouse & Keyboard Recorder & Saved Recordings"));
      m.insert(QStringLiteral("es"), QStringLiteral("Grabador de Ratón y Teclado y Grabaciones Guardadas"));
      m.insert(QStringLiteral("fr"), QStringLiteral("Enregistreur Souris & Clavier et Enregistrements"));
      m.insert(QStringLiteral("de"), QStringLiteral("Maus- & Tastatur-Recorder und Gespeicherte Aufnahmen"));
      m.insert(QStringLiteral("tr"), QStringLiteral("Fare ve Klavye Kaydedici ve Kayıtlar"));
      m.insert(QStringLiteral("ru"), QStringLiteral("Запись мыши и клавиатуры и сохраненные макросы"));
      m.insert(QStringLiteral("id"), QStringLiteral("Perekam Mouse & Keyboard dan Rekaman Tersimpan"));
      m.insert(QStringLiteral("fa"), QStringLiteral("ضبط‌کننده ماوس و کیبورد و ضبط‌های ذخیره‌شده"));
      m.insert(QStringLiteral("ur"), QStringLiteral("ماؤس اور کی بورڈ ریکارڈر اور محفوظ ریکارڈنگز"));
    }
    { auto& m = m_translations[QStringLiteral("rec_subtitle")];
      m.insert(QStringLiteral("ar"), QStringLiteral("اسحب أي خطوة بالماوس لتغيير مكانها فوراً، والتقط إحداثيات الماوس بـ F8، أو حوّل التسجيل إلى زرار ذكي متطابق ١٠٠٪."));
      m.insert(QStringLiteral("en"), QStringLiteral("Drag any step to reorder live, capture coordinates with F8, or convert 1-to-1 into a Smart Key."));
      m.insert(QStringLiteral("es"), QStringLiteral("Arrastra cualquier paso para reordenar, captura coordenadas con F8 o convierte 1:1 en Tecla Inteligente."));
      m.insert(QStringLiteral("fr"), QStringLiteral("Glissez une étape pour réorganiser, capturez avec F8 ou convertissez 1:1 en Touche Intelligente."));
      m.insert(QStringLiteral("de"), QStringLiteral("Schritte per Maus sortieren, Koordinaten mit F8 erfassen oder 1:1 in Smart-Taste umwandeln."));
      m.insert(QStringLiteral("tr"), QStringLiteral("Adımları sürükleyerek sıralayın, F8 ile koordinat alın veya %100 Akıllı Tuşa dönüştürün."));
      m.insert(QStringLiteral("ru"), QStringLiteral("Перетаскивайте шаги мышью, захватывайте координаты по F8 или конвертируйте 1:1 в Умную клавишу."));
      m.insert(QStringLiteral("id"), QStringLiteral("Seret langkah untuk mengurutkan, ambil koordinat dengan F8, atau konversi 1:1 ke Tombol Cerdas."));
      m.insert(QStringLiteral("fa"), QStringLiteral("هر گام را با ماوس جابجا کنید، مختصات را با F8 بگیرید یا ۱۰۰٪ به کلید هوشمند تبدیل کنید."));
      m.insert(QStringLiteral("ur"), QStringLiteral("ماؤس سے مراحل ترتیب دیں، F8 سے جگہ منتخب کریں یا ١٠٠٪ سمارٹ بٹن میں تبدیل کریں۔"));
    }
    { auto& m = m_translations[QStringLiteral("btn_rec_start")];
      m.insert(QStringLiteral("ar"), QStringLiteral("ابدأ التسجيل"));
      m.insert(QStringLiteral("en"), QStringLiteral("Start Recording"));
      m.insert(QStringLiteral("es"), QStringLiteral("Grabar"));
      m.insert(QStringLiteral("fr"), QStringLiteral("Enregistrer"));
      m.insert(QStringLiteral("de"), QStringLiteral("Aufnahme"));
      m.insert(QStringLiteral("tr"), QStringLiteral("Kaydet"));
      m.insert(QStringLiteral("ru"), QStringLiteral("Запись"));
      m.insert(QStringLiteral("id"), QStringLiteral("Rekam"));
      m.insert(QStringLiteral("fa"), QStringLiteral("شروع ضبط"));
      m.insert(QStringLiteral("ur"), QStringLiteral("ریکارڈنگ شروع"));
    }
    { auto& m = m_translations[QStringLiteral("btn_start_rec")];
      m.insert(QStringLiteral("ar"), QStringLiteral("ابدأ التسجيل"));
      m.insert(QStringLiteral("en"), QStringLiteral("Start Recording"));
      m.insert(QStringLiteral("es"), QStringLiteral("Grabar"));
      m.insert(QStringLiteral("fr"), QStringLiteral("Enregistrer"));
      m.insert(QStringLiteral("de"), QStringLiteral("Aufnahme"));
      m.insert(QStringLiteral("tr"), QStringLiteral("Kaydet"));
      m.insert(QStringLiteral("ru"), QStringLiteral("Запись"));
      m.insert(QStringLiteral("id"), QStringLiteral("Rekam"));
      m.insert(QStringLiteral("fa"), QStringLiteral("شروع ضبط"));
      m.insert(QStringLiteral("ur"), QStringLiteral("ریکارڈنگ شروع"));
    }
    { auto& m = m_translations[QStringLiteral("btn_rec_stop")];
      m.insert(QStringLiteral("ar"), QStringLiteral("إيقاف التسجيل"));
      m.insert(QStringLiteral("en"), QStringLiteral("Stop Recording"));
      m.insert(QStringLiteral("es"), QStringLiteral("Detener"));
      m.insert(QStringLiteral("fr"), QStringLiteral("Arrêter"));
      m.insert(QStringLiteral("de"), QStringLiteral("Stopp"));
      m.insert(QStringLiteral("tr"), QStringLiteral("Durdur"));
      m.insert(QStringLiteral("ru"), QStringLiteral("Стоп"));
      m.insert(QStringLiteral("id"), QStringLiteral("Berhenti"));
      m.insert(QStringLiteral("fa"), QStringLiteral("توقف ضبط"));
      m.insert(QStringLiteral("ur"), QStringLiteral("ریکارڈنگ روکیں"));
    }
    { auto& m = m_translations[QStringLiteral("btn_stop_rec")];
      m.insert(QStringLiteral("ar"), QStringLiteral("إيقاف التسجيل"));
      m.insert(QStringLiteral("en"), QStringLiteral("Stop Recording"));
      m.insert(QStringLiteral("es"), QStringLiteral("Detener"));
      m.insert(QStringLiteral("fr"), QStringLiteral("Arrêter"));
      m.insert(QStringLiteral("de"), QStringLiteral("Stopp"));
      m.insert(QStringLiteral("tr"), QStringLiteral("Durdur"));
      m.insert(QStringLiteral("ru"), QStringLiteral("Стоп"));
      m.insert(QStringLiteral("id"), QStringLiteral("Berhenti"));
      m.insert(QStringLiteral("fa"), QStringLiteral("توقف ضبط"));
      m.insert(QStringLiteral("ur"), QStringLiteral("ریکارڈنگ روکیں"));
    }
    { auto& m = m_translations[QStringLiteral("btn_play_start")];
      m.insert(QStringLiteral("ar"), QStringLiteral("تشغيل"));
      m.insert(QStringLiteral("en"), QStringLiteral("Play"));
      m.insert(QStringLiteral("es"), QStringLiteral("Reproducir"));
      m.insert(QStringLiteral("fr"), QStringLiteral("Lire"));
      m.insert(QStringLiteral("de"), QStringLiteral("Abspielen"));
      m.insert(QStringLiteral("tr"), QStringLiteral("Oynat"));
      m.insert(QStringLiteral("ru"), QStringLiteral("Пуск"));
      m.insert(QStringLiteral("id"), QStringLiteral("Putar"));
      m.insert(QStringLiteral("fa"), QStringLiteral("اجرا"));
      m.insert(QStringLiteral("ur"), QStringLiteral("چلائیں"));
    }
    { auto& m = m_translations[QStringLiteral("btn_play_macro")];
      m.insert(QStringLiteral("ar"), QStringLiteral("تشغيل"));
      m.insert(QStringLiteral("en"), QStringLiteral("Play"));
      m.insert(QStringLiteral("es"), QStringLiteral("Reproducir"));
      m.insert(QStringLiteral("fr"), QStringLiteral("Lire"));
      m.insert(QStringLiteral("de"), QStringLiteral("Abspielen"));
      m.insert(QStringLiteral("tr"), QStringLiteral("Oynat"));
      m.insert(QStringLiteral("ru"), QStringLiteral("Пуск"));
      m.insert(QStringLiteral("id"), QStringLiteral("Putar"));
      m.insert(QStringLiteral("fa"), QStringLiteral("اجرا"));
      m.insert(QStringLiteral("ur"), QStringLiteral("چلائیں"));
    }
    { auto& m = m_translations[QStringLiteral("btn_play_stop")];
      m.insert(QStringLiteral("ar"), QStringLiteral("إيقاف التشغيل"));
      m.insert(QStringLiteral("en"), QStringLiteral("Stop Playback"));
      m.insert(QStringLiteral("es"), QStringLiteral("Detener"));
      m.insert(QStringLiteral("fr"), QStringLiteral("Arrêter"));
      m.insert(QStringLiteral("de"), QStringLiteral("Stopp"));
      m.insert(QStringLiteral("tr"), QStringLiteral("Durdur"));
      m.insert(QStringLiteral("ru"), QStringLiteral("Стоп"));
      m.insert(QStringLiteral("id"), QStringLiteral("Berhenti"));
      m.insert(QStringLiteral("fa"), QStringLiteral("توقف اجرا"));
      m.insert(QStringLiteral("ur"), QStringLiteral("چلنا روکیں"));
    }
    { auto& m = m_translations[QStringLiteral("btn_stop_play")];
      m.insert(QStringLiteral("ar"), QStringLiteral("إيقاف التشغيل"));
      m.insert(QStringLiteral("en"), QStringLiteral("Stop Playback"));
      m.insert(QStringLiteral("es"), QStringLiteral("Detener"));
      m.insert(QStringLiteral("fr"), QStringLiteral("Arrêter"));
      m.insert(QStringLiteral("de"), QStringLiteral("Stopp"));
      m.insert(QStringLiteral("tr"), QStringLiteral("Durdur"));
      m.insert(QStringLiteral("ru"), QStringLiteral("Стоп"));
      m.insert(QStringLiteral("id"), QStringLiteral("Berhenti"));
      m.insert(QStringLiteral("fa"), QStringLiteral("توقف اجرا"));
      m.insert(QStringLiteral("ur"), QStringLiteral("چلنا روکیں"));
    }
    { auto& m = m_translations[QStringLiteral("btn_clear")];
      m.insert(QStringLiteral("ar"), QStringLiteral("مسح القائمة"));
      m.insert(QStringLiteral("en"), QStringLiteral("Clear Steps"));
      m.insert(QStringLiteral("es"), QStringLiteral("Limpiar"));
      m.insert(QStringLiteral("fr"), QStringLiteral("Effacer"));
      m.insert(QStringLiteral("de"), QStringLiteral("Leeren"));
      m.insert(QStringLiteral("tr"), QStringLiteral("Temizle"));
      m.insert(QStringLiteral("ru"), QStringLiteral("Очистить"));
      m.insert(QStringLiteral("id"), QStringLiteral("Bersihkan"));
      m.insert(QStringLiteral("fa"), QStringLiteral("پاک کردن لیست"));
      m.insert(QStringLiteral("ur"), QStringLiteral("فہرست صاف کریں"));
    }
    { auto& m = m_translations[QStringLiteral("btn_clear_events")];
      m.insert(QStringLiteral("ar"), QStringLiteral("مسح القائمة"));
      m.insert(QStringLiteral("en"), QStringLiteral("Clear Steps"));
      m.insert(QStringLiteral("es"), QStringLiteral("Limpiar"));
      m.insert(QStringLiteral("fr"), QStringLiteral("Effacer"));
      m.insert(QStringLiteral("de"), QStringLiteral("Leeren"));
      m.insert(QStringLiteral("tr"), QStringLiteral("Temizle"));
      m.insert(QStringLiteral("ru"), QStringLiteral("Очистить"));
      m.insert(QStringLiteral("id"), QStringLiteral("Bersihkan"));
      m.insert(QStringLiteral("fa"), QStringLiteral("پاک کردن لیست"));
      m.insert(QStringLiteral("ur"), QStringLiteral("فہرست صاف کریں"));
    }
    { auto& m = m_translations[QStringLiteral("btn_to_combo")];
      m.insert(QStringLiteral("ar"), QStringLiteral("حوّل لماكرو ذكي"));
      m.insert(QStringLiteral("en"), QStringLiteral("Convert to Smart Key"));
      m.insert(QStringLiteral("es"), QStringLiteral("Convertir a Tecla"));
      m.insert(QStringLiteral("fr"), QStringLiteral("Convertir en Touche"));
      m.insert(QStringLiteral("de"), QStringLiteral("In Smart-Taste"));
      m.insert(QStringLiteral("tr"), QStringLiteral("Akıllı Tuşa Dönüştür"));
      m.insert(QStringLiteral("ru"), QStringLiteral("В умную клавишу"));
      m.insert(QStringLiteral("id"), QStringLiteral("Ke Tombol Cerdas"));
      m.insert(QStringLiteral("fa"), QStringLiteral("تبدیل به کلید هوشمند"));
      m.insert(QStringLiteral("ur"), QStringLiteral("سمارٹ بٹن میں بدلیں"));
    }
    { auto& m = m_translations[QStringLiteral("btn_convert_to_combo")];
      m.insert(QStringLiteral("ar"), QStringLiteral("حوّل لماكرو ذكي"));
      m.insert(QStringLiteral("en"), QStringLiteral("Convert to Smart Key"));
      m.insert(QStringLiteral("es"), QStringLiteral("Convertir a Tecla"));
      m.insert(QStringLiteral("fr"), QStringLiteral("Convertir en Touche"));
      m.insert(QStringLiteral("de"), QStringLiteral("In Smart-Taste"));
      m.insert(QStringLiteral("tr"), QStringLiteral("Akıllı Tuşa Dönüştür"));
      m.insert(QStringLiteral("ru"), QStringLiteral("В умную клавишу"));
      m.insert(QStringLiteral("id"), QStringLiteral("Ke Tombol Cerdas"));
      m.insert(QStringLiteral("fa"), QStringLiteral("تبدیل به کلید هوشمند"));
      m.insert(QStringLiteral("ur"), QStringLiteral("سمارٹ بٹن میں بدلیں"));
    }
    { auto& m = m_translations[QStringLiteral("lbl_rec_keyboard")];
      m.insert(QStringLiteral("ar"), QStringLiteral("الكيبورد"));
      m.insert(QStringLiteral("en"), QStringLiteral("Keyboard"));
      m.insert(QStringLiteral("es"), QStringLiteral("Teclado"));
      m.insert(QStringLiteral("fr"), QStringLiteral("Clavier"));
      m.insert(QStringLiteral("de"), QStringLiteral("Tastatur"));
      m.insert(QStringLiteral("tr"), QStringLiteral("Klavye"));
      m.insert(QStringLiteral("ru"), QStringLiteral("Клавиатура"));
      m.insert(QStringLiteral("id"), QStringLiteral("Keyboard"));
      m.insert(QStringLiteral("fa"), QStringLiteral("کیبورد"));
      m.insert(QStringLiteral("ur"), QStringLiteral("کی بورڈ"));
    }
    { auto& m = m_translations[QStringLiteral("lbl_rec_clicks")];
      m.insert(QStringLiteral("ar"), QStringLiteral("أزرار الماوس"));
      m.insert(QStringLiteral("en"), QStringLiteral("Mouse Clicks"));
      m.insert(QStringLiteral("es"), QStringLiteral("Clics"));
      m.insert(QStringLiteral("fr"), QStringLiteral("Clics"));
      m.insert(QStringLiteral("de"), QStringLiteral("Mausklicks"));
      m.insert(QStringLiteral("tr"), QStringLiteral("Tıklamalar"));
      m.insert(QStringLiteral("ru"), QStringLiteral("Клики"));
      m.insert(QStringLiteral("id"), QStringLiteral("Klik Mouse"));
      m.insert(QStringLiteral("fa"), QStringLiteral("دکمه‌های ماوس"));
      m.insert(QStringLiteral("ur"), QStringLiteral("ماؤس بٹن"));
    }
    { auto& m = m_translations[QStringLiteral("lbl_rec_scroll")];
      m.insert(QStringLiteral("ar"), QStringLiteral("البكرة (Scroll)"));
      m.insert(QStringLiteral("en"), QStringLiteral("Scroll Wheel"));
      m.insert(QStringLiteral("es"), QStringLiteral("Rueda"));
      m.insert(QStringLiteral("fr"), QStringLiteral("Molette"));
      m.insert(QStringLiteral("de"), QStringLiteral("Mausrad"));
      m.insert(QStringLiteral("tr"), QStringLiteral("Tekerlek"));
      m.insert(QStringLiteral("ru"), QStringLiteral("Колесо"));
      m.insert(QStringLiteral("id"), QStringLiteral("Scroll Wheel"));
      m.insert(QStringLiteral("fa"), QStringLiteral("اسکرول ماوس"));
      m.insert(QStringLiteral("ur"), QStringLiteral("ماؤس وہیل"));
    }
    { auto& m = m_translations[QStringLiteral("lbl_rec_moves")];
      m.insert(QStringLiteral("ar"), QStringLiteral("مسار الماوس"));
      m.insert(QStringLiteral("en"), QStringLiteral("Mouse Path"));
      m.insert(QStringLiteral("es"), QStringLiteral("Movimiento"));
      m.insert(QStringLiteral("fr"), QStringLiteral("Mouvement"));
      m.insert(QStringLiteral("de"), QStringLiteral("Mauspfad"));
      m.insert(QStringLiteral("tr"), QStringLiteral("Fare Hareketi"));
      m.insert(QStringLiteral("ru"), QStringLiteral("Движение"));
      m.insert(QStringLiteral("id"), QStringLiteral("Jalur Mouse"));
      m.insert(QStringLiteral("fa"), QStringLiteral("مسیر ماوس"));
      m.insert(QStringLiteral("ur"), QStringLiteral("ماؤس کا راستہ"));
    }
    { auto& m = m_translations[QStringLiteral("lbl_speed")];
      m.insert(QStringLiteral("ar"), QStringLiteral("سرعة التكرار:"));
      m.insert(QStringLiteral("en"), QStringLiteral("Repeat Speed:"));
      m.insert(QStringLiteral("es"), QStringLiteral("Velocidad:"));
      m.insert(QStringLiteral("fr"), QStringLiteral("Vitesse :"));
      m.insert(QStringLiteral("de"), QStringLiteral("Tempo:"));
      m.insert(QStringLiteral("tr"), QStringLiteral("Tekrar Hızı:"));
      m.insert(QStringLiteral("ru"), QStringLiteral("Скорость:"));
      m.insert(QStringLiteral("id"), QStringLiteral("Kecepatan:"));
      m.insert(QStringLiteral("fa"), QStringLiteral("سرعت تکرار:"));
      m.insert(QStringLiteral("ur"), QStringLiteral("تکرار کی رفتار:"));
    }
    { auto& m = m_translations[QStringLiteral("lbl_repeat_speed")];
      m.insert(QStringLiteral("ar"), QStringLiteral("سرعة التكرار:"));
      m.insert(QStringLiteral("en"), QStringLiteral("Repeat Speed:"));
      m.insert(QStringLiteral("es"), QStringLiteral("Velocidad:"));
      m.insert(QStringLiteral("fr"), QStringLiteral("Vitesse :"));
      m.insert(QStringLiteral("de"), QStringLiteral("Tempo:"));
      m.insert(QStringLiteral("tr"), QStringLiteral("Tekrar Hızı:"));
      m.insert(QStringLiteral("ru"), QStringLiteral("Скорость:"));
      m.insert(QStringLiteral("id"), QStringLiteral("Kecepatan:"));
      m.insert(QStringLiteral("fa"), QStringLiteral("سرعت تکرار:"));
      m.insert(QStringLiteral("ur"), QStringLiteral("تکرار کی رفتار:"));
    }
    { auto& m = m_translations[QStringLiteral("lbl_saved_list")];
      m.insert(QStringLiteral("ar"), QStringLiteral("التسجيلات المحفوظة"));
      m.insert(QStringLiteral("en"), QStringLiteral("Saved Recordings"));
      m.insert(QStringLiteral("es"), QStringLiteral("Grabaciones Guardadas"));
      m.insert(QStringLiteral("fr"), QStringLiteral("Enregistrements Sauvegardés"));
      m.insert(QStringLiteral("de"), QStringLiteral("Gespeicherte Aufnahmen"));
      m.insert(QStringLiteral("tr"), QStringLiteral("Kayıtlı Makrolar"));
      m.insert(QStringLiteral("ru"), QStringLiteral("Сохраненные записи"));
      m.insert(QStringLiteral("id"), QStringLiteral("Rekaman Tersimpan"));
      m.insert(QStringLiteral("fa"), QStringLiteral("ضبط‌های ذخیره‌شده"));
      m.insert(QStringLiteral("ur"), QStringLiteral("محفوظ ریکارڈنگز"));
    }
    { auto& m = m_translations[QStringLiteral("saved_recordings_header")];
      m.insert(QStringLiteral("ar"), QStringLiteral("التسجيلات المحفوظة"));
      m.insert(QStringLiteral("en"), QStringLiteral("Saved Recordings"));
      m.insert(QStringLiteral("es"), QStringLiteral("Grabaciones Guardadas"));
      m.insert(QStringLiteral("fr"), QStringLiteral("Enregistrements Sauvegardés"));
      m.insert(QStringLiteral("de"), QStringLiteral("Gespeicherte Aufnahmen"));
      m.insert(QStringLiteral("tr"), QStringLiteral("Kayıtlı Makrolar"));
      m.insert(QStringLiteral("ru"), QStringLiteral("Сохраненные записи"));
      m.insert(QStringLiteral("id"), QStringLiteral("Rekaman Tersimpan"));
      m.insert(QStringLiteral("fa"), QStringLiteral("ضبط‌های ذخیره‌شده"));
      m.insert(QStringLiteral("ur"), QStringLiteral("محفوظ ریکارڈنگز"));
    }
    { auto& m = m_translations[QStringLiteral("timeline_empty")];
      m.insert(QStringLiteral("ar"), QStringLiteral("لا توجد خطوات مسجلة حالياً. اضغط «ابدأ التسجيل» أو أضف خطوة بالأسفل."));
      m.insert(QStringLiteral("en"), QStringLiteral("No steps recorded yet. Click 'Start Recording' or add a step below."));
      m.insert(QStringLiteral("es"), QStringLiteral("No hay pasos grabados. Haz clic en 'Grabar' o añade un paso abajo."));
      m.insert(QStringLiteral("fr"), QStringLiteral("Aucune étape enregistrée. Cliquez sur 'Enregistrer' ou ajoutez une étape ci-dessous."));
      m.insert(QStringLiteral("de"), QStringLiteral("Noch keine Schritte aufgenommen. Klicken Sie auf 'Aufnahme' oder fügen Sie unten einen Schritt hinzu."));
      m.insert(QStringLiteral("tr"), QStringLiteral("Henüz adım kaydedilmedi. 'Kaydet'e tıklayın veya aşağıdan bir adım ekleyin."));
      m.insert(QStringLiteral("ru"), QStringLiteral("Шаги еще не записаны. Нажмите «Запись» или добавьте шаг ниже."));
      m.insert(QStringLiteral("id"), QStringLiteral("Belum ada langkah terekam. Klik 'Rekam' atau tambahkan langkah di bawah."));
      m.insert(QStringLiteral("fa"), QStringLiteral("هنوز گامی ضبط نشده است. روی «شروع ضبط» کلیک کنید یا از پایین گام اضافه کنید."));
      m.insert(QStringLiteral("ur"), QStringLiteral("ابھی کوئی مرحلہ ریکارڈ نہیں ہوا۔ 'ریکارڈنگ شروع' پر کلک کریں یا نیچے سے مرحلہ شامل کریں۔"));
    }
    { auto& m = m_translations[QStringLiteral("ph_macro_name")];
      m.insert(QStringLiteral("ar"), QStringLiteral("اسم التسجيل..."));
      m.insert(QStringLiteral("en"), QStringLiteral("Recording name..."));
      m.insert(QStringLiteral("es"), QStringLiteral("Nombre de grabación..."));
      m.insert(QStringLiteral("fr"), QStringLiteral("Nom de l'enregistrement..."));
      m.insert(QStringLiteral("de"), QStringLiteral("Aufnahmename..."));
      m.insert(QStringLiteral("tr"), QStringLiteral("Kayıt adı..."));
      m.insert(QStringLiteral("ru"), QStringLiteral("Имя записи..."));
      m.insert(QStringLiteral("id"), QStringLiteral("Nama rekaman..."));
      m.insert(QStringLiteral("fa"), QStringLiteral("نام ضبط..."));
      m.insert(QStringLiteral("ur"), QStringLiteral("ریکارڈنگ کا نام..."));
    }
    { auto& m = m_translations[QStringLiteral("btn_save_macro")];
      m.insert(QStringLiteral("ar"), QStringLiteral("حفظ التسجيل"));
      m.insert(QStringLiteral("en"), QStringLiteral("Save Recording"));
      m.insert(QStringLiteral("es"), QStringLiteral("Guardar"));
      m.insert(QStringLiteral("fr"), QStringLiteral("Enregistrer"));
      m.insert(QStringLiteral("de"), QStringLiteral("Speichern"));
      m.insert(QStringLiteral("tr"), QStringLiteral("Kaydet"));
      m.insert(QStringLiteral("ru"), QStringLiteral("Сохранить"));
      m.insert(QStringLiteral("id"), QStringLiteral("Simpan"));
      m.insert(QStringLiteral("fa"), QStringLiteral("ذخیره ضبط"));
      m.insert(QStringLiteral("ur"), QStringLiteral("ریکارڈنگ محفوظ کریں"));
    }
    { auto& m = m_translations[QStringLiteral("btn_load")];
      m.insert(QStringLiteral("ar"), QStringLiteral("تعديل"));
      m.insert(QStringLiteral("en"), QStringLiteral("Load"));
      m.insert(QStringLiteral("es"), QStringLiteral("Cargar"));
      m.insert(QStringLiteral("fr"), QStringLiteral("Charger"));
      m.insert(QStringLiteral("de"), QStringLiteral("Laden"));
      m.insert(QStringLiteral("tr"), QStringLiteral("Yükle"));
      m.insert(QStringLiteral("ru"), QStringLiteral("Открыть"));
      m.insert(QStringLiteral("id"), QStringLiteral("Muat"));
      m.insert(QStringLiteral("fa"), QStringLiteral("ویرایش"));
      m.insert(QStringLiteral("ur"), QStringLiteral("ترمیم"));
    }
    { auto& m = m_translations[QStringLiteral("btn_load_to_editor")];
      m.insert(QStringLiteral("ar"), QStringLiteral("تعديل"));
      m.insert(QStringLiteral("en"), QStringLiteral("Load"));
      m.insert(QStringLiteral("es"), QStringLiteral("Cargar"));
      m.insert(QStringLiteral("fr"), QStringLiteral("Charger"));
      m.insert(QStringLiteral("de"), QStringLiteral("Laden"));
      m.insert(QStringLiteral("tr"), QStringLiteral("Yükle"));
      m.insert(QStringLiteral("ru"), QStringLiteral("Открыть"));
      m.insert(QStringLiteral("id"), QStringLiteral("Muat"));
      m.insert(QStringLiteral("fa"), QStringLiteral("ویرایش"));
      m.insert(QStringLiteral("ur"), QStringLiteral("ترمیم"));
    }
    { auto& m = m_translations[QStringLiteral("btn_run_saved")];
      m.insert(QStringLiteral("ar"), QStringLiteral("تشغيل"));
      m.insert(QStringLiteral("en"), QStringLiteral("Play"));
      m.insert(QStringLiteral("es"), QStringLiteral("Ejecutar"));
      m.insert(QStringLiteral("fr"), QStringLiteral("Lire"));
      m.insert(QStringLiteral("de"), QStringLiteral("Start"));
      m.insert(QStringLiteral("tr"), QStringLiteral("Oynat"));
      m.insert(QStringLiteral("ru"), QStringLiteral("Пуск"));
      m.insert(QStringLiteral("id"), QStringLiteral("Putar"));
      m.insert(QStringLiteral("fa"), QStringLiteral("اجرا"));
      m.insert(QStringLiteral("ur"), QStringLiteral("چلائیں"));
    }
    { auto& m = m_translations[QStringLiteral("btn_bind_hotkey")];
      m.insert(QStringLiteral("ar"), QStringLiteral("تحديد اختصار"));
      m.insert(QStringLiteral("en"), QStringLiteral("Set Hotkey"));
      m.insert(QStringLiteral("es"), QStringLiteral("Asignar Tecla"));
      m.insert(QStringLiteral("fr"), QStringLiteral("Raccourci"));
      m.insert(QStringLiteral("de"), QStringLiteral("Hotkey"));
      m.insert(QStringLiteral("tr"), QStringLiteral("Kısayol Ata"));
      m.insert(QStringLiteral("ru"), QStringLiteral("Клавиша"));
      m.insert(QStringLiteral("id"), QStringLiteral("Atur Pintasan"));
      m.insert(QStringLiteral("fa"), QStringLiteral("تعیین میانبر"));
      m.insert(QStringLiteral("ur"), QStringLiteral("شارٹ کٹ منتخب کریں"));
    }
    { auto& m = m_translations[QStringLiteral("rep_mode_once")];
      m.insert(QStringLiteral("ar"), QStringLiteral("تشغيل مرة واحدة"));
      m.insert(QStringLiteral("en"), QStringLiteral("Repeat once"));
      m.insert(QStringLiteral("es"), QStringLiteral("Una sola vez"));
      m.insert(QStringLiteral("fr"), QStringLiteral("Une seule fois"));
      m.insert(QStringLiteral("de"), QStringLiteral("Einmal"));
      m.insert(QStringLiteral("tr"), QStringLiteral("Bir kez"));
      m.insert(QStringLiteral("ru"), QStringLiteral("Один раз"));
      m.insert(QStringLiteral("id"), QStringLiteral("Sekali"));
      m.insert(QStringLiteral("fa"), QStringLiteral("اجرای یک‌باره"));
      m.insert(QStringLiteral("ur"), QStringLiteral("ایک بار چلائیں"));
    }
    { auto& m = m_translations[QStringLiteral("rec_repeat_mode_once")];
      m.insert(QStringLiteral("ar"), QStringLiteral("تشغيل مرة واحدة"));
      m.insert(QStringLiteral("en"), QStringLiteral("Repeat once"));
      m.insert(QStringLiteral("es"), QStringLiteral("Una sola vez"));
      m.insert(QStringLiteral("fr"), QStringLiteral("Une seule fois"));
      m.insert(QStringLiteral("de"), QStringLiteral("Einmal"));
      m.insert(QStringLiteral("tr"), QStringLiteral("Bir kez"));
      m.insert(QStringLiteral("ru"), QStringLiteral("Один раз"));
      m.insert(QStringLiteral("id"), QStringLiteral("Sekali"));
      m.insert(QStringLiteral("fa"), QStringLiteral("اجرای یک‌باره"));
      m.insert(QStringLiteral("ur"), QStringLiteral("ایک بار چلائیں"));
    }
    { auto& m = m_translations[QStringLiteral("rep_mode_count")];
      m.insert(QStringLiteral("ar"), QStringLiteral("تكرار بعدد المرات"));
      m.insert(QStringLiteral("en"), QStringLiteral("Repeat count"));
      m.insert(QStringLiteral("es"), QStringLiteral("Por cantidad"));
      m.insert(QStringLiteral("fr"), QStringLiteral("Par nombre"));
      m.insert(QStringLiteral("de"), QStringLiteral("Nach Anzahl"));
      m.insert(QStringLiteral("tr"), QStringLiteral("Tekrar sayısı"));
      m.insert(QStringLiteral("ru"), QStringLiteral("По числу раз"));
      m.insert(QStringLiteral("id"), QStringLiteral("Jumlah ulang"));
      m.insert(QStringLiteral("fa"), QStringLiteral("تکرار به تعداد مشخص"));
      m.insert(QStringLiteral("ur"), QStringLiteral("تعداد کے مطابق تکرار"));
    }
    { auto& m = m_translations[QStringLiteral("rec_repeat_mode_count")];
      m.insert(QStringLiteral("ar"), QStringLiteral("تكرار بعدد المرات"));
      m.insert(QStringLiteral("en"), QStringLiteral("Repeat count"));
      m.insert(QStringLiteral("es"), QStringLiteral("Por cantidad"));
      m.insert(QStringLiteral("fr"), QStringLiteral("Par nombre"));
      m.insert(QStringLiteral("de"), QStringLiteral("Nach Anzahl"));
      m.insert(QStringLiteral("tr"), QStringLiteral("Tekrar sayısı"));
      m.insert(QStringLiteral("ru"), QStringLiteral("По числу раз"));
      m.insert(QStringLiteral("id"), QStringLiteral("Jumlah ulang"));
      m.insert(QStringLiteral("fa"), QStringLiteral("تکرار به تعداد مشخص"));
      m.insert(QStringLiteral("ur"), QStringLiteral("تعداد کے مطابق تکرار"));
    }
    { auto& m = m_translations[QStringLiteral("rep_mode_duration")];
      m.insert(QStringLiteral("ar"), QStringLiteral("تكرار لمدة زمنية"));
      m.insert(QStringLiteral("en"), QStringLiteral("Repeat for time"));
      m.insert(QStringLiteral("es"), QStringLiteral("Por tiempo"));
      m.insert(QStringLiteral("fr"), QStringLiteral("Par durée"));
      m.insert(QStringLiteral("de"), QStringLiteral("Nach Dauer"));
      m.insert(QStringLiteral("tr"), QStringLiteral("Süre boyunca"));
      m.insert(QStringLiteral("ru"), QStringLiteral("По времени"));
      m.insert(QStringLiteral("id"), QStringLiteral("Selama waktu"));
      m.insert(QStringLiteral("fa"), QStringLiteral("تکرار برای مدت زمانی"));
      m.insert(QStringLiteral("ur"), QStringLiteral("مخصوص وقت تک تکرار"));
    }
    { auto& m = m_translations[QStringLiteral("rec_repeat_mode_duration")];
      m.insert(QStringLiteral("ar"), QStringLiteral("تكرار لمدة زمنية"));
      m.insert(QStringLiteral("en"), QStringLiteral("Repeat for time"));
      m.insert(QStringLiteral("es"), QStringLiteral("Por tiempo"));
      m.insert(QStringLiteral("fr"), QStringLiteral("Par durée"));
      m.insert(QStringLiteral("de"), QStringLiteral("Nach Dauer"));
      m.insert(QStringLiteral("tr"), QStringLiteral("Süre boyunca"));
      m.insert(QStringLiteral("ru"), QStringLiteral("По времени"));
      m.insert(QStringLiteral("id"), QStringLiteral("Selama waktu"));
      m.insert(QStringLiteral("fa"), QStringLiteral("تکرار برای مدت زمانی"));
      m.insert(QStringLiteral("ur"), QStringLiteral("مخصوص وقت تک تکرار"));
    }
    { auto& m = m_translations[QStringLiteral("unit_times")];
      m.insert(QStringLiteral("ar"), QStringLiteral("مرة"));
      m.insert(QStringLiteral("en"), QStringLiteral("times"));
      m.insert(QStringLiteral("es"), QStringLiteral("veces"));
      m.insert(QStringLiteral("fr"), QStringLiteral("fois"));
      m.insert(QStringLiteral("de"), QStringLiteral("Mal"));
      m.insert(QStringLiteral("tr"), QStringLiteral("kez"));
      m.insert(QStringLiteral("ru"), QStringLiteral("раз"));
      m.insert(QStringLiteral("id"), QStringLiteral("kali"));
      m.insert(QStringLiteral("fa"), QStringLiteral("بار"));
      m.insert(QStringLiteral("ur"), QStringLiteral("بار"));
    }
    { auto& m = m_translations[QStringLiteral("unit_steps")];
      m.insert(QStringLiteral("ar"), QStringLiteral("خطوة"));
      m.insert(QStringLiteral("en"), QStringLiteral("steps"));
      m.insert(QStringLiteral("es"), QStringLiteral("pasos"));
      m.insert(QStringLiteral("fr"), QStringLiteral("étapes"));
      m.insert(QStringLiteral("de"), QStringLiteral("Schritte"));
      m.insert(QStringLiteral("tr"), QStringLiteral("adım"));
      m.insert(QStringLiteral("ru"), QStringLiteral("шагов"));
      m.insert(QStringLiteral("id"), QStringLiteral("langkah"));
      m.insert(QStringLiteral("fa"), QStringLiteral("گام"));
      m.insert(QStringLiteral("ur"), QStringLiteral("مراحل"));
    }
    { auto& m = m_translations[QStringLiteral("unit_hours")];
      m.insert(QStringLiteral("ar"), QStringLiteral("ساعات"));
      m.insert(QStringLiteral("en"), QStringLiteral("Hours"));
      m.insert(QStringLiteral("es"), QStringLiteral("Horas"));
      m.insert(QStringLiteral("fr"), QStringLiteral("Heures"));
      m.insert(QStringLiteral("de"), QStringLiteral("Stunden"));
      m.insert(QStringLiteral("tr"), QStringLiteral("Saat"));
      m.insert(QStringLiteral("ru"), QStringLiteral("Часы"));
      m.insert(QStringLiteral("id"), QStringLiteral("Jam"));
      m.insert(QStringLiteral("fa"), QStringLiteral("ساعت"));
      m.insert(QStringLiteral("ur"), QStringLiteral("گھنٹے"));
    }
    { auto& m = m_translations[QStringLiteral("unit_minutes")];
      m.insert(QStringLiteral("ar"), QStringLiteral("دقائق"));
      m.insert(QStringLiteral("en"), QStringLiteral("Minutes"));
      m.insert(QStringLiteral("es"), QStringLiteral("Minutos"));
      m.insert(QStringLiteral("fr"), QStringLiteral("Minutes"));
      m.insert(QStringLiteral("de"), QStringLiteral("Minuten"));
      m.insert(QStringLiteral("tr"), QStringLiteral("Dakika"));
      m.insert(QStringLiteral("ru"), QStringLiteral("Минуты"));
      m.insert(QStringLiteral("id"), QStringLiteral("Menit"));
      m.insert(QStringLiteral("fa"), QStringLiteral("دقیقه"));
      m.insert(QStringLiteral("ur"), QStringLiteral("منٹ"));
    }
    { auto& m = m_translations[QStringLiteral("unit_seconds")];
      m.insert(QStringLiteral("ar"), QStringLiteral("ثواني"));
      m.insert(QStringLiteral("en"), QStringLiteral("Seconds"));
      m.insert(QStringLiteral("es"), QStringLiteral("Segundos"));
      m.insert(QStringLiteral("fr"), QStringLiteral("Secondes"));
      m.insert(QStringLiteral("de"), QStringLiteral("Sekunden"));
      m.insert(QStringLiteral("tr"), QStringLiteral("Saniye"));
      m.insert(QStringLiteral("ru"), QStringLiteral("Секунды"));
      m.insert(QStringLiteral("id"), QStringLiteral("Detik"));
      m.insert(QStringLiteral("fa"), QStringLiteral("ثانیه"));
      m.insert(QStringLiteral("ur"), QStringLiteral("سیکنڈ"));
    }
    { auto& m = m_translations[QStringLiteral("lbl_wait_between_repeats")];
      m.insert(QStringLiteral("ar"), QStringLiteral("فاصل زمني بين كل تكرارين:"));
      m.insert(QStringLiteral("en"), QStringLiteral("Set delay time between two repeats:"));
      m.insert(QStringLiteral("es"), QStringLiteral("Retraso entre repeticiones:"));
      m.insert(QStringLiteral("fr"), QStringLiteral("Délai entre deux répétitions :"));
      m.insert(QStringLiteral("de"), QStringLiteral("Pause zwischen Wiederholungen:"));
      m.insert(QStringLiteral("tr"), QStringLiteral("İki tekrar arası bekleme:"));
      m.insert(QStringLiteral("ru"), QStringLiteral("Пауза между повторами:"));
      m.insert(QStringLiteral("id"), QStringLiteral("Jeda antar pengulangan:"));
      m.insert(QStringLiteral("fa"), QStringLiteral("فاصله زمانی بین هر دو تکرار:"));
      m.insert(QStringLiteral("ur"), QStringLiteral("دو تکرار کے درمیان وقفہ:"));
    }
    { auto& m = m_translations[QStringLiteral("lbl_set_delay_between")];
      m.insert(QStringLiteral("ar"), QStringLiteral("فاصل زمني بين كل تكرارين:"));
      m.insert(QStringLiteral("en"), QStringLiteral("Set delay time between two repeats:"));
      m.insert(QStringLiteral("es"), QStringLiteral("Retraso entre repeticiones:"));
      m.insert(QStringLiteral("fr"), QStringLiteral("Délai entre deux répétitions :"));
      m.insert(QStringLiteral("de"), QStringLiteral("Pause zwischen Wiederholungen:"));
      m.insert(QStringLiteral("tr"), QStringLiteral("İki tekrar arası bekleme:"));
      m.insert(QStringLiteral("ru"), QStringLiteral("Пауза между повторами:"));
      m.insert(QStringLiteral("id"), QStringLiteral("Jeda antar pengulangan:"));
      m.insert(QStringLiteral("fa"), QStringLiteral("فاصله زمانی بین هر دو تکرار:"));
      m.insert(QStringLiteral("ur"), QStringLiteral("دو تکرار کے درمیان وقفہ:"));
    }
    { auto& m = m_translations[QStringLiteral("lbl_random_delay")];
      m.insert(QStringLiteral("ar"), QStringLiteral("تأخير عشوائي (من 0 إلى الوقت المحدد)"));
      m.insert(QStringLiteral("en"), QStringLiteral("Random delay (0 to above time)"));
      m.insert(QStringLiteral("es"), QStringLiteral("Tiempo aleatorio (0 al valor anterior)"));
      m.insert(QStringLiteral("fr"), QStringLiteral("Délai aléatoire (0 à la valeur ci-dessus)"));
      m.insert(QStringLiteral("de"), QStringLiteral("Zufällige Pause (0 bis obige Zeit)"));
      m.insert(QStringLiteral("tr"), QStringLiteral("Rastgele gecikme (0 ile süre arası)"));
      m.insert(QStringLiteral("ru"), QStringLiteral("Случайная задержка (от 0 до указанного)"));
      m.insert(QStringLiteral("id"), QStringLiteral("Jeda acak (0 hingga waktu di atas)"));
      m.insert(QStringLiteral("fa"), QStringLiteral("تأخیر تصادفی (از ۰ تا زمان مشخص‌شده)"));
      m.insert(QStringLiteral("ur"), QStringLiteral("بے ترتیب وقفہ (0 سے مقررہ وقت تک)"));
    }
    { auto& m = m_translations[QStringLiteral("lbl_delay_random")];
      m.insert(QStringLiteral("ar"), QStringLiteral("تأخير عشوائي (من 0 إلى الوقت المحدد)"));
      m.insert(QStringLiteral("en"), QStringLiteral("Random delay (0 to above time)"));
      m.insert(QStringLiteral("es"), QStringLiteral("Tiempo aleatorio (0 al valor anterior)"));
      m.insert(QStringLiteral("fr"), QStringLiteral("Délai aléatoire (0 à la valeur ci-dessus)"));
      m.insert(QStringLiteral("de"), QStringLiteral("Zufällige Pause (0 bis obige Zeit)"));
      m.insert(QStringLiteral("tr"), QStringLiteral("Rastgele gecikme (0 ile süre arası)"));
      m.insert(QStringLiteral("ru"), QStringLiteral("Случайная задержка (от 0 до указанного)"));
      m.insert(QStringLiteral("id"), QStringLiteral("Jeda acak (0 hingga waktu di atas)"));
      m.insert(QStringLiteral("fa"), QStringLiteral("تأخیر تصادفی (از ۰ تا زمان مشخص‌شده)"));
      m.insert(QStringLiteral("ur"), QStringLiteral("بے ترتیب وقفہ (0 سے مقررہ وقت تک)"));
    }
    { auto& m = m_translations[QStringLiteral("lbl_do_not_activate")];
      m.insert(QStringLiteral("ar"), QStringLiteral("عدم تنشيط نافذة البرنامج عند انتهاء التكرار"));
      m.insert(QStringLiteral("en"), QStringLiteral("Do not activate this window when repeating stopped"));
      m.insert(QStringLiteral("es"), QStringLiteral("No activar esta ventana al finalizar"));
      m.insert(QStringLiteral("fr"), QStringLiteral("Ne pas activer la fenêtre à l'arrêt"));
      m.insert(QStringLiteral("de"), QStringLiteral("Fenster nach Ende nicht aktivieren"));
      m.insert(QStringLiteral("tr"), QStringLiteral("Tekrar durduğunda pencereyi etkinleştirme"));
      m.insert(QStringLiteral("ru"), QStringLiteral("Не активировать окно после остановки"));
      m.insert(QStringLiteral("id"), QStringLiteral("Jangan aktifkan jendela saat selesai"));
      m.insert(QStringLiteral("fa"), QStringLiteral("عدم فعال‌سازی پنجره برنامه پس از پایان تکرار"));
      m.insert(QStringLiteral("ur"), QStringLiteral("تکرار ختم ہونے پر ونڈو کو سامنے نہ لائیں"));
    }
    { auto& m = m_translations[QStringLiteral("lbl_shutdown_when_finished")];
      m.insert(QStringLiteral("ar"), QStringLiteral("إيقاف تشغيل الكمبيوتر عند انتهاء التكرار"));
      m.insert(QStringLiteral("en"), QStringLiteral("Shutdown computer when repeating finished"));
      m.insert(QStringLiteral("es"), QStringLiteral("Apagar el equipo al finalizar"));
      m.insert(QStringLiteral("fr"), QStringLiteral("Éteindre l'ordinateur à la fin"));
      m.insert(QStringLiteral("de"), QStringLiteral("PC nach Abschluss herunterfahren"));
      m.insert(QStringLiteral("tr"), QStringLiteral("Tekrar bittiğinde bilgisayarı kapat"));
      m.insert(QStringLiteral("ru"), QStringLiteral("Выключить ПК после завершения"));
      m.insert(QStringLiteral("id"), QStringLiteral("Matikan komputer saat selesai"));
      m.insert(QStringLiteral("fa"), QStringLiteral("خاموش کردن کامپیوتر پس از پایان تکرار"));
      m.insert(QStringLiteral("ur"), QStringLiteral("تکرار مکمل ہونے پر کمپیوٹر بند کریں"));
    }
    { auto& m = m_translations[QStringLiteral("lbl_stop_play_hotkey_inline")];
      m.insert(QStringLiteral("ar"), QStringLiteral("زرار إيقاف التشغيل الفوري:"));
      m.insert(QStringLiteral("en"), QStringLiteral("Stop Playback Hotkey:"));
      m.insert(QStringLiteral("es"), QStringLiteral("Tecla para detener:"));
      m.insert(QStringLiteral("fr"), QStringLiteral("Touche d'arrêt :"));
      m.insert(QStringLiteral("de"), QStringLiteral("Stopp-Hotkey:"));
      m.insert(QStringLiteral("tr"), QStringLiteral("Oynatmayı Durdurma Tuşu:"));
      m.insert(QStringLiteral("ru"), QStringLiteral("Клавиша остановки:"));
      m.insert(QStringLiteral("id"), QStringLiteral("Tombol Hentikan:"));
      m.insert(QStringLiteral("fa"), QStringLiteral("کلید توقف فوری اجرا:"));
      m.insert(QStringLiteral("ur"), QStringLiteral("چلنا روکنے کا بٹن:"));
    }
    { auto& m = m_translations[QStringLiteral("lbl_hk_stop_play")];
      m.insert(QStringLiteral("ar"), QStringLiteral("اختصار الإيقاف الفوري للتشغيل والماكرو"));
      m.insert(QStringLiteral("en"), QStringLiteral("Emergency Stop Playback & Macro Shortcut"));
      m.insert(QStringLiteral("es"), QStringLiteral("Atajo de parada inmediata"));
      m.insert(QStringLiteral("fr"), QStringLiteral("Raccourci d'arrêt d'urgence"));
      m.insert(QStringLiteral("de"), QStringLiteral("Sofort-Stopp-Hotkey"));
      m.insert(QStringLiteral("tr"), QStringLiteral("Acil Durdurma Kısayolu"));
      m.insert(QStringLiteral("ru"), QStringLiteral("Горячая клавиша экстренной остановки"));
      m.insert(QStringLiteral("id"), QStringLiteral("Pintasan Berhenti Darurat"));
      m.insert(QStringLiteral("fa"), QStringLiteral("میانبر توقف فوری اجرا و ماکرو"));
      m.insert(QStringLiteral("ur"), QStringLiteral("چلنا اور میکرو فوری روکنے کا شارٹ کٹ"));
    }
    { auto& m = m_translations[QStringLiteral("btn_export_all")];
      m.insert(QStringLiteral("ar"), QStringLiteral("تصدير الكل"));
      m.insert(QStringLiteral("en"), QStringLiteral("Export All"));
      m.insert(QStringLiteral("es"), QStringLiteral("Exportar Todo"));
      m.insert(QStringLiteral("fr"), QStringLiteral("Tout exporter"));
      m.insert(QStringLiteral("de"), QStringLiteral("Alles exportieren"));
      m.insert(QStringLiteral("tr"), QStringLiteral("Tümünü Dışa Aktar"));
      m.insert(QStringLiteral("ru"), QStringLiteral("Экспорт всего"));
      m.insert(QStringLiteral("id"), QStringLiteral("Ekspor Semua"));
      m.insert(QStringLiteral("fa"), QStringLiteral("خروجی گرفتن از همه"));
      m.insert(QStringLiteral("ur"), QStringLiteral("سب ایکسپورٹ کریں"));
    }
    { auto& m = m_translations[QStringLiteral("btn_import_all")];
      m.insert(QStringLiteral("ar"), QStringLiteral("استيراد نسخة"));
      m.insert(QStringLiteral("en"), QStringLiteral("Import Backup"));
      m.insert(QStringLiteral("es"), QStringLiteral("Importar Copia"));
      m.insert(QStringLiteral("fr"), QStringLiteral("Importer"));
      m.insert(QStringLiteral("de"), QStringLiteral("Backup importieren"));
      m.insert(QStringLiteral("tr"), QStringLiteral("Yedeği İçe Aktar"));
      m.insert(QStringLiteral("ru"), QStringLiteral("Импорт"));
      m.insert(QStringLiteral("id"), QStringLiteral("Impor Cadangan"));
      m.insert(QStringLiteral("fa"), QStringLiteral("وارد کردن نسخه پشتیبان"));
      m.insert(QStringLiteral("ur"), QStringLiteral("بیک اپ امپورٹ کریں"));
    }
    { auto& m = m_translations[QStringLiteral("btn_export_json")];
      m.insert(QStringLiteral("ar"), QStringLiteral("تصدير JSON"));
      m.insert(QStringLiteral("en"), QStringLiteral("Export JSON"));
      m.insert(QStringLiteral("es"), QStringLiteral("Exportar JSON"));
      m.insert(QStringLiteral("fr"), QStringLiteral("Exporter JSON"));
      m.insert(QStringLiteral("de"), QStringLiteral("JSON exportieren"));
      m.insert(QStringLiteral("tr"), QStringLiteral("JSON Dışa Aktar"));
      m.insert(QStringLiteral("ru"), QStringLiteral("Экспорт JSON"));
      m.insert(QStringLiteral("id"), QStringLiteral("Ekspor JSON"));
      m.insert(QStringLiteral("fa"), QStringLiteral("تصدير JSON"));
      m.insert(QStringLiteral("ur"), QStringLiteral("تصدير JSON"));
    }
    { auto& m = m_translations[QStringLiteral("btn_import_json")];
      m.insert(QStringLiteral("ar"), QStringLiteral("استيراد JSON"));
      m.insert(QStringLiteral("en"), QStringLiteral("Import JSON"));
      m.insert(QStringLiteral("es"), QStringLiteral("Importar JSON"));
      m.insert(QStringLiteral("fr"), QStringLiteral("Importer JSON"));
      m.insert(QStringLiteral("de"), QStringLiteral("JSON importieren"));
      m.insert(QStringLiteral("tr"), QStringLiteral("JSON İçe Aktar"));
      m.insert(QStringLiteral("ru"), QStringLiteral("Импорт JSON"));
      m.insert(QStringLiteral("id"), QStringLiteral("Impor JSON"));
      m.insert(QStringLiteral("fa"), QStringLiteral("استيراد JSON"));
      m.insert(QStringLiteral("ur"), QStringLiteral("استيراد JSON"));
    }
    { auto& m = m_translations[QStringLiteral("btn_custom_color")];
      m.insert(QStringLiteral("ar"), QStringLiteral("لون مخصص"));
      m.insert(QStringLiteral("en"), QStringLiteral("Custom Color"));
      m.insert(QStringLiteral("es"), QStringLiteral("Color personalizado"));
      m.insert(QStringLiteral("fr"), QStringLiteral("Couleur personnalisée"));
      m.insert(QStringLiteral("de"), QStringLiteral("Eigene Farbe"));
      m.insert(QStringLiteral("tr"), QStringLiteral("Özel Renk"));
      m.insert(QStringLiteral("ru"), QStringLiteral("Свой цвет"));
      m.insert(QStringLiteral("id"), QStringLiteral("Warna Kustom"));
      m.insert(QStringLiteral("fa"), QStringLiteral("لون مخصص"));
      m.insert(QStringLiteral("ur"), QStringLiteral("لون مخصص"));
    }
    { auto& m = m_translations[QStringLiteral("mode_auto")];
      m.insert(QStringLiteral("ar"), QStringLiteral("تلقائي"));
      m.insert(QStringLiteral("en"), QStringLiteral("Auto"));
      m.insert(QStringLiteral("es"), QStringLiteral("Auto"));
      m.insert(QStringLiteral("fr"), QStringLiteral("Auto"));
      m.insert(QStringLiteral("de"), QStringLiteral("Auto"));
      m.insert(QStringLiteral("tr"), QStringLiteral("Otomatik"));
      m.insert(QStringLiteral("ru"), QStringLiteral("Авто"));
      m.insert(QStringLiteral("id"), QStringLiteral("Otomatis"));
      m.insert(QStringLiteral("fa"), QStringLiteral("تلقائي"));
      m.insert(QStringLiteral("ur"), QStringLiteral("تلقائي"));
    }
    { auto& m = m_translations[QStringLiteral("themes_title")];
      m.insert(QStringLiteral("ar"), QStringLiteral("المظهر والألوان"));
      m.insert(QStringLiteral("en"), QStringLiteral("Themes & Colors"));
      m.insert(QStringLiteral("es"), QStringLiteral("Themes & Colors"));
      m.insert(QStringLiteral("fr"), QStringLiteral("Themes & Colors"));
      m.insert(QStringLiteral("de"), QStringLiteral("Themes & Colors"));
      m.insert(QStringLiteral("tr"), QStringLiteral("Themes & Colors"));
      m.insert(QStringLiteral("ru"), QStringLiteral("Themes & Colors"));
      m.insert(QStringLiteral("id"), QStringLiteral("Themes & Colors"));
      m.insert(QStringLiteral("fa"), QStringLiteral("المظهر والألوان"));
      m.insert(QStringLiteral("ur"), QStringLiteral("المظهر والألوان"));
    }
    { auto& m = m_translations[QStringLiteral("themes_subtitle")];
      m.insert(QStringLiteral("ar"), QStringLiteral("اختر المظهر الداكن أو الفاتح واللون المميز"));
      m.insert(QStringLiteral("en"), QStringLiteral("Customize dark/light mode and accent palette"));
      m.insert(QStringLiteral("es"), QStringLiteral("Customize dark/light mode and accent palette"));
      m.insert(QStringLiteral("fr"), QStringLiteral("Customize dark/light mode and accent palette"));
      m.insert(QStringLiteral("de"), QStringLiteral("Customize dark/light mode and accent palette"));
      m.insert(QStringLiteral("tr"), QStringLiteral("Customize dark/light mode and accent palette"));
      m.insert(QStringLiteral("ru"), QStringLiteral("Customize dark/light mode and accent palette"));
      m.insert(QStringLiteral("id"), QStringLiteral("Customize dark/light mode and accent palette"));
      m.insert(QStringLiteral("fa"), QStringLiteral("اختر المظهر الداكن أو الفاتح واللون المميز"));
      m.insert(QStringLiteral("ur"), QStringLiteral("اختر المظهر الداكن أو الفاتح واللون المميز"));
    }
    { auto& m = m_translations[QStringLiteral("lib_title")];
      m.insert(QStringLiteral("ar"), QStringLiteral("مكتبة التسجيلات"));
      m.insert(QStringLiteral("en"), QStringLiteral("Recordings Library"));
      m.insert(QStringLiteral("es"), QStringLiteral("Recordings Library"));
      m.insert(QStringLiteral("fr"), QStringLiteral("Recordings Library"));
      m.insert(QStringLiteral("de"), QStringLiteral("Recordings Library"));
      m.insert(QStringLiteral("tr"), QStringLiteral("Recordings Library"));
      m.insert(QStringLiteral("ru"), QStringLiteral("Recordings Library"));
      m.insert(QStringLiteral("id"), QStringLiteral("Recordings Library"));
      m.insert(QStringLiteral("fa"), QStringLiteral("مكتبة التسجيلات"));
      m.insert(QStringLiteral("ur"), QStringLiteral("مكتبة التسجيلات"));
    }
    { auto& m = m_translations[QStringLiteral("lib_subtitle")];
      m.insert(QStringLiteral("ar"), QStringLiteral("إدارة وتشغيل التسجيلات المحفوظة"));
      m.insert(QStringLiteral("en"), QStringLiteral("Manage and run saved recordings"));
      m.insert(QStringLiteral("es"), QStringLiteral("Manage and run saved recordings"));
      m.insert(QStringLiteral("fr"), QStringLiteral("Manage and run saved recordings"));
      m.insert(QStringLiteral("de"), QStringLiteral("Manage and run saved recordings"));
      m.insert(QStringLiteral("tr"), QStringLiteral("Manage and run saved recordings"));
      m.insert(QStringLiteral("ru"), QStringLiteral("Manage and run saved recordings"));
      m.insert(QStringLiteral("id"), QStringLiteral("Manage and run saved recordings"));
      m.insert(QStringLiteral("fa"), QStringLiteral("إدارة وتشغيل التسجيلات المحفوظة"));
      m.insert(QStringLiteral("ur"), QStringLiteral("إدارة وتشغيل التسجيلات المحفوظة"));
    }
    { auto& m = m_translations[QStringLiteral("dash_welcome")];
      m.insert(QStringLiteral("ar"), QStringLiteral("مرحباً بك في V-MacroPlus"));
      m.insert(QStringLiteral("en"), QStringLiteral("Welcome to V-MacroPlus"));
      m.insert(QStringLiteral("es"), QStringLiteral("Welcome to V-MacroPlus"));
      m.insert(QStringLiteral("fr"), QStringLiteral("Welcome to V-MacroPlus"));
      m.insert(QStringLiteral("de"), QStringLiteral("Welcome to V-MacroPlus"));
      m.insert(QStringLiteral("tr"), QStringLiteral("Welcome to V-MacroPlus"));
      m.insert(QStringLiteral("ru"), QStringLiteral("Welcome to V-MacroPlus"));
      m.insert(QStringLiteral("id"), QStringLiteral("Welcome to V-MacroPlus"));
      m.insert(QStringLiteral("fa"), QStringLiteral("مرحباً بك في V-MacroPlus"));
      m.insert(QStringLiteral("ur"), QStringLiteral("مرحباً بك في V-MacroPlus"));
    }
    { auto& m = m_translations[QStringLiteral("dash_subtitle")];
      m.insert(QStringLiteral("ar"), QStringLiteral("استوديو الأتمتة الفائقة والاختصارات الذكية"));
      m.insert(QStringLiteral("en"), QStringLiteral("High-Precision Smart Keys & Macro Automation Studio"));
      m.insert(QStringLiteral("es"), QStringLiteral("High-Precision Smart Keys & Macro Automation Studio"));
      m.insert(QStringLiteral("fr"), QStringLiteral("High-Precision Smart Keys & Macro Automation Studio"));
      m.insert(QStringLiteral("de"), QStringLiteral("High-Precision Smart Keys & Macro Automation Studio"));
      m.insert(QStringLiteral("tr"), QStringLiteral("High-Precision Smart Keys & Macro Automation Studio"));
      m.insert(QStringLiteral("ru"), QStringLiteral("High-Precision Smart Keys & Macro Automation Studio"));
      m.insert(QStringLiteral("id"), QStringLiteral("High-Precision Smart Keys & Macro Automation Studio"));
      m.insert(QStringLiteral("fa"), QStringLiteral("استوديو الأتمتة الفائقة والاختصارات الذكية"));
      m.insert(QStringLiteral("ur"), QStringLiteral("استوديو الأتمتة الفائقة والاختصارات الذكية"));
    }
    { auto& m = m_translations[QStringLiteral("btn_new_recording")];
      m.insert(QStringLiteral("ar"), QStringLiteral("+ تسجيل جديد"));
      m.insert(QStringLiteral("en"), QStringLiteral("+ New Recording"));
      m.insert(QStringLiteral("es"), QStringLiteral("+ New Recording"));
      m.insert(QStringLiteral("fr"), QStringLiteral("+ New Recording"));
      m.insert(QStringLiteral("de"), QStringLiteral("+ New Recording"));
      m.insert(QStringLiteral("tr"), QStringLiteral("+ New Recording"));
      m.insert(QStringLiteral("ru"), QStringLiteral("+ New Recording"));
      m.insert(QStringLiteral("id"), QStringLiteral("+ New Recording"));
      m.insert(QStringLiteral("fa"), QStringLiteral("+ تسجيل جديد"));
      m.insert(QStringLiteral("ur"), QStringLiteral("+ تسجيل جديد"));
    }
    { auto& m = m_translations[QStringLiteral("btn_new_combo")];
      m.insert(QStringLiteral("ar"), QStringLiteral("+ ماكرو جديد"));
      m.insert(QStringLiteral("en"), QStringLiteral("+ New Macro"));
      m.insert(QStringLiteral("es"), QStringLiteral("+ New Macro"));
      m.insert(QStringLiteral("fr"), QStringLiteral("+ New Macro"));
      m.insert(QStringLiteral("de"), QStringLiteral("+ New Macro"));
      m.insert(QStringLiteral("tr"), QStringLiteral("+ New Macro"));
      m.insert(QStringLiteral("ru"), QStringLiteral("+ New Macro"));
      m.insert(QStringLiteral("id"), QStringLiteral("+ New Macro"));
      m.insert(QStringLiteral("fa"), QStringLiteral("+ ماكرو جديد"));
      m.insert(QStringLiteral("ur"), QStringLiteral("+ ماكرو جديد"));
    }
    { auto& m = m_translations[QStringLiteral("btn_panic_stop")];
      m.insert(QStringLiteral("ar"), QStringLiteral("إيقاف فوري"));
      m.insert(QStringLiteral("en"), QStringLiteral("Panic Stop"));
      m.insert(QStringLiteral("es"), QStringLiteral("Panic Stop"));
      m.insert(QStringLiteral("fr"), QStringLiteral("Panic Stop"));
      m.insert(QStringLiteral("de"), QStringLiteral("Panic Stop"));
      m.insert(QStringLiteral("tr"), QStringLiteral("Panic Stop"));
      m.insert(QStringLiteral("ru"), QStringLiteral("Panic Stop"));
      m.insert(QStringLiteral("id"), QStringLiteral("Panic Stop"));
      m.insert(QStringLiteral("fa"), QStringLiteral("إيقاف فوري"));
      m.insert(QStringLiteral("ur"), QStringLiteral("إيقاف فوري"));
    }
    { auto& m = m_translations[QStringLiteral("recent_activity")];
      m.insert(QStringLiteral("ar"), QStringLiteral("النشاط الأخير"));
      m.insert(QStringLiteral("en"), QStringLiteral("Recent Activity"));
      m.insert(QStringLiteral("es"), QStringLiteral("Recent Activity"));
      m.insert(QStringLiteral("fr"), QStringLiteral("Recent Activity"));
      m.insert(QStringLiteral("de"), QStringLiteral("Recent Activity"));
      m.insert(QStringLiteral("tr"), QStringLiteral("Recent Activity"));
      m.insert(QStringLiteral("ru"), QStringLiteral("Recent Activity"));
      m.insert(QStringLiteral("id"), QStringLiteral("Recent Activity"));
      m.insert(QStringLiteral("fa"), QStringLiteral("النشاط الأخير"));
      m.insert(QStringLiteral("ur"), QStringLiteral("النشاط الأخير"));
    }
    { auto& m = m_translations[QStringLiteral("stat_saved_macros")];
      m.insert(QStringLiteral("ar"), QStringLiteral("التسجيلات المحفوظة"));
      m.insert(QStringLiteral("en"), QStringLiteral("Saved Recordings"));
      m.insert(QStringLiteral("es"), QStringLiteral("Saved Recordings"));
      m.insert(QStringLiteral("fr"), QStringLiteral("Saved Recordings"));
      m.insert(QStringLiteral("de"), QStringLiteral("Saved Recordings"));
      m.insert(QStringLiteral("tr"), QStringLiteral("Saved Recordings"));
      m.insert(QStringLiteral("ru"), QStringLiteral("Saved Recordings"));
      m.insert(QStringLiteral("id"), QStringLiteral("Saved Recordings"));
      m.insert(QStringLiteral("fa"), QStringLiteral("التسجيلات المحفوظة"));
      m.insert(QStringLiteral("ur"), QStringLiteral("التسجيلات المحفوظة"));
    }
    { auto& m = m_translations[QStringLiteral("stat_active_combos")];
      m.insert(QStringLiteral("ar"), QStringLiteral("الأزرار الذكية النشطة"));
      m.insert(QStringLiteral("en"), QStringLiteral("Active Smart Keys"));
      m.insert(QStringLiteral("es"), QStringLiteral("Active Smart Keys"));
      m.insert(QStringLiteral("fr"), QStringLiteral("Active Smart Keys"));
      m.insert(QStringLiteral("de"), QStringLiteral("Active Smart Keys"));
      m.insert(QStringLiteral("tr"), QStringLiteral("Active Smart Keys"));
      m.insert(QStringLiteral("ru"), QStringLiteral("Active Smart Keys"));
      m.insert(QStringLiteral("id"), QStringLiteral("Active Smart Keys"));
      m.insert(QStringLiteral("fa"), QStringLiteral("الأزرار الذكية النشطة"));
      m.insert(QStringLiteral("ur"), QStringLiteral("الأزرار الذكية النشطة"));
    }
    { auto& m = m_translations[QStringLiteral("stat_total_runs")];
      m.insert(QStringLiteral("ar"), QStringLiteral("مرات التشغيل"));
      m.insert(QStringLiteral("en"), QStringLiteral("Total Runs"));
      m.insert(QStringLiteral("es"), QStringLiteral("Total Runs"));
      m.insert(QStringLiteral("fr"), QStringLiteral("Total Runs"));
      m.insert(QStringLiteral("de"), QStringLiteral("Total Runs"));
      m.insert(QStringLiteral("tr"), QStringLiteral("Total Runs"));
      m.insert(QStringLiteral("ru"), QStringLiteral("Total Runs"));
      m.insert(QStringLiteral("id"), QStringLiteral("Total Runs"));
      m.insert(QStringLiteral("fa"), QStringLiteral("مرات التشغيل"));
      m.insert(QStringLiteral("ur"), QStringLiteral("مرات التشغيل"));
    }
    { auto& m = m_translations[QStringLiteral("stat_simulated_events")];
      m.insert(QStringLiteral("ar"), QStringLiteral("الأحداث المنفذة"));
      m.insert(QStringLiteral("en"), QStringLiteral("Simulated Events"));
      m.insert(QStringLiteral("es"), QStringLiteral("Simulated Events"));
      m.insert(QStringLiteral("fr"), QStringLiteral("Simulated Events"));
      m.insert(QStringLiteral("de"), QStringLiteral("Simulated Events"));
      m.insert(QStringLiteral("tr"), QStringLiteral("Simulated Events"));
      m.insert(QStringLiteral("ru"), QStringLiteral("Simulated Events"));
      m.insert(QStringLiteral("id"), QStringLiteral("Simulated Events"));
      m.insert(QStringLiteral("fa"), QStringLiteral("الأحداث المنفذة"));
      m.insert(QStringLiteral("ur"), QStringLiteral("الأحداث المنفذة"));
    }
    { auto& m = m_translations[QStringLiteral("test_title")];
      m.insert(QStringLiteral("ar"), QStringLiteral("معمل التجربة ومحركات محاكاة الإدخال والكيبورد (٢٠ محركاً)"));
      m.insert(QStringLiteral("en"), QStringLiteral("Interactive Test Lab & Input Emulation Engines (20 Engines)"));
      m.insert(QStringLiteral("es"), QStringLiteral("Laboratorio de Pruebas y Motores de Emulación (20 Motores)"));
      m.insert(QStringLiteral("fr"), QStringLiteral("Laboratoire de Test et Moteurs d'Émulation (20 Moteurs)"));
      m.insert(QStringLiteral("de"), QStringLiteral("Testlabor & Eingabe-Emulations-Engines (20 Engines)"));
      m.insert(QStringLiteral("tr"), QStringLiteral("Test Laboratuvarı ve Giriş Emülasyon Motorları (20 Motor)"));
      m.insert(QStringLiteral("ru"), QStringLiteral("Тестовая лаборатория и движки эмуляции ввода (20 движков)"));
      m.insert(QStringLiteral("id"), QStringLiteral("Lab Uji & Mesin Emulasi Input (20 Mesin)"));
      m.insert(QStringLiteral("fa"), QStringLiteral("آزمایشگاه تست و موتورهای شبیه‌سازی ورودی (۲۰ موتور)"));
      m.insert(QStringLiteral("ur"), QStringLiteral("ٹیسٹ لیب اور ان پٹ ایمولیشن انجنز (20 انجن)"));
    }
    { auto& m = m_translations[QStringLiteral("test_subtitle")];
      m.insert(QStringLiteral("ar"), QStringLiteral("اختر من بين ٢٠ محركاً متقدماً (Ring-0 Interception / PS/2 Port 0x60 / USB HID 6KRO / Background HWND)، وجرّب في الصندوق أو على أي برنامج تختاره."));
      m.insert(QStringLiteral("en"), QStringLiteral("Switch between 20 advanced input engines (Ring-0 Interception, PS/2 Port 0x60, USB HID 6KRO, Background HWND) and test in the sandbox or on any target app."));
      m.insert(QStringLiteral("es"), QStringLiteral("Cambie entre 20 motores avanzados (Ring-0 Interception, PS/2 Port 0x60, USB HID 6KRO, HWND en segundo plano) y pruebe en el cuadro o en cualquier programa."));
      m.insert(QStringLiteral("fr"), QStringLiteral("Basculez entre 20 moteurs avancés (Ring-0 Interception, PS/2 Port 0x60, USB HID 6KRO, HWND d'arrière-plan) et testez dans le bac à sable ou sur une application cible."));
      m.insert(QStringLiteral("de"), QStringLiteral("Wechseln Sie zwischen 20 Engines (Ring-0 Interception, PS/2 Port 0x60, USB HID 6KRO, Hintergrund-HWND) und testen Sie in der Sandbox oder am Zielprogramm."));
      m.insert(QStringLiteral("tr"), QStringLiteral("20 gelişmiş motor (Ring-0 Interception, PS/2 Port 0x60, USB HID 6KRO, Arka Plan HWND) arasında geçiş yapın ve kutuda veya hedef uygulamada test edin."));
      m.insert(QStringLiteral("ru"), QStringLiteral("Переключайтесь между 20 движками (Ring-0 Interception, PS/2 Port 0x60, USB HID 6KRO, фоновый HWND) и тестируйте в песочнице или на выбранном окне."));
      m.insert(QStringLiteral("id"), QStringLiteral("Beralih di antara 20 mesin input canggih (Ring-0 Interception, PS/2 Port 0x60, USB HID 6KRO, HWND Latar) dan uji di kotak pasir atau aplikasi target."));
      m.insert(QStringLiteral("fa"), QStringLiteral("انتخاب از میان ۲۰ موتور پیشرفته (Ring-0 Interception / PS/2 Port 0x60 / USB HID 6KRO / Background HWND) و تست در کادر آزمایش یا روی برنامه دلخواه."));
      m.insert(QStringLiteral("ur"), QStringLiteral("20 جدید انجنز (Ring-0 Interception, PS/2 Port 0x60, USB HID 6KRO, Background HWND) میں سے منتخب کریں اور سینڈ باکس یا کسی بھی پروگرام پر ٹیسٹ کریں۔"));
    }
    { auto& m = m_translations[QStringLiteral("lbl_input_method")];
      m.insert(QStringLiteral("ar"), QStringLiteral("طريقة محاكاة الإدخال:"));
      m.insert(QStringLiteral("en"), QStringLiteral("Input Simulation Method:"));
      m.insert(QStringLiteral("es"), QStringLiteral("Método de simulación:"));
      m.insert(QStringLiteral("fr"), QStringLiteral("Méthode de simulation :"));
      m.insert(QStringLiteral("de"), QStringLiteral("Eingabesimulation:"));
      m.insert(QStringLiteral("tr"), QStringLiteral("Giriş Simülasyon Yöntemi:"));
      m.insert(QStringLiteral("ru"), QStringLiteral("Метод эмуляции ввода:"));
      m.insert(QStringLiteral("id"), QStringLiteral("Metode Simulasi Input:"));
      m.insert(QStringLiteral("fa"), QStringLiteral("روش شبیه‌سازی ورودی:"));
      m.insert(QStringLiteral("ur"), QStringLiteral("ان پٹ سمولیشن کا طریقہ:"));
    }
    { auto& m = m_translations[QStringLiteral("lbl_kernel_diag_title")];
      m.insert(QStringLiteral("ar"), QStringLiteral("فحص حالة درايفر الكيرنل (Ring-0 Interception) وبصمة الهاردوير المباشرة:"));
      m.insert(QStringLiteral("en"), QStringLiteral("Ring-0 Kernel Driver (Interception) & Live Hardware Signature Diagnostics:"));
      m.insert(QStringLiteral("es"), QStringLiteral("Diagnóstico del controlador Kernel Ring-0 (Interception):"));
      m.insert(QStringLiteral("fr"), QStringLiteral("Diagnostic du pilote Kernel Ring-0 (Interception) :"));
      m.insert(QStringLiteral("de"), QStringLiteral("Ring-0-Kernel-Treiber (Interception) & Hardware-Signatur-Diagnose:"));
      m.insert(QStringLiteral("tr"), QStringLiteral("Ring-0 Çekirdek Sürücüsü (Interception) ve Donanım İmzası Tanılama:"));
      m.insert(QStringLiteral("ru"), QStringLiteral("Диагностика драйвера ядра Ring-0 (Interception) и подписи оборудования:"));
      m.insert(QStringLiteral("id"), QStringLiteral("Diagnostik Driver Kernel Ring-0 (Interception) & Tanda Tangan Perangkat Keras:"));
      m.insert(QStringLiteral("fa"), QStringLiteral("بررسی وضعیت درایور کرنل (Ring-0 Interception) و امضای سخت‌افزاری:"));
      m.insert(QStringLiteral("ur"), QStringLiteral("رنگ-0 کرنل ڈرائیور (Interception) اور ہارڈویئر سگنیچر کی جانچ:"));
    }
    { auto& m = m_translations[QStringLiteral("btn_check_interception")];
      m.insert(QStringLiteral("ar"), QStringLiteral("فحص حالة Interception والكيرنل الآن"));
      m.insert(QStringLiteral("en"), QStringLiteral("Check Interception & Kernel Now"));
      m.insert(QStringLiteral("es"), QStringLiteral("Comprobar Interception y Kernel"));
      m.insert(QStringLiteral("fr"), QStringLiteral("Vérifier Interception et Kernel"));
      m.insert(QStringLiteral("de"), QStringLiteral("Interception & Kernel jetzt prüfen"));
      m.insert(QStringLiteral("tr"), QStringLiteral("Interception ve Çekirdeği Şimdi Kontrol Et"));
      m.insert(QStringLiteral("ru"), QStringLiteral("Проверить Interception и ядро"));
      m.insert(QStringLiteral("id"), QStringLiteral("Periksa Interception & Kernel Sekarang"));
      m.insert(QStringLiteral("fa"), QStringLiteral("بررسی وضعیت Interception و کرنل"));
      m.insert(QStringLiteral("ur"), QStringLiteral("ابھی Interception اور کرنل چیک کریں"));
    }
    { auto& m = m_translations[QStringLiteral("btn_install_interception")];
      m.insert(QStringLiteral("ar"), QStringLiteral("تثبيت / ربط ملفات Interception..."));
      m.insert(QStringLiteral("en"), QStringLiteral("Install / Link Interception Files..."));
      m.insert(QStringLiteral("es"), QStringLiteral("Instalar / Vincular Interception..."));
      m.insert(QStringLiteral("fr"), QStringLiteral("Installer / Lier Interception..."));
      m.insert(QStringLiteral("de"), QStringLiteral("Interception installieren / verknüpfen..."));
      m.insert(QStringLiteral("tr"), QStringLiteral("Interception Dosyalarını Kur / Bağla..."));
      m.insert(QStringLiteral("ru"), QStringLiteral("Установить / привязать Interception..."));
      m.insert(QStringLiteral("id"), QStringLiteral("Instal / Tautkan File Interception..."));
      m.insert(QStringLiteral("fa"), QStringLiteral("نصب / اتصال فایل‌های Interception..."));
      m.insert(QStringLiteral("ur"), QStringLiteral("Interception فائلیں انسٹال / لنک کریں..."));
    }
    { auto& m = m_translations[QStringLiteral("btn_open_drivers_dir")];
      m.insert(QStringLiteral("ar"), QStringLiteral("فتح مجلد الدرايفرات (drivers)"));
      m.insert(QStringLiteral("en"), QStringLiteral("Open Drivers Folder (drivers)"));
      m.insert(QStringLiteral("es"), QStringLiteral("Abrir carpeta drivers"));
      m.insert(QStringLiteral("fr"), QStringLiteral("Ouvrir le dossier drivers"));
      m.insert(QStringLiteral("de"), QStringLiteral("Treiber-Ordner (drivers) öffnen"));
      m.insert(QStringLiteral("tr"), QStringLiteral("Sürücü Klasörünü Aç (drivers)"));
      m.insert(QStringLiteral("ru"), QStringLiteral("Открыть папку драйверов (drivers)"));
      m.insert(QStringLiteral("id"), QStringLiteral("Buka Folder Driver (drivers)"));
      m.insert(QStringLiteral("fa"), QStringLiteral("باز کردن پوشه درایورها (drivers)"));
      m.insert(QStringLiteral("ur"), QStringLiteral("ڈرائیورز فولڈر کھولیں (drivers)"));
    }
    { auto& m = m_translations[QStringLiteral("method_scancode")];
      m.insert(QStringLiteral("ar"), QStringLiteral("الطريقة ١: SendInput ScanCode (محاكاة أزرار الهاردوير المباشرة للألعاب)"));
      m.insert(QStringLiteral("en"), QStringLiteral("Method 1: SendInput ScanCode (Direct Hardware Set-1 ScanCode)"));
      m.insert(QStringLiteral("es"), QStringLiteral("Method 1: SendInput ScanCode (Direct Hardware Set-1 ScanCode)"));
      m.insert(QStringLiteral("fr"), QStringLiteral("Method 1: SendInput ScanCode (Direct Hardware Set-1 ScanCode)"));
      m.insert(QStringLiteral("de"), QStringLiteral("Method 1: SendInput ScanCode (Direct Hardware Set-1 ScanCode)"));
      m.insert(QStringLiteral("tr"), QStringLiteral("Method 1: SendInput ScanCode (Direct Hardware Set-1 ScanCode)"));
      m.insert(QStringLiteral("ru"), QStringLiteral("Method 1: SendInput ScanCode (Direct Hardware Set-1 ScanCode)"));
      m.insert(QStringLiteral("id"), QStringLiteral("Method 1: SendInput ScanCode (Direct Hardware Set-1 ScanCode)"));
      m.insert(QStringLiteral("fa"), QStringLiteral("الطريقة ١: SendInput ScanCode (محاكاة أزرار الهاردوير المباشرة للألعاب)"));
      m.insert(QStringLiteral("ur"), QStringLiteral("الطريقة ١: SendInput ScanCode (محاكاة أزرار الهاردوير المباشرة للألعاب)"));
    }
    { auto& m = m_translations[QStringLiteral("method_hybrid")];
      m.insert(QStringLiteral("ar"), QStringLiteral("الطريقة ٢: SendInput Hybrid VK + ScanCode (توافق مزدوج للبرامج والألعاب)"));
      m.insert(QStringLiteral("en"), QStringLiteral("Method 2: SendInput Hybrid VK + ScanCode (Dual VirtualKey + ScanCode)"));
      m.insert(QStringLiteral("es"), QStringLiteral("Method 2: SendInput Hybrid VK + ScanCode (Dual VirtualKey + ScanCode)"));
      m.insert(QStringLiteral("fr"), QStringLiteral("Method 2: SendInput Hybrid VK + ScanCode (Dual VirtualKey + ScanCode)"));
      m.insert(QStringLiteral("de"), QStringLiteral("Method 2: SendInput Hybrid VK + ScanCode (Dual VirtualKey + ScanCode)"));
      m.insert(QStringLiteral("tr"), QStringLiteral("Method 2: SendInput Hybrid VK + ScanCode (Dual VirtualKey + ScanCode)"));
      m.insert(QStringLiteral("ru"), QStringLiteral("Method 2: SendInput Hybrid VK + ScanCode (Dual VirtualKey + ScanCode)"));
      m.insert(QStringLiteral("id"), QStringLiteral("Method 2: SendInput Hybrid VK + ScanCode (Dual VirtualKey + ScanCode)"));
      m.insert(QStringLiteral("fa"), QStringLiteral("الطريقة ٢: SendInput Hybrid VK + ScanCode (توافق مزدوج للبرامج والألعاب)"));
      m.insert(QStringLiteral("ur"), QStringLiteral("الطريقة ٢: SendInput Hybrid VK + ScanCode (توافق مزدوج للبرامج والألعاب)"));
    }
    { auto& m = m_translations[QStringLiteral("method_keybd_event")];
      m.insert(QStringLiteral("ar"), QStringLiteral("الطريقة ٣: Win32 keybd_event & mouse_event (للألعاب القديمة وبرامج الحماية)"));
      m.insert(QStringLiteral("en"), QStringLiteral("Method 3: Legacy Win32 keybd_event & mouse_event (NT Kernel Interrupt)"));
      m.insert(QStringLiteral("es"), QStringLiteral("Method 3: Legacy Win32 keybd_event & mouse_event (NT Kernel Interrupt)"));
      m.insert(QStringLiteral("fr"), QStringLiteral("Method 3: Legacy Win32 keybd_event & mouse_event (NT Kernel Interrupt)"));
      m.insert(QStringLiteral("de"), QStringLiteral("Method 3: Legacy Win32 keybd_event & mouse_event (NT Kernel Interrupt)"));
      m.insert(QStringLiteral("tr"), QStringLiteral("Method 3: Legacy Win32 keybd_event & mouse_event (NT Kernel Interrupt)"));
      m.insert(QStringLiteral("ru"), QStringLiteral("Method 3: Legacy Win32 keybd_event & mouse_event (NT Kernel Interrupt)"));
      m.insert(QStringLiteral("id"), QStringLiteral("Method 3: Legacy Win32 keybd_event & mouse_event (NT Kernel Interrupt)"));
      m.insert(QStringLiteral("fa"), QStringLiteral("الطريقة ٣: Win32 keybd_event & mouse_event (للألعاب القديمة وبرامج الحماية)"));
      m.insert(QStringLiteral("ur"), QStringLiteral("الطريقة ٣: Win32 keybd_event & mouse_event (للألعاب القديمة وبرامج الحماية)"));
    }
    { auto& m = m_translations[QStringLiteral("method_win32_msg")];
      m.insert(QStringLiteral("ar"), QStringLiteral("الطريقة ٤: Direct Child HWND + AttachThreadInput (إرسال مباشر مع تزامن حالة الكيبورد)"));
      m.insert(QStringLiteral("en"), QStringLiteral("Method 4: Direct Child HWND + AttachThreadInput + SetKeyboardState"));
      m.insert(QStringLiteral("es"), QStringLiteral("Method 4: Direct Child HWND + AttachThreadInput + SetKeyboardState"));
      m.insert(QStringLiteral("fr"), QStringLiteral("Method 4: Direct Child HWND + AttachThreadInput + SetKeyboardState"));
      m.insert(QStringLiteral("de"), QStringLiteral("Method 4: Direct Child HWND + AttachThreadInput + SetKeyboardState"));
      m.insert(QStringLiteral("tr"), QStringLiteral("Method 4: Direct Child HWND + AttachThreadInput + SetKeyboardState"));
      m.insert(QStringLiteral("ru"), QStringLiteral("Method 4: Direct Child HWND + AttachThreadInput + SetKeyboardState"));
      m.insert(QStringLiteral("id"), QStringLiteral("Method 4: Direct Child HWND + AttachThreadInput + SetKeyboardState"));
      m.insert(QStringLiteral("fa"), QStringLiteral("الطريقة ٤: Direct Child HWND + AttachThreadInput (إرسال مباشر مع تزامن حالة الكيبورد)"));
      m.insert(QStringLiteral("ur"), QStringLiteral("الطريقة ٤: Direct Child HWND + AttachThreadInput (إرسال مباشر مع تزامن حالة الكيبورد)"));
    }
    { auto& m = m_translations[QStringLiteral("method_cpp_stealth")];
      m.insert(QStringLiteral("ar"), QStringLiteral("الطريقة ٥ [C++ Stealth]: محاكاة كيبورد فيزيائي بدون بصمة (Zero-ExtraInfo + USB Tick + Jitter)"));
      m.insert(QStringLiteral("en"), QStringLiteral("Method 5 [C++ Stealth]: Zero-Signature Hardware Cloak + USB Tick + Micro-Jitter"));
      m.insert(QStringLiteral("es"), QStringLiteral("Method 5 [C++ Stealth]: Zero-Signature Hardware Cloak + USB Tick + Micro-Jitter"));
      m.insert(QStringLiteral("fr"), QStringLiteral("Method 5 [C++ Stealth]: Zero-Signature Hardware Cloak + USB Tick + Micro-Jitter"));
      m.insert(QStringLiteral("de"), QStringLiteral("Method 5 [C++ Stealth]: Zero-Signature Hardware Cloak + USB Tick + Micro-Jitter"));
      m.insert(QStringLiteral("tr"), QStringLiteral("Method 5 [C++ Stealth]: Zero-Signature Hardware Cloak + USB Tick + Micro-Jitter"));
      m.insert(QStringLiteral("ru"), QStringLiteral("Method 5 [C++ Stealth]: Zero-Signature Hardware Cloak + USB Tick + Micro-Jitter"));
      m.insert(QStringLiteral("id"), QStringLiteral("Method 5 [C++ Stealth]: Zero-Signature Hardware Cloak + USB Tick + Micro-Jitter"));
      m.insert(QStringLiteral("fa"), QStringLiteral("الطريقة ٥ [C++ Stealth]: محاكاة كيبورد فيزيائي بدون بصمة (Zero-ExtraInfo + USB Tick + Jitter)"));
      m.insert(QStringLiteral("ur"), QStringLiteral("الطريقة ٥ [C++ Stealth]: محاكاة كيبورد فيزيائي بدون بصمة (Zero-ExtraInfo + USB Tick + Jitter)"));
    }
    { auto& m = m_translations[QStringLiteral("method_cpp_vhid")];
      m.insert(QStringLiteral("ar"), QStringLiteral("الطريقة ٦ [C++ USB HID]: محاكاة كيبورد USB فيزيائي حقيقي (8-Byte 6KRO HID Report + Virtual HID Bus)"));
      m.insert(QStringLiteral("en"), QStringLiteral("Method 6 [C++ USB HID]: True 8-Byte 6KRO USB HID Keyboard Report (Usage Page 0x07 + VHID Bus)"));
      m.insert(QStringLiteral("es"), QStringLiteral("Method 6 [C++ USB HID]: True 8-Byte 6KRO USB HID Keyboard Report (Usage Page 0x07 + VHID Bus)"));
      m.insert(QStringLiteral("fr"), QStringLiteral("Method 6 [C++ USB HID]: True 8-Byte 6KRO USB HID Keyboard Report (Usage Page 0x07 + VHID Bus)"));
      m.insert(QStringLiteral("de"), QStringLiteral("Method 6 [C++ USB HID]: True 8-Byte 6KRO USB HID Keyboard Report (Usage Page 0x07 + VHID Bus)"));
      m.insert(QStringLiteral("tr"), QStringLiteral("Method 6 [C++ USB HID]: True 8-Byte 6KRO USB HID Keyboard Report (Usage Page 0x07 + VHID Bus)"));
      m.insert(QStringLiteral("ru"), QStringLiteral("Method 6 [C++ USB HID]: True 8-Byte 6KRO USB HID Keyboard Report (Usage Page 0x07 + VHID Bus)"));
      m.insert(QStringLiteral("id"), QStringLiteral("Method 6 [C++ USB HID]: True 8-Byte 6KRO USB HID Keyboard Report (Usage Page 0x07 + VHID Bus)"));
      m.insert(QStringLiteral("fa"), QStringLiteral("الطريقة ٦ [C++ USB HID]: محاكاة كيبورد USB فيزيائي حقيقي (8-Byte 6KRO HID Report + Virtual HID Bus)"));
      m.insert(QStringLiteral("ur"), QStringLiteral("الطريقة ٦ [C++ USB HID]: محاكاة كيبورد USB فيزيائي حقيقي (8-Byte 6KRO HID Report + Virtual HID Bus)"));
    }
    { auto& m = m_translations[QStringLiteral("method_cpp_interception")];
      m.insert(QStringLiteral("ar"), QStringLiteral("الطريقة ٧ [C++ Ring-0 Kernel]: جسر درايفر الكيرنل الفيزيائي (Interception kbdclass Upper-Filter)"));
      m.insert(QStringLiteral("en"), QStringLiteral("Method 7 [C++ Ring-0 Kernel]: Kernel Keyboard Class Filter Driver Bridge (Interception kbdclass)"));
      m.insert(QStringLiteral("es"), QStringLiteral("Method 7 [C++ Ring-0 Kernel]: Kernel Keyboard Class Filter Driver Bridge (Interception kbdclass)"));
      m.insert(QStringLiteral("fr"), QStringLiteral("Method 7 [C++ Ring-0 Kernel]: Kernel Keyboard Class Filter Driver Bridge (Interception kbdclass)"));
      m.insert(QStringLiteral("de"), QStringLiteral("Method 7 [C++ Ring-0 Kernel]: Kernel Keyboard Class Filter Driver Bridge (Interception kbdclass)"));
      m.insert(QStringLiteral("tr"), QStringLiteral("Method 7 [C++ Ring-0 Kernel]: Kernel Keyboard Class Filter Driver Bridge (Interception kbdclass)"));
      m.insert(QStringLiteral("ru"), QStringLiteral("Method 7 [C++ Ring-0 Kernel]: Kernel Keyboard Class Filter Driver Bridge (Interception kbdclass)"));
      m.insert(QStringLiteral("id"), QStringLiteral("Method 7 [C++ Ring-0 Kernel]: Kernel Keyboard Class Filter Driver Bridge (Interception kbdclass)"));
      m.insert(QStringLiteral("fa"), QStringLiteral("الطريقة ٧ [C++ Ring-0 Kernel]: جسر درايفر الكيرنل الفيزيائي (Interception kbdclass Upper-Filter)"));
      m.insert(QStringLiteral("ur"), QStringLiteral("الطريقة ٧ [C++ Ring-0 Kernel]: جسر درايفر الكيرنل الفيزيائي (Interception kbdclass Upper-Filter)"));
    }
    { auto& m = m_translations[QStringLiteral("method_cpp_ps2_port")];
      m.insert(QStringLiteral("ar"), QStringLiteral("الطريقة ٨ [C++ PS/2 Port 0x60]: حقن متحكم الكيبورد الفيزيائي على البوردة (i8042prt Port 0x60/0x64)"));
      m.insert(QStringLiteral("en"), QStringLiteral("Method 8 [C++ PS/2 Port 0x60]: Direct Motherboard i8042 Controller Port 0x60/0x64 Hardware ScanCode"));
      m.insert(QStringLiteral("es"), QStringLiteral("Method 8 [C++ PS/2 Port 0x60]: Direct Motherboard i8042 Controller Port 0x60/0x64 Hardware ScanCode"));
      m.insert(QStringLiteral("fr"), QStringLiteral("Method 8 [C++ PS/2 Port 0x60]: Direct Motherboard i8042 Controller Port 0x60/0x64 Hardware ScanCode"));
      m.insert(QStringLiteral("de"), QStringLiteral("Method 8 [C++ PS/2 Port 0x60]: Direct Motherboard i8042 Controller Port 0x60/0x64 Hardware ScanCode"));
      m.insert(QStringLiteral("tr"), QStringLiteral("Method 8 [C++ PS/2 Port 0x60]: Direct Motherboard i8042 Controller Port 0x60/0x64 Hardware ScanCode"));
      m.insert(QStringLiteral("ru"), QStringLiteral("Method 8 [C++ PS/2 Port 0x60]: Direct Motherboard i8042 Controller Port 0x60/0x64 Hardware ScanCode"));
      m.insert(QStringLiteral("id"), QStringLiteral("Method 8 [C++ PS/2 Port 0x60]: Direct Motherboard i8042 Controller Port 0x60/0x64 Hardware ScanCode"));
      m.insert(QStringLiteral("fa"), QStringLiteral("الطريقة ٨ [C++ PS/2 Port 0x60]: حقن متحكم الكيبورد الفيزيائي على البوردة (i8042prt Port 0x60/0x64)"));
      m.insert(QStringLiteral("ur"), QStringLiteral("الطريقة ٨ [C++ PS/2 Port 0x60]: حقن متحكم الكيبورد الفيزيائي على البوردة (i8042prt Port 0x60/0x64)"));
    }
    { auto& m = m_translations[QStringLiteral("method_cpp_batch_atomic")];
      m.insert(QStringLiteral("ar"), QStringLiteral("الطريقة ٩ [C++ Atomic Burst]: حزمة هاردوير ذرية فائقة السرعة (INPUT[2] Batch + QPC Spin-Wait)"));
      m.insert(QStringLiteral("en"), QStringLiteral("Method 9 [C++ Atomic Burst]: Contiguous Hardware Packet Queue (INPUT[2] + QPC Sub-ms Spin-Wait)"));
      m.insert(QStringLiteral("es"), QStringLiteral("Method 9 [C++ Atomic Burst]: Contiguous Hardware Packet Queue (INPUT[2] + QPC Sub-ms Spin-Wait)"));
      m.insert(QStringLiteral("fr"), QStringLiteral("Method 9 [C++ Atomic Burst]: Contiguous Hardware Packet Queue (INPUT[2] + QPC Sub-ms Spin-Wait)"));
      m.insert(QStringLiteral("de"), QStringLiteral("Method 9 [C++ Atomic Burst]: Contiguous Hardware Packet Queue (INPUT[2] + QPC Sub-ms Spin-Wait)"));
      m.insert(QStringLiteral("tr"), QStringLiteral("Method 9 [C++ Atomic Burst]: Contiguous Hardware Packet Queue (INPUT[2] + QPC Sub-ms Spin-Wait)"));
      m.insert(QStringLiteral("ru"), QStringLiteral("Method 9 [C++ Atomic Burst]: Contiguous Hardware Packet Queue (INPUT[2] + QPC Sub-ms Spin-Wait)"));
      m.insert(QStringLiteral("id"), QStringLiteral("Method 9 [C++ Atomic Burst]: Contiguous Hardware Packet Queue (INPUT[2] + QPC Sub-ms Spin-Wait)"));
      m.insert(QStringLiteral("fa"), QStringLiteral("الطريقة ٩ [C++ Atomic Burst]: حزمة هاردوير ذرية فائقة السرعة (INPUT[2] Batch + QPC Spin-Wait)"));
      m.insert(QStringLiteral("ur"), QStringLiteral("الطريقة ٩ [C++ Atomic Burst]: حزمة هاردوير ذرية فائقة السرعة (INPUT[2] Batch + QPC Spin-Wait)"));
    }
    { auto& m = m_translations[QStringLiteral("method_linux_uinput")];
      m.insert(QStringLiteral("ar"), QStringLiteral("الطريقة ١٠ [Linux Kernel uinput]: كيبورد وماوس USB فيزيائي افتراضي في نواة لينكس (/dev/uinput)"));
      m.insert(QStringLiteral("en"), QStringLiteral("Method 10 [Linux Kernel uinput]: Kernel Virtual USB Keyboard & Mouse (/dev/uinput + XTest)"));
      m.insert(QStringLiteral("es"), QStringLiteral("Method 10 [Linux Kernel uinput]: Kernel Virtual USB Keyboard & Mouse (/dev/uinput + XTest)"));
      m.insert(QStringLiteral("fr"), QStringLiteral("Method 10 [Linux Kernel uinput]: Kernel Virtual USB Keyboard & Mouse (/dev/uinput + XTest)"));
      m.insert(QStringLiteral("de"), QStringLiteral("Method 10 [Linux Kernel uinput]: Kernel Virtual USB Keyboard & Mouse (/dev/uinput + XTest)"));
      m.insert(QStringLiteral("tr"), QStringLiteral("Method 10 [Linux Kernel uinput]: Kernel Virtual USB Keyboard & Mouse (/dev/uinput + XTest)"));
      m.insert(QStringLiteral("ru"), QStringLiteral("Method 10 [Linux Kernel uinput]: Kernel Virtual USB Keyboard & Mouse (/dev/uinput + XTest)"));
      m.insert(QStringLiteral("id"), QStringLiteral("Method 10 [Linux Kernel uinput]: Kernel Virtual USB Keyboard & Mouse (/dev/uinput + XTest)"));
      m.insert(QStringLiteral("fa"), QStringLiteral("الطريقة ١٠ [Linux Kernel uinput]: كيبورد وماوس USB فيزيائي افتراضي في نواة لينكس (/dev/uinput)"));
      m.insert(QStringLiteral("ur"), QStringLiteral("الطريقة ١٠ [Linux Kernel uinput]: كيبورد وماوس USB فيزيائي افتراضي في نواة لينكس (/dev/uinput)"));
    }
    { auto& m = m_translations[QStringLiteral("lbl_test_select_macro")];
      m.insert(QStringLiteral("ar"), QStringLiteral("اختر الماكرو أو التسجيل المراد تجربته:"));
      m.insert(QStringLiteral("en"), QStringLiteral("Select Macro or Saved Recording to Test:"));
      m.insert(QStringLiteral("es"), QStringLiteral("Seleccionar macro o grabación:"));
      m.insert(QStringLiteral("fr"), QStringLiteral("Sélectionner macro ou enregistrement :"));
      m.insert(QStringLiteral("de"), QStringLiteral("Makro oder Aufnahme zum Testen:"));
      m.insert(QStringLiteral("tr"), QStringLiteral("Test Edilecek Makro veya Kayıt:"));
      m.insert(QStringLiteral("ru"), QStringLiteral("Выберите макрос или запись:"));
      m.insert(QStringLiteral("id"), QStringLiteral("Pilih Makro atau Rekaman:"));
      m.insert(QStringLiteral("fa"), QStringLiteral("ماکرو یا ضبط مورد نظر برای تست:"));
      m.insert(QStringLiteral("ur"), QStringLiteral("ٹیسٹ کے لیے میکرو یا ریکارڈنگ منتخب کریں:"));
    }
    { auto& m = m_translations[QStringLiteral("lbl_test_hotkey")];
      m.insert(QStringLiteral("ar"), QStringLiteral("زرار تشغيل التجربة السريعة:"));
      m.insert(QStringLiteral("en"), QStringLiteral("Quick Test Trigger Hotkey:"));
      m.insert(QStringLiteral("es"), QStringLiteral("Quick Test Trigger Hotkey:"));
      m.insert(QStringLiteral("fr"), QStringLiteral("Quick Test Trigger Hotkey:"));
      m.insert(QStringLiteral("de"), QStringLiteral("Quick Test Trigger Hotkey:"));
      m.insert(QStringLiteral("tr"), QStringLiteral("Quick Test Trigger Hotkey:"));
      m.insert(QStringLiteral("ru"), QStringLiteral("Quick Test Trigger Hotkey:"));
      m.insert(QStringLiteral("id"), QStringLiteral("Quick Test Trigger Hotkey:"));
      m.insert(QStringLiteral("fa"), QStringLiteral("زرار تشغيل التجربة السريعة:"));
      m.insert(QStringLiteral("ur"), QStringLiteral("زرار تشغيل التجربة السريعة:"));
    }
    { auto& m = m_translations[QStringLiteral("btn_run_test_now")];
      m.insert(QStringLiteral("ar"), QStringLiteral("تشغيل التجربة الآن"));
      m.insert(QStringLiteral("en"), QStringLiteral("Run Test Now"));
      m.insert(QStringLiteral("es"), QStringLiteral("Ejecutar Prueba"));
      m.insert(QStringLiteral("fr"), QStringLiteral("Lancer le Test"));
      m.insert(QStringLiteral("de"), QStringLiteral("Test jetzt starten"));
      m.insert(QStringLiteral("tr"), QStringLiteral("Testi Şimdi Çalıştır"));
      m.insert(QStringLiteral("ru"), QStringLiteral("Запустить тест"));
      m.insert(QStringLiteral("id"), QStringLiteral("Jalankan Uji Sekarang"));
      m.insert(QStringLiteral("fa"), QStringLiteral("اجرای تست هم‌اکنون"));
      m.insert(QStringLiteral("ur"), QStringLiteral("ابھی ٹیسٹ چلائیں"));
    }
    { auto& m = m_translations[QStringLiteral("lbl_test_sandbox")];
      m.insert(QStringLiteral("ar"), QStringLiteral("صندوق الاختبار المباشر (اضغط هنا أو شغّل التجربة لترى الأزرار والماوس والطريقة المستخدمة فوراً):"));
      m.insert(QStringLiteral("en"), QStringLiteral("Live Input Sandbox (Focus here or run a test to verify key/mouse reception & active method):"));
      m.insert(QStringLiteral("es"), QStringLiteral("Zona de Pruebas en Vivo (Haz clic aquí o ejecuta una prueba para verificar teclas, ratón y motor activo):"));
      m.insert(QStringLiteral("fr"), QStringLiteral("Zone de Test en Direct (Cliquez ici ou lancez un test pour vérifier les touches, la souris et le moteur actif) :"));
      m.insert(QStringLiteral("de"), QStringLiteral("Live-Eingabe-Sandbox (Hier klicken oder Test starten, um Tasten, Maus und aktive Methode zu prüfen):"));
      m.insert(QStringLiteral("tr"), QStringLiteral("Canlı Giriş Test Alanı (Tuş, fare ve aktif motoru doğrulamak için buraya tıklayın veya testi başlatın):"));
      m.insert(QStringLiteral("ru"), QStringLiteral("Интерактивная песочница (нажмите сюда или запустите тест для проверки клавиш, мыши и активного движка):"));
      m.insert(QStringLiteral("id"), QStringLiteral("Kotak Uji Langsung (Klik di sini atau jalankan uji untuk memverifikasi tombol, mouse & mesin aktif):"));
      m.insert(QStringLiteral("fa"), QStringLiteral("محیط آزمایش زنده (اینجا کلیک کنید یا تست را اجرا کنید تا کلید، ماوس و موتور فعال را ببینید):"));
      m.insert(QStringLiteral("ur"), QStringLiteral("لائیو ان پٹ سینڈ باکس (کلید، ماؤس اور فعال انجن جانچنے کے لیے یہاں کلک کریں یا ٹیسٹ چلائیں):"));
    }
    { auto& m = m_translations[QStringLiteral("btn_clear_test_log")];
      m.insert(QStringLiteral("ar"), QStringLiteral("مسح سجل الاختبار"));
      m.insert(QStringLiteral("en"), QStringLiteral("Clear Test Log"));
      m.insert(QStringLiteral("es"), QStringLiteral("Limpiar Registro"));
      m.insert(QStringLiteral("fr"), QStringLiteral("Effacer le Journal"));
      m.insert(QStringLiteral("de"), QStringLiteral("Protokoll löschen"));
      m.insert(QStringLiteral("tr"), QStringLiteral("Test Günlüğünü Temizle"));
      m.insert(QStringLiteral("ru"), QStringLiteral("Очистить журнал"));
      m.insert(QStringLiteral("id"), QStringLiteral("Bersihkan Log Uji"));
      m.insert(QStringLiteral("fa"), QStringLiteral("پاک کردن گزارش تست"));
      m.insert(QStringLiteral("ur"), QStringLiteral("ٹیسٹ لاگ صاف کریں"));
    }
    { auto& m = m_translations[QStringLiteral("settings_title")];
      m.insert(QStringLiteral("ar"), QStringLiteral("الإعدادات والمظهر وطريقة الإدخال والنسخ الاحتياطي"));
      m.insert(QStringLiteral("en"), QStringLiteral("Settings, Appearance, Input Engine & Full Backup"));
      m.insert(QStringLiteral("es"), QStringLiteral("Ajustes, Apariencia, Motor de Entrada y Copia de Seguridad"));
      m.insert(QStringLiteral("fr"), QStringLiteral("Paramètres, Apparence, Moteur de Saisie et Sauvegarde"));
      m.insert(QStringLiteral("de"), QStringLiteral("Einstellungen, Design, Eingabemotor & Komplett-Backup"));
      m.insert(QStringLiteral("tr"), QStringLiteral("Ayarlar, Görünüm, Giriş Motoru ve Tam Yedekleme"));
      m.insert(QStringLiteral("ru"), QStringLiteral("Настройки, внешний вид, метод ввода и резервная копия"));
      m.insert(QStringLiteral("id"), QStringLiteral("Pengaturan, Tampilan, Mesin Input & Cadangan Penuh"));
      m.insert(QStringLiteral("fa"), QStringLiteral("تنظیمات، ظاهر، موتور ورودی و پشتیبان‌گیری کامل"));
      m.insert(QStringLiteral("ur"), QStringLiteral("ترتیبات، ظاہری شکل، ان پٹ انجن اور مکمل بیک اپ"));
    }
    { auto& m = m_translations[QStringLiteral("settings_subtitle")];
      m.insert(QStringLiteral("ar"), QStringLiteral("تحكم كامل في اللغة، طريقة محاكاة الإدخال، التشغيل مع الويندوز، ونقل جميع أزرارك وتسجيلاتك لأي جهاز آخر."));
      m.insert(QStringLiteral("en"), QStringLiteral("Manage language, input simulation method, Windows startup, and export/import all your Smart Keys and Recordings."));
      m.insert(QStringLiteral("es"), QStringLiteral("Manage language, input simulation method, Windows startup, and export/import all your Smart Keys and Recordings."));
      m.insert(QStringLiteral("fr"), QStringLiteral("Manage language, input simulation method, Windows startup, and export/import all your Smart Keys and Recordings."));
      m.insert(QStringLiteral("de"), QStringLiteral("Manage language, input simulation method, Windows startup, and export/import all your Smart Keys and Recordings."));
      m.insert(QStringLiteral("tr"), QStringLiteral("Manage language, input simulation method, Windows startup, and export/import all your Smart Keys and Recordings."));
      m.insert(QStringLiteral("ru"), QStringLiteral("Manage language, input simulation method, Windows startup, and export/import all your Smart Keys and Recordings."));
      m.insert(QStringLiteral("id"), QStringLiteral("Manage language, input simulation method, Windows startup, and export/import all your Smart Keys and Recordings."));
      m.insert(QStringLiteral("fa"), QStringLiteral("تحكم كامل في اللغة، طريقة محاكاة الإدخال، التشغيل مع الويندوز، ونقل جميع أزرارك وتسجيلاتك لأي جهاز آخر."));
      m.insert(QStringLiteral("ur"), QStringLiteral("تحكم كامل في اللغة، طريقة محاكاة الإدخال، التشغيل مع الويندوز، ونقل جميع أزرارك وتسجيلاتك لأي جهاز آخر."));
    }
    { auto& m = m_translations[QStringLiteral("lbl_backup_section")];
      m.insert(QStringLiteral("ar"), QStringLiteral("نقل البيانات والنسخ الاحتياطي الشامل (الأزرار الذكية + الماكروز + التسجيلات):"));
      m.insert(QStringLiteral("en"), QStringLiteral("Full Data Backup & Restore (All Smart Keys + Macros + Recordings):"));
      m.insert(QStringLiteral("es"), QStringLiteral("Full Data Backup & Restore (All Smart Keys + Macros + Recordings):"));
      m.insert(QStringLiteral("fr"), QStringLiteral("Full Data Backup & Restore (All Smart Keys + Macros + Recordings):"));
      m.insert(QStringLiteral("de"), QStringLiteral("Full Data Backup & Restore (All Smart Keys + Macros + Recordings):"));
      m.insert(QStringLiteral("tr"), QStringLiteral("Full Data Backup & Restore (All Smart Keys + Macros + Recordings):"));
      m.insert(QStringLiteral("ru"), QStringLiteral("Full Data Backup & Restore (All Smart Keys + Macros + Recordings):"));
      m.insert(QStringLiteral("id"), QStringLiteral("Full Data Backup & Restore (All Smart Keys + Macros + Recordings):"));
      m.insert(QStringLiteral("fa"), QStringLiteral("نقل البيانات والنسخ الاحتياطي الشامل (الأزرار الذكية + الماكروز + التسجيلات):"));
      m.insert(QStringLiteral("ur"), QStringLiteral("نقل البيانات والنسخ الاحتياطي الشامل (الأزرار الذكية + الماكروز + التسجيلات):"));
    }
    { auto& m = m_translations[QStringLiteral("lbl_language")];
      m.insert(QStringLiteral("ar"), QStringLiteral("لغة البرنامج"));
      m.insert(QStringLiteral("en"), QStringLiteral("Application Language"));
      m.insert(QStringLiteral("es"), QStringLiteral("Idioma"));
      m.insert(QStringLiteral("fr"), QStringLiteral("Langue"));
      m.insert(QStringLiteral("de"), QStringLiteral("Sprache"));
      m.insert(QStringLiteral("tr"), QStringLiteral("Uygulama Dili"));
      m.insert(QStringLiteral("ru"), QStringLiteral("Язык"));
      m.insert(QStringLiteral("id"), QStringLiteral("Bahasa"));
      m.insert(QStringLiteral("fa"), QStringLiteral("زبان برنامه"));
      m.insert(QStringLiteral("ur"), QStringLiteral("پروگرام کی زبان"));
    }
    { auto& m = m_translations[QStringLiteral("lbl_theme_mode")];
      m.insert(QStringLiteral("ar"), QStringLiteral("مظهر الواجهة"));
      m.insert(QStringLiteral("en"), QStringLiteral("Appearance Mode"));
      m.insert(QStringLiteral("es"), QStringLiteral("Modo de apariencia"));
      m.insert(QStringLiteral("fr"), QStringLiteral("Apparence"));
      m.insert(QStringLiteral("de"), QStringLiteral("Design-Modus"));
      m.insert(QStringLiteral("tr"), QStringLiteral("Görünüm Modu"));
      m.insert(QStringLiteral("ru"), QStringLiteral("Тема"));
      m.insert(QStringLiteral("id"), QStringLiteral("Mode Tampilan"));
      m.insert(QStringLiteral("fa"), QStringLiteral("حالت ظاهر برنامه"));
      m.insert(QStringLiteral("ur"), QStringLiteral("ظاہری موڈ"));
    }
    { auto& m = m_translations[QStringLiteral("lbl_color_accent")];
      m.insert(QStringLiteral("ar"), QStringLiteral("اللون المميز"));
      m.insert(QStringLiteral("en"), QStringLiteral("Accent Color"));
      m.insert(QStringLiteral("es"), QStringLiteral("Color de acento"));
      m.insert(QStringLiteral("fr"), QStringLiteral("Couleur d'accentuation"));
      m.insert(QStringLiteral("de"), QStringLiteral("Akzentfarbe"));
      m.insert(QStringLiteral("tr"), QStringLiteral("Vurgu Rengi"));
      m.insert(QStringLiteral("ru"), QStringLiteral("Акцент"));
      m.insert(QStringLiteral("id"), QStringLiteral("Warna Aksen"));
      m.insert(QStringLiteral("fa"), QStringLiteral("رنگ برجسته"));
      m.insert(QStringLiteral("ur"), QStringLiteral("نمایاں رنگ"));
    }
    { auto& m = m_translations[QStringLiteral("mode_dark")];
      m.insert(QStringLiteral("ar"), QStringLiteral("داكن"));
      m.insert(QStringLiteral("en"), QStringLiteral("Dark"));
      m.insert(QStringLiteral("es"), QStringLiteral("Oscuro"));
      m.insert(QStringLiteral("fr"), QStringLiteral("Sombre"));
      m.insert(QStringLiteral("de"), QStringLiteral("Dunkel"));
      m.insert(QStringLiteral("tr"), QStringLiteral("Koyu"));
      m.insert(QStringLiteral("ru"), QStringLiteral("Тёмная"));
      m.insert(QStringLiteral("id"), QStringLiteral("Gelap"));
      m.insert(QStringLiteral("fa"), QStringLiteral("تیره"));
      m.insert(QStringLiteral("ur"), QStringLiteral("ڈارک"));
    }
    { auto& m = m_translations[QStringLiteral("mode_light")];
      m.insert(QStringLiteral("ar"), QStringLiteral("فاتح"));
      m.insert(QStringLiteral("en"), QStringLiteral("Light"));
      m.insert(QStringLiteral("es"), QStringLiteral("Claro"));
      m.insert(QStringLiteral("fr"), QStringLiteral("Clair"));
      m.insert(QStringLiteral("de"), QStringLiteral("Hell"));
      m.insert(QStringLiteral("tr"), QStringLiteral("Açık"));
      m.insert(QStringLiteral("ru"), QStringLiteral("Светлая"));
      m.insert(QStringLiteral("id"), QStringLiteral("Terang"));
      m.insert(QStringLiteral("fa"), QStringLiteral("روشن"));
      m.insert(QStringLiteral("ur"), QStringLiteral("لائٹ"));
    }
    { auto& m = m_translations[QStringLiteral("lbl_run_on_startup")];
      m.insert(QStringLiteral("ar"), QStringLiteral("تشغيل البرنامج تلقائياً عند فتح الويندوز"));
      m.insert(QStringLiteral("en"), QStringLiteral("Launch V-MacroPlus automatically when Windows starts"));
      m.insert(QStringLiteral("es"), QStringLiteral("Iniciar automáticamente con Windows"));
      m.insert(QStringLiteral("fr"), QStringLiteral("Lancer automatiquement au démarrage de Windows"));
      m.insert(QStringLiteral("de"), QStringLiteral("Automatisch beim Windows-Start ausführen"));
      m.insert(QStringLiteral("tr"), QStringLiteral("Windows başladığında otomatik çalıştır"));
      m.insert(QStringLiteral("ru"), QStringLiteral("Запускать автоматически при старте Windows"));
      m.insert(QStringLiteral("id"), QStringLiteral("Jalankan otomatis saat Windows mulai"));
      m.insert(QStringLiteral("fa"), QStringLiteral("اجرای خودکار برنامه هنگام روشن شدن ویندوز"));
      m.insert(QStringLiteral("ur"), QStringLiteral("ونڈوز شروع ہونے پر خودکار چلائیں"));
    }
    { auto& m = m_translations[QStringLiteral("lbl_suppress_key")];
      m.insert(QStringLiteral("ar"), QStringLiteral("منع الزرار الأصلي من الكتابة عند تشغيل الماكرو"));
      m.insert(QStringLiteral("en"), QStringLiteral("Block original trigger key from typing in apps"));
      m.insert(QStringLiteral("es"), QStringLiteral("Bloquear la tecla original al ejecutar la macro"));
      m.insert(QStringLiteral("fr"), QStringLiteral("Bloquer la touche d'origine lors de l'exécution"));
      m.insert(QStringLiteral("de"), QStringLiteral("Originaltaste beim Ausführen blockieren"));
      m.insert(QStringLiteral("tr"), QStringLiteral("Makro çalışırken orijinal tuşun yazmasını engelle"));
      m.insert(QStringLiteral("ru"), QStringLiteral("Блокировать исходную клавишу при запуске макроса"));
      m.insert(QStringLiteral("id"), QStringLiteral("Blokir tombol asli saat makro dijalankan"));
      m.insert(QStringLiteral("fa"), QStringLiteral("جلوگیری از تایپ شدن کلید اصلی هنگام اجرای ماکرو"));
      m.insert(QStringLiteral("ur"), QStringLiteral("میکرو چلتے وقت اصل بٹن کو ٹائپ کرنے سے روکیں"));
    }
    { auto& m = m_translations[QStringLiteral("lbl_min_to_tray")];
      m.insert(QStringLiteral("ar"), QStringLiteral("إبقاء البرنامج في شريط النظام عند إغلاق النافذة"));
      m.insert(QStringLiteral("en"), QStringLiteral("Keep running in System Tray when window is closed"));
      m.insert(QStringLiteral("es"), QStringLiteral("Mantener en la bandeja del sistema al cerrar"));
      m.insert(QStringLiteral("fr"), QStringLiteral("Garder dans la barre d'état système à la fermeture"));
      m.insert(QStringLiteral("de"), QStringLiteral("Beim Schließen im System-Tray weiterlaufen"));
      m.insert(QStringLiteral("tr"), QStringLiteral("Pencere kapatıldığında Sistem Tepsisinde çalışmaya devam et"));
      m.insert(QStringLiteral("ru"), QStringLiteral("Сворачивать в трей при закрытии окна"));
      m.insert(QStringLiteral("id"), QStringLiteral("Tetap berjalan di System Tray saat jendela ditutup"));
      m.insert(QStringLiteral("fa"), QStringLiteral("فعال ماندن در نوار سیستم (Tray) هنگام بستن پنجره"));
      m.insert(QStringLiteral("ur"), QStringLiteral("ونڈو بند کرنے پر سسٹم ٹرے میں چلتے رہیں"));
    }
    { auto& m = m_translations[QStringLiteral("lbl_always_on_top")];
      m.insert(QStringLiteral("ar"), QStringLiteral("تثبيت النافذة دوماً في المقدمة"));
      m.insert(QStringLiteral("en"), QStringLiteral("Keep window always on top"));
      m.insert(QStringLiteral("es"), QStringLiteral("Mantener ventana siempre visible"));
      m.insert(QStringLiteral("fr"), QStringLiteral("Toujours au premier plan"));
      m.insert(QStringLiteral("de"), QStringLiteral("Fenster immer im Vordergrund halten"));
      m.insert(QStringLiteral("tr"), QStringLiteral("Pencereyi her zaman üstte tut"));
      m.insert(QStringLiteral("ru"), QStringLiteral("Поверх всех окон"));
      m.insert(QStringLiteral("id"), QStringLiteral("Selalu tampil di paling atas"));
      m.insert(QStringLiteral("fa"), QStringLiteral("نگه‌داشتن پنجره همیشه در بالاترین سطح"));
      m.insert(QStringLiteral("ur"), QStringLiteral("ونڈو کو ہمیشہ سب سے اوپر رکھیں"));
    }
    { auto& m = m_translations[QStringLiteral("lbl_sounds")];
      m.insert(QStringLiteral("ar"), QStringLiteral("تفعيل الأصوات التنبيهية الخفيفة"));
      m.insert(QStringLiteral("en"), QStringLiteral("Enable subtle audio chimes"));
      m.insert(QStringLiteral("es"), QStringLiteral("Activar sonidos de aviso"));
      m.insert(QStringLiteral("fr"), QStringLiteral("Activer les sons de notification"));
      m.insert(QStringLiteral("de"), QStringLiteral("Dezente Hinweistöne aktivieren"));
      m.insert(QStringLiteral("tr"), QStringLiteral("Hafif bildirim seslerini etkinleştir"));
      m.insert(QStringLiteral("ru"), QStringLiteral("Включить звуковые уведомления"));
      m.insert(QStringLiteral("id"), QStringLiteral("Aktifkan suara notifikasi lembut"));
      m.insert(QStringLiteral("fa"), QStringLiteral("فعال‌سازی صداهای اعلان ملایم"));
      m.insert(QStringLiteral("ur"), QStringLiteral("ہلکی تنبیہی آوازیں فعال کریں"));
    }
    { auto& m = m_translations[QStringLiteral("lbl_hk_rec")];
      m.insert(QStringLiteral("ar"), QStringLiteral("اختصار بدء / إيقاف التسجيل"));
      m.insert(QStringLiteral("en"), QStringLiteral("Record Start / Stop Shortcut"));
      m.insert(QStringLiteral("es"), QStringLiteral("Atajo para Iniciar / Detener grabación"));
      m.insert(QStringLiteral("fr"), QStringLiteral("Raccourci Démarrer / Arrêter l'enregistrement"));
      m.insert(QStringLiteral("de"), QStringLiteral("Hotkey für Aufnahme Start / Stopp"));
      m.insert(QStringLiteral("tr"), QStringLiteral("Kaydı Başlat / Durdur Kısayolu"));
      m.insert(QStringLiteral("ru"), QStringLiteral("Горячая клавиша Старт / Стоп записи"));
      m.insert(QStringLiteral("id"), QStringLiteral("Pintasan Mulai / Hentikan Rekaman"));
      m.insert(QStringLiteral("fa"), QStringLiteral("میانبر شروع / توقف ضبط"));
      m.insert(QStringLiteral("ur"), QStringLiteral("ریکارڈنگ شروع / روکنے کا شارٹ کٹ"));
    }
    { auto& m = m_translations[QStringLiteral("lbl_hk_play")];
      m.insert(QStringLiteral("ar"), QStringLiteral("اختصار تشغيل / إيقاف التسجيل"));
      m.insert(QStringLiteral("en"), QStringLiteral("Play / Stop Recording Shortcut"));
      m.insert(QStringLiteral("es"), QStringLiteral("Atajo para Reproducir / Detener grabación"));
      m.insert(QStringLiteral("fr"), QStringLiteral("Raccourci Lire / Arrêter l'enregistrement"));
      m.insert(QStringLiteral("de"), QStringLiteral("Hotkey für Wiedergabe Start / Stopp"));
      m.insert(QStringLiteral("tr"), QStringLiteral("Kaydı Oynat / Durdur Kısayolu"));
      m.insert(QStringLiteral("ru"), QStringLiteral("Горячая клавиша Пуск / Стоп воспроизведения"));
      m.insert(QStringLiteral("id"), QStringLiteral("Pintasan Putar / Hentikan Rekaman"));
      m.insert(QStringLiteral("fa"), QStringLiteral("میانبر اجرا / توقف ضبط"));
      m.insert(QStringLiteral("ur"), QStringLiteral("ریکارڈنگ چلانے / روکنے کا شارٹ کٹ"));
    }
    { auto& m = m_translations[QStringLiteral("lbl_hk_combo")];
      m.insert(QStringLiteral("ar"), QStringLiteral("اختصار تفعيل / إيقاف الأزرار الذكية"));
      m.insert(QStringLiteral("en"), QStringLiteral("Toggle Smart Keys Shortcut"));
      m.insert(QStringLiteral("es"), QStringLiteral("Atajo para Activar / Desactivar Teclas"));
      m.insert(QStringLiteral("fr"), QStringLiteral("Raccourci Activer / Désactiver Touches"));
      m.insert(QStringLiteral("de"), QStringLiteral("Hotkey für Smart-Tasten Ein / Aus"));
      m.insert(QStringLiteral("tr"), QStringLiteral("Akıllı Tuşları Aç / Kapat Kısayolu"));
      m.insert(QStringLiteral("ru"), QStringLiteral("Горячая клавиша Вкл / Выкл умных клавиш"));
      m.insert(QStringLiteral("id"), QStringLiteral("Pintasan Aktif / Nonaktif Tombol Cerdas"));
      m.insert(QStringLiteral("fa"), QStringLiteral("میانبر فعال / غیرفعال‌سازی کلیدهای هوشمند"));
      m.insert(QStringLiteral("ur"), QStringLiteral("سمارٹ بٹن آن / آف کرنے کا شارٹ کٹ"));
    }
    { auto& m = m_translations[QStringLiteral("btn_about_app")];
      m.insert(QStringLiteral("ar"), QStringLiteral("عن البرنامج والمطور"));
      m.insert(QStringLiteral("en"), QStringLiteral("About V-MacroPlus"));
      m.insert(QStringLiteral("es"), QStringLiteral("About V-MacroPlus"));
      m.insert(QStringLiteral("fr"), QStringLiteral("About V-MacroPlus"));
      m.insert(QStringLiteral("de"), QStringLiteral("About V-MacroPlus"));
      m.insert(QStringLiteral("tr"), QStringLiteral("About V-MacroPlus"));
      m.insert(QStringLiteral("ru"), QStringLiteral("About V-MacroPlus"));
      m.insert(QStringLiteral("id"), QStringLiteral("About V-MacroPlus"));
      m.insert(QStringLiteral("fa"), QStringLiteral("عن البرنامج والمطور"));
      m.insert(QStringLiteral("ur"), QStringLiteral("عن البرنامج والمطور"));
    }
    { auto& m = m_translations[QStringLiteral("btn_reset_settings")];
      m.insert(QStringLiteral("ar"), QStringLiteral("استعادة الافتراضي"));
      m.insert(QStringLiteral("en"), QStringLiteral("Reset Defaults"));
      m.insert(QStringLiteral("es"), QStringLiteral("Reset Defaults"));
      m.insert(QStringLiteral("fr"), QStringLiteral("Reset Defaults"));
      m.insert(QStringLiteral("de"), QStringLiteral("Reset Defaults"));
      m.insert(QStringLiteral("tr"), QStringLiteral("Reset Defaults"));
      m.insert(QStringLiteral("ru"), QStringLiteral("Reset Defaults"));
      m.insert(QStringLiteral("id"), QStringLiteral("Reset Defaults"));
      m.insert(QStringLiteral("fa"), QStringLiteral("استعادة الافتراضي"));
      m.insert(QStringLiteral("ur"), QStringLiteral("استعادة الافتراضي"));
    }
    { auto& m = m_translations[QStringLiteral("about_subtitle_caps")];
      m.insert(QStringLiteral("ar"), QStringLiteral("حول البرنامج"));
      m.insert(QStringLiteral("en"), QStringLiteral("ABOUT THE APPLICATION"));
      m.insert(QStringLiteral("es"), QStringLiteral("ACERCA DE LA APLICACIÓN"));
      m.insert(QStringLiteral("fr"), QStringLiteral("À PROPOS DE L'APPLICATION"));
      m.insert(QStringLiteral("de"), QStringLiteral("ÜBER DIE ANWENDUNG"));
      m.insert(QStringLiteral("tr"), QStringLiteral("UYGULAMA HAKKINDA"));
      m.insert(QStringLiteral("ru"), QStringLiteral("О ПРИЛОЖЕНИИ"));
      m.insert(QStringLiteral("id"), QStringLiteral("TENTANG APLIKASI"));
      m.insert(QStringLiteral("fa"), QStringLiteral("حول البرنامج"));
      m.insert(QStringLiteral("ur"), QStringLiteral("حول البرنامج"));
    }
    { auto& m = m_translations[QStringLiteral("about_version_prefix")];
      m.insert(QStringLiteral("ar"), QStringLiteral("الإصدار"));
      m.insert(QStringLiteral("en"), QStringLiteral("Version"));
      m.insert(QStringLiteral("es"), QStringLiteral("Versión"));
      m.insert(QStringLiteral("fr"), QStringLiteral("Version"));
      m.insert(QStringLiteral("de"), QStringLiteral("Version"));
      m.insert(QStringLiteral("tr"), QStringLiteral("Sürüm"));
      m.insert(QStringLiteral("ru"), QStringLiteral("Версия"));
      m.insert(QStringLiteral("id"), QStringLiteral("Versi"));
      m.insert(QStringLiteral("fa"), QStringLiteral("الإصدار"));
      m.insert(QStringLiteral("ur"), QStringLiteral("الإصدار"));
    }
    { auto& m = m_translations[QStringLiteral("about_desc_body")];
      m.insert(QStringLiteral("ar"), QStringLiteral("منظومة الأتمتة الشاملة V-MacroPlus تجمع في برنامج واحد بين: استوديو الأزرار الذكية (Smart Keys مع ميزة استثناء البرامج)، استوديو الضغط التلقائي متعدد التبويبات (V-AutoKeyPresser لـ ٣٦ زراراً + أزرار مخصصة ونوافذ متعددة في الخلفية)، مسجّل الماكرو فائق الدقة، ومعمل الفحص والتجربة المباشر."));
      m.insert(QStringLiteral("en"), QStringLiteral("V-MacroPlus is the all-in-one desktop automation studio combining Smart Keys (with multi-app exclusion), the Multi-Tab Auto Key & Mouse Presser Studio (V-AutoKeyPresser for 36+ custom keys across background windows), a high-precision Macro Recorder, and a live Hardware Test Lab."));
      m.insert(QStringLiteral("es"), QStringLiteral("V-MacroPlus es el estudio integral de automatización que combina Teclas Inteligentes (con exclusión de programas), el Estudio de Pulsación Automática Multi-Pestaña (V-AutoKeyPresser), Grabador de Macros de alta precisión y Laboratorio de Pruebas en vivo."));
      m.insert(QStringLiteral("fr"), QStringLiteral("V-MacroPlus est le studio d'automatisation tout-en-un combinant Touches Intelligentes (avec exclusion d'applications), le Studio de Frappe Automatique Multi-Onglets (V-AutoKeyPresser), un Enregistreur de Macros haute précision et un Laboratoire de Test en direct."));
      m.insert(QStringLiteral("de"), QStringLiteral("V-MacroPlus vereint Smart Keys (mit Programm-Ausschluss), das Multi-Tab Auto-Key- & Maus-Presser-Studio (V-AutoKeyPresser), einen hochpräzisen Makro-Recorder und ein Live-Hardware-Testlabor in einer Suite."));
      m.insert(QStringLiteral("tr"), QStringLiteral("V-MacroPlus; Akıllı Tuşlar (uygulama hariç tutma destekli), Çok Sekmeli Otomatik Tuş ve Fare Basıcı Stüdyosu (V-AutoKeyPresser), Yüksek Hassasiyetli Makro Kaydedici ve Canlı Test Laboratuvarını tek çatı altında birleştirir."));
      m.insert(QStringLiteral("ru"), QStringLiteral("V-MacroPlus объединяет Умные клавиши (с исключением программ), многовкладочную студию автонажатия клавиш и мыши (V-AutoKeyPresser), высокоточный макро-рекордер и живую тестовую лабораторию."));
      m.insert(QStringLiteral("id"), QStringLiteral("V-MacroPlus menggabungkan Smart Keys (dengan pengecualian aplikasi), Studio Penekan Tombol & Mouse Otomatis Multi-Tab (V-AutoKeyPresser), Perekam Makro presisi tinggi, dan Lab Uji Perangkat Keras langsung."));
      m.insert(QStringLiteral("fa"), QStringLiteral("مجموعه جامع V-MacroPlus ترکیبی از کلیدهای هوشمند (با قابلیت استثنای برنامه‌ها)، استودیو فشار خودکار چندتب (V-AutoKeyPresser)، ضبط‌کننده دقیق ماکرو و آزمایشگاه تست زنده است."));
      m.insert(QStringLiteral("ur"), QStringLiteral("V-MacroPlus ایک مکمل آٹومیشن اسٹوڈیو ہے جس میں اسمارٹ بٹنز (پروگرام استثنیٰ کے ساتھ)، ملٹی ٹیب آٹو کی اینڈ ماؤس پریسر اسٹوڈیو (V-AutoKeyPresser)، میکرو ریکارڈر اور لائیو ٹیسٹ لیب شامل ہیں۔"));
    }
    { auto& m = m_translations[QStringLiteral("about_tech_highlights")];
      m.insert(QStringLiteral("ar"), QStringLiteral("مبني على ٢٠ محرك محاكاة إدخال وكيبورد فيزيائي (Ring-0 Interception + بورت البوردة PS/2 0x60/0x64 InpOutx64 + USB HID 6KRO + Stealth Cloak + ١٠ محركات نوافذ خلفية HWND) مع فحص وتشخيص مباشر للدرايفرات."));
      m.insert(QStringLiteral("en"), QStringLiteral("Powered by 20 unified hardware, kernel & background input engines (Ring-0 Interception, Motherboard PS/2 Port 0x60/0x64 InpOutx64, USB HID 6KRO, Zero-Signature Stealth Cloak & 10 Background HWND engines) with live driver diagnostics."));
      m.insert(QStringLiteral("es"), QStringLiteral("Impulsado por 20 motores unificados de hardware, kernel y segundo plano (Ring-0 Interception, Puerto PS/2 0x60/0x64 InpOutx64, USB HID 6KRO, Stealth Cloak y 10 motores HWND) con diagnóstico en vivo."));
      m.insert(QStringLiteral("fr"), QStringLiteral("Propulsé par 20 moteurs unifiés matériel, noyau et arrière-plan (Ring-0 Interception, Port PS/2 0x60/0x64 InpOutx64, USB HID 6KRO, Stealth Cloak et 10 moteurs HWND) avec diagnostic en direct."));
      m.insert(QStringLiteral("de"), QStringLiteral("Angetrieben von 20 vereinten Hardware-, Kernel- & Hintergrund-Engines (Ring-0 Interception, PS/2 Port 0x60/0x64 InpOutx64, USB HID 6KRO, Stealth Cloak & 10 HWND-Engines) mit Live-Treiberdiagnose."));
      m.insert(QStringLiteral("tr"), QStringLiteral("Canlı sürücüall tanılama destekli 20 birleşik donanım, çekirdek ve arka plan motoru (Ring-0 Interception, PS/2 Port 0x60/0x64 InpOutx64, USB HID 6KRO, Stealth Cloak ve 10 HWND motoru) ile güçlendirilmiştir."));
      m.insert(QStringLiteral("ru"), QStringLiteral("Работает на 20 единых аппаратных, ядерных и фоновых движках (Ring-0 Interception, порт PS/2 0x60/0x64 InpOutx64, USB HID 6KRO, Stealth Cloak и 10 фоновых HWND-движков) с живой диагностикой драйверов."));
      m.insert(QStringLiteral("id"), QStringLiteral("Didukung oleh 20 mesin input perangkat keras, kernel & latar belakang terpadu (Ring-0 Interception, Port PS/2 0x60/0x64 InpOutx64, USB HID 6KRO, Stealth Cloak & 10 mesin HWND) dengan diagnostik driver langsung."));
      m.insert(QStringLiteral("fa"), QStringLiteral("مجهز به ۲۰ موتور یکپارچه سخت‌افزاری، کرنل و پس‌زمینه (Ring-0 Interception، پورت PS/2 0x60/0x64 InpOutx64، USB HID 6KRO، Stealth Cloak و ۱۰ موتور HWND) همراه با عیب‌یابی زنده درایورها."));
      m.insert(QStringLiteral("ur"), QStringLiteral("20 متحدہ ہارڈویئر، کرنل اور بیک گراؤنڈ انجنز (Ring-0 Interception، مدر بورڈ PS/2 Port 0x60/0x64 InpOutx64، USB HID 6KRO، Stealth Cloak اور 10 بیک گراؤنڈ HWND انجنز) اور لائیو ڈرائیور تشخیص پر مبنی۔"));
    }
    { auto& m = m_translations[QStringLiteral("about_tech_badge")];
      m.insert(QStringLiteral("ar"), QStringLiteral("⚡ ٢٠ محرك إدخال متقدم  •  ضغط تلقائي متعدد التبويبات  •  فحص مباشر للدرايفرات"));
      m.insert(QStringLiteral("en"), QStringLiteral("⚡ 20 Input Engines  •  Multi-Tab Auto Presser  •  Live Driver Diagnostics"));
      m.insert(QStringLiteral("es"), QStringLiteral("⚡ 20 Motores de Entrada  •  Pulsador Multi-Pestaña  •  Diagnóstico en Vivo"));
      m.insert(QStringLiteral("fr"), QStringLiteral("⚡ 20 Moteurs d'Entrée  •  Frappe Auto Multi-Onglets  •  Diagnostic en Direct"));
      m.insert(QStringLiteral("de"), QStringLiteral("⚡ 20 Eingabe-Engines  •  Multi-Tab Auto-Presser  •  Live-Treiberdiagnose"));
      m.insert(QStringLiteral("tr"), QStringLiteral("⚡ 20 Giriş Motoru  •  Çok Sekmeli Otomatik Basıcı  •  Canlı Sürücü Tanılama"));
      m.insert(QStringLiteral("ru"), QStringLiteral("⚡ 20 движков ввода  •  Многовкладочное автонажатие  •  Живая диагностика"));
      m.insert(QStringLiteral("id"), QStringLiteral("⚡ 20 Mesin Input  •  Penekan Otomatis Multi-Tab  •  Diagnostik Langsung"));
      m.insert(QStringLiteral("fa"), QStringLiteral("⚡ ۲۰ موتور ورودی پیشرفته  •  فشار خودکار چندتب  •  عیب‌یابی زنده درایورها"));
      m.insert(QStringLiteral("ur"), QStringLiteral("⚡ 20 ان پٹ انجنز  •  ملٹی ٹیب آٹو پریسر  •  لائیو ڈرائیور تشخیص"));
    }
    { auto& m = m_translations[QStringLiteral("about_dev_header")];
      m.insert(QStringLiteral("ar"), QStringLiteral("المطور"));
      m.insert(QStringLiteral("en"), QStringLiteral("DEVELOPER"));
      m.insert(QStringLiteral("es"), QStringLiteral("DESARROLLADOR"));
      m.insert(QStringLiteral("fr"), QStringLiteral("DÉVELOPPEUR"));
      m.insert(QStringLiteral("de"), QStringLiteral("ENTWICKLER"));
      m.insert(QStringLiteral("tr"), QStringLiteral("GELİŞTİRİCİ"));
      m.insert(QStringLiteral("ru"), QStringLiteral("РАЗРАБОТЧИК"));
      m.insert(QStringLiteral("id"), QStringLiteral("PENGEMBANG"));
      m.insert(QStringLiteral("fa"), QStringLiteral("المطور"));
      m.insert(QStringLiteral("ur"), QStringLiteral("المطور"));
    }
    { auto& m = m_translations[QStringLiteral("about_arch_footer")];
      m.insert(QStringLiteral("ar"), QStringLiteral("هُندس بعناية ليمنحك أعلى درجات السرعة، الاعتمادية، والبساطة في أتمتة مهامك اليومية."));
      m.insert(QStringLiteral("en"), QStringLiteral("Engineered for maximum speed, reliability, and effortless control over your daily workflow."));
      m.insert(QStringLiteral("es"), QStringLiteral("Engineered for maximum speed, reliability, and effortless control over your daily workflow."));
      m.insert(QStringLiteral("fr"), QStringLiteral("Engineered for maximum speed, reliability, and effortless control over your daily workflow."));
      m.insert(QStringLiteral("de"), QStringLiteral("Engineered for maximum speed, reliability, and effortless control over your daily workflow."));
      m.insert(QStringLiteral("tr"), QStringLiteral("Engineered for maximum speed, reliability, and effortless control over your daily workflow."));
      m.insert(QStringLiteral("ru"), QStringLiteral("Engineered for maximum speed, reliability, and effortless control over your daily workflow."));
      m.insert(QStringLiteral("id"), QStringLiteral("Engineered for maximum speed, reliability, and effortless control over your daily workflow."));
      m.insert(QStringLiteral("fa"), QStringLiteral("هُندس بعناية ليمنحك أعلى درجات السرعة، الاعتمادية، والبساطة في أتمتة مهامك اليومية."));
      m.insert(QStringLiteral("ur"), QStringLiteral("هُندس بعناية ليمنحك أعلى درجات السرعة، الاعتمادية، والبساطة في أتمتة مهامك اليومية."));
    }
    { auto& m = m_translations[QStringLiteral("about_assisted_by")];
      m.insert(QStringLiteral("ar"), QStringLiteral("تم التطوير بمساعدة Argon."));
      m.insert(QStringLiteral("en"), QStringLiteral("Developed with assistance from Argon."));
      m.insert(QStringLiteral("es"), QStringLiteral("Desarrollado con la asistencia de Argon."));
      m.insert(QStringLiteral("fr"), QStringLiteral("Développé avec l'assistance d'Argon."));
      m.insert(QStringLiteral("de"), QStringLiteral("Entwickelt mit Unterstützung von Argon."));
      m.insert(QStringLiteral("tr"), QStringLiteral("Argon desteğiyle geliştirilmiştir."));
      m.insert(QStringLiteral("ru"), QStringLiteral("Разработано при поддержке Argon."));
      m.insert(QStringLiteral("id"), QStringLiteral("Dikembangkan dengan bantuan Argon."));
      m.insert(QStringLiteral("fa"), QStringLiteral("توسعه‌یافته با همراهی Argon."));
      m.insert(QStringLiteral("ur"), QStringLiteral("Argon کی معاونت سے تیار کیا گیا۔"));
    }
    { auto& m = m_translations[QStringLiteral("about_made_with_care")];
      m.insert(QStringLiteral("ar"), QStringLiteral("⚡ صُنع بإتقان لتجربة ماكرو وأتمتة احترافية فائقة السرعة."));
      m.insert(QStringLiteral("en"), QStringLiteral("⚡ Built with precision for high-speed macro & workflow automation."));
      m.insert(QStringLiteral("es"), QStringLiteral("⚡ Built with precision for high-speed macro & workflow automation."));
      m.insert(QStringLiteral("fr"), QStringLiteral("⚡ Built with precision for high-speed macro & workflow automation."));
      m.insert(QStringLiteral("de"), QStringLiteral("⚡ Built with precision for high-speed macro & workflow automation."));
      m.insert(QStringLiteral("tr"), QStringLiteral("⚡ Built with precision for high-speed macro & workflow automation."));
      m.insert(QStringLiteral("ru"), QStringLiteral("⚡ Built with precision for high-speed macro & workflow automation."));
      m.insert(QStringLiteral("id"), QStringLiteral("⚡ Built with precision for high-speed macro & workflow automation."));
      m.insert(QStringLiteral("fa"), QStringLiteral("⚡ صُنع بإتقان لتجربة ماكرو وأتمتة احترافية فائقة السرعة."));
      m.insert(QStringLiteral("ur"), QStringLiteral("⚡ صُنع بإتقان لتجربة ماكرو وأتمتة احترافية فائقة السرعة."));
    }
    { auto& m = m_translations[QStringLiteral("copied_toast")];
      m.insert(QStringLiteral("ar"), QStringLiteral("تم النسخ إلى الحافظة"));
      m.insert(QStringLiteral("en"), QStringLiteral("Copied to clipboard"));
      m.insert(QStringLiteral("es"), QStringLiteral("Copied to clipboard"));
      m.insert(QStringLiteral("fr"), QStringLiteral("Copied to clipboard"));
      m.insert(QStringLiteral("de"), QStringLiteral("Copied to clipboard"));
      m.insert(QStringLiteral("tr"), QStringLiteral("Copied to clipboard"));
      m.insert(QStringLiteral("ru"), QStringLiteral("Copied to clipboard"));
      m.insert(QStringLiteral("id"), QStringLiteral("Copied to clipboard"));
      m.insert(QStringLiteral("fa"), QStringLiteral("تم النسخ إلى الحافظة"));
      m.insert(QStringLiteral("ur"), QStringLiteral("تم النسخ إلى الحافظة"));
    }
    { auto& m = m_translations[QStringLiteral("btn_copy")];
      m.insert(QStringLiteral("ar"), QStringLiteral("نسخ"));
      m.insert(QStringLiteral("en"), QStringLiteral("Copy"));
      m.insert(QStringLiteral("es"), QStringLiteral("Copiar"));
      m.insert(QStringLiteral("fr"), QStringLiteral("Copier"));
      m.insert(QStringLiteral("de"), QStringLiteral("Kopieren"));
      m.insert(QStringLiteral("tr"), QStringLiteral("Kopyala"));
      m.insert(QStringLiteral("ru"), QStringLiteral("Копировать"));
      m.insert(QStringLiteral("id"), QStringLiteral("Salin"));
      m.insert(QStringLiteral("fa"), QStringLiteral("نسخ"));
      m.insert(QStringLiteral("ur"), QStringLiteral("نسخ"));
    }
    { auto& m = m_translations[QStringLiteral("btn_close")];
      m.insert(QStringLiteral("ar"), QStringLiteral("إغلاق"));
      m.insert(QStringLiteral("en"), QStringLiteral("Close"));
      m.insert(QStringLiteral("es"), QStringLiteral("Cerrar"));
      m.insert(QStringLiteral("fr"), QStringLiteral("Fermer"));
      m.insert(QStringLiteral("de"), QStringLiteral("Schließen"));
      m.insert(QStringLiteral("tr"), QStringLiteral("Kapat"));
      m.insert(QStringLiteral("ru"), QStringLiteral("Закрыть"));
      m.insert(QStringLiteral("id"), QStringLiteral("Tutup"));
      m.insert(QStringLiteral("fa"), QStringLiteral("إغلاق"));
      m.insert(QStringLiteral("ur"), QStringLiteral("إغلاق"));
    }
    { auto& m = m_translations[QStringLiteral("onb_title")];
      m.insert(QStringLiteral("ar"), QStringLiteral("أهلاً بك في V-MacroPlus — قوة الأتمتة في أبسط صورة!"));
      m.insert(QStringLiteral("en"), QStringLiteral("Welcome to V-MacroPlus — Powerful Automation Made Simple!"));
      m.insert(QStringLiteral("es"), QStringLiteral("¡Bienvenido a V-MacroPlus — Automatización potente y sencilla!"));
      m.insert(QStringLiteral("fr"), QStringLiteral("Bienvenue dans V-MacroPlus — L'automatisation puissante et simple !"));
      m.insert(QStringLiteral("de"), QStringLiteral("Willkommen bei V-MacroPlus — Leistungsstarke Automatisierung leicht gemacht!"));
      m.insert(QStringLiteral("tr"), QStringLiteral("V-MacroPlus'a Hoş Geldiniz — Güçlü Otomasyon Artık Çok Kolay!"));
      m.insert(QStringLiteral("ru"), QStringLiteral("Добро пожаловать в V-MacroPlus — Мощная автоматизация простыми словами!"));
      m.insert(QStringLiteral("id"), QStringLiteral("Selamat Datang di V-MacroPlus — Otomatisasi Canggih & Mudah!"));
      m.insert(QStringLiteral("fa"), QStringLiteral("به V-MacroPlus خوش آمدید — قدرت خودکارسازی در ساده‌ترین شکل!"));
      m.insert(QStringLiteral("ur"), QStringLiteral("V-MacroPlus میں خوش آمدید — طاقتور آٹومیشن اب نہایت آسان!"));
    }
    { auto& m = m_translations[QStringLiteral("onb_step_summary")];
      m.insert(QStringLiteral("ar"), QStringLiteral("١. اختر لغتك المفضلة   ←   ٢. حدد زراراً أو سجّل الماوس والكيبورد   ←   ٣. شغّله فوراً بدقة تامة"));
      m.insert(QStringLiteral("en"), QStringLiteral("1. Pick your language   ->   2. Bind a shortcut or record Mouse & Keyboard   ->   3. Run with precision"));
      m.insert(QStringLiteral("es"), QStringLiteral("1. Elige tu idioma   ->   2. Asigna un atajo o graba ratón y teclado   ->   3. Ejecuta al instante"));
      m.insert(QStringLiteral("fr"), QStringLiteral("1. Choisissez votre langue   ->   2. Créez un raccourci ou enregistrez   ->   3. Exécutez avec précision"));
      m.insert(QStringLiteral("de"), QStringLiteral("1. Sprache wählen   ->   2. Hotkey binden oder Maus & Tastatur aufnehmen   ->   3. Sofort starten"));
      m.insert(QStringLiteral("tr"), QStringLiteral("1. Dilinizi seçin   ->   2. Kısayol atayın veya Fare/Klavye kaydedin   ->   3. Anında çalıştırın"));
      m.insert(QStringLiteral("ru"), QStringLiteral("1. Выберите язык   ->   2. Назначьте клавишу или запишите действия   ->   3. Запускайте мгновенно"));
      m.insert(QStringLiteral("id"), QStringLiteral("1. Pilih bahasa Anda   ->   2. Atur pintasan atau rekam Mouse & Keyboard   ->   3. Jalankan segera"));
      m.insert(QStringLiteral("fa"), QStringLiteral("۱. زبان خود را انتخاب کنید   ←   ۲. میانبر بسازید یا ماوس و کیبورد را ضبط کنید   ←   ۳. فوراً اجرا کنید"));
      m.insert(QStringLiteral("ur"), QStringLiteral("١. اپنی زبان منتخب کریں   ←   ٢. شارٹ کٹ بنائیں یا ماؤس اور کی بورڈ ریکارڈ کریں   ←   ٣. فوراً چلائیں"));
    }
    { auto& m = m_translations[QStringLiteral("onb_pick_lang")];
      m.insert(QStringLiteral("ar"), QStringLiteral("اختر لغة البرنامج:"));
      m.insert(QStringLiteral("en"), QStringLiteral("Select Application Language:"));
      m.insert(QStringLiteral("es"), QStringLiteral("Selecciona el idioma:"));
      m.insert(QStringLiteral("fr"), QStringLiteral("Choisissez la langue :"));
      m.insert(QStringLiteral("de"), QStringLiteral("Sprache auswählen:"));
      m.insert(QStringLiteral("tr"), QStringLiteral("Uygulama Dilini Seçin:"));
      m.insert(QStringLiteral("ru"), QStringLiteral("Выберите язык программы:"));
      m.insert(QStringLiteral("id"), QStringLiteral("Pilih Bahasa Aplikasi:"));
      m.insert(QStringLiteral("fa"), QStringLiteral("زبان برنامه را انتخاب کنید:"));
      m.insert(QStringLiteral("ur"), QStringLiteral("پروگرام کی زبان منتخب کریں:"));
    }
    { auto& m = m_translations[QStringLiteral("btn_finish")];
      m.insert(QStringLiteral("ar"), QStringLiteral("ابدأ الآن"));
      m.insert(QStringLiteral("en"), QStringLiteral("Start Now"));
      m.insert(QStringLiteral("es"), QStringLiteral("Comenzar ahora"));
      m.insert(QStringLiteral("fr"), QStringLiteral("Commencer"));
      m.insert(QStringLiteral("de"), QStringLiteral("Jetzt starten"));
      m.insert(QStringLiteral("tr"), QStringLiteral("Şimdi Başla"));
      m.insert(QStringLiteral("ru"), QStringLiteral("Начать работу"));
      m.insert(QStringLiteral("id"), QStringLiteral("Mulai Sekarang"));
      m.insert(QStringLiteral("fa"), QStringLiteral("هم‌اکنون شروع کنید"));
      m.insert(QStringLiteral("ur"), QStringLiteral("ابھی شروع کریں"));
    }
    { auto& m = m_translations[QStringLiteral("tray_show")];
      m.insert(QStringLiteral("ar"), QStringLiteral("فتح نافذة البرنامج"));
      m.insert(QStringLiteral("en"), QStringLiteral("Open V-MacroPlus"));
      m.insert(QStringLiteral("es"), QStringLiteral("Abrir V-MacroPlus"));
      m.insert(QStringLiteral("fr"), QStringLiteral("Ouvrir V-MacroPlus"));
      m.insert(QStringLiteral("de"), QStringLiteral("V-MacroPlus öffnen"));
      m.insert(QStringLiteral("tr"), QStringLiteral("V-MacroPlus Aç"));
      m.insert(QStringLiteral("ru"), QStringLiteral("Открыть V-MacroPlus"));
      m.insert(QStringLiteral("id"), QStringLiteral("Buka V-MacroPlus"));
      m.insert(QStringLiteral("fa"), QStringLiteral("باز کردن پنجره برنامه"));
      m.insert(QStringLiteral("ur"), QStringLiteral("پروگرام کھولیں"));
    }
    { auto& m = m_translations[QStringLiteral("tray_quick_rec")];
      m.insert(QStringLiteral("ar"), QStringLiteral("بدء / إيقاف التسجيل"));
      m.insert(QStringLiteral("en"), QStringLiteral("Start / Stop Recording"));
      m.insert(QStringLiteral("es"), QStringLiteral("Iniciar / Detener grabación"));
      m.insert(QStringLiteral("fr"), QStringLiteral("Démarrer / Arrêter"));
      m.insert(QStringLiteral("de"), QStringLiteral("Aufnahme Start / Stopp"));
      m.insert(QStringLiteral("tr"), QStringLiteral("Kaydı Başlat / Durdur"));
      m.insert(QStringLiteral("ru"), QStringLiteral("Запись Старт / Стоп"));
      m.insert(QStringLiteral("id"), QStringLiteral("Mulai / Hentikan Rekaman"));
      m.insert(QStringLiteral("fa"), QStringLiteral("شروع / توقف ضبط"));
      m.insert(QStringLiteral("ur"), QStringLiteral("ریکارڈنگ شروع / روکیں"));
    }
    { auto& m = m_translations[QStringLiteral("tray_quick_play")];
      m.insert(QStringLiteral("ar"), QStringLiteral("تشغيل / إيقاف التسجيل"));
      m.insert(QStringLiteral("en"), QStringLiteral("Play / Stop Recording"));
      m.insert(QStringLiteral("es"), QStringLiteral("Reproducir / Detener"));
      m.insert(QStringLiteral("fr"), QStringLiteral("Lire / Arrêter"));
      m.insert(QStringLiteral("de"), QStringLiteral("Wiedergabe Start / Stopp"));
      m.insert(QStringLiteral("tr"), QStringLiteral("Kaydı Oynat / Durdur"));
      m.insert(QStringLiteral("ru"), QStringLiteral("Пуск / Стоп записи"));
      m.insert(QStringLiteral("id"), QStringLiteral("Putar / Hentikan Rekaman"));
      m.insert(QStringLiteral("fa"), QStringLiteral("اجرا / توقف ضبط"));
      m.insert(QStringLiteral("ur"), QStringLiteral("ریکارڈنگ چلائیں / روکیں"));
    }
    { auto& m = m_translations[QStringLiteral("tray_toggle_combos")];
      m.insert(QStringLiteral("ar"), QStringLiteral("الأزرار الذكية: "));
      m.insert(QStringLiteral("en"), QStringLiteral("Smart Keys: "));
      m.insert(QStringLiteral("es"), QStringLiteral("Teclas Inteligentes: "));
      m.insert(QStringLiteral("fr"), QStringLiteral("Touches Intelligentes : "));
      m.insert(QStringLiteral("de"), QStringLiteral("Smart-Tasten: "));
      m.insert(QStringLiteral("tr"), QStringLiteral("Akıllı Tuşlar: "));
      m.insert(QStringLiteral("ru"), QStringLiteral("Умные клавиши: "));
      m.insert(QStringLiteral("id"), QStringLiteral("Tombol Cerdas: "));
      m.insert(QStringLiteral("fa"), QStringLiteral("کلیدهای هوشمند: "));
      m.insert(QStringLiteral("ur"), QStringLiteral("سمارٹ بٹن: "));
    }
    { auto& m = m_translations[QStringLiteral("tray_quit")];
      m.insert(QStringLiteral("ar"), QStringLiteral("خروج نهائي"));
      m.insert(QStringLiteral("en"), QStringLiteral("Quit V-MacroPlus"));
      m.insert(QStringLiteral("es"), QStringLiteral("Salir completamente"));
      m.insert(QStringLiteral("fr"), QStringLiteral("Quitter V-MacroPlus"));
      m.insert(QStringLiteral("de"), QStringLiteral("V-MacroPlus beenden"));
      m.insert(QStringLiteral("tr"), QStringLiteral("Tamamen Çık"));
      m.insert(QStringLiteral("ru"), QStringLiteral("Выход"));
      m.insert(QStringLiteral("id"), QStringLiteral("Keluar sepenuhnya"));
      m.insert(QStringLiteral("fa"), QStringLiteral("خروج کامل"));
      m.insert(QStringLiteral("ur"), QStringLiteral("مکمل بند کریں"));
    }
    { auto& m = m_translations[QStringLiteral("state_on")];
      m.insert(QStringLiteral("ar"), QStringLiteral("مُفعّلة"));
      m.insert(QStringLiteral("en"), QStringLiteral("ON"));
      m.insert(QStringLiteral("es"), QStringLiteral("ACTIVO"));
      m.insert(QStringLiteral("fr"), QStringLiteral("ACTIF"));
      m.insert(QStringLiteral("de"), QStringLiteral("EIN"));
      m.insert(QStringLiteral("tr"), QStringLiteral("AÇIK"));
      m.insert(QStringLiteral("ru"), QStringLiteral("ВКЛ"));
      m.insert(QStringLiteral("id"), QStringLiteral("AKTIF"));
      m.insert(QStringLiteral("fa"), QStringLiteral("فعال"));
      m.insert(QStringLiteral("ur"), QStringLiteral("فعال"));
    }
    { auto& m = m_translations[QStringLiteral("state_off")];
      m.insert(QStringLiteral("ar"), QStringLiteral("متوقفة"));
      m.insert(QStringLiteral("en"), QStringLiteral("OFF"));
      m.insert(QStringLiteral("es"), QStringLiteral("INACTIVO"));
      m.insert(QStringLiteral("fr"), QStringLiteral("INACTIF"));
      m.insert(QStringLiteral("de"), QStringLiteral("AUS"));
      m.insert(QStringLiteral("tr"), QStringLiteral("KAPALI"));
      m.insert(QStringLiteral("ru"), QStringLiteral("ВЫКЛ"));
      m.insert(QStringLiteral("id"), QStringLiteral("NONAKTIF"));
      m.insert(QStringLiteral("fa"), QStringLiteral("غیرفعال"));
      m.insert(QStringLiteral("ur"), QStringLiteral("غیر فعال"));
    }
    { auto& m = m_translations[QStringLiteral("lbl_retrigger_policy")];
      m.insert(QStringLiteral("ar"), QStringLiteral("عند الضغط أثناء التشغيل:"));
      m.insert(QStringLiteral("en"), QStringLiteral("If pressed while running:"));
      m.insert(QStringLiteral("es"), QStringLiteral("Si se pulsa en ejecución:"));
      m.insert(QStringLiteral("fr"), QStringLiteral("Si pressé en cours :"));
      m.insert(QStringLiteral("de"), QStringLiteral("Bei Druck während Lauf:"));
      m.insert(QStringLiteral("tr"), QStringLiteral("Çalışırken basılırsa:"));
      m.insert(QStringLiteral("ru"), QStringLiteral("При нажатии во время работы:"));
      m.insert(QStringLiteral("id"), QStringLiteral("Jika ditekan saat berjalan:"));
      m.insert(QStringLiteral("fa"), QStringLiteral("هنگام فشردن در حال اجرا:"));
      m.insert(QStringLiteral("ur"), QStringLiteral("چلتے ہوئے دبانے پر:"));
    }
    { auto& m = m_translations[QStringLiteral("retrigger_ignore")];
      m.insert(QStringLiteral("ar"), QStringLiteral("تجاهل"));
      m.insert(QStringLiteral("en"), QStringLiteral("Ignore"));
      m.insert(QStringLiteral("es"), QStringLiteral("Ignorar"));
      m.insert(QStringLiteral("fr"), QStringLiteral("Ignorer"));
      m.insert(QStringLiteral("de"), QStringLiteral("Ignorieren"));
      m.insert(QStringLiteral("tr"), QStringLiteral("Yoksay"));
      m.insert(QStringLiteral("ru"), QStringLiteral("Игнорировать"));
      m.insert(QStringLiteral("id"), QStringLiteral("Abaikan"));
      m.insert(QStringLiteral("fa"), QStringLiteral("نادیده گرفتن"));
      m.insert(QStringLiteral("ur"), QStringLiteral("نظر انداز"));
    }
    { auto& m = m_translations[QStringLiteral("retrigger_restart")];
      m.insert(QStringLiteral("ar"), QStringLiteral("إعادة من الأول"));
      m.insert(QStringLiteral("en"), QStringLiteral("Restart"));
      m.insert(QStringLiteral("es"), QStringLiteral("Reiniciar"));
      m.insert(QStringLiteral("fr"), QStringLiteral("Redémarrer"));
      m.insert(QStringLiteral("de"), QStringLiteral("Neu starten"));
      m.insert(QStringLiteral("tr"), QStringLiteral("Yeniden Başlat"));
      m.insert(QStringLiteral("ru"), QStringLiteral("Перезапустить"));
      m.insert(QStringLiteral("id"), QStringLiteral("Mulai Ulang"));
      m.insert(QStringLiteral("fa"), QStringLiteral("شروع مجدد"));
      m.insert(QStringLiteral("ur"), QStringLiteral("دوبارہ شروع"));
    }
    { auto& m = m_translations[QStringLiteral("retrigger_queue")];
      m.insert(QStringLiteral("ar"), QStringLiteral("انتظار ثم تشغيل"));
      m.insert(QStringLiteral("en"), QStringLiteral("Wait & Run Next"));
      m.insert(QStringLiteral("es"), QStringLiteral("Esperar y ejecutar"));
      m.insert(QStringLiteral("fr"), QStringLiteral("Attendre puis lancer"));
      m.insert(QStringLiteral("de"), QStringLiteral("Warten & Starten"));
      m.insert(QStringLiteral("tr"), QStringLiteral("Bekle ve Çalıştır"));
      m.insert(QStringLiteral("ru"), QStringLiteral("Ждать и запустить"));
      m.insert(QStringLiteral("id"), QStringLiteral("Tunggu & Jalankan"));
      m.insert(QStringLiteral("fa"), QStringLiteral("انتظار و اجرا"));
      m.insert(QStringLiteral("ur"), QStringLiteral("انتظار اور چلائیں"));
    }
    { auto& m = m_translations[QStringLiteral("lbl_hk_panic_all")];
      m.insert(QStringLiteral("ar"), QStringLiteral("إيقاف كل عمليات البرنامج فوراً (طوارئ)"));
      m.insert(QStringLiteral("en"), QStringLiteral("Emergency Stop All Program Actions"));
      m.insert(QStringLiteral("es"), QStringLiteral("Detener Todo de Emergencia"));
      m.insert(QStringLiteral("fr"), QStringLiteral("Arrêt d'Urgence Global"));
      m.insert(QStringLiteral("de"), QStringLiteral("Not-Aus für alle Aktionen"));
      m.insert(QStringLiteral("tr"), QStringLiteral("Tüm Programı Acil Durdur"));
      m.insert(QStringLiteral("ru"), QStringLiteral("Экстренная остановка всего"));
      m.insert(QStringLiteral("id"), QStringLiteral("Hentikan Semua Secara Darurat"));
      m.insert(QStringLiteral("fa"), QStringLiteral("توقف اضطراری کل برنامه"));
      m.insert(QStringLiteral("ur"), QStringLiteral("پروگرام کا ہنگامی مکمل اسٹاپ"));
    }
    { auto& m = m_translations[QStringLiteral("sandbox_hint_status")];
      m.insert(QStringLiteral("ar"), QStringLiteral("اضغط هنا أو شغّل تجربة ماكرو بالأعلى — سيظهر هنا فوراً كل زرار أو كليك ماوس يستقبله النظام!"));
      m.insert(QStringLiteral("en"), QStringLiteral("Click here or run a macro test above — every received key press or mouse click will appear live!"));
      m.insert(QStringLiteral("es"), QStringLiteral("Haz clic aquí o ejecuta una prueba arriba — ¡cada tecla o clic recibido aparecerá en vivo!"));
      m.insert(QStringLiteral("fr"), QStringLiteral("Cliquez ici ou lancez un test ci-dessus — chaque touche ou clic reçu s'affichera en direct !"));
      m.insert(QStringLiteral("de"), QStringLiteral("Hier klicken oder oben einen Test starten — jeder empfangene Tastendruck oder Mausklick erscheint sofort!"));
      m.insert(QStringLiteral("tr"), QStringLiteral("Buraya tıklayın veya yukarıdan bir test başlatın — alınan her tuş ve fare tıklaması canlı görünecektir!"));
      m.insert(QStringLiteral("ru"), QStringLiteral("Нажмите сюда или запустите тест выше — каждое нажатие клавиши или клик мыши появится здесь!"));
      m.insert(QStringLiteral("id"), QStringLiteral("Klik di sini atau jalankan uji makro di atas — setiap tombol atau klik mouse akan langsung tampil!"));
      m.insert(QStringLiteral("fa"), QStringLiteral("اینجا کلیک کنید یا تست بالا را اجرا کنید — هر کلید یا کلیک دریافتی فوراً نمایش داده می‌شود!"));
      m.insert(QStringLiteral("ur"), QStringLiteral("یہاں کلک کریں یا اوپر سے ٹیسٹ چلائیں — موصول ہونے والا ہر بٹن یا ماؤس کلک فوراً نظر آئے گا!"));
    }
    { auto& m = m_translations[QStringLiteral("sandbox_input_placeholder")];
      m.insert(QStringLiteral("ar"), QStringLiteral("حقل كتابة تجريبي (جرّب تشغيل الماكرو هنا لترى الحروف والأزرار تكتب أمامك مباشرة)..."));
      m.insert(QStringLiteral("en"), QStringLiteral("Live test input field (run your macro here to see keys typed directly in front of you)..."));
      m.insert(QStringLiteral("es"), QStringLiteral("Campo de prueba en vivo (ejecuta tu macro aquí para ver las teclas escritas directamente)..."));
      m.insert(QStringLiteral("fr"), QStringLiteral("Champ de saisie de test en direct (lancez votre macro ici pour voir les touches saisies directement)..."));
      m.insert(QStringLiteral("de"), QStringLiteral("Live-Test-Eingabefeld (Makro hier ausführen, um getippte Tasten direkt zu sehen)..."));
      m.insert(QStringLiteral("tr"), QStringLiteral("Canlı test giriş alanı (tuşların doğrudan yazıldığını görmek için makronuzu burada çalıştırın)..."));
      m.insert(QStringLiteral("ru"), QStringLiteral("Поле живого ввода (запустите макрос здесь, чтобы сразу увидеть нажатия клавиш)..."));
      m.insert(QStringLiteral("id"), QStringLiteral("Kolom ketik uji langsung (jalankan makro di sini untuk melihat tombol diketik langsung)..."));
      m.insert(QStringLiteral("fa"), QStringLiteral("فیلد تایپ آزمایشی زنده (ماکرو را اینجا اجرا کنید تا کلیدها را مستقیم ببینید)..."));
      m.insert(QStringLiteral("ur"), QStringLiteral("لائیو ٹیسٹ ان پٹ فیلڈ (براہ راست کیز ٹائپ ہوتے دیکھنے کے لیے یہاں میکرو چلائیں)..."));
    }
    { auto& m = m_translations[QStringLiteral("test_quick_demo_label")];
      m.insert(QStringLiteral("ar"), QStringLiteral("⚡ اختبار سريع تلقائي (ضغطة W ثم Space)"));
      m.insert(QStringLiteral("en"), QStringLiteral("⚡ Quick Built-in Test (Tap W then Space)"));
      m.insert(QStringLiteral("es"), QStringLiteral("⚡ Prueba Rápida Integrada (Pulsar W y Espacio)"));
      m.insert(QStringLiteral("fr"), QStringLiteral("⚡ Test Rapide Intégré (Touche W puis Espace)"));
      m.insert(QStringLiteral("de"), QStringLiteral("⚡ Integrierter Schnelltest (W dann Leertaste)"));
      m.insert(QStringLiteral("tr"), QStringLiteral("⚡ Hızlı Dahili Test (W ardından Boşluk)"));
      m.insert(QStringLiteral("ru"), QStringLiteral("⚡ Быстрый встроенный тест (W затем Пробел)"));
      m.insert(QStringLiteral("id"), QStringLiteral("⚡ Uji Cepat Bawaan (Tekan W lalu Spasi)"));
      m.insert(QStringLiteral("fa"), QStringLiteral("⚡ تست سریع خودکار (کلید W سپس Space)"));
      m.insert(QStringLiteral("ur"), QStringLiteral("⚡ فوری بلٹ ان ٹیسٹ (پہلے W پھر Space)"));
    }
    { auto& m = m_translations[QStringLiteral("test_prefix_smart_key")];
      m.insert(QStringLiteral("ar"), QStringLiteral("⚡ ماكرو ذكي:"));
      m.insert(QStringLiteral("en"), QStringLiteral("⚡ Smart Key:"));
      m.insert(QStringLiteral("es"), QStringLiteral("⚡ Tecla Inteligente:"));
      m.insert(QStringLiteral("fr"), QStringLiteral("⚡ Touche Intelligente :"));
      m.insert(QStringLiteral("de"), QStringLiteral("⚡ Smart-Taste:"));
      m.insert(QStringLiteral("tr"), QStringLiteral("⚡ Akıllı Tuş:"));
      m.insert(QStringLiteral("ru"), QStringLiteral("⚡ Умная клавиша:"));
      m.insert(QStringLiteral("id"), QStringLiteral("⚡ Tombol Pintar:"));
      m.insert(QStringLiteral("fa"), QStringLiteral("⚡ کلید هوشمند:"));
      m.insert(QStringLiteral("ur"), QStringLiteral("⚡ اسمارٹ کی:"));
    }
    { auto& m = m_translations[QStringLiteral("test_prefix_saved_rec")];
      m.insert(QStringLiteral("ar"), QStringLiteral("⏺ تسجيل محفوظ:"));
      m.insert(QStringLiteral("en"), QStringLiteral("⏺ Saved Recording:"));
      m.insert(QStringLiteral("es"), QStringLiteral("⏺ Grabación Guardada:"));
      m.insert(QStringLiteral("fr"), QStringLiteral("⏺ Enregistrement Sauvegardé :"));
      m.insert(QStringLiteral("de"), QStringLiteral("⏺ Gespeicherte Aufnahme:"));
      m.insert(QStringLiteral("tr"), QStringLiteral("⏺ Kayıtlı Makro:"));
      m.insert(QStringLiteral("ru"), QStringLiteral("⏺ Сохранённая запись:"));
      m.insert(QStringLiteral("id"), QStringLiteral("⏺ Rekaman Tersimpan:"));
      m.insert(QStringLiteral("fa"), QStringLiteral("⏺ ضبط ذخیره شده:"));
      m.insert(QStringLiteral("ur"), QStringLiteral("⏺ محفوظ ریکارڈنگ:"));
    }
    { auto& m = m_translations[QStringLiteral("log_recv_mouse")];
      m.insert(QStringLiteral("ar"), QStringLiteral("🖱 تم استقبال"));
      m.insert(QStringLiteral("en"), QStringLiteral("🖱 Received"));
      m.insert(QStringLiteral("es"), QStringLiteral("🖱 Recibido"));
      m.insert(QStringLiteral("fr"), QStringLiteral("🖱 Reçu"));
      m.insert(QStringLiteral("de"), QStringLiteral("🖱 Empfangen"));
      m.insert(QStringLiteral("tr"), QStringLiteral("🖱 Alındı"));
      m.insert(QStringLiteral("ru"), QStringLiteral("🖱 Получено"));
      m.insert(QStringLiteral("id"), QStringLiteral("🖱 Diterima"));
      m.insert(QStringLiteral("fa"), QStringLiteral("🖱 دریافت شد"));
      m.insert(QStringLiteral("ur"), QStringLiteral("🖱 موصول ہوا"));
    }
    { auto& m = m_translations[QStringLiteral("log_mouse_click_at")];
      m.insert(QStringLiteral("ar"), QStringLiteral("كليك ماوس"));
      m.insert(QStringLiteral("en"), QStringLiteral("Mouse"));
      m.insert(QStringLiteral("es"), QStringLiteral("Clic de ratón"));
      m.insert(QStringLiteral("fr"), QStringLiteral("Clic souris"));
      m.insert(QStringLiteral("de"), QStringLiteral("Mausklick"));
      m.insert(QStringLiteral("tr"), QStringLiteral("Fare tıklaması"));
      m.insert(QStringLiteral("ru"), QStringLiteral("Клик мыши"));
      m.insert(QStringLiteral("id"), QStringLiteral("Klik mouse"));
      m.insert(QStringLiteral("fa"), QStringLiteral("کلیک ماوس"));
      m.insert(QStringLiteral("ur"), QStringLiteral("ماؤس کلک"));
    }
    { auto& m = m_translations[QStringLiteral("log_at_coords")];
      m.insert(QStringLiteral("ar"), QStringLiteral("عند الإحداثي"));
      m.insert(QStringLiteral("en"), QStringLiteral("at"));
      m.insert(QStringLiteral("es"), QStringLiteral("en"));
      m.insert(QStringLiteral("fr"), QStringLiteral("à"));
      m.insert(QStringLiteral("de"), QStringLiteral("bei"));
      m.insert(QStringLiteral("tr"), QStringLiteral("konum"));
      m.insert(QStringLiteral("ru"), QStringLiteral("в точке"));
      m.insert(QStringLiteral("id"), QStringLiteral("di"));
      m.insert(QStringLiteral("fa"), QStringLiteral("در مختصات"));
      m.insert(QStringLiteral("ur"), QStringLiteral("مقام پر"));
    }
    { auto& m = m_translations[QStringLiteral("log_recv_key")];
      m.insert(QStringLiteral("ar"), QStringLiteral("⌨ تم استقبال ضغطة زرار:"));
      m.insert(QStringLiteral("en"), QStringLiteral("⌨ Received Key Press:"));
      m.insert(QStringLiteral("es"), QStringLiteral("⌨ Tecla recibida:"));
      m.insert(QStringLiteral("fr"), QStringLiteral("⌨ Touche reçue :"));
      m.insert(QStringLiteral("de"), QStringLiteral("⌨ Taste empfangen:"));
      m.insert(QStringLiteral("tr"), QStringLiteral("⌨ Tuş Basımı Alındı:"));
      m.insert(QStringLiteral("ru"), QStringLiteral("⌨ Нажатие клавиши:"));
      m.insert(QStringLiteral("id"), QStringLiteral("⌨ Tombol Diterima:"));
      m.insert(QStringLiteral("fa"), QStringLiteral("⌨ کلید دریافت شد:"));
      m.insert(QStringLiteral("ur"), QStringLiteral("⌨ بٹن موصول ہوا:"));
    }
    { auto& m = m_translations[QStringLiteral("log_actual_engine")];
      m.insert(QStringLiteral("ar"), QStringLiteral("المحرك الفعلي:"));
      m.insert(QStringLiteral("en"), QStringLiteral("Actual Engine:"));
      m.insert(QStringLiteral("es"), QStringLiteral("Motor real:"));
      m.insert(QStringLiteral("fr"), QStringLiteral("Moteur réel :"));
      m.insert(QStringLiteral("de"), QStringLiteral("Aktiver Motor:"));
      m.insert(QStringLiteral("tr"), QStringLiteral("Gerçek Motor:"));
      m.insert(QStringLiteral("ru"), QStringLiteral("Фактический движок:"));
      m.insert(QStringLiteral("id"), QStringLiteral("Mesin Aktual:"));
      m.insert(QStringLiteral("fa"), QStringLiteral("موتور واقعی:"));
      m.insert(QStringLiteral("ur"), QStringLiteral("اصل انجن:"));
    }
    { auto& m = m_translations[QStringLiteral("log_fallback_from")];
      m.insert(QStringLiteral("ar"), QStringLiteral("بديل عن"));
      m.insert(QStringLiteral("en"), QStringLiteral("Fallback from"));
      m.insert(QStringLiteral("es"), QStringLiteral("Alternativa de"));
      m.insert(QStringLiteral("fr"), QStringLiteral("Repli depuis"));
      m.insert(QStringLiteral("de"), QStringLiteral("Fallback von"));
      m.insert(QStringLiteral("tr"), QStringLiteral("Yedek motor:"));
      m.insert(QStringLiteral("ru"), QStringLiteral("Замена для"));
      m.insert(QStringLiteral("id"), QStringLiteral("Pengganti dari"));
      m.insert(QStringLiteral("fa"), QStringLiteral("جایگزین برای"));
      m.insert(QStringLiteral("ur"), QStringLiteral("متبادل از"));
    }
    { auto& m = m_translations[QStringLiteral("log_auto_fallback_from")];
      m.insert(QStringLiteral("ar"), QStringLiteral("تحويل بديل تلقائي عن"));
      m.insert(QStringLiteral("en"), QStringLiteral("Auto-Fallback from"));
      m.insert(QStringLiteral("es"), QStringLiteral("Reemplazo automático de"));
      m.insert(QStringLiteral("fr"), QStringLiteral("Bascule auto depuis"));
      m.insert(QStringLiteral("de"), QStringLiteral("Auto-Fallback von"));
      m.insert(QStringLiteral("tr"), QStringLiteral("Otomatik yedek:"));
      m.insert(QStringLiteral("ru"), QStringLiteral("Автозамена для"));
      m.insert(QStringLiteral("id"), QStringLiteral("Alih otomatis dari"));
      m.insert(QStringLiteral("fa"), QStringLiteral("تغییر خودکار از"));
      m.insert(QStringLiteral("ur"), QStringLiteral("خودکار متبادل از"));
    }
    { auto& m = m_translations[QStringLiteral("log_selected_prefix")];
      m.insert(QStringLiteral("ar"), QStringLiteral("✓ تم اختيار:"));
      m.insert(QStringLiteral("en"), QStringLiteral("✓ Selected:"));
      m.insert(QStringLiteral("es"), QStringLiteral("✓ Seleccionado:"));
      m.insert(QStringLiteral("fr"), QStringLiteral("✓ Sélectionné :"));
      m.insert(QStringLiteral("de"), QStringLiteral("✓ Ausgewählt:"));
      m.insert(QStringLiteral("tr"), QStringLiteral("✓ Seçildi:"));
      m.insert(QStringLiteral("ru"), QStringLiteral("✓ Выбрано:"));
      m.insert(QStringLiteral("id"), QStringLiteral("✓ Dipilih:"));
      m.insert(QStringLiteral("fa"), QStringLiteral("✓ انتخاب شد:"));
      m.insert(QStringLiteral("ur"), QStringLiteral("✓ منتخب:"));
    }
    { auto& m = m_translations[QStringLiteral("log_actual_status")];
      m.insert(QStringLiteral("ar"), QStringLiteral("الحالة الفعلية:"));
      m.insert(QStringLiteral("en"), QStringLiteral("Actual Status:"));
      m.insert(QStringLiteral("es"), QStringLiteral("Estado real:"));
      m.insert(QStringLiteral("fr"), QStringLiteral("Statut réel :"));
      m.insert(QStringLiteral("de"), QStringLiteral("Tatsächlicher Status:"));
      m.insert(QStringLiteral("tr"), QStringLiteral("Gerçek Durum:"));
      m.insert(QStringLiteral("ru"), QStringLiteral("Фактический статус:"));
      m.insert(QStringLiteral("id"), QStringLiteral("Status Aktual:"));
      m.insert(QStringLiteral("fa"), QStringLiteral("وضعیت واقعی:"));
      m.insert(QStringLiteral("ur"), QStringLiteral("اصل صورتحال:"));
    }
    { auto& m = m_translations[QStringLiteral("log_run_combo_prefix")];
      m.insert(QStringLiteral("ar"), QStringLiteral("▶ بدء اختبار الماكرو بعد"));
      m.insert(QStringLiteral("en"), QStringLiteral("▶ Running Smart Key test in"));
      m.insert(QStringLiteral("es"), QStringLiteral("▶ Iniciando prueba de macro en"));
      m.insert(QStringLiteral("fr"), QStringLiteral("▶ Lancement du test de macro dans"));
      m.insert(QStringLiteral("de"), QStringLiteral("▶ Starte Makro-Test in"));
      m.insert(QStringLiteral("tr"), QStringLiteral("▶ Akıllı Tuş testi başlatılıyor:"));
      m.insert(QStringLiteral("ru"), QStringLiteral("▶ Запуск теста макроса через"));
      m.insert(QStringLiteral("id"), QStringLiteral("▶ Menjalankan uji Smart Key dalam"));
      m.insert(QStringLiteral("fa"), QStringLiteral("▶ اجرای تست ماکرو پس از"));
      m.insert(QStringLiteral("ur"), QStringLiteral("▶ اسمارٹ کی ٹیسٹ شروع:"));
    }
    { auto& m = m_translations[QStringLiteral("log_run_rec_prefix")];
      m.insert(QStringLiteral("ar"), QStringLiteral("▶ بدء اختبار التسجيل"));
      m.insert(QStringLiteral("en"), QStringLiteral("▶ Running Recording"));
      m.insert(QStringLiteral("es"), QStringLiteral("▶ Ejecutando grabación"));
      m.insert(QStringLiteral("fr"), QStringLiteral("▶ Lecture de l'enregistrement"));
      m.insert(QStringLiteral("de"), QStringLiteral("▶ Starte Aufnahme"));
      m.insert(QStringLiteral("tr"), QStringLiteral("▶ Kayıt çalıştırılıyor:"));
      m.insert(QStringLiteral("ru"), QStringLiteral("▶ Запуск записи"));
      m.insert(QStringLiteral("id"), QStringLiteral("▶ Menjalankan Rekaman"));
      m.insert(QStringLiteral("fa"), QStringLiteral("▶ اجرای ضبط"));
      m.insert(QStringLiteral("ur"), QStringLiteral("▶ ریکارڈنگ چلائی جا رہی ہے:"));
    }
    { auto& m = m_translations[QStringLiteral("log_run_quick_prefix")];
      m.insert(QStringLiteral("ar"), QStringLiteral("▶ تشغيل الاختبار السريع"));
      m.insert(QStringLiteral("en"), QStringLiteral("▶ Running Quick Test"));
      m.insert(QStringLiteral("es"), QStringLiteral("▶ Ejecutando prueba rápida"));
      m.insert(QStringLiteral("fr"), QStringLiteral("▶ Lancement du test rapide"));
      m.insert(QStringLiteral("de"), QStringLiteral("▶ Starte Schnelltest"));
      m.insert(QStringLiteral("tr"), QStringLiteral("▶ Hızlı test çalıştırılıyor"));
      m.insert(QStringLiteral("ru"), QStringLiteral("▶ Запуск быстрого теста"));
      m.insert(QStringLiteral("id"), QStringLiteral("▶ Menjalankan Uji Cepat"));
      m.insert(QStringLiteral("fa"), QStringLiteral("▶ اجرای تست سریع"));
      m.insert(QStringLiteral("ur"), QStringLiteral("▶ فوری ٹیسٹ شروع"));
    }
    { auto& m = m_translations[QStringLiteral("log_in_delay")];
      m.insert(QStringLiteral("ar"), QStringLiteral("بعد"));
      m.insert(QStringLiteral("en"), QStringLiteral("in"));
      m.insert(QStringLiteral("es"), QStringLiteral("en"));
      m.insert(QStringLiteral("fr"), QStringLiteral("dans"));
      m.insert(QStringLiteral("de"), QStringLiteral("in"));
      m.insert(QStringLiteral("tr"), QStringLiteral("süre:"));
      m.insert(QStringLiteral("ru"), QStringLiteral("через"));
      m.insert(QStringLiteral("id"), QStringLiteral("dalam"));
      m.insert(QStringLiteral("fa"), QStringLiteral("پس از"));
      m.insert(QStringLiteral("ur"), QStringLiteral("بعد"));
    }
    { auto& m = m_translations[QStringLiteral("log_via_actual_engine")];
      m.insert(QStringLiteral("ar"), QStringLiteral("عبر المحرك الفعلي:"));
      m.insert(QStringLiteral("en"), QStringLiteral("via actual engine:"));
      m.insert(QStringLiteral("es"), QStringLiteral("mediante motor real:"));
      m.insert(QStringLiteral("fr"), QStringLiteral("via le moteur réel :"));
      m.insert(QStringLiteral("de"), QStringLiteral("über aktiven Motor:"));
      m.insert(QStringLiteral("tr"), QStringLiteral("gerçek motor ile:"));
      m.insert(QStringLiteral("ru"), QStringLiteral("через фактический движок:"));
      m.insert(QStringLiteral("id"), QStringLiteral("melalui mesin aktual:"));
      m.insert(QStringLiteral("fa"), QStringLiteral("از طریق موتور واقعی:"));
      m.insert(QStringLiteral("ur"), QStringLiteral("اصل انجن کے ذریعے:"));
    }
    { auto& m = m_translations[QStringLiteral("eng_status_linux_win_fallback")];
      m.insert(QStringLiteral("ar"), QStringLiteral("محرك Linux (/dev/uinput) غير متاح على نظام ويندوز — تم التحويل تلقائياً إلى: Method 1 (SendInput Hardware ScanCode)"));
      m.insert(QStringLiteral("en"), QStringLiteral("Linux (/dev/uinput) is unavailable on Windows — automatically fell back to: Method 1 (SendInput Hardware ScanCode)"));
      m.insert(QStringLiteral("es"), QStringLiteral("Linux (/dev/uinput) no está disponible en Windows — cambio automático a: Método 1 (SendInput Hardware ScanCode)"));
      m.insert(QStringLiteral("fr"), QStringLiteral("Linux (/dev/uinput) est indisponible sous Windows — bascule automatique vers : Méthode 1 (SendInput Hardware ScanCode)"));
      m.insert(QStringLiteral("de"), QStringLiteral("Linux (/dev/uinput) ist unter Windows nicht verfügbar — automatischer Wechsel zu: Methode 1 (SendInput Hardware ScanCode)"));
      m.insert(QStringLiteral("tr"), QStringLiteral("Linux (/dev/uinput) Windows'ta kullanılamaz — otomatik olarak Yöntem 1'e (SendInput Hardware ScanCode) geçildi"));
      m.insert(QStringLiteral("ru"), QStringLiteral("Linux (/dev/uinput) недоступен в Windows — автоматический переход на: Метод 1 (SendInput Hardware ScanCode)"));
      m.insert(QStringLiteral("id"), QStringLiteral("Linux (/dev/uinput) tidak tersedia di Windows — otomatis beralih ke: Metode 1 (SendInput Hardware ScanCode)"));
      m.insert(QStringLiteral("fa"), QStringLiteral("موتور Linux (/dev/uinput) در ویندوز در دسترس نیست — تغییر خودکار به: روش ۱ (SendInput Hardware ScanCode)"));
      m.insert(QStringLiteral("ur"), QStringLiteral("Linux (/dev/uinput) ونڈوز پر دستیاب نہیں — خودکار طور پر Method 1 (SendInput Hardware ScanCode) پر منتقل"));
    }
    { auto& m = m_translations[QStringLiteral("eng_status_linux_active")];
      m.insert(QStringLiteral("ar"), QStringLiteral("محرك نواة لينكس /dev/uinput + X11 XTest للكيبورد والماوس الفيزيائي نشط"));
      m.insert(QStringLiteral("en"), QStringLiteral("Linux Kernel /dev/uinput Virtual USB Keyboard & Mouse + XTest Active"));
      m.insert(QStringLiteral("es"), QStringLiteral("Motor de Kernel Linux /dev/uinput Teclado y Ratón USB Virtual + XTest activo"));
      m.insert(QStringLiteral("fr"), QStringLiteral("Noyau Linux /dev/uinput Clavier et Souris USB Virtuels + XTest actif"));
      m.insert(QStringLiteral("de"), QStringLiteral("Linux-Kernel /dev/uinput Virtuelle USB-Tastatur & Maus + XTest aktiv"));
      m.insert(QStringLiteral("tr"), QStringLiteral("Linux Çekirdeği /dev/uinput Sanal USB Klavye ve Fare + XTest Aktif"));
      m.insert(QStringLiteral("ru"), QStringLiteral("Ядро Linux /dev/uinput Виртуальная USB-клавиатура и мышь + XTest активен"));
      m.insert(QStringLiteral("id"), QStringLiteral("Kernel Linux /dev/uinput Keyboard & Mouse USB Virtual + XTest Aktif"));
      m.insert(QStringLiteral("fa"), QStringLiteral("موتور هسته لینوکس /dev/uinput کیبورد و ماوس USB مجازی + XTest فعال است"));
      m.insert(QStringLiteral("ur"), QStringLiteral("Linux کرنل /dev/uinput ورچوئل USB کی بورڈ اور ماؤس + XTest فعال ہے"));
    }
    { auto& m = m_translations[QStringLiteral("eng_status_non_win")];
      m.insert(QStringLiteral("ar"), QStringLiteral("محركات Win32 مخصصة لويندوز — يعمل حالياً على جسر النظام البديل (pynput)"));
      m.insert(QStringLiteral("en"), QStringLiteral("Win32 engines require Windows — currently using cross-platform pynput fallback"));
      m.insert(QStringLiteral("es"), QStringLiteral("Los motores Win32 requieren Windows — usando respaldo multiplataforma pynput"));
      m.insert(QStringLiteral("fr"), QStringLiteral("Les moteurs Win32 nécessitent Windows — utilisation du repli multiplateforme pynput"));
      m.insert(QStringLiteral("de"), QStringLiteral("Win32-Motoren erfordern Windows — aktuell wird der plattformübergreifende pynput-Fallback verwendet"));
      m.insert(QStringLiteral("tr"), QStringLiteral("Win32 motorları Windows gerektirir — şu anda çapraz platform pynput yedeği kullanılıyor"));
      m.insert(QStringLiteral("ru"), QStringLiteral("Движки Win32 требуют Windows — используется кроссплатформенный резерв pynput"));
      m.insert(QStringLiteral("id"), QStringLiteral("Mesin Win32 memerlukan Windows — saat ini menggunakan cadangan pynput lintas platform"));
      m.insert(QStringLiteral("fa"), QStringLiteral("موتورهای Win32 مختص ویندوز هستند — در حال استفاده از پل جایگزین pynput"));
      m.insert(QStringLiteral("ur"), QStringLiteral("Win32 انجنز کے لیے ونڈوز درکار ہے — فی الحال pynput متبادل استعمال ہو رہا ہے"));
    }
    { auto& m = m_translations[QStringLiteral("eng_status_interception_active")];
      m.insert(QStringLiteral("ar"), QStringLiteral("🟢 محرك الكيرنل Ring-0 (Interception kbdclass) نشط فعلياً — ضغطة كيبورد فيزيائي بدون LLKHF_INJECTED!"));
      m.insert(QStringLiteral("en"), QStringLiteral("🟢 Ring-0 Kernel Driver (Interception kbdclass) ACTIVE — True Physical Keystroke without LLKHF_INJECTED!"));
      m.insert(QStringLiteral("es"), QStringLiteral("🟢 Controlador Kernel Ring-0 (Interception kbdclass) ACTIVO — ¡Pulsación física real sin LLKHF_INJECTED!"));
      m.insert(QStringLiteral("fr"), QStringLiteral("🟢 Pilote Noyau Ring-0 (Interception kbdclass) ACTIF — Frappe physique réelle sans LLKHF_INJECTED !"));
      m.insert(QStringLiteral("de"), QStringLiteral("🟢 Ring-0 Kernel-Treiber (Interception kbdclass) AKTIV — Echter Hardware-Tastendruck ohne LLKHF_INJECTED!"));
      m.insert(QStringLiteral("tr"), QStringLiteral("🟢 Ring-0 Çekirdek Sürücüsü (Interception kbdclass) AKTİF — LLKHF_INJECTED olmadan gerçek donanım basımı!"));
      m.insert(QStringLiteral("ru"), QStringLiteral("🟢 Драйвер ядра Ring-0 (Interception kbdclass) АКТИВЕН — физическое нажатие без флага LLKHF_INJECTED!"));
      m.insert(QStringLiteral("id"), QStringLiteral("🟢 Driver Kernel Ring-0 (Interception kbdclass) AKTIF — Tekanan fisik asli tanpa LLKHF_INJECTED!"));
      m.insert(QStringLiteral("fa"), QStringLiteral("🟢 درایور کرنل Ring-0 (Interception kbdclass) فعال است — فشردن فیزیکی واقعی بدون LLKHF_INJECTED!"));
      m.insert(QStringLiteral("ur"), QStringLiteral("🟢 Ring-0 کرنل ڈرائیور (Interception kbdclass) فعال ہے — LLKHF_INJECTED کے بغیر اصلی ہارڈویئر کلک!"));
    }
    { auto& m = m_translations[QStringLiteral("eng_status_interception_fallback")];
      m.insert(QStringLiteral("ar"), QStringLiteral("🟡 درايفر الكيرنل Interception غير نشط حالياً — يعمل على البديل الآمن: Method 1 (SendInput ScanCode). ثبّت/افحص من الإعدادات."));
      m.insert(QStringLiteral("en"), QStringLiteral("🟡 Ring-0 Interception is not active yet — using safe fallback: Method 1 (SendInput ScanCode). Check/Install in Settings."));
      m.insert(QStringLiteral("es"), QStringLiteral("🟡 Ring-0 Interception aún no está activo — usando respaldo seguro: Método 1 (SendInput ScanCode). Revisa/Instala en Ajustes."));
      m.insert(QStringLiteral("fr"), QStringLiteral("🟡 Le pilote Ring-0 Interception n'est pas encore actif — repli sûr : Méthode 1 (SendInput ScanCode). Vérifiez dans Paramètres."));
      m.insert(QStringLiteral("de"), QStringLiteral("🟡 Ring-0 Interception ist noch nicht aktiv — sicherer Fallback: Methode 1 (SendInput ScanCode). In Einstellungen prüfen."));
      m.insert(QStringLiteral("tr"), QStringLiteral("🟡 Ring-0 Interception henüz aktif değil — güvenli yedek kullanılıyor: Yöntem 1 (SendInput ScanCode). Ayarlardan kontrol edin."));
      m.insert(QStringLiteral("ru"), QStringLiteral("🟡 Драйвер Ring-0 Interception ещё не активен — безопасный резерв: Метод 1 (SendInput ScanCode). Проверьте в Настройках."));
      m.insert(QStringLiteral("id"), QStringLiteral("🟡 Ring-0 Interception belum aktif — memakai cadangan aman: Metode 1 (SendInput ScanCode). Cek/Pasang di Pengaturan."));
      m.insert(QStringLiteral("fa"), QStringLiteral("🟡 درایور کرنل Interception هنوز فعال نیست — استفاده از جایگزین ایمن: روش ۱ (SendInput ScanCode). در تنظیمات بررسی کنید."));
      m.insert(QStringLiteral("ur"), QStringLiteral("🟡 Ring-0 Interception ابھی فعال نہیں — محفوظ متبادل فعال ہے: Method 1 (SendInput ScanCode)۔ سیٹنگز میں چیک کریں۔"));
    }
    { auto& m = m_translations[QStringLiteral("eng_status_ps2_active")];
      m.insert(QStringLiteral("ar"), QStringLiteral("🟢 محرك متحكم البوردة الفيزيائي PS/2 Port 0x60/0x64 (InpOutx64) نشط فعلياً!"));
      m.insert(QStringLiteral("en"), QStringLiteral("🟢 Motherboard PS/2 Port 0x60/0x64 Controller (InpOutx64) ACTIVE!"));
      m.insert(QStringLiteral("es"), QStringLiteral("🟢 Controlador de puerto PS/2 0x60/0x64 (InpOutx64) ACTIVO!"));
      m.insert(QStringLiteral("fr"), QStringLiteral("🟢 Contrôleur matériel PS/2 Port 0x60/0x64 (InpOutx64) ACTIF !"));
      m.insert(QStringLiteral("de"), QStringLiteral("🟢 Motherboard PS/2-Port 0x60/0x64 Controller (InpOutx64) AKTIV!"));
      m.insert(QStringLiteral("tr"), QStringLiteral("🟢 Anakart PS/2 Port 0x60/0x64 Denetleyicisi (InpOutx64) AKTİF!"));
      m.insert(QStringLiteral("ru"), QStringLiteral("🟢 Контроллер порта PS/2 0x60/0x64 (InpOutx64) АКТИВЕН!"));
      m.insert(QStringLiteral("id"), QStringLiteral("🟢 Pengontrol Port PS/2 0x60/0x64 Motherboard (InpOutx64) AKTIF!"));
      m.insert(QStringLiteral("fa"), QStringLiteral("🟢 کنترلر پورت فیزیکی PS/2 0x60/0x64 مادربرد (InpOutx64) فعال است!"));
      m.insert(QStringLiteral("ur"), QStringLiteral("🟢 مدر بورڈ PS/2 پورٹ 0x60/0x64 کنٹرولر (InpOutx64) فعال ہے!"));
    }
    { auto& m = m_translations[QStringLiteral("eng_status_ps2_fallback")];
      m.insert(QStringLiteral("ar"), QStringLiteral("🟡 مكتبة inpoutx64.dll غير موجودة — يعمل حالياً على البديل الآمن: Method 1 (SendInput ScanCode)."));
      m.insert(QStringLiteral("en"), QStringLiteral("🟡 inpoutx64.dll not found — using safe fallback: Method 1 (SendInput ScanCode)."));
      m.insert(QStringLiteral("es"), QStringLiteral("🟡 No se encontró inpoutx64.dll — usando respaldo seguro: Método 1 (SendInput ScanCode)."));
      m.insert(QStringLiteral("fr"), QStringLiteral("🟡 inpoutx64.dll introuvable — repli sûr : Méthode 1 (SendInput ScanCode)."));
      m.insert(QStringLiteral("de"), QStringLiteral("🟡 inpoutx64.dll nicht gefunden — sicherer Fallback: Methode 1 (SendInput ScanCode)."));
      m.insert(QStringLiteral("tr"), QStringLiteral("🟡 inpoutx64.dll bulunamadı — güvenli yedek kullanılıyor: Yöntem 1 (SendInput ScanCode)."));
      m.insert(QStringLiteral("ru"), QStringLiteral("🟡 Библиотека inpoutx64.dll не найдена — безопасный резерв: Метод 1 (SendInput ScanCode)."));
      m.insert(QStringLiteral("id"), QStringLiteral("🟡 inpoutx64.dll tidak ditemukan — memakai cadangan aman: Metode 1 (SendInput ScanCode)."));
      m.insert(QStringLiteral("fa"), QStringLiteral("🟡 فایل inpoutx64.dll یافت نشد — استفاده از جایگزین ایمن: روش ۱ (SendInput ScanCode)."));
      m.insert(QStringLiteral("ur"), QStringLiteral("🟡 inpoutx64.dll نہیں ملی — محفوظ متبادل فعال ہے: Method 1 (SendInput ScanCode)۔"));
    }
    { auto& m = m_translations[QStringLiteral("eng_status_vhid_active")];
      m.insert(QStringLiteral("ar"), QStringLiteral("🟢 محرك USB HID الفيزيائي نشط: حزمة 8-Byte 6KRO القياسية (Usage Page 0x07) + SendInput Hybrid"));
      m.insert(QStringLiteral("en"), QStringLiteral("🟢 USB HID 6KRO Hardware Keyboard Report Active (Usage Page 0x07 + Hybrid ScanCode)"));
      m.insert(QStringLiteral("es"), QStringLiteral("🟢 Informe de teclado hardware USB HID 6KRO activo (Usage Page 0x07 + ScanCode híbrido)"));
      m.insert(QStringLiteral("fr"), QStringLiteral("🟢 Rapport clavier matériel USB HID 6KRO actif (Usage Page 0x07 + ScanCode hybride)"));
      m.insert(QStringLiteral("de"), QStringLiteral("🟢 USB HID 6KRO Hardware-Tastaturbericht aktiv (Usage Page 0x07 + Hybrid ScanCode)"));
      m.insert(QStringLiteral("tr"), QStringLiteral("🟢 USB HID 6KRO Donanım Klavye Raporu Aktif (Usage Page 0x07 + Hibrit ScanCode)"));
      m.insert(QStringLiteral("ru"), QStringLiteral("🟢 Аппаратный отчёт USB HID 6KRO активен (Usage Page 0x07 + Hybrid ScanCode)"));
      m.insert(QStringLiteral("id"), QStringLiteral("🟢 Laporan Keyboard Perangkat Keras USB HID 6KRO Aktif (Usage Page 0x07 + Hybrid ScanCode)"));
      m.insert(QStringLiteral("fa"), QStringLiteral("🟢 گزارش سخت‌افزاری کیبورد USB HID 6KRO فعال است (Usage Page 0x07 + Hybrid ScanCode)"));
      m.insert(QStringLiteral("ur"), QStringLiteral("🟢 USB HID 6KRO ہارڈویئر کی بورڈ رپورٹ فعال ہے (Usage Page 0x07 + Hybrid ScanCode)"));
    }
    { auto& m = m_translations[QStringLiteral("eng_status_atomic_active")];
      m.insert(QStringLiteral("ar"), QStringLiteral("🟢 محرك الحزمة الذرية INPUT[2] + QPC Sub-ms Spin-Wait نشط بأعلى سرعة استجابة"));
      m.insert(QStringLiteral("en"), QStringLiteral("🟢 Atomic Contiguous INPUT[2] Hardware Queue + QPC Spin-Wait Active"));
      m.insert(QStringLiteral("es"), QStringLiteral("🟢 Cola hardware atómica INPUT[2] + espera de precisión QPC activa"));
      m.insert(QStringLiteral("fr"), QStringLiteral("🟢 File matérielle atomique INPUT[2] + attente de précision QPC active"));
      m.insert(QStringLiteral("de"), QStringLiteral("🟢 Atomare INPUT[2] Hardware-Warteschlange + QPC-Spin-Wait aktiv"));
      m.insert(QStringLiteral("tr"), QStringLiteral("🟢 Atomik INPUT[2] Donanım Kuyruğu + QPC Hassas Bekleme Aktif"));
      m.insert(QStringLiteral("ru"), QStringLiteral("🟢 Атомарная аппаратная очередь INPUT[2] + QPC Spin-Wait активна"));
      m.insert(QStringLiteral("id"), QStringLiteral("🟢 Antrean Perangkat Keras Atomik INPUT[2] + QPC Spin-Wait Aktif"));
      m.insert(QStringLiteral("fa"), QStringLiteral("🟢 صف سخت‌افزاری اتمیک INPUT[2] + انتظار دقیق QPC فعال است"));
      m.insert(QStringLiteral("ur"), QStringLiteral("🟢 اٹامک ہارڈویئر قطار INPUT[2] + QPC اسپن ویٹ فعال ہے"));
    }
    { auto& m = m_translations[QStringLiteral("eng_status_native_active")];
      m.insert(QStringLiteral("ar"), QStringLiteral("🟢 المحرك نشط ويعمل مباشرة على نظام ويندوز بدون تحويل بديل"));
      m.insert(QStringLiteral("en"), QStringLiteral("🟢 Engine is active and running natively on Windows"));
      m.insert(QStringLiteral("es"), QStringLiteral("🟢 El motor está activo y funcionando de forma nativa en Windows"));
      m.insert(QStringLiteral("fr"), QStringLiteral("🟢 Le moteur est actif et fonctionne nativement sous Windows"));
      m.insert(QStringLiteral("de"), QStringLiteral("🟢 Der Motor ist aktiv und läuft nativ unter Windows"));
      m.insert(QStringLiteral("tr"), QStringLiteral("🟢 Motor aktif ve Windows üzerinde doğrudan çalışıyor"));
      m.insert(QStringLiteral("ru"), QStringLiteral("🟢 Движок активен и работает нативно в Windows"));
      m.insert(QStringLiteral("id"), QStringLiteral("🟢 Mesin aktif dan berjalan secara native di Windows"));
      m.insert(QStringLiteral("fa"), QStringLiteral("🟢 موتور فعال است و مستقیماً روی ویندوز اجرا می‌شود"));
      m.insert(QStringLiteral("ur"), QStringLiteral("🟢 انجن فعال ہے اور ونڈوز پر براہ راست کام کر رہا ہے"));
    }
    { auto& m = m_translations[QStringLiteral("lbl_admin_ok")];
      m.insert(QStringLiteral("ar"), QStringLiteral("حالة الصلاحيات: يعمل كمسؤول — جاهز لجميع الألعاب والأنظمة."));
      m.insert(QStringLiteral("en"), QStringLiteral("Privilege Status: Running as Administrator — full game & system hook access."));
      m.insert(QStringLiteral("es"), QStringLiteral("Estado de privilegios: Ejecutando como Administrador — acceso total a juegos y sistema."));
      m.insert(QStringLiteral("fr"), QStringLiteral("Statut des privilèges : Exécuté en Administrateur — accès complet aux jeux et au système."));
      m.insert(QStringLiteral("de"), QStringLiteral("Berechtigungsstatus: Als Administrator ausgeführt — voller Spiel- & Systemzugriff."));
      m.insert(QStringLiteral("tr"), QStringLiteral("Yetki Durumu: Yönetici olarak çalışıyor — tam oyun ve sistem erişimi."));
      m.insert(QStringLiteral("ru"), QStringLiteral("Права доступа: Запуск от Администратора — полный доступ к играм и системе."));
      m.insert(QStringLiteral("id"), QStringLiteral("Status Hak Akses: Berjalan sebagai Administrator — akses penuh ke game & sistem."));
      m.insert(QStringLiteral("fa"), QStringLiteral("وضعیت دسترسی: اجرا با سطح مدیر (Administrator) — دسترسی کامل به بازی‌ها و سیستم."));
      m.insert(QStringLiteral("ur"), QStringLiteral("اختیارات کی صورتحال: بطور ایڈمنسٹریٹر فعال — گیمز اور سسٹم تک مکمل رسائی۔"));
    }
    { auto& m = m_translations[QStringLiteral("lbl_admin_warn")];
      m.insert(QStringLiteral("ar"), QStringLiteral("تنبيه: البرنامج يعمل كمستخدم عادي. لتشغيل الماكرو داخل الألعاب التي تعمل كمسؤول، شغّل البرنامج كمسؤول."));
      m.insert(QStringLiteral("en"), QStringLiteral("Notice: Running as Standard User. Run as Administrator for elevated games."));
      m.insert(QStringLiteral("es"), QStringLiteral("Aviso: Ejecutando como usuario estándar. Ejecuta como Administrador para juegos elevados."));
      m.insert(QStringLiteral("fr"), QStringLiteral("Avis : Exécuté en utilisateur standard. Lancez en Administrateur pour les jeux protégés."));
      m.insert(QStringLiteral("de"), QStringLiteral("Hinweis: Läuft als Standardbenutzer. Für erhöhte Spiele als Administrator starten."));
      m.insert(QStringLiteral("tr"), QStringLiteral("Uyarı: Standart kullanıcı olarak çalışıyor. Yönetici haklı oyunlar için Yönetici olarak çalıştırın."));
      m.insert(QStringLiteral("ru"), QStringLiteral("Внимание: Запуск от обычного пользователя. Запустите от Администратора для игр с повышенными правами."));
      m.insert(QStringLiteral("id"), QStringLiteral("Peringatan: Berjalan sebagai Pengguna Standar. Jalankan sebagai Administrator untuk game dengan hak tinggi."));
      m.insert(QStringLiteral("fa"), QStringLiteral("توجه: برنامه با سطح کاربر عادی اجرا شده است. برای بازی‌های دارای دسترسی بالا، با سطح Administrator اجرا کنید."));
      m.insert(QStringLiteral("ur"), QStringLiteral("تنبیہ: عام صارف کے طور پر فعال ہے۔ ایڈمن گیمز کے لیے بطور ایڈمنسٹریٹر چلائیں۔"));
    }
    { auto& m = m_translations[QStringLiteral("toast_settings_reset")];
      m.insert(QStringLiteral("ar"), QStringLiteral("تمت استعادة الإعدادات الافتراضية"));
      m.insert(QStringLiteral("en"), QStringLiteral("Settings restored to defaults"));
      m.insert(QStringLiteral("es"), QStringLiteral("Ajustes restablecidos a los valores predeterminados"));
      m.insert(QStringLiteral("fr"), QStringLiteral("Paramètres réinitialisés par défaut"));
      m.insert(QStringLiteral("de"), QStringLiteral("Einstellungen auf Standardwerte zurückgesetzt"));
      m.insert(QStringLiteral("tr"), QStringLiteral("Ayarlar varsayılana sıfırlandı"));
      m.insert(QStringLiteral("ru"), QStringLiteral("Настройки сброшены по умолчанию"));
      m.insert(QStringLiteral("id"), QStringLiteral("Pengaturan dikembalikan ke bawaan"));
      m.insert(QStringLiteral("fa"), QStringLiteral("تنظیمات به حالت پیش‌فرض بازگردانده شد"));
      m.insert(QStringLiteral("ur"), QStringLiteral("سیٹنگز ڈیفالٹ پر بحال کر دی گئیں"));
    }
    { auto& m = m_translations[QStringLiteral("keycap_press_prompt")];
      m.insert(QStringLiteral("ar"), QStringLiteral("اضغط الزرار أو اختصار (2 أو 3 معاً)..."));
      m.insert(QStringLiteral("en"), QStringLiteral("Press key or chord (2–3 keys)..."));
      m.insert(QStringLiteral("es"), QStringLiteral("Pulsa una tecla o combinación (2–3 teclas)..."));
      m.insert(QStringLiteral("fr"), QStringLiteral("Appuyez sur une touche ou combinaison (2–3 touches)..."));
      m.insert(QStringLiteral("de"), QStringLiteral("Taste oder Tastenkombination (2–3 Tasten) drücken..."));
      m.insert(QStringLiteral("tr"), QStringLiteral("Bir tuşa veya kombinasyona (2–3 tuş) basın..."));
      m.insert(QStringLiteral("ru"), QStringLiteral("Нажмите клавишу или комбинацию (2–3 клавиши)..."));
      m.insert(QStringLiteral("id"), QStringLiteral("Tekan tombol atau kombinasi (2–3 tombol)..."));
      m.insert(QStringLiteral("fa"), QStringLiteral("کلید یا ترکیب (۲ تا ۳ کلید) را فشار دهید..."));
      m.insert(QStringLiteral("ur"), QStringLiteral("بٹن یا کمبینیشن (2–3 بٹن) دبائیں..."));
    }
    { auto& m = m_translations[QStringLiteral("keycap_manual_prompt")];
      m.insert(QStringLiteral("ar"), QStringLiteral("بدون زرار (تشغيل يدوي) — اضغط هنا لتحديد اختصار"));
      m.insert(QStringLiteral("en"), QStringLiteral("No Key (Manual Run) — Click to bind"));
      m.insert(QStringLiteral("es"), QStringLiteral("Sin tecla (Ejecución manual) — Clic para asignar"));
      m.insert(QStringLiteral("fr"), QStringLiteral("Aucune touche (Lancement manuel) — Cliquer pour assigner"));
      m.insert(QStringLiteral("de"), QStringLiteral("Keine Taste (Manueller Start) — Klicken zum Zuweisen"));
      m.insert(QStringLiteral("tr"), QStringLiteral("Tuş Yok (Manuel Çalıştır) — Atamak için tıklayın"));
      m.insert(QStringLiteral("ru"), QStringLiteral("Без клавиши (ручной запуск) — Нажмите для назначения"));
      m.insert(QStringLiteral("id"), QStringLiteral("Tanpa Tombol (Manual) — Klik untuk atur"));
      m.insert(QStringLiteral("fa"), QStringLiteral("بدون کلید (اجرای دستی) — برای تعیین کلیک کنید"));
      m.insert(QStringLiteral("ur"), QStringLiteral("بغیر بٹن (دستی چلائیں) — تفویض کرنے کے لیے کلک کریں"));
    }
    { auto& m = m_translations[QStringLiteral("keycap_chord_prompt")];
      m.insert(QStringLiteral("ar"), QStringLiteral("الاختصار المحدد: [ {key} ] (اضغط لتغييره)"));
      m.insert(QStringLiteral("en"), QStringLiteral("Bound Shortcut: [ {key} ] (Click to change)"));
      m.insert(QStringLiteral("es"), QStringLiteral("Atajo asignado: [ {key} ] (Clic para cambiar)"));
      m.insert(QStringLiteral("fr"), QStringLiteral("Raccourci assigné : [ {key} ] (Cliquer pour modifier)"));
      m.insert(QStringLiteral("de"), QStringLiteral("Zugewiesener Kurzbefehl: [ {key} ] (Klicken zum Ändern)"));
      m.insert(QStringLiteral("tr"), QStringLiteral("Atanan Kısayol: [ {key} ] (Değiştirmek için tıklayın)"));
      m.insert(QStringLiteral("ru"), QStringLiteral("Назначено сочетание: [ {key} ] (Нажмите для смены)"));
      m.insert(QStringLiteral("id"), QStringLiteral("Pintasan Terpasang: [ {key} ] (Klik untuk ubah)"));
      m.insert(QStringLiteral("fa"), QStringLiteral("میانبر تعیین‌شده: [ {key} ] (برای تغییر کلیک کنید)"));
      m.insert(QStringLiteral("ur"), QStringLiteral("مقررہ شارٹ کٹ: [ {key} ] (تبدیل کرنے کے لیے کلک کریں)"));
    }
    { auto& m = m_translations[QStringLiteral("toast_panic_stopped")];
      m.insert(QStringLiteral("ar"), QStringLiteral("تم إيقاف كل عمليات البرنامج فوراً ({hk_stop})"));
      m.insert(QStringLiteral("en"), QStringLiteral("All program actions stopped immediately ({hk_stop})"));
      m.insert(QStringLiteral("es"), QStringLiteral("Todas las acciones del programa detenidas ({hk_stop})"));
      m.insert(QStringLiteral("fr"), QStringLiteral("Toutes les actions du programme arrêtées immédiatement ({hk_stop})"));
      m.insert(QStringLiteral("de"), QStringLiteral("Alle Programmaktionen sofort gestoppt ({hk_stop})"));
      m.insert(QStringLiteral("tr"), QStringLiteral("Tüm program işlemleri anında durduruldu ({hk_stop})"));
      m.insert(QStringLiteral("ru"), QStringLiteral("Все действия программы немедленно остановлены ({hk_stop})"));
      m.insert(QStringLiteral("id"), QStringLiteral("Semua tindakan program dihentikan seketika ({hk_stop})"));
      m.insert(QStringLiteral("fa"), QStringLiteral("تمام عملیات برنامه فوراً متوقف شد ({hk_stop})"));
      m.insert(QStringLiteral("ur"), QStringLiteral("پروگرام کے تمام اعمال فوری روک دیے گئے ({hk_stop})"));
    }
    { auto& m = m_translations[QStringLiteral("btn_relaunch_admin")];
      m.insert(QStringLiteral("ar"), QStringLiteral("إعادة التشغيل كمسؤول (اختياري)"));
      m.insert(QStringLiteral("en"), QStringLiteral("Relaunch as Administrator (Optional)"));
      m.insert(QStringLiteral("es"), QStringLiteral("Reiniciar como Administrador (Opcional)"));
      m.insert(QStringLiteral("fr"), QStringLiteral("Relancer en Administrateur (Optionnel)"));
      m.insert(QStringLiteral("de"), QStringLiteral("Als Administrator neu starten (Optional)"));
      m.insert(QStringLiteral("tr"), QStringLiteral("Yönetici Olarak Yeniden Başlat (İsteğe Bağlı)"));
      m.insert(QStringLiteral("ru"), QStringLiteral("Перезапустить от Администратора (Опционально)"));
      m.insert(QStringLiteral("id"), QStringLiteral("Jalankan Ulang sebagai Administrator (Opsional)"));
      m.insert(QStringLiteral("fa"), QStringLiteral("اجرای مجدد به عنوان مدیر (اختیاری)"));
      m.insert(QStringLiteral("ur"), QStringLiteral("بطور ایڈمنسٹریٹر دوبارہ چلائیں (اختیاری)"));
    }
    { auto& m = m_translations[QStringLiteral("nav_autopresser")];
      m.insert(QStringLiteral("ar"), QStringLiteral("الضغط التلقائي"));
      m.insert(QStringLiteral("en"), QStringLiteral("Auto Presser"));
      m.insert(QStringLiteral("es"), QStringLiteral("Auto Pulsador"));
      m.insert(QStringLiteral("fr"), QStringLiteral("Auto Presseur"));
      m.insert(QStringLiteral("de"), QStringLiteral("Auto-Drücker"));
      m.insert(QStringLiteral("tr"), QStringLiteral("Oto Tuş"));
      m.insert(QStringLiteral("ru"), QStringLiteral("Авто-нажатие"));
      m.insert(QStringLiteral("id"), QStringLiteral("Tekan Otomatis"));
      m.insert(QStringLiteral("fa"), QStringLiteral("فشار خودکار"));
      m.insert(QStringLiteral("ur"), QStringLiteral("آٹو پریسر"));
    }
    { auto& m = m_translations[QStringLiteral("akp_title")];
      m.insert(QStringLiteral("ar"), QStringLiteral("استوديو الضغط التلقائي المتعدد التبويبات (V-AutoKeyPresser — 36 زراراً + نوافذ مستقلة في الخلفية)"));
      m.insert(QStringLiteral("en"), QStringLiteral("Multi-Tab Auto Key & Mouse Presser Studio (V-AutoKeyPresser — 36 Keys + Independent Background Tabs)"));
      m.insert(QStringLiteral("es"), QStringLiteral("Estudio Auto Pulsador Multi-Pestaña (V-AutoKeyPresser — 36 Teclas + Pestañas en Segundo Plano)"));
      m.insert(QStringLiteral("fr"), QStringLiteral("Studio Auto-Presseur Multi-Onglets (V-AutoKeyPresser — 36 Touches + Onglets en Arrière-Plan)"));
      m.insert(QStringLiteral("de"), QStringLiteral("Multi-Tab Auto-Tasten- & Maus-Studio (V-AutoKeyPresser — 36 Tasten + Hintergrund-Tabs)"));
      m.insert(QStringLiteral("tr"), QStringLiteral("Çok Sekmeli Otomatik Tuş & Fare Stüdyosu (V-AutoKeyPresser — 36 Tuş + Arka Plan Sekmeleri)"));
      m.insert(QStringLiteral("ru"), QStringLiteral("Многовкладочная студия авто-нажатий (V-AutoKeyPresser — 36 клавиш + работа в фоне)"));
      m.insert(QStringLiteral("id"), QStringLiteral("Studio Tekan Tombol & Mouse Multi-Tab (V-AutoKeyPresser — 36 Tombol + Tab Latar Belakang)"));
      m.insert(QStringLiteral("fa"), QStringLiteral("استودیوی فشار خودکار چند زبانه‌ای (V-AutoKeyPresser — ۳۶ کلید + اجرا در پس‌زمینه)"));
      m.insert(QStringLiteral("ur"), QStringLiteral("ملٹی ٹیب آٹو کی اور ماؤس پریسر اسٹوڈیو (V-AutoKeyPresser — 36 بٹن + بیک گراؤنڈ ٹیبز)"));
    }
    { auto& m = m_translations[QStringLiteral("akp_subtitle")];
      m.insert(QStringLiteral("ar"), QStringLiteral("شغّل عدة تبويبات مستقلة في وقت واحد، اربط كل تبويب بلعبة أو نافذة محددة، واختر من بين ٢٠ طريقة إرسال موحدة و٨ أنماط ماوس."));
      m.insert(QStringLiteral("en"), QStringLiteral("Run multiple independent tabs simultaneously, bind each tab to a specific window, and choose from 20 unified key methods & 8 mouse modes."));
      m.insert(QStringLiteral("es"), QStringLiteral("Ejecute múltiples pestañas independientes simultáneamente, vincule cada una a una ventana y elija entre 20 métodos de teclado y 8 modos de ratón."));
      m.insert(QStringLiteral("fr"), QStringLiteral("Exécutez plusieurs onglets indépendants simultanément, liez chaque onglet à une fenêtre et choisissez parmi 20 méthodes clavier et 8 modes souris."));
      m.insert(QStringLiteral("de"), QStringLiteral("Mehrere unabhängige Tabs gleichzeitig ausführen, an Zielfenster binden und aus 20 Tastaturmethoden & 8 Maus-Modi wählen."));
      m.insert(QStringLiteral("tr"), QStringLiteral("Aynı anda birden fazla bağımsız sekme çalıştırın, her sekmeyi bir pencereye bağlayın ve 20 klavye yöntemi ile 8 fare modundan seçim yapın."));
      m.insert(QStringLiteral("ru"), QStringLiteral("Запускайте несколько независимых вкладок одновременно, привязывайте их к окнам и выбирайте из 20 методов клавиш и 8 режимов мыши."));
      m.insert(QStringLiteral("id"), QStringLiteral("Jalankan beberapa tab independen secara bersamaan, ikat tiap tab ke jendela target, dan pilih dari 20 metode tombol & 8 mode mouse."));
      m.insert(QStringLiteral("fa"), QStringLiteral("اجرای همزمان چند تب مستقل، اتصال هر تب به پنجره دلخواه و انتخاب از میان ۲۰ روش ارسال کلید و ۸ حالت ماوس."));
      m.insert(QStringLiteral("ur"), QStringLiteral("بیک وقت متعدد آزاد ٹیبز چلائیں، ہر ٹیب کو مخصوص ونڈو سے جوڑیں، اور 20 کلیدی طریقوں اور 8 ماؤس موڈز میں سے انتخاب کریں۔"));
    }
    { auto& m = m_translations[QStringLiteral("akp_btn_stop_all_tabs")];
      m.insert(QStringLiteral("ar"), QStringLiteral("إيقاف كل التبويبات"));
      m.insert(QStringLiteral("en"), QStringLiteral("Stop All Tabs"));
      m.insert(QStringLiteral("es"), QStringLiteral("Detener Pestañas"));
      m.insert(QStringLiteral("fr"), QStringLiteral("Arrêter Tous les Onglets"));
      m.insert(QStringLiteral("de"), QStringLiteral("Alle Tabs stoppen"));
      m.insert(QStringLiteral("tr"), QStringLiteral("Tüm Sekmeleri Durdur"));
      m.insert(QStringLiteral("ru"), QStringLiteral("Остановить все вкладки"));
      m.insert(QStringLiteral("id"), QStringLiteral("Hentikan Semua Tab"));
      m.insert(QStringLiteral("fa"), QStringLiteral("توقف همه زبانه‌ها"));
      m.insert(QStringLiteral("ur"), QStringLiteral("تمام ٹیبز روکیں"));
    }
    { auto& m = m_translations[QStringLiteral("akp_btn_save_workspace")];
      m.insert(QStringLiteral("ar"), QStringLiteral("حفظ مساحة العمل"));
      m.insert(QStringLiteral("en"), QStringLiteral("Save Workspace"));
      m.insert(QStringLiteral("es"), QStringLiteral("Guardar Espacio"));
      m.insert(QStringLiteral("fr"), QStringLiteral("Sauver l'Espace"));
      m.insert(QStringLiteral("de"), QStringLiteral("Workspace speichern"));
      m.insert(QStringLiteral("tr"), QStringLiteral("Çalışma Alanını Kaydet"));
      m.insert(QStringLiteral("ru"), QStringLiteral("Сохранить проект"));
      m.insert(QStringLiteral("id"), QStringLiteral("Simpan Workspace"));
      m.insert(QStringLiteral("fa"), QStringLiteral("ذخیره فضای کاری"));
      m.insert(QStringLiteral("ur"), QStringLiteral("ورک اسپیس محفوظ کریں"));
    }
    { auto& m = m_translations[QStringLiteral("akp_btn_load_workspace")];
      m.insert(QStringLiteral("ar"), QStringLiteral("فتح مساحة عمل"));
      m.insert(QStringLiteral("en"), QStringLiteral("Load Workspace"));
      m.insert(QStringLiteral("es"), QStringLiteral("Cargar Espacio"));
      m.insert(QStringLiteral("fr"), QStringLiteral("Charger l'Espace"));
      m.insert(QStringLiteral("de"), QStringLiteral("Workspace laden"));
      m.insert(QStringLiteral("tr"), QStringLiteral("Çalışma Alanı Yükle"));
      m.insert(QStringLiteral("ru"), QStringLiteral("Загрузить проект"));
      m.insert(QStringLiteral("id"), QStringLiteral("Muat Workspace"));
      m.insert(QStringLiteral("fa"), QStringLiteral("بارگذاری فضای کاری"));
      m.insert(QStringLiteral("ur"), QStringLiteral("ورک اسپیس لوڈ کریں"));
    }
    { auto& m = m_translations[QStringLiteral("akp_ph_tab_name")];
      m.insert(QStringLiteral("ar"), QStringLiteral("اسم التبويب..."));
      m.insert(QStringLiteral("en"), QStringLiteral("Tab name..."));
      m.insert(QStringLiteral("es"), QStringLiteral("Nombre de pestaña..."));
      m.insert(QStringLiteral("fr"), QStringLiteral("Nom de l'onglet..."));
      m.insert(QStringLiteral("de"), QStringLiteral("Tab-Name..."));
      m.insert(QStringLiteral("tr"), QStringLiteral("Sekme adı..."));
      m.insert(QStringLiteral("ru"), QStringLiteral("Имя вкладки..."));
      m.insert(QStringLiteral("id"), QStringLiteral("Nama tab..."));
      m.insert(QStringLiteral("fa"), QStringLiteral("نام زبانه..."));
      m.insert(QStringLiteral("ur"), QStringLiteral("ٹیب کا نام..."));
    }
    { auto& m = m_translations[QStringLiteral("akp_btn_new_tab")];
      m.insert(QStringLiteral("ar"), QStringLiteral("+ تبويب جديد"));
      m.insert(QStringLiteral("en"), QStringLiteral("+ New Tab"));
      m.insert(QStringLiteral("es"), QStringLiteral("+ Nueva Pestaña"));
      m.insert(QStringLiteral("fr"), QStringLiteral("+ Nouvel Onglet"));
      m.insert(QStringLiteral("de"), QStringLiteral("+ Neuer Tab"));
      m.insert(QStringLiteral("tr"), QStringLiteral("+ Yeni Sekme"));
      m.insert(QStringLiteral("ru"), QStringLiteral("+ Новая вкладка"));
      m.insert(QStringLiteral("id"), QStringLiteral("+ Tab Baru"));
      m.insert(QStringLiteral("fa"), QStringLiteral("+ زبانه جدید"));
      m.insert(QStringLiteral("ur"), QStringLiteral("+ نیا ٹیب"));
    }
    { auto& m = m_translations[QStringLiteral("akp_btn_dup_tab")];
      m.insert(QStringLiteral("ar"), QStringLiteral("نسخ التبويب"));
      m.insert(QStringLiteral("en"), QStringLiteral("Duplicate Tab"));
      m.insert(QStringLiteral("es"), QStringLiteral("Duplicar"));
      m.insert(QStringLiteral("fr"), QStringLiteral("Dupliquer"));
      m.insert(QStringLiteral("de"), QStringLiteral("Duplizieren"));
      m.insert(QStringLiteral("tr"), QStringLiteral("Sekmeyi Çoğalt"));
      m.insert(QStringLiteral("ru"), QStringLiteral("Дублировать"));
      m.insert(QStringLiteral("id"), QStringLiteral("Duplikat Tab"));
      m.insert(QStringLiteral("fa"), QStringLiteral("تکثیر زبانه"));
      m.insert(QStringLiteral("ur"), QStringLiteral("ٹیب نقل کریں"));
    }
    { auto& m = m_translations[QStringLiteral("akp_btn_close_tab")];
      m.insert(QStringLiteral("ar"), QStringLiteral("إغلاق التبويب الحالي"));
      m.insert(QStringLiteral("en"), QStringLiteral("Close Current Tab"));
      m.insert(QStringLiteral("es"), QStringLiteral("Cerrar pestaña actual"));
      m.insert(QStringLiteral("fr"), QStringLiteral("Fermer l'onglet actif"));
      m.insert(QStringLiteral("de"), QStringLiteral("Aktuellen Tab schließen"));
      m.insert(QStringLiteral("tr"), QStringLiteral("Sekmeyi Kapat"));
      m.insert(QStringLiteral("ru"), QStringLiteral("Закрыть вкладку"));
      m.insert(QStringLiteral("id"), QStringLiteral("Tutup Tab Saat Ini"));
      m.insert(QStringLiteral("fa"), QStringLiteral("بستن زبانه فعلی"));
      m.insert(QStringLiteral("ur"), QStringLiteral("موجودہ ٹیب بند کریں"));
    }
    { auto& m = m_translations[QStringLiteral("akp_btn_choose_process")];
      m.insert(QStringLiteral("ar"), QStringLiteral("اختيار نافذة"));
      m.insert(QStringLiteral("en"), QStringLiteral("Pick Window"));
      m.insert(QStringLiteral("es"), QStringLiteral("Elegir ventana"));
      m.insert(QStringLiteral("fr"), QStringLiteral("Cibler fenêtre"));
      m.insert(QStringLiteral("de"), QStringLiteral("Fenster wählen"));
      m.insert(QStringLiteral("tr"), QStringLiteral("Pencere Seç"));
      m.insert(QStringLiteral("ru"), QStringLiteral("Выбрать окно"));
      m.insert(QStringLiteral("id"), QStringLiteral("Pilih Jendela"));
      m.insert(QStringLiteral("fa"), QStringLiteral("انتخاب پنجره"));
      m.insert(QStringLiteral("ur"), QStringLiteral("ونڈو منتخب"));
    }
    { auto& m = m_translations[QStringLiteral("akp_lbl_run_in_bg")];
      m.insert(QStringLiteral("ar"), QStringLiteral("تشغيل في الخلفية:"));
      m.insert(QStringLiteral("en"), QStringLiteral("Run in background:"));
      m.insert(QStringLiteral("es"), QStringLiteral("En segundo plano:"));
      m.insert(QStringLiteral("fr"), QStringLiteral("En arrière-plan :"));
      m.insert(QStringLiteral("de"), QStringLiteral("Im Hintergrund:"));
      m.insert(QStringLiteral("tr"), QStringLiteral("Arka planda çalıştır:"));
      m.insert(QStringLiteral("ru"), QStringLiteral("Работа в фоне:"));
      m.insert(QStringLiteral("id"), QStringLiteral("Jalan di latar:"));
      m.insert(QStringLiteral("fa"), QStringLiteral("اجرا در پس‌زمینه:"));
      m.insert(QStringLiteral("ur"), QStringLiteral("پس منظر میں چلائیں:"));
    }
    { auto& m = m_translations[QStringLiteral("akp_btn_start_tab")];
      m.insert(QStringLiteral("ar"), QStringLiteral("تشغيل التبويب"));
      m.insert(QStringLiteral("en"), QStringLiteral("Start Tab"));
      m.insert(QStringLiteral("es"), QStringLiteral("Iniciar Pestaña"));
      m.insert(QStringLiteral("fr"), QStringLiteral("Démarrer l'Onglet"));
      m.insert(QStringLiteral("de"), QStringLiteral("Tab starten"));
      m.insert(QStringLiteral("tr"), QStringLiteral("Sekmeyi Başlat"));
      m.insert(QStringLiteral("ru"), QStringLiteral("Запустить вкладку"));
      m.insert(QStringLiteral("id"), QStringLiteral("Mulai Tab"));
      m.insert(QStringLiteral("fa"), QStringLiteral("شروع زبانه"));
      m.insert(QStringLiteral("ur"), QStringLiteral("ٹیب شروع کریں"));
    }
    { auto& m = m_translations[QStringLiteral("akp_btn_stop_tab")];
      m.insert(QStringLiteral("ar"), QStringLiteral("إيقاف التبويب"));
      m.insert(QStringLiteral("en"), QStringLiteral("Stop Tab"));
      m.insert(QStringLiteral("es"), QStringLiteral("Detener Pestaña"));
      m.insert(QStringLiteral("fr"), QStringLiteral("Arrêter l'Onglet"));
      m.insert(QStringLiteral("de"), QStringLiteral("Tab stoppen"));
      m.insert(QStringLiteral("tr"), QStringLiteral("Sekmeyi Durdur"));
      m.insert(QStringLiteral("ru"), QStringLiteral("Остановить вкладку"));
      m.insert(QStringLiteral("id"), QStringLiteral("Hentikan Tab"));
      m.insert(QStringLiteral("fa"), QStringLiteral("توقف زبانه"));
      m.insert(QStringLiteral("ur"), QStringLiteral("ٹیب روکیں"));
    }
    { auto& m = m_translations[QStringLiteral("akp_lbl_method")];
      m.insert(QStringLiteral("ar"), QStringLiteral("طريقة الضغط:"));
      m.insert(QStringLiteral("en"), QStringLiteral("Press Method:"));
      m.insert(QStringLiteral("es"), QStringLiteral("Método de pulsación:"));
      m.insert(QStringLiteral("fr"), QStringLiteral("Méthode d'envoi :"));
      m.insert(QStringLiteral("de"), QStringLiteral("Tasten-Methode:"));
      m.insert(QStringLiteral("tr"), QStringLiteral("Basım Yöntemi:"));
      m.insert(QStringLiteral("ru"), QStringLiteral("Метод нажатия:"));
      m.insert(QStringLiteral("id"), QStringLiteral("Metode Tekan:"));
      m.insert(QStringLiteral("fa"), QStringLiteral("روش ارسال کلید:"));
      m.insert(QStringLiteral("ur"), QStringLiteral("پریس کا طریقہ:"));
    }
    { auto& m = m_translations[QStringLiteral("akp_btn_test_method")];
      m.insert(QStringLiteral("ar"), QStringLiteral("تجربة الطريقة"));
      m.insert(QStringLiteral("en"), QStringLiteral("Test Method"));
      m.insert(QStringLiteral("es"), QStringLiteral("Probar Método"));
      m.insert(QStringLiteral("fr"), QStringLiteral("Tester la Méthode"));
      m.insert(QStringLiteral("de"), QStringLiteral("Methode testen"));
      m.insert(QStringLiteral("tr"), QStringLiteral("Yöntemi Test Et"));
      m.insert(QStringLiteral("ru"), QStringLiteral("Проверить метод"));
      m.insert(QStringLiteral("id"), QStringLiteral("Uji Metode"));
      m.insert(QStringLiteral("fa"), QStringLiteral("تست روش"));
      m.insert(QStringLiteral("ur"), QStringLiteral("طریقہ ٹیسٹ کریں"));
    }
    { auto& m = m_translations[QStringLiteral("akp_btn_save_profile")];
      m.insert(QStringLiteral("ar"), QStringLiteral("حفظ بروفايل التبويب"));
      m.insert(QStringLiteral("en"), QStringLiteral("Save Profile..."));
      m.insert(QStringLiteral("es"), QStringLiteral("Guardar Perfil..."));
      m.insert(QStringLiteral("fr"), QStringLiteral("Sauver le Profil..."));
      m.insert(QStringLiteral("de"), QStringLiteral("Profil speichern..."));
      m.insert(QStringLiteral("tr"), QStringLiteral("Profili Kaydet..."));
      m.insert(QStringLiteral("ru"), QStringLiteral("Сохранить профиль..."));
      m.insert(QStringLiteral("id"), QStringLiteral("Simpan Profil..."));
      m.insert(QStringLiteral("fa"), QStringLiteral("ذخیره پروفایل..."));
      m.insert(QStringLiteral("ur"), QStringLiteral("پروفائل محفوظ کریں..."));
    }
    { auto& m = m_translations[QStringLiteral("akp_btn_load_profile")];
      m.insert(QStringLiteral("ar"), QStringLiteral("فتح بروفايل"));
      m.insert(QStringLiteral("en"), QStringLiteral("Load Profile..."));
      m.insert(QStringLiteral("es"), QStringLiteral("Cargar Perfil..."));
      m.insert(QStringLiteral("fr"), QStringLiteral("Charger un Profil..."));
      m.insert(QStringLiteral("de"), QStringLiteral("Profil laden..."));
      m.insert(QStringLiteral("tr"), QStringLiteral("Profil Yükle..."));
      m.insert(QStringLiteral("ru"), QStringLiteral("Загрузить профиль..."));
      m.insert(QStringLiteral("id"), QStringLiteral("Muat Profil..."));
      m.insert(QStringLiteral("fa"), QStringLiteral("بارگذاری پروفایل..."));
      m.insert(QStringLiteral("ur"), QStringLiteral("پروفائل لوڈ کریں..."));
    }
    { auto& m = m_translations[QStringLiteral("akp_sub_keys")];
      m.insert(QStringLiteral("ar"), QStringLiteral("الأزرار (36 + مخصص)"));
      m.insert(QStringLiteral("en"), QStringLiteral("Keys Grid (36 + Custom)"));
      m.insert(QStringLiteral("es"), QStringLiteral("Cuadrícula de Teclas (36 + Extra)"));
      m.insert(QStringLiteral("fr"), QStringLiteral("Grille des Touches (36 + Perso)"));
      m.insert(QStringLiteral("de"), QStringLiteral("Tastenfeld (36 + Eigene)"));
      m.insert(QStringLiteral("tr"), QStringLiteral("Tuş Tablosu (36 + Özel)"));
      m.insert(QStringLiteral("ru"), QStringLiteral("Сетка клавиш (36 + Свои)"));
      m.insert(QStringLiteral("id"), QStringLiteral("Kotak Tombol (36 + Kustom)"));
      m.insert(QStringLiteral("fa"), QStringLiteral("شبکه کلیدها (۳۶ + سفارشی)"));
      m.insert(QStringLiteral("ur"), QStringLiteral("کیز گرڈ (36 + کسٹم)"));
    }
    { auto& m = m_translations[QStringLiteral("akp_sub_mouse")];
      m.insert(QStringLiteral("ar"), QStringLiteral("نقرات الماوس المتعددة (F8)"));
      m.insert(QStringLiteral("en"), QStringLiteral("Mouse Clicks (F8)"));
      m.insert(QStringLiteral("es"), QStringLiteral("Clics de Ratón (F8)"));
      m.insert(QStringLiteral("fr"), QStringLiteral("Clics Souris (F8)"));
      m.insert(QStringLiteral("de"), QStringLiteral("Mausklicks (F8)"));
      m.insert(QStringLiteral("tr"), QStringLiteral("Fare Tıklamaları (F8)"));
      m.insert(QStringLiteral("ru"), QStringLiteral("Клики мыши (F8)"));
      m.insert(QStringLiteral("id"), QStringLiteral("Klik Mouse (F8)"));
      m.insert(QStringLiteral("fa"), QStringLiteral("کلیک‌های ماوس (F8)"));
      m.insert(QStringLiteral("ur"), QStringLiteral("ماؤس کلکس (F8)"));
    }
    { auto& m = m_translations[QStringLiteral("akp_sub_log")];
      m.insert(QStringLiteral("ar"), QStringLiteral("سجل النشاط المباشر"));
      m.insert(QStringLiteral("en"), QStringLiteral("Activity Log"));
      m.insert(QStringLiteral("es"), QStringLiteral("Registro de Actividad"));
      m.insert(QStringLiteral("fr"), QStringLiteral("Journal d'Activité"));
      m.insert(QStringLiteral("de"), QStringLiteral("Aktivitätsprotokoll"));
      m.insert(QStringLiteral("tr"), QStringLiteral("Etkinlik Günlüğü"));
      m.insert(QStringLiteral("ru"), QStringLiteral("Журнал активности"));
      m.insert(QStringLiteral("id"), QStringLiteral("Log Aktivitas"));
      m.insert(QStringLiteral("fa"), QStringLiteral("گزارش فعالیت زنده"));
      m.insert(QStringLiteral("ur"), QStringLiteral("سرگرمی لاگ"));
    }
    { auto& m = m_translations[QStringLiteral("akp_lbl_add_custom_key")];
      m.insert(QStringLiteral("ar"), QStringLiteral("+ إضافة زرار مخصص:"));
      m.insert(QStringLiteral("en"), QStringLiteral("+ Add Custom Key:"));
      m.insert(QStringLiteral("es"), QStringLiteral("+ Añadir Tecla:"));
      m.insert(QStringLiteral("fr"), QStringLiteral("+ Ajouter Touche :"));
      m.insert(QStringLiteral("de"), QStringLiteral("+ Eigene Taste:"));
      m.insert(QStringLiteral("tr"), QStringLiteral("+ Özel Tuş Ekle:"));
      m.insert(QStringLiteral("ru"), QStringLiteral("+ Своя клавиша:"));
      m.insert(QStringLiteral("id"), QStringLiteral("+ Tombol Kustom:"));
      m.insert(QStringLiteral("fa"), QStringLiteral("+ افزودن کلید سفارشی:"));
      m.insert(QStringLiteral("ur"), QStringLiteral("+ کسٹم بٹن شامل کریں:"));
    }
    { auto& m = m_translations[QStringLiteral("akp_btn_arm_all")];
      m.insert(QStringLiteral("ar"), QStringLiteral("تفعيل الكل"));
      m.insert(QStringLiteral("en"), QStringLiteral("Arm All"));
      m.insert(QStringLiteral("es"), QStringLiteral("Activar Todo"));
      m.insert(QStringLiteral("fr"), QStringLiteral("Tout Activer"));
      m.insert(QStringLiteral("de"), QStringLiteral("Alle aktivieren"));
      m.insert(QStringLiteral("tr"), QStringLiteral("Tümünü Seç"));
      m.insert(QStringLiteral("ru"), QStringLiteral("Включить все"));
      m.insert(QStringLiteral("id"), QStringLiteral("Aktifkan Semua"));
      m.insert(QStringLiteral("fa"), QStringLiteral("فعال‌سازی همه"));
      m.insert(QStringLiteral("ur"), QStringLiteral("سب فعال کریں"));
    }
    { auto& m = m_translations[QStringLiteral("akp_btn_disarm_all")];
      m.insert(QStringLiteral("ar"), QStringLiteral("إلغاء الكل"));
      m.insert(QStringLiteral("en"), QStringLiteral("Disarm All"));
      m.insert(QStringLiteral("es"), QStringLiteral("Desactivar Todo"));
      m.insert(QStringLiteral("fr"), QStringLiteral("Tout Désactiver"));
      m.insert(QStringLiteral("de"), QStringLiteral("Alle deaktivieren"));
      m.insert(QStringLiteral("tr"), QStringLiteral("Tümünü Kaldır"));
      m.insert(QStringLiteral("ru"), QStringLiteral("Отключить все"));
      m.insert(QStringLiteral("id"), QStringLiteral("Nonaktifkan Semua"));
      m.insert(QStringLiteral("fa"), QStringLiteral("غیرفعال‌سازی همه"));
      m.insert(QStringLiteral("ur"), QStringLiteral("سب غیر فعال کریں"));
    }
    { auto& m = m_translations[QStringLiteral("akp_test_single_key")];
      m.insert(QStringLiteral("ar"), QStringLiteral("تجربة إرسال هذا الزرار الآن"));
      m.insert(QStringLiteral("en"), QStringLiteral("Test sending this key now"));
      m.insert(QStringLiteral("es"), QStringLiteral("Probar esta tecla ahora"));
      m.insert(QStringLiteral("fr"), QStringLiteral("Tester cette touche maintenant"));
      m.insert(QStringLiteral("de"), QStringLiteral("Diese Taste jetzt testen"));
      m.insert(QStringLiteral("tr"), QStringLiteral("Bu tuşu şimdi test et"));
      m.insert(QStringLiteral("ru"), QStringLiteral("Проверить эту клавишу сейчас"));
      m.insert(QStringLiteral("id"), QStringLiteral("Uji tombol ini sekarang"));
      m.insert(QStringLiteral("fa"), QStringLiteral("تست ارسال این کلید"));
      m.insert(QStringLiteral("ur"), QStringLiteral("اس بٹن کو ابھی ٹیسٹ کریں"));
    }
    { auto& m = m_translations[QStringLiteral("akp_lbl_enable_mouse")];
      m.insert(QStringLiteral("ar"), QStringLiteral("تفعيل نقرات الماوس التلقائية في هذا التبويب (اضغط F8 لإضافة أو التقاط إحداثيات)"));
      m.insert(QStringLiteral("en"), QStringLiteral("Enable Multi-Point Mouse Clicks in this Tab (Press F8 to capture coordinates)"));
      m.insert(QStringLiteral("es"), QStringLiteral("Activar clics de ratón multipunto en esta pestaña (Pulsa F8 para capturar coordenadas)"));
      m.insert(QStringLiteral("fr"), QStringLiteral("Activer les clics souris multi-points dans cet onglet (Appuyez sur F8 pour capturer)"));
      m.insert(QStringLiteral("de"), QStringLiteral("Mehrpunkt-Mausklicks in diesem Tab aktivieren (F8 drücken zum Erfassen)"));
      m.insert(QStringLiteral("tr"), QStringLiteral("Bu sekmede çok noktalı fare tıklamasını etkinleştir (Koordinat için F8'e basın)"));
      m.insert(QStringLiteral("ru"), QStringLiteral("Включить многоточечные клики мыши во вкладке (нажмите F8 для захвата координат)"));
      m.insert(QStringLiteral("id"), QStringLiteral("Aktifkan klik mouse multi-titik di tab ini (Tekan F8 untuk menangkap koordinat)"));
      m.insert(QStringLiteral("fa"), QStringLiteral("فعال‌سازی کلیک‌های چندنقطه‌ای ماوس در این زبانه (برای ثبت مختصات F8 را بزنید)"));
      m.insert(QStringLiteral("ur"), QStringLiteral("اس ٹیب میں ملٹی پوائنٹ ماؤس کلکس فعال کریں (مقام پکڑنے کے لیے F8 دبائیں)"));
    }
    { auto& m = m_translations[QStringLiteral("akp_lbl_mouse_mode")];
      m.insert(QStringLiteral("ar"), QStringLiteral("نمط كليك الماوس:"));
      m.insert(QStringLiteral("en"), QStringLiteral("Mouse Click Mode:"));
      m.insert(QStringLiteral("es"), QStringLiteral("Modo de Clic:"));
      m.insert(QStringLiteral("fr"), QStringLiteral("Mode de Clic :"));
      m.insert(QStringLiteral("de"), QStringLiteral("Mausklick-Modus:"));
      m.insert(QStringLiteral("tr"), QStringLiteral("Fare Tıklama Modu:"));
      m.insert(QStringLiteral("ru"), QStringLiteral("Режим клика мыши:"));
      m.insert(QStringLiteral("id"), QStringLiteral("Mode Klik Mouse:"));
      m.insert(QStringLiteral("fa"), QStringLiteral("حالت کلیک ماوس:"));
      m.insert(QStringLiteral("ur"), QStringLiteral("ماؤس کلک موڈ:"));
    }
    { auto& m = m_translations[QStringLiteral("akp_btn_add_mouse_pt")];
      m.insert(QStringLiteral("ar"), QStringLiteral("+ إضافة نقطة كليك (F8)"));
      m.insert(QStringLiteral("en"), QStringLiteral("+ Add Click Point (F8)"));
      m.insert(QStringLiteral("es"), QStringLiteral("+ Añadir Punto de Clic (F8)"));
      m.insert(QStringLiteral("fr"), QStringLiteral("+ Ajouter un Point de Clic (F8)"));
      m.insert(QStringLiteral("de"), QStringLiteral("+ Klickpunkt hinzufügen (F8)"));
      m.insert(QStringLiteral("tr"), QStringLiteral("+ Tıklama Noktası Ekle (F8)"));
      m.insert(QStringLiteral("ru"), QStringLiteral("+ Добавить точку клика (F8)"));
      m.insert(QStringLiteral("id"), QStringLiteral("+ Tambah Titik Klik (F8)"));
      m.insert(QStringLiteral("fa"), QStringLiteral("+ افزودن نقطه کلیک (F8)"));
      m.insert(QStringLiteral("ur"), QStringLiteral("+ کلک پوائنٹ شامل کریں (F8)"));
    }
    { auto& m = m_translations[QStringLiteral("akp_lbl_activity_log")];
      m.insert(QStringLiteral("ar"), QStringLiteral("سجل نشاط التبويب المباشر (محفوظ ومترجم تلقائياً بجميع اللغات):"));
      m.insert(QStringLiteral("en"), QStringLiteral("Live Tab Activity Log (Persistent & Auto-Translated Across All Languages):"));
      m.insert(QStringLiteral("es"), QStringLiteral("Registro de Actividad en Vivo (Persistente y autotraducido en todos los idiomas):"));
      m.insert(QStringLiteral("fr"), QStringLiteral("Journal d'Activité en Direct (Persistant et traduit automatiquement) :"));
      m.insert(QStringLiteral("de"), QStringLiteral("Live-Aktivitätsprotokoll (Bleibt beim Sprachwechsel erhalten & übersetzt):"));
      m.insert(QStringLiteral("tr"), QStringLiteral("Canlı Sekme Etkinlik Günlüğü (Dil değişiminde korunur ve otomatik çevrilir):"));
      m.insert(QStringLiteral("ru"), QStringLiteral("Живой журнал активности вкладки (сохраняется и переводится при смене языка):"));
      m.insert(QStringLiteral("id"), QStringLiteral("Log Aktivitas Tab Langsung (Tersimpan & diterjemahkan otomatis):"));
      m.insert(QStringLiteral("fa"), QStringLiteral("گزارش فعالیت زنده زبانه (پایدار و ترجمه خودکار در تمام زبان‌ها):"));
      m.insert(QStringLiteral("ur"), QStringLiteral("لائیو ٹیب سرگرمی لاگ (زبان बदलने پر محفوظ اور خودکار ترجمہ شدہ):"));
    }
    { auto& m = m_translations[QStringLiteral("akp_log_started")];
      m.insert(QStringLiteral("ar"), QStringLiteral("بدأ تشغيل التبويب"));
      m.insert(QStringLiteral("en"), QStringLiteral("Tab Automation Started"));
      m.insert(QStringLiteral("es"), QStringLiteral("Automatización de pestaña iniciada"));
      m.insert(QStringLiteral("fr"), QStringLiteral("Automatisation de l'onglet démarrée"));
      m.insert(QStringLiteral("de"), QStringLiteral("Tab-Automatisierung gestartet"));
      m.insert(QStringLiteral("tr"), QStringLiteral("Sekme Otomasyonu Başlatıldı"));
      m.insert(QStringLiteral("ru"), QStringLiteral("Автоматизация вкладки запущена"));
      m.insert(QStringLiteral("id"), QStringLiteral("Otomatisasi Tab Dimulai"));
      m.insert(QStringLiteral("fa"), QStringLiteral("خودکارسازی زبانه شروع شد"));
      m.insert(QStringLiteral("ur"), QStringLiteral("ٹیب آٹومیشن شروع ہو گئی"));
    }
    { auto& m = m_translations[QStringLiteral("akp_log_stopped")];
      m.insert(QStringLiteral("ar"), QStringLiteral("تم إيقاف التبويب"));
      m.insert(QStringLiteral("en"), QStringLiteral("Tab Automation Stopped"));
      m.insert(QStringLiteral("es"), QStringLiteral("Automatización de pestaña detenida"));
      m.insert(QStringLiteral("fr"), QStringLiteral("Automatisation de l'onglet arrêtée"));
      m.insert(QStringLiteral("de"), QStringLiteral("Tab-Automatisierung gestoppt"));
      m.insert(QStringLiteral("tr"), QStringLiteral("Sekme Otomasyonu Durduruldu"));
      m.insert(QStringLiteral("ru"), QStringLiteral("Автоматизация вкладки остановлена"));
      m.insert(QStringLiteral("id"), QStringLiteral("Otomatisasi Tab Dihentikan"));
      m.insert(QStringLiteral("fa"), QStringLiteral("خودکارسازی زبانه متوقف شد"));
      m.insert(QStringLiteral("ur"), QStringLiteral("ٹیب آٹومیشن روک دی گئی"));
    }
    { auto& m = m_translations[QStringLiteral("akp_log_no_keys")];
      m.insert(QStringLiteral("ar"), QStringLiteral("يرجى تفعيل زرار واحد على الأقل أو نقطة كليك ماوس قبل بدء التشغيل!"));
      m.insert(QStringLiteral("en"), QStringLiteral("Please arm at least one key or mouse click point before starting!"));
      m.insert(QStringLiteral("es"), QStringLiteral("¡Activa al menos una tecla o punto de clic antes de iniciar!"));
      m.insert(QStringLiteral("fr"), QStringLiteral("Veuillez activer au moins une touche ou un point de clic avant de démarrer !"));
      m.insert(QStringLiteral("de"), QStringLiteral("Bitte aktivieren Sie mindestens eine Taste oder einen Klickpunkt vor dem Start!"));
      m.insert(QStringLiteral("tr"), QStringLiteral("Başlatmadan önce lütfen en az bir tuşu veya fare noktasını etkinleştirin!"));
      m.insert(QStringLiteral("ru"), QStringLiteral("Включите хотя бы одну клавишу или точку клика мыши перед запуском!"));
      m.insert(QStringLiteral("id"), QStringLiteral("Harap aktifkan minimal satu tombol atau titik klik mouse sebelum memulai!"));
      m.insert(QStringLiteral("fa"), QStringLiteral("لطفاً پیش از شروع، حداقل یک کلید یا نقطه کلیک ماوس را فعال کنید!"));
      m.insert(QStringLiteral("ur"), QStringLiteral("شروع کرنے سے پہلے کم از کم ایک بٹن یا ماؤس کلک پوائنٹ فعال کریں!"));
    }
    { auto& m = m_translations[QStringLiteral("akp_log_sent_key")];
      m.insert(QStringLiteral("ar"), QStringLiteral("تم إرسال زرار:"));
      m.insert(QStringLiteral("en"), QStringLiteral("Dispatched Key:"));
      m.insert(QStringLiteral("es"), QStringLiteral("Tecla enviada:"));
      m.insert(QStringLiteral("fr"), QStringLiteral("Touche envoyée :"));
      m.insert(QStringLiteral("de"), QStringLiteral("Taste gesendet:"));
      m.insert(QStringLiteral("tr"), QStringLiteral("Gönderilen Tuş:"));
      m.insert(QStringLiteral("ru"), QStringLiteral("Отправлена клавиша:"));
      m.insert(QStringLiteral("id"), QStringLiteral("Tombol Dikirim:"));
      m.insert(QStringLiteral("fa"), QStringLiteral("کلید ارسال شد:"));
      m.insert(QStringLiteral("ur"), QStringLiteral("بٹن بھیجا گیا:"));
    }
    { auto& m = m_translations[QStringLiteral("akp_method_sendinput")];
      m.insert(QStringLiteral("ar"), QStringLiteral("1. SendInput (مقدمة — افتراضي لويندوز)"));
      m.insert(QStringLiteral("en"), QStringLiteral("1. SendInput (Foreground — Standard Windows)"));
      m.insert(QStringLiteral("es"), QStringLiteral("1. SendInput (Primer plano — Estándar Windows)"));
      m.insert(QStringLiteral("fr"), QStringLiteral("1. SendInput (Premier plan — Standard Windows)"));
      m.insert(QStringLiteral("de"), QStringLiteral("1. SendInput (Vordergrund — Standard Windows)"));
      m.insert(QStringLiteral("tr"), QStringLiteral("1. SendInput (Ön Plan — Standart Windows)"));
      m.insert(QStringLiteral("ru"), QStringLiteral("1. SendInput (Активное окно — Стандарт Windows)"));
      m.insert(QStringLiteral("id"), QStringLiteral("1. SendInput (Latar Depan — Standar Windows)"));
      m.insert(QStringLiteral("fa"), QStringLiteral("1. SendInput (پیش‌زمینه — استاندارد ویندوز)"));
      m.insert(QStringLiteral("ur"), QStringLiteral("1. SendInput (فرنٹ گراؤنڈ — معیاری ونڈوز)"));
    }
    { auto& m = m_translations[QStringLiteral("akp_method_scancode")];
      m.insert(QStringLiteral("ar"), QStringLiteral("2. Hardware ScanCode (مقدمة — للألعاب DirectInput)"));
      m.insert(QStringLiteral("en"), QStringLiteral("2. Hardware ScanCode (Foreground — DirectInput Games)"));
      m.insert(QStringLiteral("es"), QStringLiteral("2. Hardware ScanCode (Primer plano — Juegos DirectInput)"));
      m.insert(QStringLiteral("fr"), QStringLiteral("2. Hardware ScanCode (Premier plan — Jeux DirectInput)"));
      m.insert(QStringLiteral("de"), QStringLiteral("2. Hardware ScanCode (Vordergrund — DirectInput-Spiele)"));
      m.insert(QStringLiteral("tr"), QStringLiteral("2. Hardware ScanCode (Ön Plan — DirectInput Oyunlar)"));
      m.insert(QStringLiteral("ru"), QStringLiteral("2. Hardware ScanCode (Активное окно — Игры DirectInput)"));
      m.insert(QStringLiteral("id"), QStringLiteral("2. Hardware ScanCode (Latar Depan — Game DirectInput)"));
      m.insert(QStringLiteral("fa"), QStringLiteral("2. Hardware ScanCode (پیش‌زمینه — بازی‌های DirectInput)"));
      m.insert(QStringLiteral("ur"), QStringLiteral("2. Hardware ScanCode (فرنٹ گراؤنڈ — DirectInput گیمز)"));
    }
    { auto& m = m_translations[QStringLiteral("akp_method_hybrid_hw")];
      m.insert(QStringLiteral("ar"), QStringLiteral("3. ★ Hybrid VK + ScanCode + Zero-Sig (مقدمة — تخطي الحماية)"));
      m.insert(QStringLiteral("en"), QStringLiteral("3. ★ Hybrid VK + ScanCode + Zero-Sig (Foreground Stealth)"));
      m.insert(QStringLiteral("es"), QStringLiteral("3. ★ Híbrido VK + ScanCode + Zero-Sig (Sigilo en primer plano)"));
      m.insert(QStringLiteral("fr"), QStringLiteral("3. ★ Hybride VK + ScanCode + Zero-Sig (Furtif premier plan)"));
      m.insert(QStringLiteral("de"), QStringLiteral("3. ★ Hybrid VK + ScanCode + Zero-Sig (Vordergrund-Stealth)"));
      m.insert(QStringLiteral("tr"), QStringLiteral("3. ★ Hibrit VK + ScanCode + Zero-Sig (Ön Plan Gizli)"));
      m.insert(QStringLiteral("ru"), QStringLiteral("3. ★ Гибрид VK + ScanCode + Zero-Sig (Скрытный режим)"));
      m.insert(QStringLiteral("id"), QStringLiteral("3. ★ Hibrida VK + ScanCode + Zero-Sig (Siluman Latar Depan)"));
      m.insert(QStringLiteral("fa"), QStringLiteral("3. ★ ترکیبی VK + ScanCode + بدون امضا (پیش‌زمینه مخفی)"));
      m.insert(QStringLiteral("ur"), QStringLiteral("3. ★ ہائبرڈ VK + ScanCode + Zero-Sig (فرنٹ گراؤنڈ اسٹیلتھ)"));
    }
    { auto& m = m_translations[QStringLiteral("akp_method_ring0")];
      m.insert(QStringLiteral("ar"), QStringLiteral("4. ★ Ring-0 Kernel Interception (مقدمة — درايفر الكيرنل)"));
      m.insert(QStringLiteral("en"), QStringLiteral("4. ★ Ring-0 Kernel Interception (Hardware Driver Filter)"));
      m.insert(QStringLiteral("es"), QStringLiteral("4. ★ Ring-0 Kernel Interception (Controlador de Kernel)"));
      m.insert(QStringLiteral("fr"), QStringLiteral("4. ★ Ring-0 Kernel Interception (Pilote Noyau)"));
      m.insert(QStringLiteral("de"), QStringLiteral("4. ★ Ring-0 Kernel Interception (Kernel-Filtertreiber)"));
      m.insert(QStringLiteral("tr"), QStringLiteral("4. ★ Ring-0 Çekirdek Interception (Donanım Sürücüsü)"));
      m.insert(QStringLiteral("ru"), QStringLiteral("4. ★ Ring-0 Ядро Interception (Драйвер фильтра)"));
      m.insert(QStringLiteral("id"), QStringLiteral("4. ★ Ring-0 Kernel Interception (Driver Filter Kernel)"));
      m.insert(QStringLiteral("fa"), QStringLiteral("4. ★ کرنل Ring-0 Interception (درایور سطح هسته)"));
      m.insert(QStringLiteral("ur"), QStringLiteral("4. ★ Ring-0 کرنل Interception (ہارڈویئر ڈرائیور)"));
    }
    { auto& m = m_translations[QStringLiteral("akp_method_keybd_event")];
      m.insert(QStringLiteral("ar"), QStringLiteral("5. keybd_event (مقدمة — محرك ويندوز الكلاسيكي)"));
      m.insert(QStringLiteral("en"), QStringLiteral("5. keybd_event (Foreground — Legacy Win32 Interrupt)"));
      m.insert(QStringLiteral("es"), QStringLiteral("5. keybd_event (Primer plano — API clásica Win32)"));
      m.insert(QStringLiteral("fr"), QStringLiteral("5. keybd_event (Premier plan — API classique Win32)"));
      m.insert(QStringLiteral("de"), QStringLiteral("5. keybd_event (Vordergrund — Klassische Win32-API)"));
      m.insert(QStringLiteral("tr"), QStringLiteral("5. keybd_event (Ön Plan — Klasik Win32 API)"));
      m.insert(QStringLiteral("ru"), QStringLiteral("5. keybd_event (Активное окно — Классический Win32)"));
      m.insert(QStringLiteral("id"), QStringLiteral("5. keybd_event (Latar Depan — API Win32 Klasik)"));
      m.insert(QStringLiteral("fa"), QStringLiteral("5. keybd_event (پیش‌زمینه — رابط کلاسیک ویندوز)"));
      m.insert(QStringLiteral("ur"), QStringLiteral("5. keybd_event (فرنٹ گراؤنڈ — کلاسک Win32)"));
    }
    { auto& m = m_translations[QStringLiteral("akp_method_unicode")];
      m.insert(QStringLiteral("ar"), QStringLiteral("الطريقة ٤ [Unicode Stream]: إرسال حروف UTF-16 مباشرةً (KEYEVENTF_UNICODE)"));
      m.insert(QStringLiteral("en"), QStringLiteral("Method 4 [Unicode Stream]: Direct UTF-16 Character Packet (KEYEVENTF_UNICODE)"));
      m.insert(QStringLiteral("es"), QStringLiteral("Método 4 [Unicode Stream]: Paquete directo UTF-16 (KEYEVENTF_UNICODE)"));
      m.insert(QStringLiteral("fr"), QStringLiteral("Méthode 4 [Unicode Stream] : Paquet direct UTF-16 (KEYEVENTF_UNICODE)"));
      m.insert(QStringLiteral("de"), QStringLiteral("Methode 4 [Unicode Stream]: Direktes UTF-16-Zeichenpaket (KEYEVENTF_UNICODE)"));
      m.insert(QStringLiteral("tr"), QStringLiteral("Yöntem 4 [Unicode Stream]: Doğrudan UTF-16 Karakter Paketi (KEYEVENTF_UNICODE)"));
      m.insert(QStringLiteral("ru"), QStringLiteral("Метод 4 [Unicode Stream]: Прямой UTF-16 пакет символов (KEYEVENTF_UNICODE)"));
      m.insert(QStringLiteral("id"), QStringLiteral("Metode 4 [Unicode Stream]: Paket Karakter UTF-16 Langsung (KEYEVENTF_UNICODE)"));
      m.insert(QStringLiteral("fa"), QStringLiteral("روش ۴ [Unicode Stream]: ارسال مستقیم کاراکترهای UTF-16 (KEYEVENTF_UNICODE)"));
      m.insert(QStringLiteral("ur"), QStringLiteral("طریقہ 4 [Unicode Stream]: براہِ راست UTF-16 کریکٹر پیکٹ (KEYEVENTF_UNICODE)"));
    }
    { auto& m = m_translations[QStringLiteral("akp_method_postmessage")];
      m.insert(QStringLiteral("ar"), QStringLiteral("الطريقة ١١ [خلفية PostMessage]: إرسال غير متزامن لنافذة البرنامج (WM_KEYDOWN / WM_KEYUP)"));
      m.insert(QStringLiteral("en"), QStringLiteral("Method 11 [Background PostMessage]: Async Window Message Queue (WM_KEYDOWN / WM_KEYUP)"));
      m.insert(QStringLiteral("es"), QStringLiteral("Método 11 [Fondo PostMessage]: Cola de mensajes asíncrona (WM_KEYDOWN / WM_KEYUP)"));
      m.insert(QStringLiteral("fr"), QStringLiteral("Méthode 11 [Arrière-plan PostMessage] : File de messages asynchrone (WM_KEYDOWN / WM_KEYUP)"));
      m.insert(QStringLiteral("de"), QStringLiteral("Methode 11 [Hintergrund PostMessage]: Asynchrone Fensternachricht (WM_KEYDOWN / WM_KEYUP)"));
      m.insert(QStringLiteral("tr"), QStringLiteral("Yöntem 11 [Arka Plan PostMessage]: Asenkron Pencere Mesajı (WM_KEYDOWN / WM_KEYUP)"));
      m.insert(QStringLiteral("ru"), QStringLiteral("Метод 11 [Фон PostMessage]: Асинхронная очередь окна (WM_KEYDOWN / WM_KEYUP)"));
      m.insert(QStringLiteral("id"), QStringLiteral("Metode 11 [Latar PostMessage]: Antrean Pesan Jendela Asinkron (WM_KEYDOWN / WM_KEYUP)"));
      m.insert(QStringLiteral("fa"), QStringLiteral("روش ۱۱ [پس‌زمینه PostMessage]: ارسال غیرهمگام به پنجره هدف (WM_KEYDOWN / WM_KEYUP)"));
      m.insert(QStringLiteral("ur"), QStringLiteral("طریقہ 11 [پس منظر PostMessage]: ونڈو کو غیر ہم وقت ساز پیغام (WM_KEYDOWN / WM_KEYUP)"));
    }
    { auto& m = m_translations[QStringLiteral("akp_method_sendmessage")];
      m.insert(QStringLiteral("ar"), QStringLiteral("الطريقة ١٢ [خلفية SendMessage]: إرسال متزامن بانتظار استجابة النافذة (SendMessageTimeoutW)"));
      m.insert(QStringLiteral("en"), QStringLiteral("Method 12 [Background SendMessage]: Sync Window Procedure Dispatch (SendMessageTimeoutW)"));
      m.insert(QStringLiteral("es"), QStringLiteral("Método 12 [Fondo SendMessage]: Envío síncrono a la ventana (SendMessageTimeoutW)"));
      m.insert(QStringLiteral("fr"), QStringLiteral("Méthode 12 [Arrière-plan SendMessage] : Envoi synchrone à la fenêtre (SendMessageTimeoutW)"));
      m.insert(QStringLiteral("de"), QStringLiteral("Methode 12 [Hintergrund SendMessage]: Synchroner Fensteraufruf (SendMessageTimeoutW)"));
      m.insert(QStringLiteral("tr"), QStringLiteral("Yöntem 12 [Arka Plan SendMessage]: Senkron Pencere İletimi (SendMessageTimeoutW)"));
      m.insert(QStringLiteral("ru"), QStringLiteral("Метод 12 [Фон SendMessage]: Синхронная отправка окну (SendMessageTimeoutW)"));
      m.insert(QStringLiteral("id"), QStringLiteral("Metode 12 [Latar SendMessage]: Pengiriman Sinkron Jendela (SendMessageTimeoutW)"));
      m.insert(QStringLiteral("fa"), QStringLiteral("روش ۱۲ [پس‌زمینه SendMessage]: ارسال همگام با انتظار پاسخ پنجره (SendMessageTimeoutW)"));
      m.insert(QStringLiteral("ur"), QStringLiteral("طریقہ 12 [پس منظر SendMessage]: ونڈو کے جواب کے انتظار کے ساتھ ہم وقت ساز ارسال"));
    }
    { auto& m = m_translations[QStringLiteral("akp_method_post_child")];
      m.insert(QStringLiteral("ar"), QStringLiteral("الطريقة ١٣ [خلفية Child HWND]: توجيه مباشر لأعمق نافذة ريندر داخلية (EnumChildWindows)"));
      m.insert(QStringLiteral("en"), QStringLiteral("Method 13 [Background Child HWND]: Deepest Inner Render Canvas HWND (EnumChildWindows)"));
      m.insert(QStringLiteral("es"), QStringLiteral("Método 13 [Fondo Child HWND]: Ventana hija interna de renderizado (EnumChildWindows)"));
      m.insert(QStringLiteral("fr"), QStringLiteral("Méthode 13 [Arrière-plan Child HWND] : Sous-fenêtre interne de rendu (EnumChildWindows)"));
      m.insert(QStringLiteral("de"), QStringLiteral("Methode 13 [Hintergrund Child HWND]: Tiefstes inneres Kindfenster (EnumChildWindows)"));
      m.insert(QStringLiteral("tr"), QStringLiteral("Yöntem 13 [Arka Plan Child HWND]: En Derin Alt Pencereye İletim (EnumChildWindows)"));
      m.insert(QStringLiteral("ru"), QStringLiteral("Метод 13 [Фон Child HWND]: Отправка во внутреннее дочернее окно (EnumChildWindows)"));
      m.insert(QStringLiteral("id"), QStringLiteral("Metode 13 [Latar Child HWND]: Jendela Anak Render Terdalam (EnumChildWindows)"));
      m.insert(QStringLiteral("fa"), QStringLiteral("روش ۱۳ [پس‌زمینه Child HWND]: ارسال به عمیق‌ترین پنجره فرزند داخلی (EnumChildWindows)"));
      m.insert(QStringLiteral("ur"), QStringLiteral("طریقہ 13 [پس منظر Child HWND]: اندرونی چائلڈ ونڈو کو براہِ راست ارسال (EnumChildWindows)"));
    }
    { auto& m = m_translations[QStringLiteral("akp_method_post_children")];
      m.insert(QStringLiteral("ar"), QStringLiteral("الطريقة ١٤ [خلفية All Children]: بث شامل للنافذة الرئيسية وجميع النوافذ الفرعية معاً"));
      m.insert(QStringLiteral("en"), QStringLiteral("Method 14 [Background All Children]: Broadcast PostMessage to All Child HWNDs"));
      m.insert(QStringLiteral("es"), QStringLiteral("Método 14 [Fondo All Children]: Difusión a todas las ventanas hijas"));
      m.insert(QStringLiteral("fr"), QStringLiteral("Méthode 14 [Arrière-plan All Children] : Diffusion à toutes les sous-fenêtres"));
      m.insert(QStringLiteral("de"), QStringLiteral("Methode 14 [Hintergrund All Children]: Broadcast an alle Kindfenster"));
      m.insert(QStringLiteral("tr"), QStringLiteral("Yöntem 14 [Arka Plan All Children]: Tüm Alt Pencerelere Toplu İletim"));
      m.insert(QStringLiteral("ru"), QStringLiteral("Метод 14 [Фон All Children]: Широковещательная отправка всем дочерним окнам"));
      m.insert(QStringLiteral("id"), QStringLiteral("Metode 14 [Latar All Children]: Siaran ke Semua Jendela Anak"));
      m.insert(QStringLiteral("fa"), QStringLiteral("روش ۱۴ [پس‌زمینه All Children]: ارسال همزمان به پنجره اصلی و تمام پنجره‌های فرعی"));
      m.insert(QStringLiteral("ur"), QStringLiteral("طریقہ 14 [پس منظر All Children]: تمام ذیلی ونڈوز کو بیک وقت ارسال"));
    }
    { auto& m = m_translations[QStringLiteral("akp_method_post_active")];
      m.insert(QStringLiteral("ar"), QStringLiteral("الطريقة ١٥ [خلفية Focus Spoof]: إيهام النافذة بالتركيز (WM_ACTIVATE) دون سحب الماوس"));
      m.insert(QStringLiteral("en"), QStringLiteral("Method 15 [Background Focus Spoof]: WM_ACTIVATE(WA_ACTIVE) + PostMessage (No Focus Steal)"));
      m.insert(QStringLiteral("es"), QStringLiteral("Método 15 [Fondo Focus Spoof]: Simulación de foco WM_ACTIVATE sin robar foco"));
      m.insert(QStringLiteral("fr"), QStringLiteral("Méthode 15 [Arrière-plan Focus Spoof] : Simulation de focus WM_ACTIVATE sans voler le focus"));
      m.insert(QStringLiteral("de"), QStringLiteral("Methode 15 [Hintergrund Focus Spoof]: WM_ACTIVATE Fokus-Simulation ohne Fokusverlust"));
      m.insert(QStringLiteral("tr"), QStringLiteral("Yöntem 15 [Arka Plan Focus Spoof]: Odak Çalmadan WM_ACTIVATE Odak Simülasyonu"));
      m.insert(QStringLiteral("ru"), QStringLiteral("Метод 15 [Фон Focus Spoof]: Имитация фокуса WM_ACTIVATE без перехвата окна"));
      m.insert(QStringLiteral("id"), QStringLiteral("Metode 15 [Latar Focus Spoof]: Simulasi Fokus WM_ACTIVATE Tanpa Mencuri Fokus"));
      m.insert(QStringLiteral("fa"), QStringLiteral("روش ۱۵ [پس‌زمینه Focus Spoof]: شبیه‌سازی فوکوس (WM_ACTIVATE) بدون پرش پنجره"));
      m.insert(QStringLiteral("ur"), QStringLiteral("طریقہ 15 [پس منظر Focus Spoof]: فوکس چرائے بغیر ونڈو کو فعال ظاہر کرنا (WM_ACTIVATE)"));
    }
    { auto& m = m_translations[QStringLiteral("akp_method_attach_state")];
      m.insert(QStringLiteral("ar"), QStringLiteral("الطريقة ١٦ ★ [خلفية KeyState Sync]: ربط Thread النافذة + تحديث مصفوفة الكيبورد (AttachThreadInput)"));
      m.insert(QStringLiteral("en"), QStringLiteral("Method 16 ★ [Background KeyState Sync]: AttachThreadInput + 256-Byte SetKeyboardState + PostMessage"));
      m.insert(QStringLiteral("es"), QStringLiteral("Método 16 ★ [Fondo KeyState Sync]: AttachThreadInput + SetKeyboardState + PostMessage"));
      m.insert(QStringLiteral("fr"), QStringLiteral("Méthode 16 ★ [Arrière-plan KeyState Sync] : AttachThreadInput + SetKeyboardState + PostMessage"));
      m.insert(QStringLiteral("de"), QStringLiteral("Methode 16 ★ [Hintergrund KeyState Sync]: AttachThreadInput + SetKeyboardState + PostMessage"));
      m.insert(QStringLiteral("tr"), QStringLiteral("Yöntem 16 ★ [Arka Plan KeyState Sync]: AttachThreadInput + SetKeyboardState + PostMessage"));
      m.insert(QStringLiteral("ru"), QStringLiteral("Метод 16 ★ [Фон KeyState Sync]: AttachThreadInput + SetKeyboardState + PostMessage"));
      m.insert(QStringLiteral("id"), QStringLiteral("Metode 16 ★ [Latar KeyState Sync]: AttachThreadInput + SetKeyboardState + PostMessage"));
      m.insert(QStringLiteral("fa"), QStringLiteral("روش ۱۶ ★ [پس‌زمینه KeyState Sync]: اتصال Thread + بروزرسانی جدول کیبورد (AttachThreadInput)"));
      m.insert(QStringLiteral("ur"), QStringLiteral("طریقہ 16 ★ [پس منظر KeyState Sync]: AttachThreadInput + SetKeyboardState + PostMessage"));
    }
    { auto& m = m_translations[QStringLiteral("akp_method_guithread_focus")];
      m.insert(QStringLiteral("ar"), QStringLiteral("الطريقة ١٧ ★ [خلفية GUIThreadInfo]: استهداف عنصر التركيز النشط للبرامج المحمية (GetGUIThreadInfo)"));
      m.insert(QStringLiteral("en"), QStringLiteral("Method 17 ★ [Background GUIThreadInfo]: Live Focus HWND Discovery + KeyState Sync"));
      m.insert(QStringLiteral("es"), QStringLiteral("Método 17 ★ [Fondo GUIThreadInfo]: Descubrimiento de foco activo + KeyState Sync"));
      m.insert(QStringLiteral("fr"), QStringLiteral("Méthode 17 ★ [Arrière-plan GUIThreadInfo] : Détection du focus actif + KeyState Sync"));
      m.insert(QStringLiteral("de"), QStringLiteral("Methode 17 ★ [Hintergrund GUIThreadInfo]: Aktives Fokus-HWND + KeyState Sync"));
      m.insert(QStringLiteral("tr"), QStringLiteral("Yöntem 17 ★ [Arka Plan GUIThreadInfo]: Canlı Odak HWND Tespiti + KeyState Sync"));
      m.insert(QStringLiteral("ru"), QStringLiteral("Метод 17 ★ [Фон GUIThreadInfo]: Поиск активного фокуса потока + KeyState Sync"));
      m.insert(QStringLiteral("id"), QStringLiteral("Metode 17 ★ [Latar GUIThreadInfo]: Deteksi HWND Fokus Aktif + KeyState Sync"));
      m.insert(QStringLiteral("fa"), QStringLiteral("روش ۱۷ ★ [پس‌زمینه GUIThreadInfo]: شناسایی عنصر فوکوس فعال در برنامه‌های محافظت‌شده"));
      m.insert(QStringLiteral("ur"), QStringLiteral("طریقہ 17 ★ [پس منظر GUIThreadInfo]: محفوظ پروگرامز کے فعال فوکس عنصر کو براہِ راست ارسال"));
    }
    { auto& m = m_translations[QStringLiteral("akp_method_wm_char_triplet")];
      m.insert(QStringLiteral("ar"), QStringLiteral("الطريقة ١٨ ★ [خلفية Key+Char Triplet]: الثلاثية الكاملة (WM_KEYDOWN + WM_CHAR + WM_KEYUP)"));
      m.insert(QStringLiteral("en"), QStringLiteral("Method 18 ★ [Background Key+Char Triplet]: Full WM_KEYDOWN + WM_CHAR + WM_KEYUP Sequence"));
      m.insert(QStringLiteral("es"), QStringLiteral("Método 18 ★ [Fondo Key+Char Triplet]: Secuencia completa WM_KEYDOWN + WM_CHAR + WM_KEYUP"));
      m.insert(QStringLiteral("fr"), QStringLiteral("Méthode 18 ★ [Arrière-plan Key+Char Triplet] : Séquence complète WM_KEYDOWN + WM_CHAR + WM_KEYUP"));
      m.insert(QStringLiteral("de"), QStringLiteral("Methode 18 ★ [Hintergrund Key+Char Triplet]: Vollständige WM_KEYDOWN + WM_CHAR + WM_KEYUP Sequenz"));
      m.insert(QStringLiteral("tr"), QStringLiteral("Yöntem 18 ★ [Arka Plan Key+Char Triplet]: Tam WM_KEYDOWN + WM_CHAR + WM_KEYUP Dizisi"));
      m.insert(QStringLiteral("ru"), QStringLiteral("Метод 18 ★ [Фон Key+Char Triplet]: Полная тройка WM_KEYDOWN + WM_CHAR + WM_KEYUP"));
      m.insert(QStringLiteral("id"), QStringLiteral("Metode 18 ★ [Latar Key+Char Triplet]: Urutan Lengkap WM_KEYDOWN + WM_CHAR + WM_KEYUP"));
      m.insert(QStringLiteral("fa"), QStringLiteral("روش ۱۸ ★ [پس‌زمینه Key+Char Triplet]: ارسال سه‌گانه کامل (WM_KEYDOWN + WM_CHAR + WM_KEYUP)"));
      m.insert(QStringLiteral("ur"), QStringLiteral("طریقہ 18 ★ [پس منظر Key+Char Triplet]: مکمل ثلاثیہ (WM_KEYDOWN + WM_CHAR + WM_KEYUP)"));
    }
    { auto& m = m_translations[QStringLiteral("akp_method_ime_pipeline")];
      m.insert(QStringLiteral("ar"), QStringLiteral("الطريقة ١٩ ★ [خلفية IME Pipeline]: التمرير عبر قناة مدخلات النظام (WM_IME_KEYDOWN / UP)"));
      m.insert(QStringLiteral("en"), QStringLiteral("Method 19 ★ [Background IME Pipeline]: OS Input Method Editor Bypass (WM_IME_KEYDOWN / UP)"));
      m.insert(QStringLiteral("es"), QStringLiteral("Método 19 ★ [Fondo IME Pipeline]: Canal IME del sistema (WM_IME_KEYDOWN / UP)"));
      m.insert(QStringLiteral("fr"), QStringLiteral("Méthode 19 ★ [Arrière-plan IME Pipeline] : Canal IME du système (WM_IME_KEYDOWN / UP)"));
      m.insert(QStringLiteral("de"), QStringLiteral("Methode 19 ★ [Hintergrund IME Pipeline]: OS-IME-Kanal Bypass (WM_IME_KEYDOWN / UP)"));
      m.insert(QStringLiteral("tr"), QStringLiteral("Yöntem 19 ★ [Arka Plan IME Pipeline]: Sistem IME Kanalı Geçişi (WM_IME_KEYDOWN / UP)"));
      m.insert(QStringLiteral("ru"), QStringLiteral("Метод 19 ★ [Фон IME Pipeline]: Обход через системный канал IME (WM_IME_KEYDOWN / UP)"));
      m.insert(QStringLiteral("id"), QStringLiteral("Metode 19 ★ [Latar IME Pipeline]: Jalur IME Sistem (WM_IME_KEYDOWN / UP)"));
      m.insert(QStringLiteral("fa"), QStringLiteral("روش ۱۹ ★ [پس‌زمینه IME Pipeline]: عبور از کانال ورودی سیستم (WM_IME_KEYDOWN / UP)"));
      m.insert(QStringLiteral("ur"), QStringLiteral("طریقہ 19 ★ [پس منظر IME Pipeline]: سسٹم IME چینل کے ذریعے ارسال (WM_IME_KEYDOWN / UP)"));
    }
    { auto& m = m_translations[QStringLiteral("akp_method_sendnotify_tree")];
      m.insert(QStringLiteral("ar"), QStringLiteral("الطريقة ٢٠ ★ [خلفية SendNotify Tree]: إرسال فوري عبر النوى لشجرة النوافذ بالكامل (SendNotifyMessageW)"));
      m.insert(QStringLiteral("en"), QStringLiteral("Method 20 ★ [Background SendNotify Tree]: Cross-Thread Non-Blocking Tree Dispatch (SendNotifyMessageW)"));
      m.insert(QStringLiteral("es"), QStringLiteral("Método 20 ★ [Fondo SendNotify Tree]: Envío instantáneo a todo el árbol de ventanas (SendNotifyMessageW)"));
      m.insert(QStringLiteral("fr"), QStringLiteral("Méthode 20 ★ [Arrière-plan SendNotify Tree] : Envoi instantané à tout l'arbre de fenêtres (SendNotifyMessageW)"));
      m.insert(QStringLiteral("de"), QStringLiteral("Methode 20 ★ [Hintergrund SendNotify Tree]: Sofort-Dispatch an den gesamten Fensterbaum (SendNotifyMessageW)"));
      m.insert(QStringLiteral("tr"), QStringLiteral("Yöntem 20 ★ [Arka Plan SendNotify Tree]: Tüm Pencere Ağacına Anında İletim (SendNotifyMessageW)"));
      m.insert(QStringLiteral("ru"), QStringLiteral("Метод 20 ★ [Фон SendNotify Tree]: Мгновенная отправка всему дереву окон (SendNotifyMessageW)"));
      m.insert(QStringLiteral("id"), QStringLiteral("Metode 20 ★ [Latar SendNotify Tree]: Pengiriman Instan ke Seluruh Pohon Jendela (SendNotifyMessageW)"));
      m.insert(QStringLiteral("fa"), QStringLiteral("روش ۲۰ ★ [پس‌زمینه SendNotify Tree]: ارسال فوری به کل درخت پنجره‌ها (SendNotifyMessageW)"));
      m.insert(QStringLiteral("ur"), QStringLiteral("طریقہ 20 ★ [پس منظر SendNotify Tree]: ونڈوز کے پورے شجرے کو فوری ارسال (SendNotifyMessageW)"));
    }
    { auto& m = m_translations[QStringLiteral("akp_mouse_mode_auto")];
      m.insert(QStringLiteral("ar"), QStringLiteral("1. تلقائي (يتبع وضع التبويب: خلفية أو مقدمة)"));
      m.insert(QStringLiteral("en"), QStringLiteral("1. Auto (Follows tab Foreground / Background mode)"));
      m.insert(QStringLiteral("es"), QStringLiteral("1. Automático (Sigue el modo de la pestaña)"));
      m.insert(QStringLiteral("fr"), QStringLiteral("1. Automatique (Suit le mode de l'onglet)"));
      m.insert(QStringLiteral("de"), QStringLiteral("1. Automatisch (Folgt dem Tab-Modus)"));
      m.insert(QStringLiteral("tr"), QStringLiteral("1. Otomatik (Sekme modunu takip eder)"));
      m.insert(QStringLiteral("ru"), QStringLiteral("1. Авто (Следует режиму вкладки)"));
      m.insert(QStringLiteral("id"), QStringLiteral("1. Otomatis (Mengikuti mode tab)"));
      m.insert(QStringLiteral("fa"), QStringLiteral("1. خودکار (مطابق حالت پیش‌زمینه/پس‌زمینه زبانه)"));
      m.insert(QStringLiteral("ur"), QStringLiteral("1. خودکار (ٹیب موڈ کے مطابق)"));
    }
    { auto& m = m_translations[QStringLiteral("akp_mouse_mode_real")];
      m.insert(QStringLiteral("ar"), QStringLiteral("2. Real Click (تحريك الماوس الفعلي والضغط)"));
      m.insert(QStringLiteral("en"), QStringLiteral("2. Real Click (Moves physical cursor & clicks)"));
      m.insert(QStringLiteral("es"), QStringLiteral("2. Clic Real (Mueve el cursor físico y hace clic)"));
      m.insert(QStringLiteral("fr"), QStringLiteral("2. Clic Réel (Déplace le curseur et clique)"));
      m.insert(QStringLiteral("de"), QStringLiteral("2. Echter Klick (Bewegt den Mauszeiger & klickt)"));
      m.insert(QStringLiteral("tr"), QStringLiteral("2. Gerçek Tıklama (İmleci taşır ve tıklar)"));
      m.insert(QStringLiteral("ru"), QStringLiteral("2. Реальный клик (Перемещает курсор и кликает)"));
      m.insert(QStringLiteral("id"), QStringLiteral("2. Klik Nyata (Menggerakkan kursor & klik)"));
      m.insert(QStringLiteral("fa"), QStringLiteral("2. کلیک واقعی (حرکت نشانگر ماوس و کلیک)"));
      m.insert(QStringLiteral("ur"), QStringLiteral("2. حقیقی کلک (کرسر ہلا کر کلک کرتا ہے)"));
    }
    { auto& m = m_translations[QStringLiteral("akp_mouse_mode_post")];
      m.insert(QStringLiteral("ar"), QStringLiteral("3. Background Inner Window (كليك في الخلفية للنافذة الداخلية)"));
      m.insert(QStringLiteral("en"), QStringLiteral("3. Background Inner Window (PostMessage to child control)"));
      m.insert(QStringLiteral("es"), QStringLiteral("3. Segundo Plano Interior (PostMessage al control hijo)"));
      m.insert(QStringLiteral("fr"), QStringLiteral("3. Arrière-Plan Interne (PostMessage au contrôle enfant)"));
      m.insert(QStringLiteral("de"), QStringLiteral("3. Hintergrund-Innenfenster (PostMessage an Kind-Steuerelement)"));
      m.insert(QStringLiteral("tr"), QStringLiteral("3. Arka Plan İç Pencere (Alt denetime PostMessage)"));
      m.insert(QStringLiteral("ru"), QStringLiteral("3. В фоне внутреннее окно (PostMessage дочернему элементу)"));
      m.insert(QStringLiteral("id"), QStringLiteral("3. Jendela Dalam Latar (PostMessage ke kontrol anak)"));
      m.insert(QStringLiteral("fa"), QStringLiteral("3. پنجره داخلی در پس‌زمینه (PostMessage به کنترل فرزند)"));
      m.insert(QStringLiteral("ur"), QStringLiteral("3. پس منظر اندرونی ونڈو (PostMessage چائلڈ کنٹرول)"));
    }
    { auto& m = m_translations[QStringLiteral("akp_mouse_mode_post_top")];
      m.insert(QStringLiteral("ar"), QStringLiteral("4. Background Main Window (كليك في الخلفية للنافذة الرئيسية)"));
      m.insert(QStringLiteral("en"), QStringLiteral("4. Background Main Window (PostMessage to top-level HWND)"));
      m.insert(QStringLiteral("es"), QStringLiteral("4. Segundo Plano Principal (PostMessage a ventana principal)"));
      m.insert(QStringLiteral("fr"), QStringLiteral("4. Arrière-Plan Principal (PostMessage à la fenêtre principale)"));
      m.insert(QStringLiteral("de"), QStringLiteral("4. Hintergrund-Hauptfenster (PostMessage an Haupt-HWND)"));
      m.insert(QStringLiteral("tr"), QStringLiteral("4. Arka Plan Ana Pencere (Ana pencereye PostMessage)"));
      m.insert(QStringLiteral("ru"), QStringLiteral("4. В фоне главное окно (PostMessage главному окну)"));
      m.insert(QStringLiteral("id"), QStringLiteral("4. Jendela Utama Latar (PostMessage ke HWND utama)"));
      m.insert(QStringLiteral("fa"), QStringLiteral("4. پنجره اصلی در پس‌زمینه (PostMessage به پنجره اصلی)"));
      m.insert(QStringLiteral("ur"), QStringLiteral("4. پس منظر مرکزی ونڈو (PostMessage ٹاپ ونڈو)"));
    }
    { auto& m = m_translations[QStringLiteral("akp_mouse_mode_hittest")];
      m.insert(QStringLiteral("ar"), QStringLiteral("5. ★ HitTest + Activate Chain (سلسلة تفعيل كاملة قبل الكليك)"));
      m.insert(QStringLiteral("en"), QStringLiteral("5. ★ HitTest + Activate Chain (WM_NCHITTEST -> WM_SETCURSOR -> Click)"));
      m.insert(QStringLiteral("es"), QStringLiteral("5. ★ Cadena HitTest + Activar (WM_NCHITTEST -> WM_SETCURSOR -> Clic)"));
      m.insert(QStringLiteral("fr"), QStringLiteral("5. ★ Chaîne HitTest + Activer (WM_NCHITTEST -> WM_SETCURSOR -> Clic)"));
      m.insert(QStringLiteral("de"), QStringLiteral("5. ★ HitTest + Aktivierungskette (WM_NCHITTEST -> WM_SETCURSOR -> Klick)"));
      m.insert(QStringLiteral("tr"), QStringLiteral("5. ★ HitTest + Etkinleştirme Zinciri (WM_NCHITTEST -> WM_SETCURSOR -> Tık)"));
      m.insert(QStringLiteral("ru"), QStringLiteral("5. ★ Цепочка HitTest + Активация (WM_NCHITTEST -> WM_SETCURSOR -> Клик)"));
      m.insert(QStringLiteral("id"), QStringLiteral("5. ★ Rantai HitTest + Aktivasi (WM_NCHITTEST -> WM_SETCURSOR -> Klik)"));
      m.insert(QStringLiteral("fa"), QStringLiteral("5. ★ زنجیره HitTest + فعال‌سازی (WM_NCHITTEST -> WM_SETCURSOR -> کلیک)"));
      m.insert(QStringLiteral("ur"), QStringLiteral("5. ★ HitTest + ایکٹیویٹ چین (WM_NCHITTEST -> کلک)"));
    }
    { auto& m = m_translations[QStringLiteral("akp_mouse_mode_attach")];
      m.insert(QStringLiteral("ar"), QStringLiteral("6. ★ KeyState Sync Click (مزامنة حالة زر الماوس في الخلفية)"));
      m.insert(QStringLiteral("en"), QStringLiteral("6. ★ KeyState Sync Click (AttachThreadInput + Mouse Button State)"));
      m.insert(QStringLiteral("es"), QStringLiteral("6. ★ Clic KeyState Sync (AttachThreadInput + Estado de botón)"));
      m.insert(QStringLiteral("fr"), QStringLiteral("6. ★ Clic KeyState Sync (AttachThreadInput + État du bouton)"));
      m.insert(QStringLiteral("de"), QStringLiteral("6. ★ KeyState-Sync-Klick (AttachThreadInput + Maustastenstatus)"));
      m.insert(QStringLiteral("tr"), QStringLiteral("6. ★ KeyState Senkron Tıklama (AttachThreadInput + Fare Durumu)"));
      m.insert(QStringLiteral("ru"), QStringLiteral("6. ★ Клик с синхронизацией KeyState (AttachThreadInput + Состояние мыши)"));
      m.insert(QStringLiteral("id"), QStringLiteral("6. ★ Klik Sinkron KeyState (AttachThreadInput + Status Tombol)"));
      m.insert(QStringLiteral("fa"), QStringLiteral("6. ★ کلیک همگام KeyState (AttachThreadInput + وضعیت دکمه ماوس)"));
      m.insert(QStringLiteral("ur"), QStringLiteral("6. ★ KeyState سنک کلک (AttachThreadInput + ماؤس اسٹیٹ)"));
    }
    { auto& m = m_translations[QStringLiteral("akp_mouse_mode_guithread")];
      m.insert(QStringLiteral("ar"), QStringLiteral("7. ★ GUIThread Capture Target (استهداف عنصر الالتقاط النشط)"));
      m.insert(QStringLiteral("en"), QStringLiteral("7. ★ GUIThread Capture Target (GetGUIThreadInfo hwndCapture)"));
      m.insert(QStringLiteral("es"), QStringLiteral("7. ★ Objetivo GUIThread Capture (GetGUIThreadInfo hwndCapture)"));
      m.insert(QStringLiteral("fr"), QStringLiteral("7. ★ Cible GUIThread Capture (GetGUIThreadInfo hwndCapture)"));
      m.insert(QStringLiteral("de"), QStringLiteral("7. ★ GUIThread-Capture-Ziel (GetGUIThreadInfo hwndCapture)"));
      m.insert(QStringLiteral("tr"), QStringLiteral("7. ★ GUIThread Yakalama Hedefi (GetGUIThreadInfo hwndCapture)"));
      m.insert(QStringLiteral("ru"), QStringLiteral("7. ★ Цель захвата GUIThread (GetGUIThreadInfo hwndCapture)"));
      m.insert(QStringLiteral("id"), QStringLiteral("7. ★ Target Tangkap GUIThread (GetGUIThreadInfo hwndCapture)"));
      m.insert(QStringLiteral("fa"), QStringLiteral("7. ★ هدف جذب GUIThread (GetGUIThreadInfo hwndCapture)"));
      m.insert(QStringLiteral("ur"), QStringLiteral("7. ★ GUIThread کیپچر ٹارگٹ (hwndCapture)"));
    }
    { auto& m = m_translations[QStringLiteral("akp_mouse_mode_abs")];
      m.insert(QStringLiteral("ar"), QStringLiteral("8. ★ SendInput Absolute 65535 (إحداثيات هاردوير مطلقة)"));
      m.insert(QStringLiteral("en"), QStringLiteral("8. ★ SendInput Absolute 65535 (Normalized Hardware Coordinates)"));
      m.insert(QStringLiteral("es"), QStringLiteral("8. ★ SendInput Absoluto 65535 (Coordenadas hardware normalizadas)"));
      m.insert(QStringLiteral("fr"), QStringLiteral("8. ★ SendInput Absolu 65535 (Coordonnées matérielles normalisées)"));
      m.insert(QStringLiteral("de"), QStringLiteral("8. ★ SendInput Absolut 65535 (Normalisierte Hardware-Koordinaten)"));
      m.insert(QStringLiteral("tr"), QStringLiteral("8. ★ SendInput Mutlak 65535 (Normalize Donanım Koordinatları)"));
      m.insert(QStringLiteral("ru"), QStringLiteral("8. ★ SendInput Абсолютный 65535 (Нормализованные координаты)"));
      m.insert(QStringLiteral("id"), QStringLiteral("8. ★ SendInput Absolut 65535 (Koordinat Perangkat Keras)"));
      m.insert(QStringLiteral("fa"), QStringLiteral("8. ★ SendInput مطلق 65535 (مختصات سخت‌افزاری نرمال‌شده)"));
      m.insert(QStringLiteral("ur"), QStringLiteral("8. ★ SendInput ایبسولیوٹ 65535 (ہارڈویئر کوآرڈینیٹس)"));
    }
    { auto& m = m_translations[QStringLiteral("lbl_ps2_diag_title")];
      m.insert(QStringLiteral("ar"), QStringLiteral("فحص وتثبيت درايفر منفذ البوردة الفيزيائي PS/2 Port 0x60/0x64 (InpOutx64 / i8042prt):"));
      m.insert(QStringLiteral("en"), QStringLiteral("Motherboard PS/2 Port 0x60/0x64 Driver Diagnostics & Setup (InpOutx64 / i8042prt):"));
      m.insert(QStringLiteral("es"), QStringLiteral("Diagnóstico y Configuración del Puerto PS/2 0x60/0x64 (InpOutx64 / i8042prt):"));
      m.insert(QStringLiteral("fr"), QStringLiteral("Diagnostic et Configuration du Port PS/2 0x60/0x64 (InpOutx64 / i8042prt) :"));
      m.insert(QStringLiteral("de"), QStringLiteral("Mainboard PS/2-Port 0x60/0x64 Treiberdiagnose & Setup (InpOutx64 / i8042prt):"));
      m.insert(QStringLiteral("tr"), QStringLiteral("Anakart PS/2 Port 0x60/0x64 Sürücü Tanılama ve Kurulum (InpOutx64 / i8042prt):"));
      m.insert(QStringLiteral("ru"), QStringLiteral("Диагностика и настройка драйвера порта PS/2 0x60/0x64 (InpOutx64 / i8042prt):"));
      m.insert(QStringLiteral("id"), QStringLiteral("Diagnostik & Pengaturan Driver Port PS/2 0x60/0x64 (InpOutx64 / i8042prt):"));
      m.insert(QStringLiteral("fa"), QStringLiteral("بررسی و نصب درایور پورت فیزیکی مادربرد PS/2 Port 0x60/0x64 (InpOutx64 / i8042prt):"));
      m.insert(QStringLiteral("ur"), QStringLiteral("مدر بورڈ PS/2 پورٹ 0x60/0x64 ڈرائیور تشخیص اور سیٹ اپ (InpOutx64 / i8042prt):"));
    }
    { auto& m = m_translations[QStringLiteral("btn_check_ps2")];
      m.insert(QStringLiteral("ar"), QStringLiteral("فحص منفذ PS/2 و InpOutx64 الآن"));
      m.insert(QStringLiteral("en"), QStringLiteral("Check PS/2 Port & InpOutx64 Now"));
      m.insert(QStringLiteral("es"), QStringLiteral("Verificar Puerto PS/2 e InpOutx64"));
      m.insert(QStringLiteral("fr"), QStringLiteral("Vérifier le Port PS/2 et InpOutx64"));
      m.insert(QStringLiteral("de"), QStringLiteral("PS/2-Port & InpOutx64 jetzt prüfen"));
      m.insert(QStringLiteral("tr"), QStringLiteral("PS/2 Port ve InpOutx64'ü Şimdi Denetle"));
      m.insert(QStringLiteral("ru"), QStringLiteral("Проверить порт PS/2 и InpOutx64"));
      m.insert(QStringLiteral("id"), QStringLiteral("Periksa Port PS/2 & InpOutx64 Sekarang"));
      m.insert(QStringLiteral("fa"), QStringLiteral("بررسی پورت PS/2 و InpOutx64 اکنون"));
      m.insert(QStringLiteral("ur"), QStringLiteral("PS/2 پورٹ اور InpOutx64 ابھی چیک کریں"));
    }
    { auto& m = m_translations[QStringLiteral("btn_install_ps2")];
      m.insert(QStringLiteral("ar"), QStringLiteral("تثبيت / ربط ملفات InpOutx64..."));
      m.insert(QStringLiteral("en"), QStringLiteral("Install / Link InpOutx64 Files..."));
      m.insert(QStringLiteral("es"), QStringLiteral("Instalar / Vincular InpOutx64..."));
      m.insert(QStringLiteral("fr"), QStringLiteral("Installer / Lier InpOutx64..."));
      m.insert(QStringLiteral("de"), QStringLiteral("InpOutx64 installieren / verknüpfen..."));
      m.insert(QStringLiteral("tr"), QStringLiteral("InpOutx64 Kur / Bağla..."));
      m.insert(QStringLiteral("ru"), QStringLiteral("Установить / Привязать InpOutx64..."));
      m.insert(QStringLiteral("id"), QStringLiteral("Pasang / Tautkan File InpOutx64..."));
      m.insert(QStringLiteral("fa"), QStringLiteral("نصب / اتصال فایل‌های InpOutx64..."));
      m.insert(QStringLiteral("ur"), QStringLiteral("InpOutx64 فائلیں انسٹال / لنک کریں..."));
    }
    { auto& m = m_translations[QStringLiteral("press_f8_live")];
      m.insert(QStringLiteral("ar"), QStringLiteral("حرّك الماوس للمكان المطلوب واضغط F8 لالتقاط الإحداثيات..."));
      m.insert(QStringLiteral("en"), QStringLiteral("Move cursor to target position and press F8 to capture coordinates..."));
      m.insert(QStringLiteral("es"), QStringLiteral("Mueva el cursor al punto deseado y presione F8 para capturar coordenadas..."));
      m.insert(QStringLiteral("fr"), QStringLiteral("Déplacez le curseur et appuyez sur F8 pour capturer les coordonnées..."));
      m.insert(QStringLiteral("de"), QStringLiteral("Mauszeiger bewegen und F8 drücken, um Koordinaten zu erfassen..."));
      m.insert(QStringLiteral("tr"), QStringLiteral("İmleci hedef konuma getirin ve koordinatları yakalamak için F8 tuşuna basın..."));
      m.insert(QStringLiteral("ru"), QStringLiteral("Наведите курсор на нужную точку и нажмите F8 для захвата координат..."));
      m.insert(QStringLiteral("id"), QStringLiteral("Arahkan kursor ke posisi target lalu tekan F8 untuk menangkap koordinat..."));
      m.insert(QStringLiteral("fa"), QStringLiteral("ماوس را به نقطه مورد نظر ببرید و F8 را برای ثبت مختصات فشار دهید..."));
      m.insert(QStringLiteral("ur"), QStringLiteral("ماؤس کو مطلوبہ جگہ پر لے جائیں اور کوآرڈینیٹس لینے کے لیے F8 دبائیں..."));
    }
    { auto& m = m_translations[QStringLiteral("toast_apps_refreshed")];
      m.insert(QStringLiteral("ar"), QStringLiteral("تم تحديث قائمة البرامج والنوافذ النشطة"));
      m.insert(QStringLiteral("en"), QStringLiteral("Active windows & processes list refreshed"));
      m.insert(QStringLiteral("es"), QStringLiteral("Lista de ventanas y procesos actualizada"));
      m.insert(QStringLiteral("fr"), QStringLiteral("Liste des fenêtres et processus actualisée"));
      m.insert(QStringLiteral("de"), QStringLiteral("Fenster- und Prozessliste aktualisiert"));
      m.insert(QStringLiteral("tr"), QStringLiteral("Etkin pencere ve işlem listesi yenilendi"));
      m.insert(QStringLiteral("ru"), QStringLiteral("Список активных окон и процессов обновлён"));
      m.insert(QStringLiteral("id"), QStringLiteral("Daftar jendela & proses aktif diperbarui"));
      m.insert(QStringLiteral("fa"), QStringLiteral("فهرست برنامه‌ها و پنجره‌های فعال بروزرسانی شد"));
      m.insert(QStringLiteral("ur"), QStringLiteral("فعال ونڈوز اور پروگرامز کی فہرست اپ ڈیٹ ہو گئی"));
    }
    { auto& m = m_translations[QStringLiteral("toast_backup_exported")];
      m.insert(QStringLiteral("ar"), QStringLiteral("تم حفظ الملف بنجاح!"));
      m.insert(QStringLiteral("en"), QStringLiteral("File saved successfully!"));
      m.insert(QStringLiteral("es"), QStringLiteral("Archivo guardado correctamente!"));
      m.insert(QStringLiteral("fr"), QStringLiteral("Fichier enregistré avec succès !"));
      m.insert(QStringLiteral("de"), QStringLiteral("Datei erfolgreich gespeichert!"));
      m.insert(QStringLiteral("tr"), QStringLiteral("Dosya başarıyla kaydedildi!"));
      m.insert(QStringLiteral("ru"), QStringLiteral("Файл успешно сохранён!"));
      m.insert(QStringLiteral("id"), QStringLiteral("File berhasil disimpan!"));
      m.insert(QStringLiteral("fa"), QStringLiteral("فایل با موفقیت ذخیره شد!"));
      m.insert(QStringLiteral("ur"), QStringLiteral("فائل کامیابی سے محفوظ ہو گئی!"));
    }
    { auto& m = m_translations[QStringLiteral("toast_backup_imported")];
      m.insert(QStringLiteral("ar"), QStringLiteral("تم تحميل الملف بنجاح!"));
      m.insert(QStringLiteral("en"), QStringLiteral("File loaded successfully!"));
      m.insert(QStringLiteral("es"), QStringLiteral("Archivo cargado correctamente!"));
      m.insert(QStringLiteral("fr"), QStringLiteral("Fichier chargé avec succès !"));
      m.insert(QStringLiteral("de"), QStringLiteral("Datei erfolgreich geladen!"));
      m.insert(QStringLiteral("tr"), QStringLiteral("Dosya başarıyla yüklendi!"));
      m.insert(QStringLiteral("ru"), QStringLiteral("Файл успешно загружен!"));
      m.insert(QStringLiteral("id"), QStringLiteral("File berhasil dimuat!"));
      m.insert(QStringLiteral("fa"), QStringLiteral("فایل با موفقیت بارگذاری شد!"));
      m.insert(QStringLiteral("ur"), QStringLiteral("فائل کامیابی سے لوڈ ہو گئی!"));
    }
    { auto& m = m_translations[QStringLiteral("akp_btn_add_custom_key")];
      m.insert(QStringLiteral("ar"), QStringLiteral("+ إضافة زرار مخصص"));
      m.insert(QStringLiteral("en"), QStringLiteral("+ Add Custom Key"));
      m.insert(QStringLiteral("es"), QStringLiteral("+ Añadir tecla personalizada"));
      m.insert(QStringLiteral("fr"), QStringLiteral("+ Ajouter une touche personnalisée"));
      m.insert(QStringLiteral("de"), QStringLiteral("+ Eigene Taste hinzufügen"));
      m.insert(QStringLiteral("tr"), QStringLiteral("+ Özel Tuş Ekle"));
      m.insert(QStringLiteral("ru"), QStringLiteral("+ Добавить свою клавишу"));
      m.insert(QStringLiteral("id"), QStringLiteral("+ Tambah Tombol Kustom"));
      m.insert(QStringLiteral("fa"), QStringLiteral("+ افزودن کلید سفارشی"));
      m.insert(QStringLiteral("ur"), QStringLiteral("+ کسٹم بٹن شامل کریں"));
    }
    { auto& m = m_translations[QStringLiteral("akp_btn_capturing_custom_key")];
      m.insert(QStringLiteral("ar"), QStringLiteral("⌨ اضغط أي زرار الآن لإضافته... (Esc للإلغاء)"));
      m.insert(QStringLiteral("en"), QStringLiteral("⌨ Press any key now to add... (Esc to cancel)"));
      m.insert(QStringLiteral("es"), QStringLiteral("⌨ Presione cualquier tecla ahora... (Esc para cancelar)"));
      m.insert(QStringLiteral("fr"), QStringLiteral("⌨ Appuyez sur une touche maintenant... (Échap pour annuler)"));
      m.insert(QStringLiteral("de"), QStringLiteral("⌨ Drücken Sie jetzt eine Taste... (Esc zum Abbrechen)"));
      m.insert(QStringLiteral("tr"), QStringLiteral("⌨ Eklemek için şimdi bir tuşa basın... (İptal: Esc)"));
      m.insert(QStringLiteral("ru"), QStringLiteral("⌨ Нажмите любую клавишу сейчас... (Esc для отмены)"));
      m.insert(QStringLiteral("id"), QStringLiteral("⌨ Tekan tombol apa saja sekarang... (Esc untuk batal)"));
      m.insert(QStringLiteral("fa"), QStringLiteral("⌨ اکنون هر کلیدی را فشار دهید... (Esc برای لغو)"));
      m.insert(QStringLiteral("ur"), QStringLiteral("⌨ شامل کرنے کے لیے ابھی کوئی بھی بٹن دبائیں... (Esc منسوخ)"));
    }
    { auto& m = m_translations[QStringLiteral("akp_toast_custom_key_added")];
      m.insert(QStringLiteral("ar"), QStringLiteral("✓ تمت إضافة الزر المخصص:"));
      m.insert(QStringLiteral("en"), QStringLiteral("✓ Added custom key:"));
      m.insert(QStringLiteral("es"), QStringLiteral("✓ Tecla personalizada añadida:"));
      m.insert(QStringLiteral("fr"), QStringLiteral("✓ Touche personnalisée ajoutée :"));
      m.insert(QStringLiteral("de"), QStringLiteral("✓ Eigene Taste hinzugefügt:"));
      m.insert(QStringLiteral("tr"), QStringLiteral("✓ Özel tuş eklendi:"));
      m.insert(QStringLiteral("ru"), QStringLiteral("✓ Пользовательская клавиша добавлена:"));
      m.insert(QStringLiteral("id"), QStringLiteral("✓ Tombol kustom ditambahkan:"));
      m.insert(QStringLiteral("fa"), QStringLiteral("✓ کلید سفارشی اضافه شد:"));
      m.insert(QStringLiteral("ur"), QStringLiteral("✓ کسٹم بٹن شامل کر دیا گیا:"));
    }
    { auto& m = m_translations[QStringLiteral("akp_btn_test_all_methods")];
      m.insert(QStringLiteral("ar"), QStringLiteral("فحص الكل"));
      m.insert(QStringLiteral("en"), QStringLiteral("Test All"));
      m.insert(QStringLiteral("es"), QStringLiteral("Probar todo"));
      m.insert(QStringLiteral("fr"), QStringLiteral("Tout tester"));
      m.insert(QStringLiteral("de"), QStringLiteral("Alle testen"));
      m.insert(QStringLiteral("tr"), QStringLiteral("Tümünü Test Et"));
      m.insert(QStringLiteral("ru"), QStringLiteral("Проверить все"));
      m.insert(QStringLiteral("id"), QStringLiteral("Uji Semua"));
      m.insert(QStringLiteral("fa"), QStringLiteral("فحص همه"));
      m.insert(QStringLiteral("ur"), QStringLiteral("سب ٹیسٹ کریں"));
    }
    { auto& m = m_translations[QStringLiteral("akp_btn_test_all_mouse_modes")];
      m.insert(QStringLiteral("ar"), QStringLiteral("فحص كل أنماط نقرات الماوس (٨ أنماط)"));
      m.insert(QStringLiteral("en"), QStringLiteral("Test All 8 Mouse Modes on Target"));
      m.insert(QStringLiteral("es"), QStringLiteral("Probar los 8 modos de ratón"));
      m.insert(QStringLiteral("fr"), QStringLiteral("Tester les 8 modes souris sur la cible"));
      m.insert(QStringLiteral("de"), QStringLiteral("Alle 8 Maus-Modi am Ziel testen"));
      m.insert(QStringLiteral("tr"), QStringLiteral("Tüm 8 Fare Modunu Test Et"));
      m.insert(QStringLiteral("ru"), QStringLiteral("Проверить все 8 режимов мыши"));
      m.insert(QStringLiteral("id"), QStringLiteral("Uji Semua 8 Mode Mouse"));
      m.insert(QStringLiteral("fa"), QStringLiteral("تست هر ۸ حالت کلیک ماوس"));
      m.insert(QStringLiteral("ur"), QStringLiteral("ماؤس کے تمام 8 موڈز ٹیسٹ کریں"));
    }
    { auto& m = m_translations[QStringLiteral("akp_log_sweep_header")];
      m.insert(QStringLiteral("ar"), QStringLiteral("بدء فحص واختبار جميع طرق الإرسال على البرنامج المحدد:"));
      m.insert(QStringLiteral("en"), QStringLiteral("Starting comprehensive method compatibility sweep on target:"));
      m.insert(QStringLiteral("es"), QStringLiteral("Iniciando prueba de compatibilidad de métodos en el destino:"));
      m.insert(QStringLiteral("fr"), QStringLiteral("Démarrage du test complet des méthodes sur la cible :"));
      m.insert(QStringLiteral("de"), QStringLiteral("Starte vollständigen Methodentest am Zielprozess:"));
      m.insert(QStringLiteral("tr"), QStringLiteral("Hedef işlem üzerinde kapsamlı yöntem testi başlatılıyor:"));
      m.insert(QStringLiteral("ru"), QStringLiteral("Запуск полной проверки совместимости методов на цели:"));
      m.insert(QStringLiteral("id"), QStringLiteral("Memulai pengujian kompatibilitas semua metode pada target:"));
      m.insert(QStringLiteral("fa"), QStringLiteral("شروع بررسی و تست تمام روش‌های ارسال روی برنامه هدف:"));
      m.insert(QStringLiteral("ur"), QStringLiteral("ہدف پروگرام پر تمام طریقوں کی مطابقت کا ٹیسٹ شروع:"));
    }
    { auto& m = m_translations[QStringLiteral("akp_log_sweep_item_ok")];
      m.insert(QStringLiteral("ar"), QStringLiteral("✓ مقبول ونجح"));
      m.insert(QStringLiteral("en"), QStringLiteral("✓ Accepted & Verified"));
      m.insert(QStringLiteral("es"), QStringLiteral("✓ Aceptado y verificado"));
      m.insert(QStringLiteral("fr"), QStringLiteral("✓ Accepté et vérifié"));
      m.insert(QStringLiteral("de"), QStringLiteral("✓ Akzeptiert & Verifiziert"));
      m.insert(QStringLiteral("tr"), QStringLiteral("✓ Kabul Edildi ve Doğrulandı"));
      m.insert(QStringLiteral("ru"), QStringLiteral("✓ Принято и подтверждено"));
      m.insert(QStringLiteral("id"), QStringLiteral("✓ Diterima & Terverifikasi"));
      m.insert(QStringLiteral("fa"), QStringLiteral("✓ پذیرفته و تایید شد"));
      m.insert(QStringLiteral("ur"), QStringLiteral("✓ قبول و تصدیق شدہ"));
    }
    { auto& m = m_translations[QStringLiteral("akp_log_sweep_item_fail")];
      m.insert(QStringLiteral("ar"), QStringLiteral("✗ مرفوض أو غير متاح"));
      m.insert(QStringLiteral("en"), QStringLiteral("✗ Rejected or Unavailable"));
      m.insert(QStringLiteral("es"), QStringLiteral("✗ Rechazado o no disponible"));
      m.insert(QStringLiteral("fr"), QStringLiteral("✗ Rejeté ou indisponible"));
      m.insert(QStringLiteral("de"), QStringLiteral("✗ Abgelehnt oder nicht verfügbar"));
      m.insert(QStringLiteral("tr"), QStringLiteral("✗ Reddedildi veya Kullanılamıyor"));
      m.insert(QStringLiteral("ru"), QStringLiteral("✗ Отклонено или недоступно"));
      m.insert(QStringLiteral("id"), QStringLiteral("✗ Ditolak atau Tidak Tersedia"));
      m.insert(QStringLiteral("fa"), QStringLiteral("✗ رد شده یا ناموجود"));
      m.insert(QStringLiteral("ur"), QStringLiteral("✗ مسترد یا غیر دستیاب"));
    }
    { auto& m = m_translations[QStringLiteral("akp_log_sweep_summary")];
      m.insert(QStringLiteral("ar"), QStringLiteral("نتيجة فحص الطرق: نجح {ok} طريقة ✓ | رُفض/غير متاح {fail} طريقة ✗"));
      m.insert(QStringLiteral("en"), QStringLiteral("Sweep Complete: {ok} Accepted ✓ | {fail} Rejected/Unavailable ✗"));
      m.insert(QStringLiteral("es"), QStringLiteral("Prueba completada: {ok} aceptados ✓ | {fail} rechazados ✗"));
      m.insert(QStringLiteral("fr"), QStringLiteral("Test terminé : {ok} acceptés ✓ | {fail} rejetés ✗"));
      m.insert(QStringLiteral("de"), QStringLiteral("Test abgeschlossen: {ok} akzeptiert ✓ | {fail} abgelehnt ✗"));
      m.insert(QStringLiteral("tr"), QStringLiteral("Test Tamamlandı: {ok} Kabul Edildi ✓ | {fail} Reddedildi ✗"));
      m.insert(QStringLiteral("ru"), QStringLiteral("Проверка завершена: {ok} принято ✓ | {fail} отклонено ✗"));
      m.insert(QStringLiteral("id"), QStringLiteral("Uji Selesai: {ok} Diterima ✓ | {fail} Ditolak ✗"));
      m.insert(QStringLiteral("fa"), QStringLiteral("نتیجه بررسی روش‌ها: {ok} موفق ✓ | {fail} ناموفق ✗"));
      m.insert(QStringLiteral("ur"), QStringLiteral("ٹیسٹ مکمل: {ok} کامیاب ✓ | {fail} مسترد ✗"));
    }
    { auto& m = m_translations[QStringLiteral("akp_lbl_tab_hotkey")];
      m.insert(QStringLiteral("ar"), QStringLiteral("اختصار تشغيل/إيقاف التبويب:"));
      m.insert(QStringLiteral("en"), QStringLiteral("Tab Start/Stop Hotkey:"));
      m.insert(QStringLiteral("es"), QStringLiteral("Atajo Iniciar/Detener pestaña:"));
      m.insert(QStringLiteral("fr"), QStringLiteral("Raccourci Démarrer/Arrêter l'onglet :"));
      m.insert(QStringLiteral("de"), QStringLiteral("Tab Start/Stopp-Hotkey:"));
      m.insert(QStringLiteral("tr"), QStringLiteral("Sekme Başlat/Durdur Kısayolu:"));
      m.insert(QStringLiteral("ru"), QStringLiteral("Клавиша Старт/Стоп вкладки:"));
      m.insert(QStringLiteral("id"), QStringLiteral("Pintasan Mulai/Hentikan Tab:"));
      m.insert(QStringLiteral("fa"), QStringLiteral("میانبر شروع/توقف تب:"));
      m.insert(QStringLiteral("ur"), QStringLiteral("ٹیب اسٹارٹ/اسٹاپ شارٹ کٹ:"));
    }
    { auto& m = m_translations[QStringLiteral("akp_lbl_stop_all_hotkey")];
      m.insert(QStringLiteral("ar"), QStringLiteral("اختصار إيقاف كل التبويبات:"));
      m.insert(QStringLiteral("en"), QStringLiteral("Stop All Tabs Hotkey:"));
      m.insert(QStringLiteral("es"), QStringLiteral("Atajo Detener todas las pestañas:"));
      m.insert(QStringLiteral("fr"), QStringLiteral("Raccourci Arrêter tous les onglets :"));
      m.insert(QStringLiteral("de"), QStringLiteral("Alle Tabs stoppen Hotkey:"));
      m.insert(QStringLiteral("tr"), QStringLiteral("Tüm Sekmeleri Durdur Kısayolu:"));
      m.insert(QStringLiteral("ru"), QStringLiteral("Клавиша остановки всех вкладок:"));
      m.insert(QStringLiteral("id"), QStringLiteral("Pintasan Hentikan Semua Tab:"));
      m.insert(QStringLiteral("fa"), QStringLiteral("میانبر توقف همه تب‌ها:"));
      m.insert(QStringLiteral("ur"), QStringLiteral("تمام ٹیبز روکنے کا شارٹ کٹ:"));
    }
    { auto& m = m_translations[QStringLiteral("settings_akp_hotkeys_title")];
      m.insert(QStringLiteral("ar"), QStringLiteral("إعدادات اختصارات استوديو الضغط التلقائي (V-AutoKeyPresser)"));
      m.insert(QStringLiteral("en"), QStringLiteral("Auto Key Presser Studio Hotkeys (V-AutoKeyPresser)"));
      m.insert(QStringLiteral("es"), QStringLiteral("Atajos del Estudio de Pulsación Automática (V-AutoKeyPresser)"));
      m.insert(QStringLiteral("fr"), QStringLiteral("Raccourcis du Studio de Frappe Automatique (V-AutoKeyPresser)"));
      m.insert(QStringLiteral("de"), QStringLiteral("Auto-Key-Presser-Studio Tastenkürzel (V-AutoKeyPresser)"));
      m.insert(QStringLiteral("tr"), QStringLiteral("Otomatik Tuş Basıcı Stüdyosu Kısayolları (V-AutoKeyPresser)"));
      m.insert(QStringLiteral("ru"), QStringLiteral("Горячие клавиши студии автонажатия (V-AutoKeyPresser)"));
      m.insert(QStringLiteral("id"), QStringLiteral("Pintasan Studio Penekan Tombol Otomatis (V-AutoKeyPresser)"));
      m.insert(QStringLiteral("fa"), QStringLiteral("تنظیمات میانبرهای استودیو فشار خودکار (V-AutoKeyPresser)"));
      m.insert(QStringLiteral("ur"), QStringLiteral("آٹو کی پریسر اسٹوڈیو شارٹ کٹس (V-AutoKeyPresser)"));
    }
    { auto& m = m_translations[QStringLiteral("settings_akp_hotkeys_note")];
      m.insert(QStringLiteral("ar"), QStringLiteral("💡 ملاحظة: يمكنك تعيين اختصار مستقل لتشغيل أو إيقاف كل تبويب على حدة من داخل قسم «الضغط التلقائي» نفسه، وأي تعديل لاختصار «إيقاف كل التبويبات» هنا أو هناك يتزامن تلقائياً في المكانين."));
      m.insert(QStringLiteral("en"), QStringLiteral("💡 Note: Per-tab Start/Stop toggle hotkeys can be assigned directly inside the 'Auto Presser' tab. Changing the 'Stop All Tabs' hotkey here or in Auto Presser automatically syncs in both places."));
      m.insert(QStringLiteral("es"), QStringLiteral("💡 Nota: Los atajos de cada pestaña se asignan directamente en 'Pulsador Automático'. El atajo 'Detener todas las pestañas' se sincroniza automáticamente en ambos lugares."));
      m.insert(QStringLiteral("fr"), QStringLiteral("💡 Remarque : Les raccourcis par onglet se définissent dans l'onglet « Frappe Auto ». Le raccourci « Arrêter tous les onglets » est synchronisé automatiquement aux deux endroits."));
      m.insert(QStringLiteral("de"), QStringLiteral("💡 Hinweis: Tab-spezifische Start/Stopp-Hotkeys können direkt im Reiter „Auto-Presser“ zugewiesen werden. Änderungen am „Alle Tabs stoppen“-Hotkey werden automatisch synchronisiert."));
      m.insert(QStringLiteral("tr"), QStringLiteral("💡 Not: Her sekmenin kendi Başlat/Durdur kısayolu 'Otomatik Basıcı' sekmesinden atanabilir. 'Tüm Sekmeleri Durdur' kısayolu her iki yerde otomatik senkronize edilir."));
      m.insert(QStringLiteral("ru"), QStringLiteral("💡 Примечание: Клавиши Старт/Стоп для каждой вкладки назначаются в разделе «Автонажатие». Клавиша «Остановить все вкладки» автоматически синхронизируется с настройками."));
      m.insert(QStringLiteral("id"), QStringLiteral("💡 Catatan: Pintasan Mulai/Hentikan per tab dapat diatur langsung di tab 'Penekan Otomatis'. Pintasan 'Hentikan Semua Tab' tersinkronisasi otomatis di kedua tempat."));
      m.insert(QStringLiteral("fa"), QStringLiteral("💡 نکته: میانبر شروع/توقف هر تب را می‌توانید مستقیماً در بخش «فشار خودکار» تعیین کنید. تغییر میانبر «توقف همه تب‌ها» به صورت خودکار در هر دو بخش همگام‌سازی می‌شود."));
      m.insert(QStringLiteral("ur"), QStringLiteral("💡 نوٹ: ہر ٹیب کا الگ اسٹارٹ/اسٹاپ شارٹ کٹ 'آٹو پریسر' ٹیب کے اندر سے مقرر کیا جا سکتا ہے، جبکہ 'تمام ٹیبز روکیں' کا شارٹ کٹ دونوں جگہ خودکار سنک ہوتا ہے۔"));
    }
    { auto& m = m_translations[QStringLiteral("lbl_builtin_36_section")];
      m.insert(QStringLiteral("ar"), QStringLiteral("الأزرار الأساسية (٣٦ زراراً مقسّمة على ٣ أسطر — اضغط على الزرار لتحديده، وسيعمل عند الضغط على «تشغيل التبويب» أو اختصار التبويب):"));
      m.insert(QStringLiteral("en"), QStringLiteral("Built-in 36 Keys (3 Rows x 12 Keys — Click any key to arm/select it; pressing starts when you click 'Start Tab' or press the Tab Hotkey):"));
      m.insert(QStringLiteral("es"), QStringLiteral("36 teclas integradas (3 filas x 12 teclas — Haga clic para activar; inicia al pulsar 'Iniciar pestaña' o el atajo):"));
      m.insert(QStringLiteral("fr"), QStringLiteral("36 touches intégrées (3 lignes x 12 touches — Cliquez pour activer ; démarre via 'Démarrer l'onglet' ou le raccourci) :"));
      m.insert(QStringLiteral("de"), QStringLiteral("36 Standardtasten (3 Reihen x 12 Tasten — Zum Aktivieren anklicken; startet per 'Tab starten' oder Tab-Hotkey):"));
      m.insert(QStringLiteral("tr"), QStringLiteral("36 Yerleşik Tuş (3 Satır x 12 Tuş — Seçmek için tıklayın; 'Sekmeyi Başlat' veya sekme kısayolu ile çalışır):"));
      m.insert(QStringLiteral("ru"), QStringLiteral("36 основных клавиш (3 ряда по 12 клавиш — нажмите для выбора; запуск по кнопке «Запустить вкладку» или горячей клавише):"));
      m.insert(QStringLiteral("id"), QStringLiteral("36 Tombol Bawaan (3 Baris x 12 Tombol — Klik untuk memilih; berjalan saat klik 'Mulai Tab' atau pintasan tab):"));
      m.insert(QStringLiteral("fa"), QStringLiteral("۳۶ کلید اصلی (در ۳ سطر ۱۲ تایی — برای انتخاب روی کلید بزنید؛ با زدن «شروع تب» یا میانبر تب اجرا می‌شود):"));
      m.insert(QStringLiteral("ur"), QStringLiteral("36 بنیادی بٹن (3 قطاریں x 12 بٹن — منتخب کرنے کے لیے بٹن پر کلک کریں؛ 'ٹیب چلائیں' یا شارٹ کٹ دبانے پر چلے گا):"));
    }
    { auto& m = m_translations[QStringLiteral("lbl_custom_keys_section")];
      m.insert(QStringLiteral("ar"), QStringLiteral("الأزرار المخصصة الإضافية:"));
      m.insert(QStringLiteral("en"), QStringLiteral("Custom Added Keys:"));
      m.insert(QStringLiteral("es"), QStringLiteral("Teclas personalizadas añadidas:"));
      m.insert(QStringLiteral("fr"), QStringLiteral("Touches personnalisées ajoutées :"));
      m.insert(QStringLiteral("de"), QStringLiteral("Eigene hinzugefügte Tasten:"));
      m.insert(QStringLiteral("tr"), QStringLiteral("Eklenen Özel Tuşlar:"));
      m.insert(QStringLiteral("ru"), QStringLiteral("Добавленные пользовательские клавиши:"));
      m.insert(QStringLiteral("id"), QStringLiteral("Tombol Kustom Tambahan:"));
      m.insert(QStringLiteral("fa"), QStringLiteral("کلیدهای سفارشی اضافه‌شده:"));
      m.insert(QStringLiteral("ur"), QStringLiteral("اضافی کسٹم بٹن:"));
    }
    { auto& m = m_translations[QStringLiteral("akp_btn_switch_win")];
      m.insert(QStringLiteral("ar"), QStringLiteral("إنتقال"));
      m.insert(QStringLiteral("en"), QStringLiteral("Switch"));
      m.insert(QStringLiteral("es"), QStringLiteral("Ir"));
      m.insert(QStringLiteral("fr"), QStringLiteral("Basculer"));
      m.insert(QStringLiteral("de"), QStringLiteral("Wechseln"));
      m.insert(QStringLiteral("tr"), QStringLiteral("Geç"));
      m.insert(QStringLiteral("ru"), QStringLiteral("Перейти"));
      m.insert(QStringLiteral("id"), QStringLiteral("Beralih"));
      m.insert(QStringLiteral("fa"), QStringLiteral("انتقال"));
      m.insert(QStringLiteral("ur"), QStringLiteral("منتقل"));
    }
    { auto& m = m_translations[QStringLiteral("akp_err_must_select_window")];
      m.insert(QStringLiteral("ar"), QStringLiteral("⚠ لا يمكن إجراء الاختبار على «كل البرامج» — يرجى اختيار نافذة/برنامج محدد أولاً من قائمة «اختيار البرنامج / النافذة»!"));
      m.insert(QStringLiteral("en"), QStringLiteral("⚠ Cannot run test on 'All Applications' — Please select a specific target window/process first!"));
      m.insert(QStringLiteral("es"), QStringLiteral("⚠ No se puede probar en 'Todas las aplicaciones' — ¡Seleccione primero una ventana/programa específico!"));
      m.insert(QStringLiteral("fr"), QStringLiteral("⚠ Impossible de tester sur « Toutes les applications » — Veuillez d'abord choisir une fenêtre cible !"));
      m.insert(QStringLiteral("de"), QStringLiteral("⚠ Test auf „Alle Programme“ nicht möglich — Bitte wählen Sie zuerst ein Zielfenster aus!"));
      m.insert(QStringLiteral("tr"), QStringLiteral("⚠ 'Tüm Uygulamalar' üzerinde test yapılamaz — Lütfen önce belirli bir hedef pencere seçin!"));
      m.insert(QStringLiteral("ru"), QStringLiteral("⚠ Нельзя тестировать на «Все программы» — сначала выберите конкретное окно/процесс!"));
      m.insert(QStringLiteral("id"), QStringLiteral("⚠ Tidak dapat menguji pada 'Semua Aplikasi' — Pilih jendela/program target terlebih dahulu!"));
      m.insert(QStringLiteral("fa"), QStringLiteral("⚠ تست روی «همه برنامه‌ها» مجاز نیست — لطفاً ابتدا یک پنجره/برنامه مشخص را انتخاب کنید!"));
      m.insert(QStringLiteral("ur"), QStringLiteral("⚠ 'تمام پروگرامز' پر ٹیسٹ نہیں ہو سکتا — براہ کرم پہلے کوئی مخصوص ونڈو/پروگرام منتخب کریں!"));
    }
    { auto& m = m_translations[QStringLiteral("akp_err_window_not_found")];
      m.insert(QStringLiteral("ar"), QStringLiteral("⚠ النافذة المحددة غير مفتوحة حالياً — تأكد من تشغيل البرنامج أو حدّث قائمة النوافذ!"));
      m.insert(QStringLiteral("en"), QStringLiteral("⚠ Selected target window is not currently open — Ensure the app is running or refresh the window list!"));
      m.insert(QStringLiteral("es"), QStringLiteral("⚠ La ventana seleccionada no está abierta actualmente — ¡Actualice la lista de ventanas!"));
      m.insert(QStringLiteral("fr"), QStringLiteral("⚠ La fenêtre sélectionnée n'est pas ouverte — Vérifiez que le programme est lancé !"));
      m.insert(QStringLiteral("de"), QStringLiteral("⚠ Das gewählte Fenster ist derzeit nicht geöffnet — Bitte Fensterliste aktualisieren!"));
      m.insert(QStringLiteral("tr"), QStringLiteral("⚠ Seçilen pencere şu anda açık değil — Uygulamanın çalıştığından emin olun!"));
      m.insert(QStringLiteral("ru"), QStringLiteral("⚠ Выбранное окно сейчас не открыто — убедитесь, что программа запущена!"));
      m.insert(QStringLiteral("id"), QStringLiteral("⚠ Jendela yang dipilih tidak terbuka saat ini — Segarkan daftar jendela!"));
      m.insert(QStringLiteral("fa"), QStringLiteral("⚠ پنجره انتخاب‌شده در حال حاضر باز نیست — لیست پنجره‌ها را بروزرسانی کنید!"));
      m.insert(QStringLiteral("ur"), QStringLiteral("⚠ منتخب کردہ ونڈو اس وقت کھلی نہیں ہے — ونڈوز کی فہرست ریفریش کریں!"));
    }
    { auto& m = m_translations[QStringLiteral("akp_badge_used_in_other_tab")];
      m.insert(QStringLiteral("ar"), QStringLiteral("مختار في"));
      m.insert(QStringLiteral("en"), QStringLiteral("Bound in"));
      m.insert(QStringLiteral("es"), QStringLiteral("Usado en"));
      m.insert(QStringLiteral("fr"), QStringLiteral("Utilisé dans"));
      m.insert(QStringLiteral("de"), QStringLiteral("Belegt in"));
      m.insert(QStringLiteral("tr"), QStringLiteral("Kullanılan sekme:"));
      m.insert(QStringLiteral("ru"), QStringLiteral("Выбрано в"));
      m.insert(QStringLiteral("id"), QStringLiteral("Dipakai di"));
      m.insert(QStringLiteral("fa"), QStringLiteral("انتخاب‌شده در"));
      m.insert(QStringLiteral("ur"), QStringLiteral("منتخب شدہ در"));
    }
    { auto& m = m_translations[QStringLiteral("akp_badge_used_in_this_tab")];
      m.insert(QStringLiteral("ar"), QStringLiteral("نافذة هذا التبويب"));
      m.insert(QStringLiteral("en"), QStringLiteral("Current Tab Target"));
      m.insert(QStringLiteral("es"), QStringLiteral("Ventana de esta pestaña"));
      m.insert(QStringLiteral("fr"), QStringLiteral("Cible de cet onglet"));
      m.insert(QStringLiteral("de"), QStringLiteral("Aktuelles Tab-Ziel"));
      m.insert(QStringLiteral("tr"), QStringLiteral("Bu Sekmenin Hedefi"));
      m.insert(QStringLiteral("ru"), QStringLiteral("Цель этой вкладки"));
      m.insert(QStringLiteral("id"), QStringLiteral("Target Tab Ini"));
      m.insert(QStringLiteral("fa"), QStringLiteral("پنجره همین تب"));
      m.insert(QStringLiteral("ur"), QStringLiteral("اس ٹیب کی ونڈو"));
    }
    { auto& m = m_translations[QStringLiteral("akp_tip_key_mode_normal")];
      m.insert(QStringLiteral("ar"), QStringLiteral("الوضع الحالي: إرسال الزرار كحرف عادي (اضغط لتشغيل الماكرو الذكي المرتبط بهذا الزرار إن وُجد)"));
      m.insert(QStringLiteral("en"), QStringLiteral("Current Mode: Normal Key Press (Click to trigger linked Smart Key Macro instead)"));
      m.insert(QStringLiteral("es"), QStringLiteral("Modo actual: Tecla normal (Clic para ejecutar el Macro Inteligente vinculado)"));
      m.insert(QStringLiteral("fr"), QStringLiteral("Mode actuel : Touche normale (Cliquez pour exécuter la macro intelligente liée)"));
      m.insert(QStringLiteral("de"), QStringLiteral("Modus: Normale Taste (Klicken, um verknüpftes Smart-Key-Makro auszuführen)"));
      m.insert(QStringLiteral("tr"), QStringLiteral("Mevcut Mod: Normal Tuş (Bağlı Akıllı Tuş Makrosunu çalıştırmak için tıklayın)"));
      m.insert(QStringLiteral("ru"), QStringLiteral("Режим: Обычная клавиша (Нажмите для запуска связанного умного макроса)"));
      m.insert(QStringLiteral("id"), QStringLiteral("Mode: Tombol Normal (Klik untuk menjalankan Makro Smart Key terkait)"));
      m.insert(QStringLiteral("fa"), QStringLiteral("حالت فعلی: ارسال کلید عادی (برای اجرای ماکروی هوشمند مرتبط کلیک کنید)"));
      m.insert(QStringLiteral("ur"), QStringLiteral("موجودہ موڈ: عام بٹن (مربوط اسمارٹ میکرو چلانے کے لیے کلک کریں)"));
    }
    { auto& m = m_translations[QStringLiteral("akp_tip_key_mode_macro")];
      m.insert(QStringLiteral("ar"), QStringLiteral("⚡ وضع الماكرو الذكي مفعّل: يشغّل الماكرو المربوط بهذا الزرار في «الأزرار الذكية» (اضغط للعودة لإرسال الحرف عادي)"));
      m.insert(QStringLiteral("en"), QStringLiteral("⚡ Smart Macro Mode ON: Executes the Smart Key macro bound to this key (Click to switch back to Normal Key)"));
      m.insert(QStringLiteral("es"), QStringLiteral("⚡ Modo Macro Inteligente ACTIVO: Ejecuta el macro vinculado a esta tecla (Clic para volver a tecla normal)"));
      m.insert(QStringLiteral("fr"), QStringLiteral("⚡ Mode Macro Intelligente ACTIF : Exécute la macro liée à cette touche (Cliquez pour touche normale)"));
      m.insert(QStringLiteral("de"), QStringLiteral("⚡ Smart-Makro-Modus AKTIV: Führt das verknüpfte Smart-Key-Makro aus (Klicken für normale Taste)"));
      m.insert(QStringLiteral("tr"), QStringLiteral("⚡ Akıllı Makro Modu AÇIK: Bu tuşa bağlı makroyu çalıştırır (Normal tuşa dönmek için tıklayın)"));
      m.insert(QStringLiteral("ru"), QStringLiteral("⚡ Режим Умного Макроса ВКЛ: Запускает макрос из «Умных клавиш» (Нажмите для обычной клавиши)"));
      m.insert(QStringLiteral("id"), QStringLiteral("⚡ Mode Makro Pintar AKTIF: Menjalankan makro yang terhubung ke tombol ini (Klik untuk tombol normal)"));
      m.insert(QStringLiteral("fa"), QStringLiteral("⚡ حالت ماکروی هوشمند فعال: ماکروی مرتبط در «کلیدهای هوشمند» را اجرا می‌کند (برای کلید عادی کلیک کنید)"));
      m.insert(QStringLiteral("ur"), QStringLiteral("⚡ اسمارٹ میکرو موڈ فعال: اس بٹن سے جڑا میکرو چلائے گا (عام بٹن کے لیے دوبارہ کلک کریں)"));
    }
    { auto& m = m_translations[QStringLiteral("akp_log_triggered_smart_macro")];
      m.insert(QStringLiteral("ar"), QStringLiteral("⚡ تم تشغيل الماكرو الذكي المرتبط بالزرار:"));
      m.insert(QStringLiteral("en"), QStringLiteral("⚡ Triggered linked Smart Key Macro for:"));
      m.insert(QStringLiteral("es"), QStringLiteral("⚡ Macro Inteligente vinculado ejecutado para:"));
      m.insert(QStringLiteral("fr"), QStringLiteral("⚡ Macro intelligente liée exécutée pour :"));
      m.insert(QStringLiteral("de"), QStringLiteral("⚡ Verknüpftes Smart-Key-Makro ausgeführt für:"));
      m.insert(QStringLiteral("tr"), QStringLiteral("⚡ Bağlı Akıllı Tuş Makrosu çalıştırıldı:"));
      m.insert(QStringLiteral("ru"), QStringLiteral("⚡ Запущен связанный умный макрос для клавиши:"));
      m.insert(QStringLiteral("id"), QStringLiteral("⚡ Makro Smart Key terkait dijalankan untuk:"));
      m.insert(QStringLiteral("fa"), QStringLiteral("⚡ ماکروی هوشمند مرتبط با کلید اجرا شد:"));
      m.insert(QStringLiteral("ur"), QStringLiteral("⚡ بٹن سے مربوط اسمارٹ میکرو چلایا گیا:"));
    }
    { auto& m = m_translations[QStringLiteral("lbl_excluded_apps")];
      m.insert(QStringLiteral("ar"), QStringLiteral("إستثناء"));
      m.insert(QStringLiteral("en"), QStringLiteral("Exclude:"));
      m.insert(QStringLiteral("es"), QStringLiteral("Excluir:"));
      m.insert(QStringLiteral("fr"), QStringLiteral("Exclure :"));
      m.insert(QStringLiteral("de"), QStringLiteral("Ausschluss:"));
      m.insert(QStringLiteral("tr"), QStringLiteral("Hariç:"));
      m.insert(QStringLiteral("ru"), QStringLiteral("Исключить:"));
      m.insert(QStringLiteral("id"), QStringLiteral("Kecualikan:"));
      m.insert(QStringLiteral("fa"), QStringLiteral("استثنا:"));
      m.insert(QStringLiteral("ur"), QStringLiteral("استثنیٰ:"));
    }
    { auto& m = m_translations[QStringLiteral("btn_add_excluded_app")];
      m.insert(QStringLiteral("ar"), QStringLiteral("+ استثناء البرنامج"));
      m.insert(QStringLiteral("en"), QStringLiteral("+ Exclude App"));
      m.insert(QStringLiteral("es"), QStringLiteral("+ Excluir app"));
      m.insert(QStringLiteral("fr"), QStringLiteral("+ Exclure l'app"));
      m.insert(QStringLiteral("de"), QStringLiteral("+ App ausschließen"));
      m.insert(QStringLiteral("tr"), QStringLiteral("+ Hariç Tut"));
      m.insert(QStringLiteral("ru"), QStringLiteral("+ Исключить"));
      m.insert(QStringLiteral("id"), QStringLiteral("+ Kecualikan"));
      m.insert(QStringLiteral("fa"), QStringLiteral("+ افزودن به استثنا"));
      m.insert(QStringLiteral("ur"), QStringLiteral("+ مستثنیٰ کریں"));
    }
    { auto& m = m_translations[QStringLiteral("btn_clear_excluded_apps")];
      m.insert(QStringLiteral("ar"), QStringLiteral("مسح الاستثناءات"));
      m.insert(QStringLiteral("en"), QStringLiteral("Clear Exclusions"));
      m.insert(QStringLiteral("es"), QStringLiteral("Limpiar exclusiones"));
      m.insert(QStringLiteral("fr"), QStringLiteral("Effacer les exclusions"));
      m.insert(QStringLiteral("de"), QStringLiteral("Ausschlüsse löschen"));
      m.insert(QStringLiteral("tr"), QStringLiteral("Hariç Tutulanları Temizle"));
      m.insert(QStringLiteral("ru"), QStringLiteral("Очистить исключения"));
      m.insert(QStringLiteral("id"), QStringLiteral("Hapus Pengecualian"));
      m.insert(QStringLiteral("fa"), QStringLiteral("پاک کردن استثناها"));
      m.insert(QStringLiteral("ur"), QStringLiteral("استثنیٰ صاف کریں"));
    }
    { auto& m = m_translations[QStringLiteral("lbl_no_excluded_apps")];
      m.insert(QStringLiteral("ar"), QStringLiteral("لا توجد برامج مستثناة حالياً"));
      m.insert(QStringLiteral("en"), QStringLiteral("No excluded apps"));
      m.insert(QStringLiteral("es"), QStringLiteral("Sin programas excluidos"));
      m.insert(QStringLiteral("fr"), QStringLiteral("Aucune application exclue"));
      m.insert(QStringLiteral("de"), QStringLiteral("Keine ausgeschlossenen Apps"));
      m.insert(QStringLiteral("tr"), QStringLiteral("Hariç tutulan uygulama yok"));
      m.insert(QStringLiteral("ru"), QStringLiteral("Нет исключённых программ"));
      m.insert(QStringLiteral("id"), QStringLiteral("Tidak ada aplikasi yang dikecualikan"));
      m.insert(QStringLiteral("fa"), QStringLiteral("برنامه‌ای مستثنی نشده است"));
      m.insert(QStringLiteral("ur"), QStringLiteral("کوئی پروگرام مستثنیٰ نہیں"));
    }
    { auto& m = m_translations[QStringLiteral("lbl_test_target_app_opt")];
      m.insert(QStringLiteral("ar"), QStringLiteral("تطبيق مستهدف للتجربة (اختياري):"));
      m.insert(QStringLiteral("en"), QStringLiteral("Target App to Test On (Optional):"));
      m.insert(QStringLiteral("es"), QStringLiteral("Aplicación de destino para probar (Opcional):"));
      m.insert(QStringLiteral("fr"), QStringLiteral("Application cible pour le test (Optionnel) :"));
      m.insert(QStringLiteral("de"), QStringLiteral("Zielprogramm für den Test (Optional):"));
      m.insert(QStringLiteral("tr"), QStringLiteral("Test Edilecek Hedef Uygulama (İsteğe Bağlı):"));
      m.insert(QStringLiteral("ru"), QStringLiteral("Целевое окно для теста (необязательно):"));
      m.insert(QStringLiteral("id"), QStringLiteral("Aplikasi Target Uji (Opsional):"));
      m.insert(QStringLiteral("fa"), QStringLiteral("برنامه هدف برای تست (اختیاری):"));
      m.insert(QStringLiteral("ur"), QStringLiteral("ٹیسٹ کے لیے ہدف پروگرام (اختیاری):"));
    }
    { auto& m = m_translations[QStringLiteral("test_target_app_sandbox_free")];
      m.insert(QStringLiteral("ar"), QStringLiteral("🌐 بدون تحديد — تجربة حرة داخل صندوق الاختبار بالأسفل"));
      m.insert(QStringLiteral("en"), QStringLiteral("🌐 None (Free Test Inside Sandbox Box Below)"));
      m.insert(QStringLiteral("es"), QStringLiteral("🌐 Ninguno (Prueba libre en el cuadro de abajo)"));
      m.insert(QStringLiteral("fr"), QStringLiteral("🌐 Aucune (Test libre dans le bac à sable ci-dessous)"));
      m.insert(QStringLiteral("de"), QStringLiteral("🌐 Keine (Freier Test im Sandbox-Feld unten)"));
      m.insert(QStringLiteral("tr"), QStringLiteral("🌐 Yok (Aşağıdaki Test Kutusunda Serbest Test)"));
      m.insert(QStringLiteral("ru"), QStringLiteral("🌐 Не выбрано — свободный тест в поле песочницы ниже"));
      m.insert(QStringLiteral("id"), QStringLiteral("🌐 Tidak Ada (Uji Bebas di Kotak Sandbox di Bawah)"));
      m.insert(QStringLiteral("fa"), QStringLiteral("🌐 بدون انتخاب — تست آزاد در کادر آزمایش پایین"));
      m.insert(QStringLiteral("ur"), QStringLiteral("🌐 کوئی نہیں — نیچے ٹیسٹ باکس میں آزادانہ ٹیسٹ"));
    }
    { auto& m = m_translations[QStringLiteral("dialog_admin_title")];
      m.insert(QStringLiteral("ar"), QStringLiteral("إعادة التشغيل كمسؤول (Administrator)"));
      m.insert(QStringLiteral("en"), QStringLiteral("Restart as Administrator"));
      m.insert(QStringLiteral("es"), QStringLiteral("Reiniciar como Administrador"));
      m.insert(QStringLiteral("fr"), QStringLiteral("Redémarrer en tant qu'Administrateur"));
      m.insert(QStringLiteral("de"), QStringLiteral("Als Administrator neu starten"));
      m.insert(QStringLiteral("tr"), QStringLiteral("Yönetici Olarak Yeniden Başlat"));
      m.insert(QStringLiteral("ru"), QStringLiteral("Перезапуск от имени администратора"));
      m.insert(QStringLiteral("id"), QStringLiteral("Mulai Ulang sebagai Administrator"));
      m.insert(QStringLiteral("fa"), QStringLiteral("راه‌اندازی مجدد به عنوان مدیر (Administrator)"));
      m.insert(QStringLiteral("ur"), QStringLiteral("ایڈمنسٹریٹر کے طور پر دوبارہ چلائیں"));
    }
    { auto& m = m_translations[QStringLiteral("dialog_admin_body")];
      m.insert(QStringLiteral("ar"), QStringLiteral("هل تريد إعادة تشغيل برنامج V-MacroPlus بصلاحيات المسؤول (Administrator) الآن؟\n\n(مطلوب فقط عند إرسال الضغوطات لبعض الألعاب أو البرامج المحمية)"));
      m.insert(QStringLiteral("en"), QStringLiteral("Do you want to restart V-MacroPlus with Administrator privileges now?\n\n(Only needed when automating elevated/protected games or apps)"));
      m.insert(QStringLiteral("es"), QStringLiteral("¿Desea reiniciar V-MacroPlus con privilegios de Administrador ahora?\n\n(Solo necesario para juegos o aplicaciones protegidas)"));
      m.insert(QStringLiteral("fr"), QStringLiteral("Voulez-vous redémarrer V-MacroPlus en tant qu'Administrateur maintenant ?\n\n(Nécessaire uniquement pour les jeux ou applications protégés)"));
      m.insert(QStringLiteral("de"), QStringLiteral("Möchten Sie V-MacroPlus jetzt mit Administratorrechten neu starten?\n\n(Nur für geschützte Spiele oder Apps erforderlich)"));
      m.insert(QStringLiteral("tr"), QStringLiteral("V-MacroPlus'ı şimdi Yönetici ayrıcalıklarıyla yeniden başlatmak istiyor musunuz?\n\n(Yalnızca korumalı oyunlar/uygulamalar için gereklidir)"));
      m.insert(QStringLiteral("ru"), QStringLiteral("Перезапустить V-MacroPlus с правами администратора сейчас?\n\n(Требуется только для защищённых игр и программ)"));
      m.insert(QStringLiteral("id"), QStringLiteral("Apakah Anda ingin memulai ulang V-MacroPlus sebagai Administrator sekarang?\n\n(Hanya diperlukan untuk game/aplikasi yang dilindungi)"));
      m.insert(QStringLiteral("fa"), QStringLiteral("آیا می‌خواهید V-MacroPlus را با دسترسی مدیر (Administrator) مجدداً راه‌اندازی کنید؟\n\n(فقط برای بازی‌ها یا برنامه‌های محافظت‌شده لازم است)"));
      m.insert(QStringLiteral("ur"), QStringLiteral("کیا آپ V-MacroPlus کو ابھی ایڈمنسٹریٹر کے طور پر دوبارہ چلانا چاہتے ہیں؟\n\n(صرف محفوظ گیمز یا پروگرامز کے لیے ضروری ہے)"));
    }
    { auto& m = m_translations[QStringLiteral("step_add_type_text")];
      m.insert(QStringLiteral("ar"), QStringLiteral("+ كتابة نص"));
      m.insert(QStringLiteral("en"), QStringLiteral("+ Type Text"));
      m.insert(QStringLiteral("es"), QStringLiteral("+ Escribir texto"));
      m.insert(QStringLiteral("fr"), QStringLiteral("+ Saisir texte"));
      m.insert(QStringLiteral("de"), QStringLiteral("+ Text tippen"));
      m.insert(QStringLiteral("tr"), QStringLiteral("+ Metin Yaz"));
      m.insert(QStringLiteral("ru"), QStringLiteral("+ Ввести текст"));
      m.insert(QStringLiteral("id"), QStringLiteral("+ Ketik Teks"));
      m.insert(QStringLiteral("fa"), QStringLiteral("+ تایپ متن"));
      m.insert(QStringLiteral("ur"), QStringLiteral("+ متن لکھیں"));
    }
    { auto& m = m_translations[QStringLiteral("step_row_type_text")];
      m.insert(QStringLiteral("ar"), QStringLiteral("كتابة نص:"));
      m.insert(QStringLiteral("en"), QStringLiteral("Type Text:"));
      m.insert(QStringLiteral("es"), QStringLiteral("Escribir texto:"));
      m.insert(QStringLiteral("fr"), QStringLiteral("Saisir texte :"));
      m.insert(QStringLiteral("de"), QStringLiteral("Text tippen:"));
      m.insert(QStringLiteral("tr"), QStringLiteral("Metin Yaz:"));
      m.insert(QStringLiteral("ru"), QStringLiteral("Ввести текст:"));
      m.insert(QStringLiteral("id"), QStringLiteral("Ketik Teks:"));
      m.insert(QStringLiteral("fa"), QStringLiteral("تایپ متن:"));
      m.insert(QStringLiteral("ur"), QStringLiteral("متن لکھیں:"));
    }
    { auto& m = m_translations[QStringLiteral("ph_type_text")];
      m.insert(QStringLiteral("ar"), QStringLiteral("اكتب النص الذي تريد إدخاله تلقائياً..."));
      m.insert(QStringLiteral("en"), QStringLiteral("Enter text to type automatically..."));
      m.insert(QStringLiteral("es"), QStringLiteral("Escriba el texto a ingresar automáticamente..."));
      m.insert(QStringLiteral("fr"), QStringLiteral("Entrez le texte à saisir automatiquement..."));
      m.insert(QStringLiteral("de"), QStringLiteral("Text für automatische Eingabe eingeben..."));
      m.insert(QStringLiteral("tr"), QStringLiteral("Otomatik yazılacak metni girin..."));
      m.insert(QStringLiteral("ru"), QStringLiteral("Введите текст для автоматического ввода..."));
      m.insert(QStringLiteral("id"), QStringLiteral("Masukkan teks yang akan diketik otomatis..."));
      m.insert(QStringLiteral("fa"), QStringLiteral("متنی را که می‌خواهید تایپ شود وارد کنید..."));
      m.insert(QStringLiteral("ur"), QStringLiteral("خودکار لکھنے کے لیے متن درج کریں..."));
    }
    { auto& m = m_translations[QStringLiteral("step_add_python_script")];
      m.insert(QStringLiteral("ar"), QStringLiteral("+ سكربت بايثون مصغّر"));
      m.insert(QStringLiteral("en"), QStringLiteral("+ Python Mini-Script"));
      m.insert(QStringLiteral("es"), QStringLiteral("+ Mini-Script Python"));
      m.insert(QStringLiteral("fr"), QStringLiteral("+ Mini-Script Python"));
      m.insert(QStringLiteral("de"), QStringLiteral("+ Python Mini-Skript"));
      m.insert(QStringLiteral("tr"), QStringLiteral("+ Python Mini-Betik"));
      m.insert(QStringLiteral("ru"), QStringLiteral("+ Мини-скрипт Python"));
      m.insert(QStringLiteral("id"), QStringLiteral("+ Mini-Skrip Python"));
      m.insert(QStringLiteral("fa"), QStringLiteral("+ اسکریپت کوچک پایتون"));
      m.insert(QStringLiteral("ur"), QStringLiteral("+ پائتھون منی اسکرپٹ"));
    }
    { auto& m = m_translations[QStringLiteral("step_row_python_script")];
      m.insert(QStringLiteral("ar"), QStringLiteral("سكربت بايثون (for / if / while / print):"));
      m.insert(QStringLiteral("en"), QStringLiteral("Python Script (for / if / while / print):"));
      m.insert(QStringLiteral("es"), QStringLiteral("Script Python (for / if / while / print):"));
      m.insert(QStringLiteral("fr"), QStringLiteral("Script Python (for / if / while / print) :"));
      m.insert(QStringLiteral("de"), QStringLiteral("Python-Skript (for / if / while / print):"));
      m.insert(QStringLiteral("tr"), QStringLiteral("Python Betiği (for / if / while / print):"));
      m.insert(QStringLiteral("ru"), QStringLiteral("Python-скрипт (for / if / while / print):"));
      m.insert(QStringLiteral("id"), QStringLiteral("Skrip Python (for / if / while / print):"));
      m.insert(QStringLiteral("fa"), QStringLiteral("اسکریپت پایتون (for / if / while / print):"));
      m.insert(QStringLiteral("ur"), QStringLiteral("پائتھون اسکرپٹ (for / if / while / print):"));
    }
    { auto& m = m_translations[QStringLiteral("ph_python_script")];
      m.insert(QStringLiteral("ar"), QStringLiteral("for i in range(3):\n    print(\"Hello \")\n    press(\"Enter\")\n    wait(200)"));
      m.insert(QStringLiteral("en"), QStringLiteral("for i in range(3):\n    print(\"Hello \")\n    press(\"Enter\")\n    wait(200)"));
      m.insert(QStringLiteral("es"), QStringLiteral("for i in range(3):\n    print(\"Hola \")\n    press(\"Enter\")\n    wait(200)"));
      m.insert(QStringLiteral("fr"), QStringLiteral("for i in range(3):\n    print(\"Bonjour \")\n    press(\"Enter\")\n    wait(200)"));
      m.insert(QStringLiteral("de"), QStringLiteral("for i in range(3):\n    print(\"Hallo \")\n    press(\"Enter\")\n    wait(200)"));
      m.insert(QStringLiteral("tr"), QStringLiteral("for i in range(3):\n    print(\"Merhaba \")\n    press(\"Enter\")\n    wait(200)"));
      m.insert(QStringLiteral("ru"), QStringLiteral("for i in range(3):\n    print(\"Привет \")\n    press(\"Enter\")\n    wait(200)"));
      m.insert(QStringLiteral("id"), QStringLiteral("for i in range(3):\n    print(\"Halo \")\n    press(\"Enter\")\n    wait(200)"));
      m.insert(QStringLiteral("fa"), QStringLiteral("for i in range(3):\n    print(\"سلام \")\n    press(\"Enter\")\n    wait(200)"));
      m.insert(QStringLiteral("ur"), QStringLiteral("for i in range(3):\n    print(\"سلام \")\n    press(\"Enter\")\n    wait(200)"));
    }
    { auto& m = m_translations[QStringLiteral("lbl_test_action_mode")];
      m.insert(QStringLiteral("ar"), QStringLiteral("نوع الاختبار المطلوب:"));
      m.insert(QStringLiteral("en"), QStringLiteral("Test Action Mode:"));
      m.insert(QStringLiteral("es"), QStringLiteral("Modo de prueba:"));
      m.insert(QStringLiteral("fr"), QStringLiteral("Mode d'action du test :"));
      m.insert(QStringLiteral("de"), QStringLiteral("Test-Aktionsmodus:"));
      m.insert(QStringLiteral("tr"), QStringLiteral("Test Eylem Modu:"));
      m.insert(QStringLiteral("ru"), QStringLiteral("Режим теста:"));
      m.insert(QStringLiteral("id"), QStringLiteral("Mode Aksi Uji:"));
      m.insert(QStringLiteral("fa"), QStringLiteral("نوع عملیات تست:"));
      m.insert(QStringLiteral("ur"), QStringLiteral("ٹیسٹ ایکشن موڈ:"));
    }
    { auto& m = m_translations[QStringLiteral("test_mode_macro")];
      m.insert(QStringLiteral("ar"), QStringLiteral("تشغيل ماكرو أو اختبار سريع (W -> Space)"));
      m.insert(QStringLiteral("en"), QStringLiteral("Run Macro / Quick Test (W -> Space)"));
      m.insert(QStringLiteral("es"), QStringLiteral("Ejecutar Macro / Prueba rápida (W -> Space)"));
      m.insert(QStringLiteral("fr"), QStringLiteral("Exécuter Macro / Test rapide (W -> Space)"));
      m.insert(QStringLiteral("de"), QStringLiteral("Makro / Schnelltest ausführen (W -> Space)"));
      m.insert(QStringLiteral("tr"), QStringLiteral("Makro / Hızlı Test Çalıştır (W -> Space)"));
      m.insert(QStringLiteral("ru"), QStringLiteral("Запустить макрос / Быстрый тест (W -> Space)"));
      m.insert(QStringLiteral("id"), QStringLiteral("Jalankan Makro / Uji Cepat (W -> Space)"));
      m.insert(QStringLiteral("fa"), QStringLiteral("اجرای ماکرو / تست سریع (W -> Space)"));
      m.insert(QStringLiteral("ur"), QStringLiteral("میکرو / فوری ٹیسٹ چلائیں (W -> Space)"));
    }
    { auto& m = m_translations[QStringLiteral("test_mode_type_text")];
      m.insert(QStringLiteral("ar"), QStringLiteral("كتابة كلمة أو جملة محددة داخل البرنامج"));
      m.insert(QStringLiteral("en"), QStringLiteral("Type Custom Word / Text into Target"));
      m.insert(QStringLiteral("es"), QStringLiteral("Escribir palabra o texto personalizado"));
      m.insert(QStringLiteral("fr"), QStringLiteral("Saisir un mot ou texte personnalisé"));
      m.insert(QStringLiteral("de"), QStringLiteral("Eigenes Wort / Text in Ziel eingeben"));
      m.insert(QStringLiteral("tr"), QStringLiteral("Hedefe Özel Kelime / Metin Yaz"));
      m.insert(QStringLiteral("ru"), QStringLiteral("Ввести заданное слово / текст в цель"));
      m.insert(QStringLiteral("id"), QStringLiteral("Ketik Kata / Teks Kustom ke Target"));
      m.insert(QStringLiteral("fa"), QStringLiteral("تایپ کلمه یا متن دلخواه در برنامه هدف"));
      m.insert(QStringLiteral("ur"), QStringLiteral("ہدف میں مخصوص لفظ / متن لکھیں"));
    }
    { auto& m = m_translations[QStringLiteral("test_mode_simple_key")];
      m.insert(QStringLiteral("ar"), QStringLiteral("ضغط زرار بسيط (مثل Enter / Space / Tab / W)"));
      m.insert(QStringLiteral("en"), QStringLiteral("Simple Single Key Tap (e.g. Enter / Space / Tab / W)"));
      m.insert(QStringLiteral("es"), QStringLiteral("Pulsación simple de tecla (Enter / Space / Tab / W)"));
      m.insert(QStringLiteral("fr"), QStringLiteral("Appui simple sur une touche (Entrée / Espace / Tab / W)"));
      m.insert(QStringLiteral("de"), QStringLiteral("Einfacher Tastendruck (z. B. Enter / Space / Tab / W)"));
      m.insert(QStringLiteral("tr"), QStringLiteral("Basit Tek Tuş Basımı (örn. Enter / Space / Tab / W)"));
      m.insert(QStringLiteral("ru"), QStringLiteral("Нажатие одной клавиши (Enter / Space / Tab / W)"));
      m.insert(QStringLiteral("id"), QStringLiteral("Tekan Satu Tombol Sederhana (mis. Enter / Space / Tab / W)"));
      m.insert(QStringLiteral("fa"), QStringLiteral("فشار یک کلید ساده (مانند Enter / Space / Tab / W)"));
      m.insert(QStringLiteral("ur"), QStringLiteral("سادہ ایک بٹن دبائیں (جیسے Enter / Space / Tab / W)"));
    }
    { auto& m = m_translations[QStringLiteral("test_mode_sweep_20")];
      m.insert(QStringLiteral("ar"), QStringLiteral("٤. فحص كل الـ ٢٠ طريقة بالترتيب (مع كود بايثون لكل Method)"));
      m.insert(QStringLiteral("en"), QStringLiteral("4. Sweep All 20 Methods Sequentially (With Editable Python Per-Method Script)"));
      m.insert(QStringLiteral("es"), QStringLiteral("4. Probar los 20 métodos en orden (con script Python por método)"));
      m.insert(QStringLiteral("fr"), QStringLiteral("4. Tester les 20 méthodes dans l’ordre (avec script Python par méthode)"));
      m.insert(QStringLiteral("de"), QStringLiteral("4. Alle 20 Methoden der Reihe nach testen (mit Python-Skript pro Methode)"));
      m.insert(QStringLiteral("tr"), QStringLiteral("4. Tüm 20 Yöntemi Sırayla Test Et (Yöntem Başına Python Kodu İle)"));
      m.insert(QStringLiteral("ru"), QStringLiteral("4. Проверить все 20 методов по очереди (с Python-скриптом для каждого метода)"));
      m.insert(QStringLiteral("id"), QStringLiteral("4. Uji Semua 20 Metode Berurutan (Dengan Skrip Python per Metode)"));
      m.insert(QStringLiteral("fa"), QStringLiteral("۴. تست ترتیبی هر ۲۰ روش (همراه با کد پایتون برای هر روش)"));
      m.insert(QStringLiteral("ur"), QStringLiteral("4. تمام 20 طریقوں کی ترتیب وار جانچ (ہر طریقے کے پائتھون کوڈ کے ساتھ)"));
    }
    { auto& m = m_translations[QStringLiteral("lbl_test_custom_text")];
      m.insert(QStringLiteral("ar"), QStringLiteral("النص المراد كتابته:"));
      m.insert(QStringLiteral("en"), QStringLiteral("Text to Type:"));
      m.insert(QStringLiteral("es"), QStringLiteral("Texto a escribir:"));
      m.insert(QStringLiteral("fr"), QStringLiteral("Texte à saisir :"));
      m.insert(QStringLiteral("de"), QStringLiteral("Einzugebender Text:"));
      m.insert(QStringLiteral("tr"), QStringLiteral("Yazılacak Metin:"));
      m.insert(QStringLiteral("ru"), QStringLiteral("Текст для ввода:"));
      m.insert(QStringLiteral("id"), QStringLiteral("Teks untuk Diketik:"));
      m.insert(QStringLiteral("fa"), QStringLiteral("متن جهت تایپ:"));
      m.insert(QStringLiteral("ur"), QStringLiteral("لکھنے کے لیے متن:"));
    }
    { auto& m = m_translations[QStringLiteral("ph_test_custom_text")];
      m.insert(QStringLiteral("ar"), QStringLiteral("مثال: Hello V-MacroPlus"));
      m.insert(QStringLiteral("en"), QStringLiteral("Example: Hello V-MacroPlus"));
      m.insert(QStringLiteral("es"), QStringLiteral("Ejemplo: Hola V-MacroPlus"));
      m.insert(QStringLiteral("fr"), QStringLiteral("Exemple : Bonjour V-MacroPlus"));
      m.insert(QStringLiteral("de"), QStringLiteral("Beispiel: Hallo V-MacroPlus"));
      m.insert(QStringLiteral("tr"), QStringLiteral("Örnek: Merhaba V-MacroPlus"));
      m.insert(QStringLiteral("ru"), QStringLiteral("Пример: Привет V-MacroPlus"));
      m.insert(QStringLiteral("id"), QStringLiteral("Contoh: Halo V-MacroPlus"));
      m.insert(QStringLiteral("fa"), QStringLiteral("مثال: سلام V-MacroPlus"));
      m.insert(QStringLiteral("ur"), QStringLiteral("مثال: سلام V-MacroPlus"));
    }
    { auto& m = m_translations[QStringLiteral("lbl_test_simple_key")];
      m.insert(QStringLiteral("ar"), QStringLiteral("الزرار البسيط:"));
      m.insert(QStringLiteral("en"), QStringLiteral("Simple Key:"));
      m.insert(QStringLiteral("es"), QStringLiteral("Tecla simple:"));
      m.insert(QStringLiteral("fr"), QStringLiteral("Touche simple :"));
      m.insert(QStringLiteral("de"), QStringLiteral("Einfache Taste:"));
      m.insert(QStringLiteral("tr"), QStringLiteral("Basit Tuş:"));
      m.insert(QStringLiteral("ru"), QStringLiteral("Клавиша:"));
      m.insert(QStringLiteral("id"), QStringLiteral("Tombol Sederhana:"));
      m.insert(QStringLiteral("fa"), QStringLiteral("کلید ساده:"));
      m.insert(QStringLiteral("ur"), QStringLiteral("سادہ بٹن:"));
    }
    { auto& m = m_translations[QStringLiteral("chk_test_press_enter")];
      m.insert(QStringLiteral("ar"), QStringLiteral("ضغط Enter بعد الإرسال"));
      m.insert(QStringLiteral("en"), QStringLiteral("Press Enter After"));
      m.insert(QStringLiteral("es"), QStringLiteral("Pulsar Enter después"));
      m.insert(QStringLiteral("fr"), QStringLiteral("Appuyer sur Entrée après"));
      m.insert(QStringLiteral("de"), QStringLiteral("Danach Enter drücken"));
      m.insert(QStringLiteral("tr"), QStringLiteral("Sonrasında Enter'a Bas"));
      m.insert(QStringLiteral("ru"), QStringLiteral("Нажать Enter после"));
      m.insert(QStringLiteral("id"), QStringLiteral("Tekan Enter Setelahnya"));
      m.insert(QStringLiteral("fa"), QStringLiteral("فشار Enter پس از ارسال"));
      m.insert(QStringLiteral("ur"), QStringLiteral("بعد میں Enter دبائیں"));
    }
    { auto& m = m_translations[QStringLiteral("chk_test_type_method_name")];
      m.insert(QStringLiteral("ar"), QStringLiteral("كتابة اسم كل طريقة داخل البرنامج أثناء الفحص"));
      m.insert(QStringLiteral("en"), QStringLiteral("Type Each Method Name into Target During Sweep"));
      m.insert(QStringLiteral("es"), QStringLiteral("Escribir el nombre de cada método durante la prueba"));
      m.insert(QStringLiteral("fr"), QStringLiteral("Saisir le nom de chaque méthode pendant le test"));
      m.insert(QStringLiteral("de"), QStringLiteral("Methodennamen beim Testen ins Ziel tippen"));
      m.insert(QStringLiteral("tr"), QStringLiteral("Test Sırasında Her Yöntem Adını Hedefe Yaz"));
      m.insert(QStringLiteral("ru"), QStringLiteral("Печатать имя каждого метода в цель при проверке"));
      m.insert(QStringLiteral("id"), QStringLiteral("Ketik Nama Setiap Metode ke Target Saat Uji"));
      m.insert(QStringLiteral("fa"), QStringLiteral("تایپ نام هر روش در برنامه هنگام تست"));
      m.insert(QStringLiteral("ur"), QStringLiteral("ٹیسٹ کے دوران ہر طریقے کا نام ہدف میں لکھیں"));
    }
    { auto& m = m_translations[QStringLiteral("btn_hide_macros_sidebar")];
      m.insert(QStringLiteral("ar"), QStringLiteral("إخفاء القائمة الجانبية"));
      m.insert(QStringLiteral("en"), QStringLiteral("Hide Sidebar"));
      m.insert(QStringLiteral("es"), QStringLiteral("Ocultar panel lateral"));
      m.insert(QStringLiteral("fr"), QStringLiteral("Masquer le panneau"));
      m.insert(QStringLiteral("de"), QStringLiteral("Seitenleiste ausblenden"));
      m.insert(QStringLiteral("tr"), QStringLiteral("Kenar Çubuğunu Gizle"));
      m.insert(QStringLiteral("ru"), QStringLiteral("Скрыть боковую панель"));
      m.insert(QStringLiteral("id"), QStringLiteral("Sembunyikan Panel Samping"));
      m.insert(QStringLiteral("fa"), QStringLiteral("پنهان کردن منوی کناری"));
      m.insert(QStringLiteral("ur"), QStringLiteral("سائیڈ بار چھپائیں"));
    }
    { auto& m = m_translations[QStringLiteral("btn_show_macros_sidebar")];
      m.insert(QStringLiteral("ar"), QStringLiteral("إظهار القائمة الجانبية"));
      m.insert(QStringLiteral("en"), QStringLiteral("Show Sidebar"));
      m.insert(QStringLiteral("es"), QStringLiteral("Mostrar panel lateral"));
      m.insert(QStringLiteral("fr"), QStringLiteral("Afficher le panneau"));
      m.insert(QStringLiteral("de"), QStringLiteral("Seitenleiste einblenden"));
      m.insert(QStringLiteral("tr"), QStringLiteral("Kenar Çubuğunu Göster"));
      m.insert(QStringLiteral("ru"), QStringLiteral("Показать боковую панель"));
      m.insert(QStringLiteral("id"), QStringLiteral("Tampilkan Panel Samping"));
      m.insert(QStringLiteral("fa"), QStringLiteral("نمایش منوی کناری"));
      m.insert(QStringLiteral("ur"), QStringLiteral("سائیڈ بار دکھائیں"));
    }
    { auto& m = m_translations[QStringLiteral("lbl_panic_section_title")];
      m.insert(QStringLiteral("ar"), QStringLiteral("منطقة الطوارئ والإيقاف الفوري الشامل"));
      m.insert(QStringLiteral("en"), QStringLiteral("Emergency Global Stop Zone"));
      m.insert(QStringLiteral("es"), QStringLiteral("Zona de Parada de Emergencia Global"));
      m.insert(QStringLiteral("fr"), QStringLiteral("Zone d'Arrêt d'Urgence Global"));
      m.insert(QStringLiteral("de"), QStringLiteral("Not-Aus-Bereich (Globaler Sofort-Stopp)"));
      m.insert(QStringLiteral("tr"), QStringLiteral("Acil Durum Genel Durdurma Bölgesi"));
      m.insert(QStringLiteral("ru"), QStringLiteral("Зона экстренной полной остановки"));
      m.insert(QStringLiteral("id"), QStringLiteral("Zona Berhenti Darurat Global"));
      m.insert(QStringLiteral("fa"), QStringLiteral("بخش اضطراری و توقف فوری کل عملیات برنامه"));
      m.insert(QStringLiteral("ur"), QStringLiteral("ہنگامی مکمل اسٹاپ زون"));
    }
    { auto& m = m_translations[QStringLiteral("lbl_panic_section_desc")];
      m.insert(QStringLiteral("ar"), QStringLiteral("يوقف فوراً جميع الماكروز، الأزرار الذكية، التسجيلات، وتبويبات الضغط التلقائي النشطة بضغطة واحدة."));
      m.insert(QStringLiteral("en"), QStringLiteral("Immediately halts all running macros, smart keys, recordings, and active auto-presser tabs with a single shortcut."));
      m.insert(QStringLiteral("es"), QStringLiteral("Detiene inmediatamente todas las macros, teclas inteligentes, grabaciones y pestañas activas con un solo atajo."));
      m.insert(QStringLiteral("fr"), QStringLiteral("Arrête immédiatement toutes les macros, touches intelligentes, enregistrements et onglets actifs."));
      m.insert(QStringLiteral("de"), QStringLiteral("Stoppt sofort alle laufenden Makros, Smart-Tasten, Aufnahmen und aktiven Auto-Presser-Tabs."));
      m.insert(QStringLiteral("tr"), QStringLiteral("Çalışan tüm makroları, akıllı tuşları, kayıtları ve aktif otomatik basıcı sekmelerini tek tuşla anında durdurur."));
      m.insert(QStringLiteral("ru"), QStringLiteral("Мгновенно останавливает все запущенные макросы, умные клавиши, записи и активные вкладки авто-нажатия."));
      m.insert(QStringLiteral("id"), QStringLiteral("Menghentikan segera semua makro, tombol cerdas, rekaman, dan tab penekan otomatis yang sedang berjalan."));
      m.insert(QStringLiteral("fa"), QStringLiteral("تمام ماکروها، کلیدهای هوشمند، ضبط‌ها و زبانه‌های فشار خودکار فعال را فوراً با یک کلید متوقف می‌کند."));
      m.insert(QStringLiteral("ur"), QStringLiteral("ایک ہی بٹن سے تمام چلنے والے میکروز، اسمارٹ بٹنز، ریکارڈنگز اور آٹو پریسر ٹیبز کو فوراً روکتا ہے۔"));
    }
    { auto& m = m_translations[QStringLiteral("akp_btn_test_mouse_pt")];
      m.insert(QStringLiteral("ar"), QStringLiteral("تجربة"));
      m.insert(QStringLiteral("en"), QStringLiteral("Test"));
      m.insert(QStringLiteral("es"), QStringLiteral("Probar"));
      m.insert(QStringLiteral("fr"), QStringLiteral("Tester"));
      m.insert(QStringLiteral("de"), QStringLiteral("Testen"));
      m.insert(QStringLiteral("tr"), QStringLiteral("Test"));
      m.insert(QStringLiteral("ru"), QStringLiteral("Тест"));
      m.insert(QStringLiteral("id"), QStringLiteral("Uji"));
      m.insert(QStringLiteral("fa"), QStringLiteral("تست"));
      m.insert(QStringLiteral("ur"), QStringLiteral("ٹیسٹ"));
    }
    { auto& m = m_translations[QStringLiteral("akp_tip_test_mouse_pt")];
      m.insert(QStringLiteral("ar"), QStringLiteral("تجربة إرسال هذه النقرة الآن على النافذة المختارة"));
      m.insert(QStringLiteral("en"), QStringLiteral("Test clicking this point now on the selected target window"));
      m.insert(QStringLiteral("es"), QStringLiteral("Probar este clic ahora en la ventana seleccionada"));
      m.insert(QStringLiteral("fr"), QStringLiteral("Tester ce clic maintenant sur la fenêtre sélectionnée"));
      m.insert(QStringLiteral("de"), QStringLiteral("Diesen Klick jetzt im ausgewählten Zielfenster testen"));
      m.insert(QStringLiteral("tr"), QStringLiteral("Bu tıklamayı seçili hedef pencerede şimdi test et"));
      m.insert(QStringLiteral("ru"), QStringLiteral("Проверить этот клик сейчас в выбранном окне"));
      m.insert(QStringLiteral("id"), QStringLiteral("Uji klik titik ini sekarang pada jendela target"));
      m.insert(QStringLiteral("fa"), QStringLiteral("تست ارسال این کلیک اکنون روی پنجره انتخاب‌شده"));
      m.insert(QStringLiteral("ur"), QStringLiteral("منتخب ونڈو پر اس کلک کو ابھی ٹیسٹ کریں"));
    }
    { auto& m = m_translations[QStringLiteral("akp_btn_add_mouse_back")];
      m.insert(QStringLiteral("ar"), QStringLiteral("+ ماوس رجوع (Back)"));
      m.insert(QStringLiteral("en"), QStringLiteral("+ Mouse Back (X1)"));
      m.insert(QStringLiteral("es"), QStringLiteral("+ Ratón Atrás (Back)"));
      m.insert(QStringLiteral("fr"), QStringLiteral("+ Souris Retour (Back)"));
      m.insert(QStringLiteral("de"), QStringLiteral("+ Maus Zurück (Back)"));
      m.insert(QStringLiteral("tr"), QStringLiteral("+ Fare Geri (Back)"));
      m.insert(QStringLiteral("ru"), QStringLiteral("+ Мышь Назад (Back)"));
      m.insert(QStringLiteral("id"), QStringLiteral("+ Mouse Mundur (Back)"));
      m.insert(QStringLiteral("fa"), QStringLiteral("+ ماوس بازگشت (Back)"));
      m.insert(QStringLiteral("ur"), QStringLiteral("+ ماؤس بیک (Back)"));
    }
    { auto& m = m_translations[QStringLiteral("akp_btn_add_mouse_forward")];
      m.insert(QStringLiteral("ar"), QStringLiteral("+ ماوس تقدم (Forward)"));
      m.insert(QStringLiteral("en"), QStringLiteral("+ Mouse Forward (X2)"));
      m.insert(QStringLiteral("es"), QStringLiteral("+ Ratón Adelante (Forward)"));
      m.insert(QStringLiteral("fr"), QStringLiteral("+ Souris Suivant (Forward)"));
      m.insert(QStringLiteral("de"), QStringLiteral("+ Maus Vorwärts (Forward)"));
      m.insert(QStringLiteral("tr"), QStringLiteral("+ Fare İleri (Forward)"));
      m.insert(QStringLiteral("ru"), QStringLiteral("+ Мышь Вперёд (Forward)"));
      m.insert(QStringLiteral("id"), QStringLiteral("+ Mouse Maju (Forward)"));
      m.insert(QStringLiteral("fa"), QStringLiteral("+ ماوس جلو (Forward)"));
      m.insert(QStringLiteral("ur"), QStringLiteral("+ ماؤس فارورڈ (Forward)"));
    }
    { auto& m = m_translations[QStringLiteral("lbl_test_bg_mode")];
      m.insert(QStringLiteral("ar"), QStringLiteral("فحص في الخلفية (بدون الانتقال للنافذة):"));
      m.insert(QStringLiteral("en"), QStringLiteral("Background Test (Do not focus window):"));
      m.insert(QStringLiteral("es"), QStringLiteral("Prueba en 2.º plano (sin enfocar ventana):"));
      m.insert(QStringLiteral("fr"), QStringLiteral("Test en arrière-plan (sans activer la fenêtre) :"));
      m.insert(QStringLiteral("de"), QStringLiteral("Hintergrund-Test (Fenster nicht fokussieren):"));
      m.insert(QStringLiteral("tr"), QStringLiteral("Arka Planda Test (Pencereye geçmeden):"));
      m.insert(QStringLiteral("ru"), QStringLiteral("Тест в фоне (не переключать окно):"));
      m.insert(QStringLiteral("id"), QStringLiteral("Uji di Latar Belakang (Tanpa fokus jendela):"));
      m.insert(QStringLiteral("fa"), QStringLiteral("تست در پس‌زمینه (بدون انتقال به پنجره):"));
      m.insert(QStringLiteral("ur"), QStringLiteral("پس منظر میں ٹیسٹ (ونڈو پر منتقل ہوئے بغیر):"));
    }
    { auto& m = m_translations[QStringLiteral("lbl_py_sweep_editor")];
      m.insert(QStringLiteral("ar"), QStringLiteral("كود بايثون لفحص كل طريقة (يُنفّذ لكل Method من ١ إلى ٢٠ بنفس الطريقة فقط — عدّل الكود كما تحب):"));
      m.insert(QStringLiteral("en"), QStringLiteral("Editable Python Per-Method Test Script (Runs for each Method 1..20 strictly using that Method):"));
      m.insert(QStringLiteral("es"), QStringLiteral("Script Python editable por método (se ejecuta para cada Método 1..20 usando solo ese método):"));
      m.insert(QStringLiteral("fr"), QStringLiteral("Script Python modifiable par méthode (exécuté pour chaque Méthode 1..20 via cette méthode) :"));
      m.insert(QStringLiteral("de"), QStringLiteral("Editierbares Python-Testskript pro Methode (wird für jede Methode 1..20 exakt damit ausgeführt):"));
      m.insert(QStringLiteral("tr"), QStringLiteral("Yöntem Başına Düzenlenebilir Python Test Kodu (1..20 arası her yöntem için yalnızca o yöntemle çalışır):"));
      m.insert(QStringLiteral("ru"), QStringLiteral("Редактируемый Python-скрипт для каждого метода (выполняется для методов 1..20 строго этим методом):"));
      m.insert(QStringLiteral("id"), QStringLiteral("Skrip Uji Python per Metode (Dijalankan untuk tiap Metode 1..20 hanya dengan metode tersebut):"));
      m.insert(QStringLiteral("fa"), QStringLiteral("کد پایتون قابل ویرایش برای تست هر روش (برای هر روش ۱ تا ۲۰ صرفاً با همان روش اجرا می‌شود):"));
      m.insert(QStringLiteral("ur"), QStringLiteral("ہر طریقے کے لیے قابلِ تدوین پائتھون ٹیسٹ کوڈ (طریقہ 1 تا 20 کے لیے صرف اسی طریقے سے چلتا ہے):"));
    }
    { auto& m = m_translations[QStringLiteral("btn_reset_py_sweep")];
      m.insert(QStringLiteral("ar"), QStringLiteral("استعادة الكود الافتراضي"));
      m.insert(QStringLiteral("en"), QStringLiteral("Reset Default Script"));
      m.insert(QStringLiteral("es"), QStringLiteral("Restaurar Script"));
      m.insert(QStringLiteral("fr"), QStringLiteral("Réinitialiser le Script"));
      m.insert(QStringLiteral("de"), QStringLiteral("Skript zurücksetzen"));
      m.insert(QStringLiteral("tr"), QStringLiteral("Varsayılan Kodu Yükle"));
      m.insert(QStringLiteral("ru"), QStringLiteral("Сбросить скрипт"));
      m.insert(QStringLiteral("id"), QStringLiteral("Reset Skrip Bawaan"));
      m.insert(QStringLiteral("fa"), QStringLiteral("بازنشانی کد پیش‌فرض"));
      m.insert(QStringLiteral("ur"), QStringLiteral("ڈیفالٹ کوڈ بحال کریں"));
    }
    { auto& m = m_translations[QStringLiteral("test_mode_sweep_20_template")];
      m.insert(QStringLiteral("ar"), QStringLiteral("٥. فحص كل الـ ٢٠ طريقة — بجملة وقالب متغيرات ذكي ({method_id}, key=\"Enter\")"));
      m.insert(QStringLiteral("en"), QStringLiteral("5. Sweep All 20 Methods — Smart Variable Template ({method_id}, key=\"Enter\")"));
      m.insert(QStringLiteral("es"), QStringLiteral("5. Probar los 20 métodos — Plantilla inteligente ({method_id}, key=\"Enter\")"));
      m.insert(QStringLiteral("fr"), QStringLiteral("5. Tester les 20 méthodes — Modèle de variables ({method_id}, key=\"Enter\")"));
      m.insert(QStringLiteral("de"), QStringLiteral("5. Alle 20 Methoden testen — Smart-Vorlage ({method_id}, key=\"Enter\")"));
      m.insert(QStringLiteral("tr"), QStringLiteral("5. Tüm 20 Yöntemi Test Et — Akıllı Değişken Şablonu ({method_id}, key=\"Enter\")"));
      m.insert(QStringLiteral("ru"), QStringLiteral("5. Проверить 20 методов — Умный шаблон ({method_id}, key=\"Enter\")"));
      m.insert(QStringLiteral("id"), QStringLiteral("5. Uji Semua 20 Metode — Templat Variabel Cerdas ({method_id}, key=\"Enter\")"));
      m.insert(QStringLiteral("fa"), QStringLiteral("۵. تست هر ۲۰ روش — قالب متغیر هوشمند ({method_id}, key=\"Enter\")"));
      m.insert(QStringLiteral("ur"), QStringLiteral("5. تمام 20 طریقوں کی جانچ — اسمارٹ ویری ایبل ٹیمپلیٹ ({method_id}, key=\"Enter\")"));
    }
    { auto& m = m_translations[QStringLiteral("test_mode_sweep_20_macro")];
      m.insert(QStringLiteral("ar"), QStringLiteral("٦. فحص كل الـ ٢٠ طريقة — بتشغيل ماكرو محفوظ أو زر ذكي لكل طريقة"));
      m.insert(QStringLiteral("en"), QStringLiteral("6. Sweep All 20 Methods — Run Selected Smart Key / Macro with Each Method"));
      m.insert(QStringLiteral("es"), QStringLiteral("6. Probar los 20 métodos — Ejecutar macro seleccionada con cada método"));
      m.insert(QStringLiteral("fr"), QStringLiteral("6. Tester les 20 méthodes — Exécuter la macro choisie avec chaque méthode"));
      m.insert(QStringLiteral("de"), QStringLiteral("6. Alle 20 Methoden testen — Ausgewähltes Makro mit jeder Methode ausführen"));
      m.insert(QStringLiteral("tr"), QStringLiteral("6. Tüm 20 Yöntemi Test Et — Seçili Makroyu Her Yöntemle Çalıştır"));
      m.insert(QStringLiteral("ru"), QStringLiteral("6. Проверить 20 методов — Запустить выбранный макрос каждым методом"));
      m.insert(QStringLiteral("id"), QStringLiteral("6. Uji Semua 20 Metode — Jalankan Makro Terpilih dengan Tiap Metode"));
      m.insert(QStringLiteral("fa"), QStringLiteral("۶. تست هر ۲۰ روش — اجرای ماکروی انتخاب‌شده با تک‌تک روش‌ها"));
      m.insert(QStringLiteral("ur"), QStringLiteral("6. تمام 20 طریقوں کی جانچ — منتخب میکرو کو ہر طریقے سے چلائیں"));
    }
    { auto& m = m_translations[QStringLiteral("lbl_template_sweep_box")];
      m.insert(QStringLiteral("ar"), QStringLiteral("قالب النص والأزرار الذكي (يدعم key=\"Enter\" و \\n وجميع المتغيرات بالأسفل — اضغط على أي متغير لإضافته):"));
      m.insert(QStringLiteral("en"), QStringLiteral("Smart Key & Variable Template (Supports key=\"Enter\", \\n, wait=100 & variables below — click any chip to insert):"));
      m.insert(QStringLiteral("es"), QStringLiteral("Plantilla inteligente de teclas y variables (Soporta key=\"Enter\", \\n, wait=100 y variables):"));
      m.insert(QStringLiteral("fr"), QStringLiteral("Modèle intelligent de touches et variables (Supporte key=\"Enter\", \\n, wait=100 et variables) :"));
      m.insert(QStringLiteral("de"), QStringLiteral("Smart-Tasten- & Variablen-Vorlage (Unterstützt key=\"Enter\", \\n, wait=100 & Variablen):"));
      m.insert(QStringLiteral("tr"), QStringLiteral("Akıllı Tuş ve Değişken Şablonu (key=\"Enter\", \\n, wait=100 ve değişkenleri destekler):"));
      m.insert(QStringLiteral("ru"), QStringLiteral("Умный шаблон клавиш и переменных (Поддерживает key=\"Enter\", \\n, wait=100 и переменные):"));
      m.insert(QStringLiteral("id"), QStringLiteral("Templat Tombol & Variabel Cerdas (Mendukung key=\"Enter\", \\n, wait=100 & variabel):"));
      m.insert(QStringLiteral("fa"), QStringLiteral("قالب هوشمند کلید و متغیرها (پشتیبانی از key=\"Enter\"، \\n، wait=100 و متغیرهای زیر):"));
      m.insert(QStringLiteral("ur"), QStringLiteral("اسمارٹ بٹن اور ویری ایبل ٹیمپلیٹ (key=\"Enter\"، \\n، wait=100 اور متغیرات کی سپورٹ):"));
    }
    { auto& m = m_translations[QStringLiteral("chk_macro_sweep_prefix")];
      m.insert(QStringLiteral("ar"), QStringLiteral("كتابة قالب المتغيرات التعريفي قبل تشغيل الماكرو في كل طريقة:"));
      m.insert(QStringLiteral("en"), QStringLiteral("Type variable template prefix before running macro on each method:"));
      m.insert(QStringLiteral("es"), QStringLiteral("Escribir plantilla de variables antes de ejecutar la macro en cada método:"));
      m.insert(QStringLiteral("fr"), QStringLiteral("Écrire le modèle de variables avant d’exécuter la macro pour chaque méthode :"));
      m.insert(QStringLiteral("de"), QStringLiteral("Variablen-Vorlage vor Ausführung des Makros bei jeder Methode tippen:"));
      m.insert(QStringLiteral("tr"), QStringLiteral("Her yöntemde makroyu çalıştırmadan önce değişken şablonunu yaz:"));
      m.insert(QStringLiteral("ru"), QStringLiteral("Печатать шаблон переменных перед запуском макроса для каждого метода:"));
      m.insert(QStringLiteral("id"), QStringLiteral("Ketik templat variabel sebelum menjalankan makro di tiap metode:"));
      m.insert(QStringLiteral("fa"), QStringLiteral("تایپ قالب متغیرها قبل از اجرای ماکرو در هر روش:"));
      m.insert(QStringLiteral("ur"), QStringLiteral("ہر طریقے میں میکرو چلانے سے پہلے ویری ایبل ٹیمپلیٹ ٹائپ کریں:"));
    }
    { auto& m = m_translations[QStringLiteral("btn_reset_template_sweep")];
      m.insert(QStringLiteral("ar"), QStringLiteral("استعادة القالب الافتراضي"));
      m.insert(QStringLiteral("en"), QStringLiteral("Reset Template"));
      m.insert(QStringLiteral("es"), QStringLiteral("Restaurar Plantilla"));
      m.insert(QStringLiteral("fr"), QStringLiteral("Réinitialiser Modèle"));
      m.insert(QStringLiteral("de"), QStringLiteral("Vorlage zurücksetzen"));
      m.insert(QStringLiteral("tr"), QStringLiteral("Şablonu Sıfırla"));
      m.insert(QStringLiteral("ru"), QStringLiteral("Сбросить шаблон"));
      m.insert(QStringLiteral("id"), QStringLiteral("Reset Templat"));
      m.insert(QStringLiteral("fa"), QStringLiteral("بازنشانی قالب"));
      m.insert(QStringLiteral("ur"), QStringLiteral("ٹیمپلیٹ بحال کریں"));
    }
    { auto& m = m_translations[QStringLiteral("log_lbl_target")];
      m.insert(QStringLiteral("ar"), QStringLiteral("الهدف:"));
      m.insert(QStringLiteral("en"), QStringLiteral("Target:"));
      m.insert(QStringLiteral("es"), QStringLiteral("Destino:"));
      m.insert(QStringLiteral("fr"), QStringLiteral("Cible :"));
      m.insert(QStringLiteral("de"), QStringLiteral("Ziel:"));
      m.insert(QStringLiteral("tr"), QStringLiteral("Hedef:"));
      m.insert(QStringLiteral("ru"), QStringLiteral("Цель:"));
      m.insert(QStringLiteral("id"), QStringLiteral("Target:"));
      m.insert(QStringLiteral("fa"), QStringLiteral("هدف:"));
      m.insert(QStringLiteral("ur"), QStringLiteral("ہدف:"));
    }
    { auto& m = m_translations[QStringLiteral("log_lbl_action")];
      m.insert(QStringLiteral("ar"), QStringLiteral("الإجراء:"));
      m.insert(QStringLiteral("en"), QStringLiteral("Action:"));
      m.insert(QStringLiteral("es"), QStringLiteral("Acción:"));
      m.insert(QStringLiteral("fr"), QStringLiteral("Action :"));
      m.insert(QStringLiteral("de"), QStringLiteral("Aktion:"));
      m.insert(QStringLiteral("tr"), QStringLiteral("İşlem:"));
      m.insert(QStringLiteral("ru"), QStringLiteral("Действие:"));
      m.insert(QStringLiteral("id"), QStringLiteral("Aksi:"));
      m.insert(QStringLiteral("fa"), QStringLiteral("عملیات:"));
      m.insert(QStringLiteral("ur"), QStringLiteral("عمل:"));
    }
    { auto& m = m_translations[QStringLiteral("log_lbl_details")];
      m.insert(QStringLiteral("ar"), QStringLiteral("التفاصيل:"));
      m.insert(QStringLiteral("en"), QStringLiteral("Details:"));
      m.insert(QStringLiteral("es"), QStringLiteral("Detalles:"));
      m.insert(QStringLiteral("fr"), QStringLiteral("Détails :"));
      m.insert(QStringLiteral("de"), QStringLiteral("Details:"));
      m.insert(QStringLiteral("tr"), QStringLiteral("Detaylar:"));
      m.insert(QStringLiteral("ru"), QStringLiteral("Детали:"));
      m.insert(QStringLiteral("id"), QStringLiteral("Detail:"));
      m.insert(QStringLiteral("fa"), QStringLiteral("جزئیات:"));
      m.insert(QStringLiteral("ur"), QStringLiteral("تفصیلات:"));
    }
    { auto& m = m_translations[QStringLiteral("log_lbl_mode_bg")];
      m.insert(QStringLiteral("ar"), QStringLiteral("خلفية Background"));
      m.insert(QStringLiteral("en"), QStringLiteral("Background Mode"));
      m.insert(QStringLiteral("es"), QStringLiteral("Segundo plano"));
      m.insert(QStringLiteral("fr"), QStringLiteral("Arrière-plan"));
      m.insert(QStringLiteral("de"), QStringLiteral("Hintergrund"));
      m.insert(QStringLiteral("tr"), QStringLiteral("Arka Plan"));
      m.insert(QStringLiteral("ru"), QStringLiteral("Фоновый режим"));
      m.insert(QStringLiteral("id"), QStringLiteral("Latar Belakang"));
      m.insert(QStringLiteral("fa"), QStringLiteral("حالت پس‌زمینه"));
      m.insert(QStringLiteral("ur"), QStringLiteral("بیک گراؤنڈ موڈ"));
    }
    { auto& m = m_translations[QStringLiteral("log_lbl_mode_fg")];
      m.insert(QStringLiteral("ar"), QStringLiteral("أمامية Foreground"));
      m.insert(QStringLiteral("en"), QStringLiteral("Foreground Mode"));
      m.insert(QStringLiteral("es"), QStringLiteral("Primer plano"));
      m.insert(QStringLiteral("fr"), QStringLiteral("Premier plan"));
      m.insert(QStringLiteral("de"), QStringLiteral("Vordergrund"));
      m.insert(QStringLiteral("tr"), QStringLiteral("Ön Plan"));
      m.insert(QStringLiteral("ru"), QStringLiteral("Активное окно"));
      m.insert(QStringLiteral("id"), QStringLiteral("Latar Depan"));
      m.insert(QStringLiteral("fa"), QStringLiteral("حالت پیش‌زمینه"));
      m.insert(QStringLiteral("ur"), QStringLiteral("فور گراؤنڈ موڈ"));
    }
    { auto& m = m_translations[QStringLiteral("log_reason_delivered_fg")];
      m.insert(QStringLiteral("ar"), QStringLiteral("تم الإرسال بنجاح عبر محرك الواجهة الأمامية ({engine})"));
      m.insert(QStringLiteral("en"), QStringLiteral("Delivered via foreground hardware engine ({engine})"));
      m.insert(QStringLiteral("es"), QStringLiteral("Enviado mediante motor de primer plano ({engine})"));
      m.insert(QStringLiteral("fr"), QStringLiteral("Envoyé via le moteur de premier plan ({engine})"));
      m.insert(QStringLiteral("de"), QStringLiteral("Über Vordergrund-Hardware-Engine gesendet ({engine})"));
      m.insert(QStringLiteral("tr"), QStringLiteral("Ön plan donanım motoru ile gönderildi ({engine})"));
      m.insert(QStringLiteral("ru"), QStringLiteral("Доставлено через активный движок ({engine})"));
      m.insert(QStringLiteral("id"), QStringLiteral("Terkirim melalui mesin latar depan ({engine})"));
      m.insert(QStringLiteral("fa"), QStringLiteral("ارسال موفق از طریق موتور پیش‌زمینه ({engine})"));
      m.insert(QStringLiteral("ur"), QStringLiteral("فور گراؤنڈ ہارڈویئر انجن ({engine}) کے ذریعے کامیاب ارسال"));
    }
    { auto& m = m_translations[QStringLiteral("log_reason_delivered_bg")];
      m.insert(QStringLiteral("ar"), QStringLiteral("تم الإرسال في الخلفية بنجاح عبر ({engine}) إلى العنصر الداخلي [{child}]"));
      m.insert(QStringLiteral("en"), QStringLiteral("Delivered in background via ({engine}) to inner control [{child}]"));
      m.insert(QStringLiteral("es"), QStringLiteral("Enviado en segundo plano vía ({engine}) al control [{child}]"));
      m.insert(QStringLiteral("fr"), QStringLiteral("Envoyé en arrière-plan via ({engine}) au contrôle [{child}]"));
      m.insert(QStringLiteral("de"), QStringLiteral("Im Hintergrund über ({engine}) an Steuerelement [{child}] gesendet"));
      m.insert(QStringLiteral("tr"), QStringLiteral("Arka planda ({engine}) ile iç kontrole [{child}] gönderildi"));
      m.insert(QStringLiteral("ru"), QStringLiteral("Доставлено в фоне через ({engine}) в элемент [{child}]"));
      m.insert(QStringLiteral("id"), QStringLiteral("Terkirim di latar belakang via ({engine}) ke kontrol [{child}]"));
      m.insert(QStringLiteral("fa"), QStringLiteral("ارسال موفق در پس‌زمینه از طریق ({engine}) به کنترل داخلی [{child}]"));
      m.insert(QStringLiteral("ur"), QStringLiteral("پس منظر میں ({engine}) کے ذریعے اندرونی کنٹرول [{child}] کو کامیاب ارسال"));
    }
    { auto& m = m_translations[QStringLiteral("log_reason_skipped_fg_in_bg")];
      m.insert(QStringLiteral("ar"), QStringLiteral("تخطّي تلقائي — هذه الطريقة أمامية فقط والنافذة في الخلفية حالياً (حماية من الكتابة خارج الهدف)"));
      m.insert(QStringLiteral("en"), QStringLiteral("Skipped — Foreground-only method while target window is unfocused in Background Mode"));
      m.insert(QStringLiteral("es"), QStringLiteral("Omitido — Método de primer plano mientras la ventana está en segundo plano"));
      m.insert(QStringLiteral("fr"), QStringLiteral("Ignoré — Méthode de premier plan alors que la fenêtre cible est en arrière-plan"));
      m.insert(QStringLiteral("de"), QStringLiteral("Übersprungen — Vordergrund-Methode während Zielfenster im Hintergrund ist"));
      m.insert(QStringLiteral("tr"), QStringLiteral("Atlandı — Hedef pencere arka plandayken yalnızca ön plan yöntemi"));
      m.insert(QStringLiteral("ru"), QStringLiteral("Пропущено — Метод только для активного окна (целевое окно сейчас в фоне)"));
      m.insert(QStringLiteral("id"), QStringLiteral("Dilewati — Metode khusus latar depan saat jendela target di latar belakang"));
      m.insert(QStringLiteral("fa"), QStringLiteral("رد شد — این روش مخصوص پیش‌زمینه است و پنجره هدف در پس‌زمینه قرار دارد"));
      m.insert(QStringLiteral("ur"), QStringLiteral("نظر انداز — یہ طریقہ صرف فور گراؤنڈ کے لیے ہے جبکہ ہدف ونڈو پس منظر میں ہے"));
    }
    { auto& m = m_translations[QStringLiteral("log_reason_interception_missing")];
      m.insert(QStringLiteral("ar"), QStringLiteral("غير متاح — درايفر الكيرنل Interception (keyboard.sys) غير مثبّت على النظام"));
      m.insert(QStringLiteral("en"), QStringLiteral("Unavailable — Interception Ring-0 Kernel Driver (keyboard.sys) is not installed"));
      m.insert(QStringLiteral("es"), QStringLiteral("No disponible — El controlador Interception Ring-0 no está instalado"));
      m.insert(QStringLiteral("fr"), QStringLiteral("Indisponible — Le pilote noyau Interception Ring-0 n’est pas installé"));
      m.insert(QStringLiteral("de"), QStringLiteral("Nicht verfügbar — Interception Ring-0 Kernel-Treiber ist nicht installiert"));
      m.insert(QStringLiteral("tr"), QStringLiteral("Kullanılamaz — Interception Ring-0 Çekirdek Sürücüsü kurulu değil"));
      m.insert(QStringLiteral("ru"), QStringLiteral("Недоступно — Драйвер ядра Interception Ring-0 не установлен"));
      m.insert(QStringLiteral("id"), QStringLiteral("Tidak tersedia — Driver Kernel Interception Ring-0 belum terinstal"));
      m.insert(QStringLiteral("fa"), QStringLiteral("ناموجود — درایور کرنل Interception روی سیستم نصب نشده است"));
      m.insert(QStringLiteral("ur"), QStringLiteral("دستیاب نہیں — Interception Ring-0 کرنل ڈرائیور انسٹال نہیں ہے"));
    }
    { auto& m = m_translations[QStringLiteral("log_reason_ps2_missing")];
      m.insert(QStringLiteral("ar"), QStringLiteral("غير متاح — درايفر منفذ البوردة InpOutx64 (Port 0x60/0x64) غير محمّل"));
      m.insert(QStringLiteral("en"), QStringLiteral("Unavailable — InpOutx64 Motherboard PS/2 Port 0x60/0x64 Driver is not loaded"));
      m.insert(QStringLiteral("es"), QStringLiteral("No disponible — El controlador InpOutx64 Puerto PS/2 0x60 no está cargado"));
      m.insert(QStringLiteral("fr"), QStringLiteral("Indisponible — Le pilote InpOutx64 Port PS/2 0x60 n’est pas chargé"));
      m.insert(QStringLiteral("de"), QStringLiteral("Nicht verfügbar — InpOutx64 PS/2-Port 0x60 Treiber ist nicht geladen"));
      m.insert(QStringLiteral("tr"), QStringLiteral("Kullanılamaz — InpOutx64 PS/2 Port 0x60 Sürücüsü yüklü değil"));
      m.insert(QStringLiteral("ru"), QStringLiteral("Недоступно — Драйвер порта PS/2 0x60 (InpOutx64) не загружен"));
      m.insert(QStringLiteral("id"), QStringLiteral("Tidak tersedia — Driver Port PS/2 0x60 InpOutx64 belum dimuat"));
      m.insert(QStringLiteral("fa"), QStringLiteral("ناموجود — درایور پورت مادربرد InpOutx64 (Port 0x60) بارگذاری نشده است"));
      m.insert(QStringLiteral("ur"), QStringLiteral("دستیاب نہیں — InpOutx64 مدر بورڈ PS/2 پورٹ 0x60 ڈرائیور لوڈ نہیں ہے"));
    }
    { auto& m = m_translations[QStringLiteral("log_reason_linux_on_win")];
      m.insert(QStringLiteral("ar"), QStringLiteral("غير متاح على ويندوز — محرك نواة لينكس (/dev/uinput) مخصص لأنظمة Linux فقط"));
      m.insert(QStringLiteral("en"), QStringLiteral("Unavailable on Windows — Linux Kernel (/dev/uinput) requires a Linux OS"));
      m.insert(QStringLiteral("es"), QStringLiteral("No disponible en Windows — /dev/uinput requiere sistema Linux"));
      m.insert(QStringLiteral("fr"), QStringLiteral("Indisponible sous Windows — /dev/uinput nécessite un système Linux"));
      m.insert(QStringLiteral("de"), QStringLiteral("Unter Windows nicht verfügbar — /dev/uinput erfordert Linux"));
      m.insert(QStringLiteral("tr"), QStringLiteral("Windows’ta kullanılamaz — /dev/uinput yalnızca Linux içindir"));
      m.insert(QStringLiteral("ru"), QStringLiteral("Недоступно в Windows — Движок /dev/uinput предназначен только для Linux"));
      m.insert(QStringLiteral("id"), QStringLiteral("Tidak tersedia di Windows — /dev/uinput khusus untuk sistem Linux"));
      m.insert(QStringLiteral("fa"), QStringLiteral("در ویندوز در دسترس نیست — موتور /dev/uinput مخصوص لینوکس است"));
      m.insert(QStringLiteral("ur"), QStringLiteral("ونڈوز پر دستیاب نہیں — /dev/uinput صرف لینکس سسٹمز کے لیے ہے"));
    }
    { auto& m = m_translations[QStringLiteral("log_reason_hwnd_rejected")];
      m.insert(QStringLiteral("ar"), QStringLiteral("رفضت النافذة المستهدفة الرسالة (UIPI — جرّب تشغيل البرنامج كمسؤول Administrator)"));
      m.insert(QStringLiteral("en"), QStringLiteral("Target HWND rejected Win32 message (UIPI — try running V-MacroPlus as Administrator)"));
      m.insert(QStringLiteral("es"), QStringLiteral("La ventana rechazó el mensaje Win32 (prueba ejecutar como Administrador)"));
      m.insert(QStringLiteral("fr"), QStringLiteral("La fenêtre cible a rejeté le message Win32 (essayez en tant qu’Administrateur)"));
      m.insert(QStringLiteral("de"), QStringLiteral("Zielfenster hat Win32-Nachricht abgelehnt (als Administrator ausführen)"));
      m.insert(QStringLiteral("tr"), QStringLiteral("Hedef pencere Win32 mesajını reddetti (Yönetici olarak çalıştırmayı deneyin)"));
      m.insert(QStringLiteral("ru"), QStringLiteral("Окно отклонило сообщение Win32 (попробуйте запустить от имени Администратора)"));
      m.insert(QStringLiteral("id"), QStringLiteral("Jendela target menolak pesan Win32 (coba jalankan sebagai Administrator)"));
      m.insert(QStringLiteral("fa"), QStringLiteral("پنجره هدف پیام Win32 را رد کرد (برنامه را با دسترسی Administrator اجرا کنید)"));
      m.insert(QStringLiteral("ur"), QStringLiteral("ہدف ونڈو نے Win32 پیغام مسترد کر دیا (بطور ایڈمنسٹریٹر چلا کر دیکھیں)"));
    }
    { auto& m = m_translations[QStringLiteral("log_reason_mouse_bg_ok")];
      m.insert(QStringLiteral("ar"), QStringLiteral("تم إرسال كليك ({btn}) في الخلفية عبر [{mode}] — شاشة ({sx}, {sy}) ← داخل النافذة ({cx}, {cy}) [{child}]"));
      m.insert(QStringLiteral("en"), QStringLiteral("Background click ({btn}) delivered via [{mode}] — Screen ({sx}, {sy}) -> Client ({cx}, {cy}) [{child}]"));
      m.insert(QStringLiteral("es"), QStringLiteral("Clic en 2.º plano ({btn}) enviado vía [{mode}] — Pantalla ({sx}, {sy}) -> Cliente ({cx}, {cy}) [{child}]"));
      m.insert(QStringLiteral("fr"), QStringLiteral("Clic en arrière-plan ({btn}) envoyé via [{mode}] — Écran ({sx}, {sy}) -> Client ({cx}, {cy}) [{child}]"));
      m.insert(QStringLiteral("de"), QStringLiteral("Hintergrund-Klick ({btn}) über [{mode}] — Bildschirm ({sx}, {sy}) -> Client ({cx}, {cy}) [{child}]"));
      m.insert(QStringLiteral("tr"), QStringLiteral("Arka plan tıklaması ({btn}) [{mode}] ile gönderildi — Ekran ({sx}, {sy}) -> İstemci ({cx}, {cy}) [{child}]"));
      m.insert(QStringLiteral("ru"), QStringLiteral("Клик в фоне ({btn}) через [{mode}] — Экран ({sx}, {sy}) -> Клиент ({cx}, {cy}) [{child}]"));
      m.insert(QStringLiteral("id"), QStringLiteral("Klik latar ({btn}) terkirim via [{mode}] — Layar ({sx}, {sy}) -> Klien ({cx}, {cy}) [{child}]"));
      m.insert(QStringLiteral("fa"), QStringLiteral("کلیک پس‌زمینه ({btn}) از طریق [{mode}] ارسال شد — صفحه ({sx}, {sy}) ← داخل پنجره ({cx}, {cy}) [{child}]"));
      m.insert(QStringLiteral("ur"), QStringLiteral("بیک گراؤنڈ کلک ({btn}) بذریعہ [{mode}] ارسال — اسکرین ({sx}, {sy}) -> ونڈو ({cx}, {cy}) [{child}]"));
    }
    { auto& m = m_translations[QStringLiteral("log_reason_mouse_fg_ok")];
      m.insert(QStringLiteral("ar"), QStringLiteral("تم تنفيذ كليك ماوس أمامي ({btn}) عبر [{mode}] عند إحداثي الشاشة ({sx}, {sy})"));
      m.insert(QStringLiteral("en"), QStringLiteral("Foreground mouse click ({btn}) executed via [{mode}] at Screen ({sx}, {sy})"));
      m.insert(QStringLiteral("es"), QStringLiteral("Clic de primer plano ({btn}) ejecutado vía [{mode}] en ({sx}, {sy})"));
      m.insert(QStringLiteral("fr"), QStringLiteral("Clic de premier plan ({btn}) exécuté via [{mode}] à ({sx}, {sy})"));
      m.insert(QStringLiteral("de"), QStringLiteral("Vordergrund-Mausklick ({btn}) über [{mode}] bei ({sx}, {sy}) ausgeführt"));
      m.insert(QStringLiteral("tr"), QStringLiteral("Ön plan fare tıklaması ({btn}) [{mode}] ile ({sx}, {sy}) konumunda yapıldı"));
      m.insert(QStringLiteral("ru"), QStringLiteral("Клик мыши ({btn}) выполнен через [{mode}] в точке ({sx}, {sy})"));
      m.insert(QStringLiteral("id"), QStringLiteral("Klik mouse latar depan ({btn}) dieksekusi via [{mode}] di ({sx}, {sy})"));
      m.insert(QStringLiteral("fa"), QStringLiteral("کلیک ماوس پیش‌زمینه ({btn}) از طریق [{mode}] در مختصات ({sx}, {sy}) انجام شد"));
      m.insert(QStringLiteral("ur"), QStringLiteral("فور گراؤنڈ ماؤس کلک ({btn}) بذریعہ [{mode}] مقام ({sx}, {sy}) پر مکمل"));
    }
    { auto& m = m_translations[QStringLiteral("log_reason_mouse_skipped_bg")];
      m.insert(QStringLiteral("ar"), QStringLiteral("تخطّي تلقائي — النمط [{mode}] يحرك الماوس الفعلي والنافذة في الخلفية حالياً (حماية للماوس)"));
      m.insert(QStringLiteral("en"), QStringLiteral("Skipped — Mode [{mode}] moves physical cursor while target window is in background"));
      m.insert(QStringLiteral("es"), QStringLiteral("Omitido — El modo [{mode}] mueve el cursor físico mientras la ventana está en 2.º plano"));
      m.insert(QStringLiteral("fr"), QStringLiteral("Ignoré — Le mode [{mode}] déplace le curseur physique alors que la fenêtre est en arrière-plan"));
      m.insert(QStringLiteral("de"), QStringLiteral("Übersprungen — Modus [{mode}] bewegt echten Mauszeiger während Fenster im Hintergrund ist"));
      m.insert(QStringLiteral("tr"), QStringLiteral("Atlandı — [{mode}] modu fiziksel fareyi hareket ettirir (hedef pencere arka planda)"));
      m.insert(QStringLiteral("ru"), QStringLiteral("Пропущено — Режим [{mode}] двигает физический курсор, пока целевое окно в фоне"));
      m.insert(QStringLiteral("id"), QStringLiteral("Dilewati — Mode [{mode}] menggerakkan kursor fisik saat jendela target di latar belakang"));
      m.insert(QStringLiteral("fa"), QStringLiteral("رد شد — حالت [{mode}] ماوس واقعی را حرکت می‌دهد در حالی که پنجره در پس‌زمینه است"));
      m.insert(QStringLiteral("ur"), QStringLiteral("نظر انداز — موڈ [{mode}] اصلی ماؤس کو حرکت دیتا ہے جبکہ ہدف ونڈو پس منظر میں ہے"));
    }
    { auto& m = m_translations[QStringLiteral("py_saved_prefix")];
      m.insert(QStringLiteral("ar"), QStringLiteral("سكربت محفوظ:"));
      m.insert(QStringLiteral("en"), QStringLiteral("Saved Script:"));
      m.insert(QStringLiteral("es"), QStringLiteral("Script guardado:"));
      m.insert(QStringLiteral("fr"), QStringLiteral("Script enregistré :"));
      m.insert(QStringLiteral("de"), QStringLiteral("Gespeichertes Skript:"));
      m.insert(QStringLiteral("tr"), QStringLiteral("Kayıtlı Kod:"));
      m.insert(QStringLiteral("ru"), QStringLiteral("Сохранённый скрипт:"));
      m.insert(QStringLiteral("id"), QStringLiteral("Skrip Tersimpan:"));
      m.insert(QStringLiteral("fa"), QStringLiteral("اسکریپت ذخیره‌شده:"));
      m.insert(QStringLiteral("ur"), QStringLiteral("محفوظ اسکرپٹ:"));
    }
    { auto& m = m_translations[QStringLiteral("py_preset_chat_sweep")];
      m.insert(QStringLiteral("ar"), QStringLiteral("١. [جاهز] فحص الشات داخل الألعاب (فتح Enter ← كتابة الطريقة ← إرسال Enter)"));
      m.insert(QStringLiteral("en"), QStringLiteral("1. [Preset] In-Game Chat Sweep (Open Enter -> Type Method -> Send Enter)"));
      m.insert(QStringLiteral("es"), QStringLiteral("1. [Preset] Chat en juego (Abrir Enter -> Escribir método -> Enviar Enter)"));
      m.insert(QStringLiteral("fr"), QStringLiteral("1. [Preset] Chat en jeu (Ouvrir Entrée -> Écrire méthode -> Envoyer Entrée)"));
      m.insert(QStringLiteral("de"), QStringLiteral("1. [Preset] In-Game-Chat-Test (Enter öffnen -> Methode tippen -> Enter senden)"));
      m.insert(QStringLiteral("tr"), QStringLiteral("1. [Hazır] Oyun İçi Sohbet Testi (Enter Aç -> Yöntemi Yaz -> Enter Gönder)"));
      m.insert(QStringLiteral("ru"), QStringLiteral("1. [Шаблон] Тест чата в игре (Открыть Enter -> Написать метод -> Отправить Enter)"));
      m.insert(QStringLiteral("id"), QStringLiteral("1. [Preset] Uji Chat Game (Buka Enter -> Ketik Metode -> Kirim Enter)"));
      m.insert(QStringLiteral("fa"), QStringLiteral("۱. [آماده] تست چت داخل بازی (باز کردن Enter ← تایپ روش ← ارسال Enter)"));
      m.insert(QStringLiteral("ur"), QStringLiteral("1. [تیار] گیم چیٹ ٹیسٹ (انٹر کھولیں -> طریقہ لکھیں -> انٹر بھیجیں)"));
    }
    { auto& m = m_translations[QStringLiteral("py_preset_human_3d_keys")];
      m.insert(QStringLiteral("ar"), QStringLiteral("٢. [جاهز] ضغطات بشرية للألعاب 3D (تحرك WASD + أرقام 1/2/3 + قفز Space)"));
      m.insert(QStringLiteral("en"), QStringLiteral("2. [Preset] Humanized 3D Game Keys (WASD Move + 1/2/3 Switch + Space Jump)"));
      m.insert(QStringLiteral("es"), QStringLiteral("2. [Preset] Teclas 3D humanizadas (Movimiento WASD + 1/2/3 + Salto Espacio)"));
      m.insert(QStringLiteral("fr"), QStringLiteral("2. [Preset] Touches 3D humanisées (WASD + Armes 1/2/3 + Saut Espace)"));
      m.insert(QStringLiteral("de"), QStringLiteral("2. [Preset] Menschliche 3D-Spieltasten (WASD + 1/2/3 + Leertaste)"));
      m.insert(QStringLiteral("tr"), QStringLiteral("2. [Hazır] İnsansı 3D Oyun Tuşları (WASD Hareket + 1/2/3 + Space Zıplama)"));
      m.insert(QStringLiteral("ru"), QStringLiteral("2. [Шаблон] Человеческие нажатия для 3D-игр (WASD + 1/2/3 + Пробел)"));
      m.insert(QStringLiteral("id"), QStringLiteral("2. [Preset] Tombol Game 3D Natural (Gerak WASD + 1/2/3 + Lompat Spasi)"));
      m.insert(QStringLiteral("fa"), QStringLiteral("۲. [آماده] فشردن طبیعی انسانی برای بازی 3D (حرکت WASD + اعداد 1/2/3 + پرش Space)"));
      m.insert(QStringLiteral("ur"), QStringLiteral("2. [تیار] انسانی انداز کے 3D گیم بٹن (WASD حرکت + 1/2/3 + Space جمپ)"));
    }
    { auto& m = m_translations[QStringLiteral("py_preset_3d_mouse")];
      m.insert(QStringLiteral("ar"), QStringLiteral("٣. [جاهز] كليكات ماوس وتحريك كاميرا 3D داخل اللعب (in_game_3d + move_rel)"));
      m.insert(QStringLiteral("en"), QStringLiteral("3. [Preset] In-Match 3D Crosshair Click & Relative Camera Aim (in_game_3d + move_rel)"));
      m.insert(QStringLiteral("es"), QStringLiteral("3. [Preset] Clic 3D en partida y giro de cámara relativo (in_game_3d + move_rel)"));
      m.insert(QStringLiteral("fr"), QStringLiteral("3. [Preset] Clic 3D en match et visée caméra relative (in_game_3d + move_rel)"));
      m.insert(QStringLiteral("de"), QStringLiteral("3. [Preset] 3D-Fadenkreuz-Klick & relative Kamerabewegung (in_game_3d + move_rel)"));
      m.insert(QStringLiteral("tr"), QStringLiteral("3. [Hazır] Maç İçi 3D Nişangah Tıklaması ve Kamera Çevirme (in_game_3d + move_rel)"));
      m.insert(QStringLiteral("ru"), QStringLiteral("3. [Шаблон] 3D-клик прицела и поворот камеры в матче (in_game_3d + move_rel)"));
      m.insert(QStringLiteral("id"), QStringLiteral("3. [Preset] Klik Crosshair 3D & Gerak Kamera Relatif (in_game_3d + move_rel)"));
      m.insert(QStringLiteral("fa"), QStringLiteral("۳. [آماده] کلیک ماوس و چرخش دوربین 3D داخل مسابقه (in_game_3d + move_rel)"));
      m.insert(QStringLiteral("ur"), QStringLiteral("3. [تیار] میچ کے اندر 3D ماؤس کلک اور کیمرہ حرکت (in_game_3d + move_rel)"));
    }
    { auto& m = m_translations[QStringLiteral("py_preset_human_typing")];
      m.insert(QStringLiteral("ar"), QStringLiteral("٤. [جاهز] كتابة نص بسرعة كيبورد بشرية طبيعية متغيرة (human=True)"));
      m.insert(QStringLiteral("en"), QStringLiteral("4. [Preset] Humanized Natural Keyboard Typing Rhythm (human=True)"));
      m.insert(QStringLiteral("es"), QStringLiteral("4. [Preset] Escritura con ritmo humano natural (human=True)"));
      m.insert(QStringLiteral("fr"), QStringLiteral("4. [Preset] Frappe au clavier à rythme humain naturel (human=True)"));
      m.insert(QStringLiteral("de"), QStringLiteral("4. [Preset] Natürlicher menschlicher Tipp-Rhythmus (human=True)"));
      m.insert(QStringLiteral("tr"), QStringLiteral("4. [Hazır] Doğal İnsan Klavye Yazım Hızı (human=True)"));
      m.insert(QStringLiteral("ru"), QStringLiteral("4. [Шаблон] Естественная человеческая скорость печати (human=True)"));
      m.insert(QStringLiteral("id"), QStringLiteral("4. [Preset] Ritme Ketik Keyboard Manusia Natural (human=True)"));
      m.insert(QStringLiteral("fa"), QStringLiteral("۴. [آماده] تایپ متن با سرعت طبیعی دست انسان (human=True)"));
      m.insert(QStringLiteral("ur"), QStringLiteral("4. [تیار] قدرتی انسانی رفتار سے ٹائپنگ (human=True)"));
    }
    { auto& m = m_translations[QStringLiteral("py_preset_skill_loop")];
      m.insert(QStringLiteral("ar"), QStringLiteral("٥. [جاهز] كومبو مهارات متتابع بتوقيت بشري عشوائي (1 -> 2 -> 3 -> Tab)"));
      m.insert(QStringLiteral("en"), QStringLiteral("5. [Preset] Randomized Human Skill Combo Loop (1 -> 2 -> 3 -> Tab)"));
      m.insert(QStringLiteral("es"), QStringLiteral("5. [Preset] Combo de habilidades con tiempo aleatorio (1 -> 2 -> 3 -> Tab)"));
      m.insert(QStringLiteral("fr"), QStringLiteral("5. [Preset] Combo de compétences à timing aléatoire (1 -> 2 -> 3 -> Tab)"));
      m.insert(QStringLiteral("de"), QStringLiteral("5. [Preset] Skill-Kombo mit Zufalls-Timing (1 -> 2 -> 3 -> Tab)"));
      m.insert(QStringLiteral("tr"), QStringLiteral("5. [Hazır] Rastgele İnsansı Yetenek Kombosu (1 -> 2 -> 3 -> Tab)"));
      m.insert(QStringLiteral("ru"), QStringLiteral("5. [Шаблон] Комбо умений со случайной задержкой (1 -> 2 -> 3 -> Tab)"));
      m.insert(QStringLiteral("id"), QStringLiteral("5. [Preset] Kombo Skill Waktu Acak Natural (1 -> 2 -> 3 -> Tab)"));
      m.insert(QStringLiteral("fa"), QStringLiteral("۵. [آماده] کمبوی مهارت تصادفی طبیعی (1 -> 2 -> 3 -> Tab)"));
      m.insert(QStringLiteral("ur"), QStringLiteral("5. [تیار] بے ترتیب انسانی اسکل کومبو (1 -> 2 -> 3 -> Tab)"));
    }
    { auto& m = m_translations[QStringLiteral("py_preset_default_sweep")];
      m.insert(QStringLiteral("ar"), QStringLiteral("٦. [جاهز] السكربت القياسي لفحص الطرق (Method N [id] - OK)"));
      m.insert(QStringLiteral("en"), QStringLiteral("6. [Preset] Standard Per-Method Diagnostic Script (Method N [id] - OK)"));
      m.insert(QStringLiteral("es"), QStringLiteral("6. [Preset] Script estándar de diagnóstico por método"));
      m.insert(QStringLiteral("fr"), QStringLiteral("6. [Preset] Script standard de diagnostic par méthode"));
      m.insert(QStringLiteral("de"), QStringLiteral("6. [Preset] Standard-Diagnoseskript pro Methode"));
      m.insert(QStringLiteral("tr"), QStringLiteral("6. [Hazır] Standart Yöntem Tanılama Kodu"));
      m.insert(QStringLiteral("ru"), QStringLiteral("6. [Шаблон] Стандартный диагностический скрипт методов"));
      m.insert(QStringLiteral("id"), QStringLiteral("6. [Preset] Skrip Diagnostik Metode Standar"));
      m.insert(QStringLiteral("fa"), QStringLiteral("۶. [آماده] اسکریپت استاندارد تست روش‌ها"));
      m.insert(QStringLiteral("ur"), QStringLiteral("6. [تیار] معیاری طریقہ تشخیصی اسکرپٹ"));
    }
    { auto& m = m_translations[QStringLiteral("ph_py_script_name")];
      m.insert(QStringLiteral("ar"), QStringLiteral("اسم السكربت لحفظه..."));
      m.insert(QStringLiteral("en"), QStringLiteral("Script name to save..."));
      m.insert(QStringLiteral("es"), QStringLiteral("Nombre del script..."));
      m.insert(QStringLiteral("fr"), QStringLiteral("Nom du script..."));
      m.insert(QStringLiteral("de"), QStringLiteral("Skriptname zum Speichern..."));
      m.insert(QStringLiteral("tr"), QStringLiteral("Kaydedilecek kod adı..."));
      m.insert(QStringLiteral("ru"), QStringLiteral("Имя скрипта для сохранения..."));
      m.insert(QStringLiteral("id"), QStringLiteral("Nama skrip untuk disimpan..."));
      m.insert(QStringLiteral("fa"), QStringLiteral("نام اسکریپت برای ذخیره..."));
      m.insert(QStringLiteral("ur"), QStringLiteral("محفوظ کرنے کے لیے اسکرپٹ کا نام..."));
    }
    { auto& m = m_translations[QStringLiteral("btn_save_py_script")];
      m.insert(QStringLiteral("ar"), QStringLiteral("+ حفظ السكربت"));
      m.insert(QStringLiteral("en"), QStringLiteral("+ Save Script"));
      m.insert(QStringLiteral("es"), QStringLiteral("+ Guardar Script"));
      m.insert(QStringLiteral("fr"), QStringLiteral("+ Enregistrer Script"));
      m.insert(QStringLiteral("de"), QStringLiteral("+ Skript speichern"));
      m.insert(QStringLiteral("tr"), QStringLiteral("+ Kodu Kaydet"));
      m.insert(QStringLiteral("ru"), QStringLiteral("+ Сохранить скрипт"));
      m.insert(QStringLiteral("id"), QStringLiteral("+ Simpan Skrip"));
      m.insert(QStringLiteral("fa"), QStringLiteral("+ ذخیره اسکریپت"));
      m.insert(QStringLiteral("ur"), QStringLiteral("+ اسکرپٹ محفوظ کریں"));
    }
    { auto& m = m_translations[QStringLiteral("tip_del_py_script")];
      m.insert(QStringLiteral("ar"), QStringLiteral("حذف السكربت المحفوظ المحدد"));
      m.insert(QStringLiteral("en"), QStringLiteral("Delete selected saved script"));
      m.insert(QStringLiteral("es"), QStringLiteral("Eliminar script guardado seleccionado"));
      m.insert(QStringLiteral("fr"), QStringLiteral("Supprimer le script enregistré sélectionné"));
      m.insert(QStringLiteral("de"), QStringLiteral("Ausgewähltes gespeichertes Skript löschen"));
      m.insert(QStringLiteral("tr"), QStringLiteral("Seçili kayıtlı kodu sil"));
      m.insert(QStringLiteral("ru"), QStringLiteral("Удалить выбранный сохранённый скрипт"));
      m.insert(QStringLiteral("id"), QStringLiteral("Hapus skrip tersimpan yang dipilih"));
      m.insert(QStringLiteral("fa"), QStringLiteral("حذف اسکریپت ذخیره‌شده انتخابی"));
      m.insert(QStringLiteral("ur"), QStringLiteral("منتخب محفوظ اسکرپٹ حذف کریں"));
    }
    { auto& m = m_translations[QStringLiteral("toast_py_script_saved")];
      m.insert(QStringLiteral("ar"), QStringLiteral("تم حفظ سكربت البايثون في مكتبتك المشتركة:"));
      m.insert(QStringLiteral("en"), QStringLiteral("Saved Python script to your shared library:"));
      m.insert(QStringLiteral("es"), QStringLiteral("Script Python guardado en tu biblioteca:"));
      m.insert(QStringLiteral("fr"), QStringLiteral("Script Python enregistré dans votre bibliothèque :"));
      m.insert(QStringLiteral("de"), QStringLiteral("Python-Skript in gemeinsamer Bibliothek gespeichert:"));
      m.insert(QStringLiteral("tr"), QStringLiteral("Python kodu ortak kütüphanenize kaydedildi:"));
      m.insert(QStringLiteral("ru"), QStringLiteral("Python-скрипт сохранён в общую библиотеку:"));
      m.insert(QStringLiteral("id"), QStringLiteral("Skrip Python disimpan ke pustaka bersama:"));
      m.insert(QStringLiteral("fa"), QStringLiteral("اسکریپت پایتون در کتابخانه مشترک ذخیره شد:"));
      m.insert(QStringLiteral("ur"), QStringLiteral("پائتھون اسکرپٹ آپ کی مشترکہ لائبریری میں محفوظ ہو گیا:"));
    }
    { auto& m = m_translations[QStringLiteral("toast_py_script_deleted")];
      m.insert(QStringLiteral("ar"), QStringLiteral("تم حذف السكربت المحفوظ من المكتبة"));
      m.insert(QStringLiteral("en"), QStringLiteral("Removed saved script from library"));
      m.insert(QStringLiteral("es"), QStringLiteral("Script eliminado de la biblioteca"));
      m.insert(QStringLiteral("fr"), QStringLiteral("Script supprimé de la bibliothèque"));
      m.insert(QStringLiteral("de"), QStringLiteral("Gespeichertes Skript aus Bibliothek entfernt"));
      m.insert(QStringLiteral("tr"), QStringLiteral("Kayıtlı kod kütüphaneden silindi"));
      m.insert(QStringLiteral("ru"), QStringLiteral("Сохранённый скрипт удалён из библиотеки"));
      m.insert(QStringLiteral("id"), QStringLiteral("Skrip tersimpan dihapus dari pustaka"));
      m.insert(QStringLiteral("fa"), QStringLiteral("اسکریپت ذخیره‌شده از کتابخانه حذف شد"));
      m.insert(QStringLiteral("ur"), QStringLiteral("محفوظ اسکرپٹ لائبریری سے حذف کر دیا گیا"));
    }
    { auto& m = m_translations[QStringLiteral("step_row_open_folder")];
      m.insert(QStringLiteral("ar"), QStringLiteral("فتح فولدر أو برنامج:"));
      m.insert(QStringLiteral("en"), QStringLiteral("Open Folder / App:"));
      m.insert(QStringLiteral("es"), QStringLiteral("Abrir carpeta / app:"));
      m.insert(QStringLiteral("fr"), QStringLiteral("Ouvrir dossier / app :"));
      m.insert(QStringLiteral("de"), QStringLiteral("Ordner / App öffnen:"));
      m.insert(QStringLiteral("tr"), QStringLiteral("Klasör / Program Aç:"));
      m.insert(QStringLiteral("ru"), QStringLiteral("Папка / программа:"));
      m.insert(QStringLiteral("id"), QStringLiteral("Buka Folder / App:"));
      m.insert(QStringLiteral("fa"), QStringLiteral("باز کردن پوشه یا برنامه:"));
      m.insert(QStringLiteral("ur"), QStringLiteral("فولڈر یا پروگرام کھولیں:"));
    }
    { auto& m = m_translations[QStringLiteral("btn_browse_app_file")];
      m.insert(QStringLiteral("ar"), QStringLiteral("برنامج / ملف..."));
      m.insert(QStringLiteral("en"), QStringLiteral("Program / File..."));
      m.insert(QStringLiteral("es"), QStringLiteral("Programa / Archivo..."));
      m.insert(QStringLiteral("fr"), QStringLiteral("Programme / Fichier..."));
      m.insert(QStringLiteral("de"), QStringLiteral("Programm / Datei..."));
      m.insert(QStringLiteral("tr"), QStringLiteral("Program / Dosya..."));
      m.insert(QStringLiteral("ru"), QStringLiteral("Программа / Файл..."));
      m.insert(QStringLiteral("id"), QStringLiteral("Program / File..."));
      m.insert(QStringLiteral("fa"), QStringLiteral("برنامه / فایل..."));
      m.insert(QStringLiteral("ur"), QStringLiteral("پروگرام / فائل..."));
    }
    { auto& m = m_translations[QStringLiteral("lbl_combo_stop_hotkey")];
      m.insert(QStringLiteral("ar"), QStringLiteral("زرار إيقاف التكرار فوراً (إجباري):"));
      m.insert(QStringLiteral("en"), QStringLiteral("Stop Repeat Hotkey (Required):"));
      m.insert(QStringLiteral("es"), QStringLiteral("Tecla para detener repetición (Obligatorio):"));
      m.insert(QStringLiteral("fr"), QStringLiteral("Touche d'arrêt de répétition (Obligatoire) :"));
      m.insert(QStringLiteral("de"), QStringLiteral("Stopp-Taste für Wiederholung (Pflicht):"));
      m.insert(QStringLiteral("tr"), QStringLiteral("Tekrarı Durdurma Tuşu (Zorunlu):"));
      m.insert(QStringLiteral("ru"), QStringLiteral("Клавиша остановки повтора (Обязательно):"));
      m.insert(QStringLiteral("id"), QStringLiteral("Tombol Hentikan Pengulangan (Wajib):"));
      m.insert(QStringLiteral("fa"), QStringLiteral("کلید توقف فوری تکرار (اجباری):"));
      m.insert(QStringLiteral("ur"), QStringLiteral("تکرار فوری روکنے کا بٹن (لازمی):"));
    }
    { auto& m = m_translations[QStringLiteral("err_combo_stop_hotkey_required")];
      m.insert(QStringLiteral("ar"), QStringLiteral("يجب تحديد «زرار إيقاف التكرار فوراً» أولاً قبل تشغيل نمط التكرار المستمر أو التكرار مع الضغط!"));
      m.insert(QStringLiteral("en"), QStringLiteral("You must set a \"Stop Repeat Hotkey\" first before enabling or running a repeating Smart Key!"));
      m.insert(QStringLiteral("es"), QStringLiteral("¡Debes asignar una tecla para detener la repetición antes de activar el modo repetitivo!"));
      m.insert(QStringLiteral("fr"), QStringLiteral("Vous devez définir une touche d'arrêt avant d'activer le mode répétition !"));
      m.insert(QStringLiteral("de"), QStringLiteral("Bitte legen Sie zuerst eine Stopp-Taste fest, bevor Sie den Wiederholungsmodus aktivieren!"));
      m.insert(QStringLiteral("tr"), QStringLiteral("Tekrarlama modunu etkinleştirmeden önce bir Durdurma Tuşu belirlemelisiniz!"));
      m.insert(QStringLiteral("ru"), QStringLiteral("Сначала назначьте клавишу остановки повтора перед включением режима повтора!"));
      m.insert(QStringLiteral("id"), QStringLiteral("Harap atur Tombol Hentikan Pengulangan terlebih dahulu sebelum mengaktifkan mode ulang!"));
      m.insert(QStringLiteral("fa"), QStringLiteral("لطفاً قبل از فعال‌سازی حالت تکرار، ابتدا کلید توقف فوری تکرار را مشخص کنید!"));
      m.insert(QStringLiteral("ur"), QStringLiteral("تکرار موڈ چلانے سے پہلے براہ کرم فوری روکنے کا بٹن منتخب کریں!"));
    }
    { auto& m = m_translations[QStringLiteral("toast_combo_stopped_by_hk")];
      m.insert(QStringLiteral("ar"), QStringLiteral("تم إيقاف تكرار الماكرو فوراً بواسطة زرار الإيقاف المخصص"));
      m.insert(QStringLiteral("en"), QStringLiteral("Repeating macro stopped immediately via Stop Hotkey"));
      m.insert(QStringLiteral("es"), QStringLiteral("Macro repetitivo detenido inmediatamente con la tecla de parada"));
      m.insert(QStringLiteral("fr"), QStringLiteral("Macro en boucle arrêté immédiatement via la touche d'arrêt"));
      m.insert(QStringLiteral("de"), QStringLiteral("Wiederholendes Makro sofort über Stopp-Taste gestoppt"));
      m.insert(QStringLiteral("tr"), QStringLiteral("Tekrarlayan makro Durdurma Tuşu ile anında durduruldu"));
      m.insert(QStringLiteral("ru"), QStringLiteral("Повторяющийся макрос мгновенно остановлен клавишей стоп"));
      m.insert(QStringLiteral("id"), QStringLiteral("Makro berulang segera dihentikan melalui Tombol Berhenti"));
      m.insert(QStringLiteral("fa"), QStringLiteral("تکرار ماکرو فوراً با کلید توقف متوقف شد"));
      m.insert(QStringLiteral("ur"), QStringLiteral("تکرار والا میکرو اسٹاپ بٹن کے ذریعے فوراً روک دیا گیا"));
    }
    { auto& m = m_translations[QStringLiteral("tip_step_enable_toggle")];
      m.insert(QStringLiteral("ar"), QStringLiteral("تفعيل / إيقاف مؤقت لهذه الخطوة دون حذفها (تخطّي عند التشغيل)"));
      m.insert(QStringLiteral("en"), QStringLiteral("Enable / Pause (skip) this step during execution without deleting it"));
      m.insert(QStringLiteral("es"), QStringLiteral("Activar / Pausar (omitir) este paso sin eliminarlo"));
      m.insert(QStringLiteral("fr"), QStringLiteral("Activer / Mettre en pause (ignorer) cette étape sans la supprimer"));
      m.insert(QStringLiteral("de"), QStringLiteral("Diesen Schritt aktivieren / pausieren (überspringen) ohne ihn zu löschen"));
      m.insert(QStringLiteral("tr"), QStringLiteral("Bu adımı silmeden etkinleştir / geçici olarak duraklat (atla)"));
      m.insert(QStringLiteral("ru"), QStringLiteral("Включить / приостановить (пропустить) этот шаг без удаления"));
      m.insert(QStringLiteral("id"), QStringLiteral("Aktifkan / Jeda (lewati) langkah ini tanpa menghapusnya"));
      m.insert(QStringLiteral("fa"), QStringLiteral("فعال / توقف موقت (رد کردن) این مرحله بدون حذف آن"));
      m.insert(QStringLiteral("ur"), QStringLiteral("اس مرحلے کو حذف کیے بغیر فعال یا عارضی طور پر موقوف (اسکپ) کریں"));
    }
    { auto& m = m_translations[QStringLiteral("tip_step_note_btn")];
      m.insert(QStringLiteral("ar"), QStringLiteral("اضغط لإضافة أو تعديل ملاحظة (أو قف بالماوس على العلامة لعرضها فوراً)"));
      m.insert(QStringLiteral("en"), QStringLiteral("Click to add/edit step note (or hover on this icon to read immediately)"));
      m.insert(QStringLiteral("es"), QStringLiteral("Clic para añadir/editar nota (o mantén el cursor 2 s sobre el icono para leer)"));
      m.insert(QStringLiteral("fr"), QStringLiteral("Cliquez pour ajouter/modifier une note (ou survolez l'icône 2 s pour lire)"));
      m.insert(QStringLiteral("de"), QStringLiteral("Klicken zum Bearbeiten der Notiz (oder 2 Sek. auf dem Symbol verweilen zum Lesen)"));
      m.insert(QStringLiteral("tr"), QStringLiteral("Not eklemek/düzenlemek için tıklayın (okumak için simge üzerinde 2 sn bekleyin)"));
      m.insert(QStringLiteral("ru"), QStringLiteral("Нажмите для заметки (или наведите курсор на значок на 2 сек для чтения)"));
      m.insert(QStringLiteral("id"), QStringLiteral("Klik untuk tambah/edit catatan (atau arahkan kursor 2 detik pada ikon untuk membaca)"));
      m.insert(QStringLiteral("fa"), QStringLiteral("برای افزودن/ویرایش یادداشت کلیک کنید (یا برای خواندن ۲ ثانیه روی آیکون نگه دارید)"));
      m.insert(QStringLiteral("ur"), QStringLiteral("نوٹ شامل/ترمیم کرنے کے لیے کلک کریں (یا پڑھنے کے لیے آئیکن پر 2 سیکنڈ ماؤس رکھیں)"));
    }
    { auto& m = m_translations[QStringLiteral("lbl_step_note_title")];
      m.insert(QStringLiteral("ar"), QStringLiteral("ملاحظة الخطوة"));
      m.insert(QStringLiteral("en"), QStringLiteral("Step Note"));
      m.insert(QStringLiteral("es"), QStringLiteral("Nota del Paso"));
      m.insert(QStringLiteral("fr"), QStringLiteral("Note de l'étape"));
      m.insert(QStringLiteral("de"), QStringLiteral("Schritt-Notiz"));
      m.insert(QStringLiteral("tr"), QStringLiteral("Adım Notu"));
      m.insert(QStringLiteral("ru"), QStringLiteral("Заметка к шагу"));
      m.insert(QStringLiteral("id"), QStringLiteral("Catatan Langkah"));
      m.insert(QStringLiteral("fa"), QStringLiteral("یادداشت مرحله"));
      m.insert(QStringLiteral("ur"), QStringLiteral("مرحلے کا نوٹ"));
    }
    { auto& m = m_translations[QStringLiteral("ph_step_note_input")];
      m.insert(QStringLiteral("ar"), QStringLiteral("اكتب ملاحظتك لهذه الخطوة هنا..."));
      m.insert(QStringLiteral("en"), QStringLiteral("Write your note for this step here..."));
      m.insert(QStringLiteral("es"), QStringLiteral("Escribe tu nota para este paso aquí..."));
      m.insert(QStringLiteral("fr"), QStringLiteral("Écrivez votre note pour cette étape ici..."));
      m.insert(QStringLiteral("de"), QStringLiteral("Notiz für diesen Schritt hier eingeben..."));
      m.insert(QStringLiteral("tr"), QStringLiteral("Bu adım için notunuzu buraya yazın..."));
      m.insert(QStringLiteral("ru"), QStringLiteral("Введите заметку для этого шага..."));
      m.insert(QStringLiteral("id"), QStringLiteral("Tulis catatan Anda untuk langkah ini di sini..."));
      m.insert(QStringLiteral("fa"), QStringLiteral("یادداشت خود را برای این مرحله اینجا بنویسید..."));
      m.insert(QStringLiteral("ur"), QStringLiteral("اس مرحلے کے لیے اپنا نوٹ یہاں لکھیں..."));
    }
    { auto& m = m_translations[QStringLiteral("btn_save_note")];
      m.insert(QStringLiteral("ar"), QStringLiteral("حفظ الملاحظة"));
      m.insert(QStringLiteral("en"), QStringLiteral("Save Note"));
      m.insert(QStringLiteral("es"), QStringLiteral("Guardar Nota"));
      m.insert(QStringLiteral("fr"), QStringLiteral("Enregistrer"));
      m.insert(QStringLiteral("de"), QStringLiteral("Notiz speichern"));
      m.insert(QStringLiteral("tr"), QStringLiteral("Notu Kaydet"));
      m.insert(QStringLiteral("ru"), QStringLiteral("Сохранить"));
      m.insert(QStringLiteral("id"), QStringLiteral("Simpan Catatan"));
      m.insert(QStringLiteral("fa"), QStringLiteral("ذخیره یادداشت"));
      m.insert(QStringLiteral("ur"), QStringLiteral("نوٹ محفوظ کریں"));
    }
    { auto& m = m_translations[QStringLiteral("btn_clear_note")];
      m.insert(QStringLiteral("ar"), QStringLiteral("مسح"));
      m.insert(QStringLiteral("en"), QStringLiteral("Clear"));
      m.insert(QStringLiteral("es"), QStringLiteral("Borrar"));
      m.insert(QStringLiteral("fr"), QStringLiteral("Effacer"));
      m.insert(QStringLiteral("de"), QStringLiteral("Löschen"));
      m.insert(QStringLiteral("tr"), QStringLiteral("Temizle"));
      m.insert(QStringLiteral("ru"), QStringLiteral("Очистить"));
      m.insert(QStringLiteral("id"), QStringLiteral("Hapus"));
      m.insert(QStringLiteral("fa"), QStringLiteral("پاک کردن"));
      m.insert(QStringLiteral("ur"), QStringLiteral("صاف کریں"));
    }
    { auto& m = m_translations[QStringLiteral("lbl_step_paused_badge")];
      m.insert(QStringLiteral("ar"), QStringLiteral("(موقوفة مؤقتاً)"));
      m.insert(QStringLiteral("en"), QStringLiteral("(Paused / Skipped)"));
      m.insert(QStringLiteral("es"), QStringLiteral("(En pausa / Omitido)"));
      m.insert(QStringLiteral("fr"), QStringLiteral("(En pause / Ignorée)"));
      m.insert(QStringLiteral("de"), QStringLiteral("(Pausiert / Übersprungen)"));
      m.insert(QStringLiteral("tr"), QStringLiteral("(Duraklatıldı / Atlandı)"));
      m.insert(QStringLiteral("ru"), QStringLiteral("(Приостановлен / Пропущен)"));
      m.insert(QStringLiteral("id"), QStringLiteral("(Dijeda / Dilewati)"));
      m.insert(QStringLiteral("fa"), QStringLiteral("(متوقف / رد شده)"));
      m.insert(QStringLiteral("ur"), QStringLiteral("(موقوف / اسکپ شدہ)"));
    }
    { auto& m = m_translations[QStringLiteral("log_reason_self_app_protected")];
      m.insert(QStringLiteral("ar"), QStringLiteral("حماية تلقائية — تم منع الضغط على نافذة البرنامج نفسه (V-MacroPlus)"));
      m.insert(QStringLiteral("en"), QStringLiteral("Self-Protection — Blocked injecting input into V-MacroPlus itself"));
      m.insert(QStringLiteral("es"), QStringLiteral("Autoprotección — Se bloqueó la pulsación sobre la propia ventana de V-MacroPlus"));
      m.insert(QStringLiteral("fr"), QStringLiteral("Auto-protection — Injection bloquée sur la fenêtre V-MacroPlus elle-même"));
      m.insert(QStringLiteral("de"), QStringLiteral("Selbstschutz — Eingabe auf das eigene V-MacroPlus-Fenster blockiert"));
      m.insert(QStringLiteral("tr"), QStringLiteral("Öz-Koruma — V-MacroPlus kendi penceresine tuş/fare gönderimi engellendi"));
      m.insert(QStringLiteral("ru"), QStringLiteral("Самозащита — Нажатие в собственное окно V-MacroPlus заблокировано"));
      m.insert(QStringLiteral("id"), QStringLiteral("Proteksi Diri — Input ke jendela V-MacroPlus sendiri diblokir"));
      m.insert(QStringLiteral("fa"), QStringLiteral("حفاظت خودکار — ارسال ورودی به پنجره خود برنامه V-MacroPlus مسدود شد"));
      m.insert(QStringLiteral("ur"), QStringLiteral("خودکار تحفظ — V-MacroPlus کی اپنی ونڈو پر بٹن دبانا روک دیا گیا"));
    }
    { auto& m = m_translations[QStringLiteral("method_pico_hid")];
      m.insert(QStringLiteral("ar"), QStringLiteral("الطريقة 21 ★ [هاردوير خارجي — Raspberry Pi Pico]: كيبورد وماوس USB HID فيزيائي عبر بوردة Pico"));
      m.insert(QStringLiteral("en"), QStringLiteral("Method 21 ★ [Hardware MCU — Raspberry Pi Pico]: External Physical USB HID Keyboard & Mouse via Pico"));
      m.insert(QStringLiteral("es"), QStringLiteral("Método 21 ★ [Hardware MCU — Raspberry Pi Pico]: Teclado y ratón USB HID físico externo vía Pico"));
      m.insert(QStringLiteral("fr"), QStringLiteral("Méthode 21 ★ [Hardware MCU — Raspberry Pi Pico] : Clavier et souris USB HID physique via Pico"));
      m.insert(QStringLiteral("de"), QStringLiteral("Methode 21 ★ [Hardware-MCU — Raspberry Pi Pico]: Physische USB-HID-Tastatur & -Maus über Pico"));
      m.insert(QStringLiteral("tr"), QStringLiteral("Yöntem 21 ★ [Donanım MCU — Raspberry Pi Pico]: Pico üzerinden fiziksel USB HID Klavye ve Fare"));
      m.insert(QStringLiteral("ru"), QStringLiteral("Метод 21 ★ [Аппаратный MCU — Raspberry Pi Pico]: Физическая USB HID клавиатура и мышь через Pico"));
      m.insert(QStringLiteral("id"), QStringLiteral("Metode 21 ★ [Hardware MCU — Raspberry Pi Pico]: Keyboard & Mouse USB HID Fisik via Pico"));
      m.insert(QStringLiteral("fa"), QStringLiteral("روش ۲۱ ★ [سخت‌افزار خارجی — Raspberry Pi Pico]: کیبورد و ماوس فیزیکی USB HID با برد Pico"));
      m.insert(QStringLiteral("ur"), QStringLiteral("طریقہ 21 ★ [ہارڈویئر MCU — Raspberry Pi Pico]: بذرِیعہ Pico فزیکل USB HID کی بورڈ اور ماؤس"));
    }
    { auto& m = m_translations[QStringLiteral("method_arduino_hid")];
      m.insert(QStringLiteral("ar"), QStringLiteral("الطريقة 22 ★ [هاردوير خارجي — Arduino Leonardo/Pro Micro]: كيبورد وماوس USB HID فيزيائي (ATmega32U4)"));
      m.insert(QStringLiteral("en"), QStringLiteral("Method 22 ★ [Hardware MCU — Arduino Leonardo/Pro Micro]: External Physical USB HID (ATmega32U4)"));
      m.insert(QStringLiteral("es"), QStringLiteral("Método 22 ★ [Hardware MCU — Arduino Leonardo/Pro Micro]: Teclado y ratón USB HID físico (ATmega32U4)"));
      m.insert(QStringLiteral("fr"), QStringLiteral("Méthode 22 ★ [Hardware MCU — Arduino Leonardo/Pro Micro] : Clavier et souris USB HID (ATmega32U4)"));
      m.insert(QStringLiteral("de"), QStringLiteral("Methode 22 ★ [Hardware-MCU — Arduino Leonardo/Pro Micro]: Physische USB-HID (ATmega32U4)"));
      m.insert(QStringLiteral("tr"), QStringLiteral("Yöntem 22 ★ [Donanım MCU — Arduino Leonardo/Pro Micro]: Fiziksel USB HID Klavye & Fare (ATmega32U4)"));
      m.insert(QStringLiteral("ru"), QStringLiteral("Метод 22 ★ [Аппаратный MCU — Arduino Leonardo/Pro Micro]: Физический USB HID (ATmega32U4)"));
      m.insert(QStringLiteral("id"), QStringLiteral("Metode 22 ★ [Hardware MCU — Arduino Leonardo/Pro Micro]: USB HID Fisik (ATmega32U4)"));
      m.insert(QStringLiteral("fa"), QStringLiteral("روش ۲۲ ★ [سخت‌افزار خارجی — Arduino Leonardo/Pro Micro]: کیبورد و ماوس فیزیکی USB HID (ATmega32U4)"));
      m.insert(QStringLiteral("ur"), QStringLiteral("طریقہ 22 ★ [ہارڈویئر MCU — Arduino Leonardo/Pro Micro]: فزیکل USB HID کی بورڈ اور ماؤس (ATmega32U4)"));
    }
    { auto& m = m_translations[QStringLiteral("lbl_mcu_diag_title")];
      m.insert(QStringLiteral("ar"), QStringLiteral("🔌 جسر الهاردوير الخارجي (Raspberry Pi Pico & Arduino Leonardo / Pro Micro — الطريقة 21 و 22):"));
      m.insert(QStringLiteral("en"), QStringLiteral("🔌 External USB HID Microcontroller Bridge (Raspberry Pi Pico & Arduino Leonardo / Pro Micro — Methods 21 & 22):"));
      m.insert(QStringLiteral("es"), QStringLiteral("🔌 Puente Microcontrolador USB HID Externo (Raspberry Pi Pico y Arduino Leonardo / Pro Micro — Métodos 21 y 22):"));
      m.insert(QStringLiteral("fr"), QStringLiteral("🔌 Pont Microcontrôleur USB HID Externe (Raspberry Pi Pico & Arduino Leonardo / Pro Micro — Méthodes 21 & 22) :"));
      m.insert(QStringLiteral("de"), QStringLiteral("🔌 Externe USB-HID-Mikrocontroller-Brücke (Raspberry Pi Pico & Arduino Leonardo / Pro Micro — Methode 21 & 22):"));
      m.insert(QStringLiteral("tr"), QStringLiteral("🔌 Harici USB HID Mikrodenetleyici Köprüsü (Raspberry Pi Pico & Arduino Leonardo / Pro Micro — Yöntem 21 & 22):"));
      m.insert(QStringLiteral("ru"), QStringLiteral("🔌 Внешний USB HID микроконтроллер (Raspberry Pi Pico и Arduino Leonardo / Pro Micro — Методы 21 и 22):"));
      m.insert(QStringLiteral("id"), QStringLiteral("🔌 Jembatan Mikrokontroler USB HID Eksternal (Raspberry Pi Pico & Arduino — Metode 21 & 22):"));
      m.insert(QStringLiteral("fa"), QStringLiteral("🔌 پل سخت‌افزاری میکروکنترلر USB HID (برد Raspberry Pi Pico و Arduino — روش ۲۱ و ۲۲):"));
      m.insert(QStringLiteral("ur"), QStringLiteral("🔌 بیرونی USB HID مائیکرو کنٹرولر برج (Raspberry Pi Pico اور Arduino — طریقہ 21 اور 22):"));
    }
    { auto& m = m_translations[QStringLiteral("lbl_mcu_com_port")];
      m.insert(QStringLiteral("ar"), QStringLiteral("منفذ الاتصال بالقطعة (COM Port):"));
      m.insert(QStringLiteral("en"), QStringLiteral("Microcontroller Serial Port (COM Port):"));
      m.insert(QStringLiteral("es"), QStringLiteral("Puerto serie del microcontrolador (COM):"));
      m.insert(QStringLiteral("fr"), QStringLiteral("Port série du microcontrôleur (COM) :"));
      m.insert(QStringLiteral("de"), QStringLiteral("Serieller Mikrocontroller-Port (COM):"));
      m.insert(QStringLiteral("tr"), QStringLiteral("Mikrodenetleyici Seri Portu (COM):"));
      m.insert(QStringLiteral("ru"), QStringLiteral("COM-порт микроконтроллера:"));
      m.insert(QStringLiteral("id"), QStringLiteral("Port Serial Mikrokontroler (COM):"));
      m.insert(QStringLiteral("fa"), QStringLiteral("پورت سریال اتصال به برد (COM Port):"));
      m.insert(QStringLiteral("ur"), QStringLiteral("مائیکرو کنٹرولر سیریل پورٹ (COM Port):"));
    }
    { auto& m = m_translations[QStringLiteral("mcu_port_auto")];
      m.insert(QStringLiteral("ar"), QStringLiteral("⚡ كشف تلقائي ذكي (Auto-Detect — موصى به)"));
      m.insert(QStringLiteral("en"), QStringLiteral("⚡ Smart Auto-Detect (Recommended)"));
      m.insert(QStringLiteral("es"), QStringLiteral("⚡ Detección automática inteligente (Recomendado)"));
      m.insert(QStringLiteral("fr"), QStringLiteral("⚡ Détection automatique intelligente (Recommandé)"));
      m.insert(QStringLiteral("de"), QStringLiteral("⚡ Intelligente Automatische Erkennung (Empfohlen)"));
      m.insert(QStringLiteral("tr"), QStringLiteral("⚡ Akıllı Otomatik Algılama (Önerilen)"));
      m.insert(QStringLiteral("ru"), QStringLiteral("⚡ Умное автоопределение (Рекомендуется)"));
      m.insert(QStringLiteral("id"), QStringLiteral("⚡ Deteksi Otomatis Cerdas (Disarankan)"));
      m.insert(QStringLiteral("fa"), QStringLiteral("⚡ تشخیص خودکار هوشمند (پیشنهادی)"));
      m.insert(QStringLiteral("ur"), QStringLiteral("⚡ خودکار اسمارٹ شناخت (تجویز کردہ)"));
    }
    { auto& m = m_translations[QStringLiteral("btn_check_mcu")];
      m.insert(QStringLiteral("ar"), QStringLiteral("🔍 فحص وكشف تلقائي للقطعة الآن (Auto-Detect & Test)"));
      m.insert(QStringLiteral("en"), QStringLiteral("🔍 Auto-Detect & Test Board Now"));
      m.insert(QStringLiteral("es"), QStringLiteral("🔍 Auto-detectar y probar placa ahora"));
      m.insert(QStringLiteral("fr"), QStringLiteral("🔍 Détecter et tester la carte maintenant"));
      m.insert(QStringLiteral("de"), QStringLiteral("🔍 Board jetzt automatisch erkennen & testen"));
      m.insert(QStringLiteral("tr"), QStringLiteral("🔍 Kartı Şimdi Otomatik Algıla ve Test Et"));
      m.insert(QStringLiteral("ru"), QStringLiteral("🔍 Автоопределение и тест платы сейчас"));
      m.insert(QStringLiteral("id"), QStringLiteral("🔍 Deteksi Otomatis & Uji Papan Sekarang"));
      m.insert(QStringLiteral("fa"), QStringLiteral("🔍 تشخیص خودکار و تست اتصال برد"));
      m.insert(QStringLiteral("ur"), QStringLiteral("🔍 بورڈ کی خودکار شناخت اور ٹیسٹ کریں"));
    }
    { auto& m = m_translations[QStringLiteral("btn_auto_install_pico")];
      m.insert(QStringLiteral("ar"), QStringLiteral("⚡ تثبيت تلقائي على فلاشة الـ Pico (CIRCUITPY)"));
      m.insert(QStringLiteral("en"), QStringLiteral("⚡ 1-Click Install to Pico Drive (CIRCUITPY)"));
      m.insert(QStringLiteral("es"), QStringLiteral("⚡ Instalar en unidad Pico (CIRCUITPY)"));
      m.insert(QStringLiteral("fr"), QStringLiteral("⚡ Installer sur le lecteur Pico (CIRCUITPY)"));
      m.insert(QStringLiteral("de"), QStringLiteral("⚡ 1-Klick-Installation auf Pico-Laufwerk (CIRCUITPY)"));
      m.insert(QStringLiteral("tr"), QStringLiteral("⚡ Pico Sürücüsüne (CIRCUITPY) 1-Tıkla Kur"));
      m.insert(QStringLiteral("ru"), QStringLiteral("⚡ Автоустановка на диск Pico (CIRCUITPY)"));
      m.insert(QStringLiteral("id"), QStringLiteral("⚡ Instal Otomatis ke Drive Pico (CIRCUITPY)"));
      m.insert(QStringLiteral("fa"), QStringLiteral("⚡ نصب خودکار روی درایو Pico (CIRCUITPY)"));
      m.insert(QStringLiteral("ur"), QStringLiteral("⚡ Pico ڈرائیو (CIRCUITPY) پر خودکار انسٹال کریں"));
    }
    { auto& m = m_translations[QStringLiteral("btn_extract_mcu_firmware")];
      m.insert(QStringLiteral("ar"), QStringLiteral("📂 استخراج السوفتوير الجاهز (Pico CircuitPython + C++ + Arduino)"));
      m.insert(QStringLiteral("en"), QStringLiteral("📂 Extract Ready Firmwares (Pico CircuitPython + C++ + Arduino)"));
      m.insert(QStringLiteral("es"), QStringLiteral("📂 Extraer firmwares listos (Pico CircuitPython + C++ + Arduino)"));
      m.insert(QStringLiteral("fr"), QStringLiteral("📂 Extraire les firmwares prêts (Pico CircuitPython + C++ + Arduino)"));
      m.insert(QStringLiteral("de"), QStringLiteral("📂 Fertige Firmwares extrahieren (Pico CircuitPython + C++ + Arduino)"));
      m.insert(QStringLiteral("tr"), QStringLiteral("📂 Hazır Yazılımları Çıkar (Pico CircuitPython + C++ + Arduino)"));
      m.insert(QStringLiteral("ru"), QStringLiteral("📂 Извлечь готовые прошивки (Pico CircuitPython + C++ + Arduino)"));
      m.insert(QStringLiteral("id"), QStringLiteral("📂 Ekstrak Firmware Siap Pakai (Pico CircuitPython + C++ + Arduino)"));
      m.insert(QStringLiteral("fa"), QStringLiteral("📂 استخراج فایل‌های فیرم‌ور آماده (Pico + Arduino)"));
      m.insert(QStringLiteral("ur"), QStringLiteral("📂 تیار فرم ویئر فائلز نکالیں (Pico + Arduino)"));
    }
    { auto& m = m_translations[QStringLiteral("eng_status_pico_active")];
      m.insert(QStringLiteral("ar"), QStringLiteral("بوردة Raspberry Pi Pico USB HID متصلة ونشطة فعلياً"));
      m.insert(QStringLiteral("en"), QStringLiteral("Raspberry Pi Pico USB HID Board Active & Connected"));
      m.insert(QStringLiteral("es"), QStringLiteral("Placa Raspberry Pi Pico USB HID activa y conectada"));
      m.insert(QStringLiteral("fr"), QStringLiteral("Carte Raspberry Pi Pico USB HID active et connectée"));
      m.insert(QStringLiteral("de"), QStringLiteral("Raspberry Pi Pico USB-HID-Board aktiv & verbunden"));
      m.insert(QStringLiteral("tr"), QStringLiteral("Raspberry Pi Pico USB HID Kartı Aktif ve Bağlı"));
      m.insert(QStringLiteral("ru"), QStringLiteral("Плата Raspberry Pi Pico USB HID активна и подключена"));
      m.insert(QStringLiteral("id"), QStringLiteral("Papan Raspberry Pi Pico USB HID Aktif & Terhubung"));
      m.insert(QStringLiteral("fa"), QStringLiteral("برد Raspberry Pi Pico USB HID متصل و فعال است"));
      m.insert(QStringLiteral("ur"), QStringLiteral("Raspberry Pi Pico USB HID بورڈ فعال اور جڑا ہوا ہے"));
    }
    { auto& m = m_translations[QStringLiteral("eng_status_pico_fallback")];
      m.insert(QStringLiteral("ar"), QStringLiteral("تم اختيار الطريقة 21 (Raspberry Pi Pico) ولكن القطعة غير متصلة حالياً — يعمل البديل الآمن (SendInput ScanCode) لحين توصيلها"));
      m.insert(QStringLiteral("en"), QStringLiteral("Method 21 (Raspberry Pi Pico) selected, board not plugged in yet — using safe SendInput ScanCode fallback until connected"));
      m.insert(QStringLiteral("es"), QStringLiteral("Método 21 (Raspberry Pi Pico) seleccionado, placa no conectada — usando SendInput ScanCode temporalmente"));
      m.insert(QStringLiteral("fr"), QStringLiteral("Méthode 21 (Raspberry Pi Pico) sélectionnée, carte non branchée — repli sécurisé sur SendInput ScanCode"));
      m.insert(QStringLiteral("de"), QStringLiteral("Methode 21 (Raspberry Pi Pico) gewählt, Board noch nicht verbunden — sicherer SendInput-Fallback aktiv"));
      m.insert(QStringLiteral("tr"), QStringLiteral("Yöntem 21 (Raspberry Pi Pico) seçildi, kart henüz takılı değil — güvenli SendInput ScanCode yedeği aktif"));
      m.insert(QStringLiteral("ru"), QStringLiteral("Выбран Метод 21 (Raspberry Pi Pico), плата не подключена — временно используется SendInput ScanCode"));
      m.insert(QStringLiteral("id"), QStringLiteral("Metode 21 (Raspberry Pi Pico) dipilih, papan belum terhubung — menggunakan fallback SendInput ScanCode"));
      m.insert(QStringLiteral("fa"), QStringLiteral("روش ۲۱ (Raspberry Pi Pico) انتخاب شده اما برد متصل نیست — استفاده موقت از جایگزین امن SendInput"));
      m.insert(QStringLiteral("ur"), QStringLiteral("طریقہ 21 (Raspberry Pi Pico) منتخب ہے لیکن بورڈ منسلک نہیں — عارضی طور پر محفوظ SendInput استعمال ہو رہا ہے"));
    }
    { auto& m = m_translations[QStringLiteral("eng_status_arduino_active")];
      m.insert(QStringLiteral("ar"), QStringLiteral("بوردة Arduino Leonardo / Pro Micro USB HID متصلة ونشطة فعلياً"));
      m.insert(QStringLiteral("en"), QStringLiteral("Arduino Leonardo / Pro Micro USB HID Board Active & Connected"));
      m.insert(QStringLiteral("es"), QStringLiteral("Placa Arduino Leonardo / Pro Micro USB HID activa y conectada"));
      m.insert(QStringLiteral("fr"), QStringLiteral("Carte Arduino Leonardo / Pro Micro USB HID active et connectée"));
      m.insert(QStringLiteral("de"), QStringLiteral("Arduino Leonardo / Pro Micro USB-HID-Board aktiv & verbunden"));
      m.insert(QStringLiteral("tr"), QStringLiteral("Arduino Leonardo / Pro Micro USB HID Kartı Aktif ve Bağlı"));
      m.insert(QStringLiteral("ru"), QStringLiteral("Плата Arduino Leonardo / Pro Micro USB HID активна и подключена"));
      m.insert(QStringLiteral("id"), QStringLiteral("Papan Arduino Leonardo / Pro Micro USB HID Aktif & Terhubung"));
      m.insert(QStringLiteral("fa"), QStringLiteral("برد Arduino Leonardo / Pro Micro USB HID متصل و فعال است"));
      m.insert(QStringLiteral("ur"), QStringLiteral("Arduino Leonardo / Pro Micro USB HID بورڈ فعال اور جڑا ہوا ہے"));
    }
    { auto& m = m_translations[QStringLiteral("eng_status_arduino_fallback")];
      m.insert(QStringLiteral("ar"), QStringLiteral("تم اختيار الطريقة 22 (Arduino HID) ولكن القطعة غير متصلة حالياً — يعمل البديل الآمن (SendInput ScanCode) لحين توصيلها"));
      m.insert(QStringLiteral("en"), QStringLiteral("Method 22 (Arduino HID) selected, board not plugged in yet — using safe SendInput ScanCode fallback until connected"));
      m.insert(QStringLiteral("es"), QStringLiteral("Método 22 (Arduino HID) seleccionado, placa no conectada — usando SendInput ScanCode temporalmente"));
      m.insert(QStringLiteral("fr"), QStringLiteral("Méthode 22 (Arduino HID) sélectionnée, carte non branchée — repli sécurisé sur SendInput ScanCode"));
      m.insert(QStringLiteral("de"), QStringLiteral("Methode 22 (Arduino HID) gewählt, Board noch nicht verbunden — sicherer SendInput-Fallback aktiv"));
      m.insert(QStringLiteral("tr"), QStringLiteral("Yöntem 22 (Arduino HID) seçildi, kart henüz takılı değil — güvenli SendInput ScanCode yedeği aktif"));
      m.insert(QStringLiteral("ru"), QStringLiteral("Выбран Метод 22 (Arduino HID), плата не подключена — временно используется SendInput ScanCode"));
      m.insert(QStringLiteral("id"), QStringLiteral("Metode 22 (Arduino HID) dipilih, papan belum terhubung — menggunakan fallback SendInput ScanCode"));
      m.insert(QStringLiteral("fa"), QStringLiteral("روش ۲۲ (Arduino HID) انتخاب شده اما برد متصل نیست — استفاده موقت از جایگزین امن SendInput"));
      m.insert(QStringLiteral("ur"), QStringLiteral("طریقہ 22 (Arduino HID) منتخب ہے لیکن بورڈ منسلک نہیں — عارضی طور پر محفوظ SendInput استعمال ہو رہا ہے"));
    }
    { auto& m = m_translations[QStringLiteral("log_reason_pico_missing")];
      m.insert(QStringLiteral("ar"), QStringLiteral("متوقف — بوردة Raspberry Pi Pico (الطريقة 21) غير متصلة بمنفذ USB/COM حالياً"));
      m.insert(QStringLiteral("en"), QStringLiteral("Skipped — Raspberry Pi Pico USB HID board (Method 21) is not connected on USB/COM port"));
      m.insert(QStringLiteral("es"), QStringLiteral("Omitido — La placa Raspberry Pi Pico USB HID (Método 21) no está conectada por USB/COM"));
      m.insert(QStringLiteral("fr"), QStringLiteral("Ignoré — Carte Raspberry Pi Pico USB HID (Méthode 21) non connectée sur USB/COM"));
      m.insert(QStringLiteral("de"), QStringLiteral("Übersprungen — Raspberry Pi Pico USB-HID-Board (Methode 21) ist nicht über USB/COM verbunden"));
      m.insert(QStringLiteral("tr"), QStringLiteral("Atlandı — Raspberry Pi Pico USB HID kartı (Yöntem 21) USB/COM portuna bağlı değil"));
      m.insert(QStringLiteral("ru"), QStringLiteral("Пропущено — Плата Raspberry Pi Pico USB HID (Метод 21) не подключена к USB/COM"));
      m.insert(QStringLiteral("id"), QStringLiteral("Dilewati — Papan Raspberry Pi Pico USB HID (Metode 21) tidak terhubung di USB/COM"));
      m.insert(QStringLiteral("fa"), QStringLiteral("رد شد — برد Raspberry Pi Pico (روش ۲۱) به درگاه USB/COM متصل نیست"));
      m.insert(QStringLiteral("ur"), QStringLiteral("اسکپ — Raspberry Pi Pico بورڈ (طریقہ 21) اس وقت USB/COM پورٹ سے منسلک نہیں ہے"));
    }
    { auto& m = m_translations[QStringLiteral("log_reason_arduino_missing")];
      m.insert(QStringLiteral("ar"), QStringLiteral("متوقف — بوردة Arduino Leonardo/Pro Micro (الطريقة 22) غير متصلة بمنفذ USB/COM حالياً"));
      m.insert(QStringLiteral("en"), QStringLiteral("Skipped — Arduino Leonardo/Pro Micro USB HID board (Method 22) is not connected on USB/COM port"));
      m.insert(QStringLiteral("es"), QStringLiteral("Omitido — La placa Arduino Leonardo/Pro Micro USB HID (Método 22) no está conectada por USB/COM"));
      m.insert(QStringLiteral("fr"), QStringLiteral("Ignoré — Carte Arduino Leonardo/Pro Micro USB HID (Méthode 22) non connectée sur USB/COM"));
      m.insert(QStringLiteral("de"), QStringLiteral("Übersprungen — Arduino Leonardo/Pro Micro USB-HID-Board (Methode 22) ist nicht über USB/COM verbunden"));
      m.insert(QStringLiteral("tr"), QStringLiteral("Atlandı — Arduino Leonardo/Pro Micro USB HID kartı (Yöntem 22) USB/COM portuna bağlı değil"));
      m.insert(QStringLiteral("ru"), QStringLiteral("Пропущено — Плата Arduino Leonardo/Pro Micro USB HID (Метод 22) не подключена к USB/COM"));
      m.insert(QStringLiteral("id"), QStringLiteral("Dilewati — Papan Arduino Leonardo/Pro Micro USB HID (Metode 22) tidak terhubung di USB/COM"));
      m.insert(QStringLiteral("fa"), QStringLiteral("رد شد — برد Arduino Leonardo/Pro Micro (روش ۲۲) به درگاه USB/COM متصل نیست"));
      m.insert(QStringLiteral("ur"), QStringLiteral("اسکپ — Arduino Leonardo/Pro Micro بورڈ (طریقہ 22) اس وقت USB/COM پورٹ سے منسلک نہیں ہے"));
    }
    { auto& m = m_translations[QStringLiteral("lbl_show_floating_pill")];
      m.insert(QStringLiteral("ar"), QStringLiteral("إظهار نافذة إيقاف التشغيل العائمة"));
      m.insert(QStringLiteral("en"), QStringLiteral("Show floating stop pill during playback"));
      m.insert(QStringLiteral("es"), QStringLiteral("Mostrar píldora flotante al reproducir"));
      m.insert(QStringLiteral("fr"), QStringLiteral("Afficher la pilule flottante en lecture"));
      m.insert(QStringLiteral("de"), QStringLiteral("Schwebendes Stopp-Fenster anzeigen"));
      m.insert(QStringLiteral("tr"), QStringLiteral("Oynatmada yüzen durdurma penceresini göster"));
      m.insert(QStringLiteral("ru"), QStringLiteral("Показывать плавающую кнопку стоп"));
      m.insert(QStringLiteral("id"), QStringLiteral("Tampilkan tombol melayang saat memutar"));
      m.insert(QStringLiteral("fa"), QStringLiteral("نمایش دکمه شناور توقف هنگام پخش"));
      m.insert(QStringLiteral("ur"), QStringLiteral("پلے بیک کے دوران فلوٹنگ اسٹاپ بٹن دکھائیں"));
    }
    { auto& m = m_translations[QStringLiteral("lbl_playback_start_delay")];
      m.insert(QStringLiteral("ar"), QStringLiteral("بدء التشغيل بعد:"));
      m.insert(QStringLiteral("en"), QStringLiteral("Start playback after:"));
      m.insert(QStringLiteral("es"), QStringLiteral("Iniciar reproducción después de:"));
      m.insert(QStringLiteral("fr"), QStringLiteral("Démarrer après :"));
      m.insert(QStringLiteral("de"), QStringLiteral("Wiedergabe starten nach:"));
      m.insert(QStringLiteral("tr"), QStringLiteral("Oynatmayı başlatma gecikmesi:"));
      m.insert(QStringLiteral("ru"), QStringLiteral("Запуск воспроизведения через:"));
      m.insert(QStringLiteral("id"), QStringLiteral("Mulai pemutaran setelah:"));
      m.insert(QStringLiteral("fa"), QStringLiteral("شروع پخش بعد از:"));
      m.insert(QStringLiteral("ur"), QStringLiteral("پلے بیک شروع ہونے کا وقفہ:"));
    }
    { auto& m = m_translations[QStringLiteral("btn_edit_steps")];
      m.insert(QStringLiteral("ar"), QStringLiteral("تعديل الخطوات"));
      m.insert(QStringLiteral("en"), QStringLiteral("Edit Steps"));
      m.insert(QStringLiteral("es"), QStringLiteral("Editar pasos"));
      m.insert(QStringLiteral("fr"), QStringLiteral("Modifier les étapes"));
      m.insert(QStringLiteral("de"), QStringLiteral("Schritte bearbeiten"));
      m.insert(QStringLiteral("tr"), QStringLiteral("Adımları Düzenle"));
      m.insert(QStringLiteral("ru"), QStringLiteral("Редактировать шаги"));
      m.insert(QStringLiteral("id"), QStringLiteral("Edit Langkah"));
      m.insert(QStringLiteral("fa"), QStringLiteral("ویرایش مراحل"));
      m.insert(QStringLiteral("ur"), QStringLiteral("مراحل میں ترمیم کریں"));
    }
    { auto& m = m_translations[QStringLiteral("btn_hide_steps")];
      m.insert(QStringLiteral("ar"), QStringLiteral("إخفاء الخطوات"));
      m.insert(QStringLiteral("en"), QStringLiteral("Hide Steps"));
      m.insert(QStringLiteral("es"), QStringLiteral("Ocultar pasos"));
      m.insert(QStringLiteral("fr"), QStringLiteral("Masquer les étapes"));
      m.insert(QStringLiteral("de"), QStringLiteral("Schritte ausblenden"));
      m.insert(QStringLiteral("tr"), QStringLiteral("Adımları Gizle"));
      m.insert(QStringLiteral("ru"), QStringLiteral("Скрыть шаги"));
      m.insert(QStringLiteral("id"), QStringLiteral("Sembunyikan Langkah"));
      m.insert(QStringLiteral("fa"), QStringLiteral("مخفی کردن مراحل"));
      m.insert(QStringLiteral("ur"), QStringLiteral("مراحل چھپائیں"));
    }
    { auto& m = m_translations[QStringLiteral("btn_save_changes")];
      m.insert(QStringLiteral("ar"), QStringLiteral("حفظ التعديلات على الحالي"));
      m.insert(QStringLiteral("en"), QStringLiteral("Save Changes"));
      m.insert(QStringLiteral("es"), QStringLiteral("Guardar cambios"));
      m.insert(QStringLiteral("fr"), QStringLiteral("Enregistrer les modifications"));
      m.insert(QStringLiteral("de"), QStringLiteral("Änderungen speichern"));
      m.insert(QStringLiteral("tr"), QStringLiteral("Değişiklikleri Kaydet"));
      m.insert(QStringLiteral("ru"), QStringLiteral("Сохранить изменения"));
      m.insert(QStringLiteral("id"), QStringLiteral("Simpan Perubahan"));
      m.insert(QStringLiteral("fa"), QStringLiteral("ذخیره تغییرات"));
      m.insert(QStringLiteral("ur"), QStringLiteral("تبدیلیاں محفوظ کریں"));
    }
    { auto& m = m_translations[QStringLiteral("btn_save_as_new")];
      m.insert(QStringLiteral("ar"), QStringLiteral("حفظ كنسخة جديدة"));
      m.insert(QStringLiteral("en"), QStringLiteral("Save as New"));
      m.insert(QStringLiteral("es"), QStringLiteral("Guardar como nuevo"));
      m.insert(QStringLiteral("fr"), QStringLiteral("Enregistrer sous"));
      m.insert(QStringLiteral("de"), QStringLiteral("Als neu speichern"));
      m.insert(QStringLiteral("tr"), QStringLiteral("Yeni Olarak Kaydet"));
      m.insert(QStringLiteral("ru"), QStringLiteral("Сохранить как новый"));
      m.insert(QStringLiteral("id"), QStringLiteral("Simpan sebagai Baru"));
      m.insert(QStringLiteral("fa"), QStringLiteral("ذخیره به عنوان جدید"));
      m.insert(QStringLiteral("ur"), QStringLiteral("نئے کے طور پر محفوظ کریں"));
    }
    { auto& m = m_translations[QStringLiteral("btn_undo")];
      m.insert(QStringLiteral("ar"), QStringLiteral("تراجع"));
      m.insert(QStringLiteral("en"), QStringLiteral("Undo"));
      m.insert(QStringLiteral("es"), QStringLiteral("Deshacer"));
      m.insert(QStringLiteral("fr"), QStringLiteral("Annuler"));
      m.insert(QStringLiteral("de"), QStringLiteral("Rückgängig"));
      m.insert(QStringLiteral("tr"), QStringLiteral("Geri Al"));
      m.insert(QStringLiteral("ru"), QStringLiteral("Отменить"));
      m.insert(QStringLiteral("id"), QStringLiteral("Urungkan"));
      m.insert(QStringLiteral("fa"), QStringLiteral("بازگشت"));
      m.insert(QStringLiteral("ur"), QStringLiteral("واپس (Undo)"));
    }
    { auto& m = m_translations[QStringLiteral("btn_redo")];
      m.insert(QStringLiteral("ar"), QStringLiteral("إعادة"));
      m.insert(QStringLiteral("en"), QStringLiteral("Redo"));
      m.insert(QStringLiteral("es"), QStringLiteral("Rehacer"));
      m.insert(QStringLiteral("fr"), QStringLiteral("Rétablir"));
      m.insert(QStringLiteral("de"), QStringLiteral("Wiederholen"));
      m.insert(QStringLiteral("tr"), QStringLiteral("Yinele"));
      m.insert(QStringLiteral("ru"), QStringLiteral("Повторить"));
      m.insert(QStringLiteral("id"), QStringLiteral("Ulangi"));
      m.insert(QStringLiteral("fa"), QStringLiteral("تکرار"));
      m.insert(QStringLiteral("ur"), QStringLiteral("دوبارہ (Redo)"));
    }
    { auto& m = m_translations[QStringLiteral("lbl_steps_hidden_hint")];
      m.insert(QStringLiteral("ar"), QStringLiteral("الخطوات مخفية لتبسيط الواجهة — اضغط على «تعديل الخطوات» لعرضها أو تعديلها أو إعادة ترتيبها."));
      m.insert(QStringLiteral("en"), QStringLiteral("Steps are hidden for a cleaner view — click \"Edit Steps\" to view, edit, or reorder them."));
      m.insert(QStringLiteral("es"), QStringLiteral("Los pasos están ocultos — haz clic en \"Editar pasos\" para verlos o editarlos."));
      m.insert(QStringLiteral("fr"), QStringLiteral("Les étapes sont masquées — cliquez sur « Modifier les étapes » pour les afficher."));
      m.insert(QStringLiteral("de"), QStringLiteral("Schritte sind ausgeblendet — klicken Sie auf „Schritte bearbeiten“."));
      m.insert(QStringLiteral("tr"), QStringLiteral("Adımlar gizlendi — görüntülemek veya düzenlemek için \"Adımları Düzenle\"ye tıklayın."));
      m.insert(QStringLiteral("ru"), QStringLiteral("Шаги скрыты — нажмите «Редактировать шаги» для просмотра и правки."));
      m.insert(QStringLiteral("id"), QStringLiteral("Langkah disembunyikan — klik \"Edit Langkah\" untuk melihat atau mengedit."));
      m.insert(QStringLiteral("fa"), QStringLiteral("مراحل برای سادگی مخفی هستند — برای مشاهده یا ویرایش روی «ویرایش مراحل» کلیک کنید."));
      m.insert(QStringLiteral("ur"), QStringLiteral("آسانی کے لیے مراحل چھپائے گئے ہیں — دیکھنے یا ترمیم کے لیے «مراحل میں ترمیم کریں» پر کلک کریں۔"));
    }
    { auto& m = m_translations[QStringLiteral("toast_changes_saved")];
      m.insert(QStringLiteral("ar"), QStringLiteral("✓ تم حفظ التعديلات بنجاح"));
      m.insert(QStringLiteral("en"), QStringLiteral("✓ Changes saved successfully"));
      m.insert(QStringLiteral("es"), QStringLiteral("✓ Cambios guardados con éxito"));
      m.insert(QStringLiteral("fr"), QStringLiteral("✓ Modifications enregistrées"));
      m.insert(QStringLiteral("de"), QStringLiteral("✓ Änderungen erfolgreich gespeichert"));
      m.insert(QStringLiteral("tr"), QStringLiteral("✓ Değişiklikler başarıyla kaydedildi"));
      m.insert(QStringLiteral("ru"), QStringLiteral("✓ Изменения успешно сохранены"));
      m.insert(QStringLiteral("id"), QStringLiteral("✓ Perubahan berhasil disimpan"));
      m.insert(QStringLiteral("fa"), QStringLiteral("✓ تغییرات با موفقیت ذخیره شد"));
      m.insert(QStringLiteral("ur"), QStringLiteral("✓ تبدیلیاں کامیابی سے محفوظ ہو گئیں"));
    }
}

void I18n::setLanguage(const QString& code) {
    m_lang = code.isEmpty() ? QStringLiteral("ar") : code;
}

bool I18n::currentRtl() const {
    return (m_lang == QStringLiteral("ar") || m_lang == QStringLiteral("fa") || m_lang == QStringLiteral("ur"));
}

QString I18n::fontFamily() const {
    if (m_lang == QStringLiteral("ar")) return QStringLiteral("Tajawal");
    if (m_lang == QStringLiteral("fa") || m_lang == QStringLiteral("ur")) return QStringLiteral("Tahoma");
    return QStringLiteral("Segoe UI");
}

QString I18n::translate(const QString& key) const {
    static const QMap<QString, QString> kAliases = {
        {QStringLiteral("app_all"), QStringLiteral("all_apps")},
        {QStringLiteral("about_subtitle"), QStringLiteral("app_Tagline")},
        {QStringLiteral("about_description"), QStringLiteral("about_desc_body")},
        {QStringLiteral("developer"), QStringLiteral("about_dev_header")},
        {QStringLiteral("ai_partner"), QStringLiteral("about_assisted_by")},
        {QStringLiteral("close"), QStringLiteral("btn_close")},
        {QStringLiteral("clear"), QStringLiteral("btn_clear_note")},
        {QStringLiteral("clear_list"), QStringLiteral("btn_clear_events")},
        {QStringLiteral("delete"), QStringLiteral("btn_delete")},
        {QStringLiteral("edit"), QStringLiteral("btn_load_to_editor")},
        {QStringLiteral("run"), QStringLiteral("btn_run_saved")},
        {QStringLiteral("save"), QStringLiteral("btn_save_note")},
        {QStringLiteral("undo"), QStringLiteral("btn_undo")},
        {QStringLiteral("redo"), QStringLiteral("btn_redo")},
        {QStringLiteral("save_changes"), QStringLiteral("btn_save_changes")},
        {QStringLiteral("save_as_new"), QStringLiteral("btn_save_as_new")},
        {QStringLiteral("edit_steps_btn"), QStringLiteral("btn_edit_steps")},
        {QStringLiteral("hide_steps_btn"), QStringLiteral("btn_hide_steps")},
        {QStringLiteral("steps_hidden_msg"), QStringLiteral("lbl_steps_hidden_hint")},
        {QStringLiteral("toggle_sidebar"), QStringLiteral("btn_hide_macros_sidebar")},
        {QStringLiteral("new_combo"), QStringLiteral("btn_new_smart_key")},
        {QStringLiteral("master_active"), QStringLiteral("status_combos_active")},
        {QStringLiteral("master_paused"), QStringLiteral("status_idle")},
        {QStringLiteral("combo_name_ph"), QStringLiteral("ph_macro_title_opt")},
        {QStringLiteral("trigger_key"), QStringLiteral("lbl_combo_mode")},
        {QStringLiteral("start_delay_lbl"), QStringLiteral("lbl_run_after")},
        {QStringLiteral("target_proc_ph"), QStringLiteral("all_apps")},
        {QStringLiteral("stop_hotkey_mandatory"), QStringLiteral("lbl_combo_stop_hotkey")},
        {QStringLiteral("add_key_tap"), QStringLiteral("step_add_key_tap")},
        {QStringLiteral("add_key_hold"), QStringLiteral("step_add_key_hold")},
        {QStringLiteral("add_wait"), QStringLiteral("step_add_wait")},
        {QStringLiteral("add_mouse_click"), QStringLiteral("step_add_mouse_click")},
        {QStringLiteral("add_type_text"), QStringLiteral("step_add_type_text")},
        {QStringLiteral("add_open_folder_app"), QStringLiteral("step_add_folder")},
        {QStringLiteral("add_cmd"), QStringLiteral("step_add_cmd")},
        {QStringLiteral("add_python_script"), QStringLiteral("step_add_python_script")},
        {QStringLiteral("pick_program_or_folder"), QStringLiteral("btn_browse_app_file")},
        {QStringLiteral("search_combos_ph"), QStringLiteral("search_placeholder")},
        {QStringLiteral("edit_python_script"), QStringLiteral("step_add_python_script")},
        {QStringLiteral("load_preset"), QStringLiteral("py_preset_default_sweep")},
        {QStringLiteral("save_to_library"), QStringLiteral("btn_save_py_script")},
        {QStringLiteral("script_name_ph"), QStringLiteral("ph_py_script_name")},
        {QStringLiteral("toast_saved"), QStringLiteral("toast_changes_saved")},
        {QStringLiteral("start_recording"), QStringLiteral("btn_start_rec")},
        {QStringLiteral("stop_recording"), QStringLiteral("btn_stop_rec")},
        {QStringLiteral("play_macro"), QStringLiteral("btn_play_macro")},
        {QStringLiteral("stop_playback"), QStringLiteral("btn_stop_play")},
        {QStringLiteral("convert_to_smart"), QStringLiteral("btn_convert_to_combo")},
        {QStringLiteral("toast_converted_smart"), QStringLiteral("toast_changes_saved")},
        {QStringLiteral("rec_keyboard"), QStringLiteral("lbl_rec_keyboard")},
        {QStringLiteral("rec_clicks"), QStringLiteral("lbl_rec_clicks")},
        {QStringLiteral("rec_scroll"), QStringLiteral("lbl_rec_scroll")},
        {QStringLiteral("rec_moves"), QStringLiteral("lbl_rec_moves")},
        {QStringLiteral("repeat_once"), QStringLiteral("rec_repeat_mode_once")},
        {QStringLiteral("repeat_count"), QStringLiteral("rec_repeat_mode_count")},
        {QStringLiteral("repeat_duration"), QStringLiteral("rec_repeat_mode_duration")},
        {QStringLiteral("playback_start_delay"), QStringLiteral("lbl_playback_start_delay")},
        {QStringLiteral("show_floating_pill"), QStringLiteral("lbl_show_floating_pill")},
        {QStringLiteral("save_macro"), QStringLiteral("btn_save_macro")},
        {QStringLiteral("add_tab"), QStringLiteral("akp_btn_new_tab")},
        {QStringLiteral("ap_start_tab"), QStringLiteral("akp_btn_start_tab")},
        {QStringLiteral("ap_stop_tab"), QStringLiteral("akp_btn_stop_tab")},
        {QStringLiteral("target_none"), QStringLiteral("all_apps")},
        {QStringLiteral("refresh"), QStringLiteral("btn_reset_py_sweep")},
        {QStringLiteral("run_in_background"), QStringLiteral("akp_lbl_run_in_bg")},
        {QStringLiteral("target_window"), QStringLiteral("lbl_target_app")},
        {QStringLiteral("input_method"), QStringLiteral("lbl_input_method")},
        {QStringLiteral("hold_ms"), QStringLiteral("lbl_hold_duration")},
        {QStringLiteral("sweep_22_methods"), QStringLiteral("akp_btn_test_all_methods")},
        {QStringLiteral("sandbox_self_target"), QStringLiteral("test_target_app_sandbox_free")},
        {QStringLiteral("test_mode_python_22"), QStringLiteral("test_mode_sweep_20")},
        {QStringLiteral("test_mode_single"), QStringLiteral("test_mode_macro")},
        {QStringLiteral("test_mode_macro_22"), QStringLiteral("test_mode_sweep_20_macro")},
        {QStringLiteral("run_test_btn"), QStringLiteral("btn_run_test_now")},
        {QStringLiteral("stop_test_btn"), QStringLiteral("btn_panic_stop")},
        {QStringLiteral("sandbox_input_ph"), QStringLiteral("sandbox_input_placeholder")},
        {QStringLiteral("settings_appearance_title"), QStringLiteral("themes_title")},
        {QStringLiteral("language_label"), QStringLiteral("lbl_language")},
        {QStringLiteral("theme_dark"), QStringLiteral("mode_dark")},
        {QStringLiteral("theme_light"), QStringLiteral("mode_light")},
        {QStringLiteral("custom_accent_color"), QStringLiteral("btn_custom_color")},
        {QStringLiteral("settings_hotkeys_title"), QStringLiteral("settings_akp_hotkeys_title")},
        {QStringLiteral("hk_record_toggle"), QStringLiteral("lbl_hk_rec")},
        {QStringLiteral("hk_playback_toggle"), QStringLiteral("lbl_hk_play")},
        {QStringLiteral("hk_combo_master"), QStringLiteral("lbl_hk_combo")},
        {QStringLiteral("hk_panic_stop"), QStringLiteral("lbl_hk_panic_all")},
        {QStringLiteral("hk_ap_stop_all"), QStringLiteral("akp_lbl_stop_all_hotkey")},
        {QStringLiteral("suppress_trigger_key"), QStringLiteral("lbl_suppress_key")},
        {QStringLiteral("minimize_to_tray"), QStringLiteral("lbl_min_to_tray")},
        {QStringLiteral("always_on_top"), QStringLiteral("lbl_always_on_top")},
        {QStringLiteral("run_on_startup"), QStringLiteral("lbl_run_on_startup")},
        {QStringLiteral("mcu_card_title"), QStringLiteral("lbl_mcu_diag_title")},
        {QStringLiteral("mcu_scan_test_btn"), QStringLiteral("btn_check_mcu")},
        {QStringLiteral("mcu_install_pico_btn"), QStringLiteral("btn_auto_install_pico")},
        {QStringLiteral("mcu_extract_fw_btn"), QStringLiteral("btn_extract_mcu_firmware")},
        {QStringLiteral("mcu_status_idle"), QStringLiteral("eng_status_pico_fallback")},
        {QStringLiteral("mcu_scanning"), QStringLiteral("btn_check_mcu")},
        {QStringLiteral("settings_drivers_title"), QStringLiteral("lbl_kernel_diag_title")},
        {QStringLiteral("diagnose_btn"), QStringLiteral("btn_check_interception")},
        {QStringLiteral("import_driver_btn"), QStringLiteral("btn_install_interception")},
        {QStringLiteral("diagnose_ps2_btn"), QStringLiteral("btn_check_ps2")},
        {QStringLiteral("import_ps2_btn"), QStringLiteral("btn_install_ps2")},
        {QStringLiteral("backup_title"), QStringLiteral("lbl_backup_section")},
        {QStringLiteral("export_all"), QStringLiteral("btn_export_all")},
        {QStringLiteral("import_backup"), QStringLiteral("btn_import_all")},
        {QStringLiteral("step_disable_tip"), QStringLiteral("tip_step_enable_toggle")},
        {QStringLiteral("step_enable_tip"), QStringLiteral("tip_step_enable_toggle")},
        {QStringLiteral("step_note_title"), QStringLiteral("lbl_step_note_title")},
        {QStringLiteral("step_note_hint"), QStringLiteral("ph_step_note_input")},
        {QStringLiteral("welcome_title"), QStringLiteral("onb_title")},
        {QStringLiteral("welcome_desc"), QStringLiteral("onb_step_summary")},
        {QStringLiteral("get_started"), QStringLiteral("btn_finish")},
        {QStringLiteral("stop"), QStringLiteral("btn_stop_play")},
        {QStringLiteral("pill_recording"), QStringLiteral("status_recording")},
        {QStringLiteral("pill_playing"), QStringLiteral("status_playing")},
        {QStringLiteral("tray_stop_all"), QStringLiteral("btn_panic_stop")}
    };
    const QString lookupKey = kAliases.value(key, key);
    auto keyIt = m_translations.constFind(lookupKey);
    if (keyIt != m_translations.constEnd()) {
        auto langIt = keyIt->constFind(m_lang);
        if (langIt != keyIt->constEnd() && !langIt->isEmpty()) return *langIt;
        auto enIt = keyIt->constFind(QStringLiteral("en"));
        if (enIt != keyIt->constEnd() && !enIt->isEmpty()) return *enIt;
        auto arIt = keyIt->constFind(QStringLiteral("ar"));
        if (arIt != keyIt->constEnd() && !arIt->isEmpty()) return *arIt;
    }
    return key;
}

QString I18n::translateDual(const QString& arText, const QString& enText) const {
    return currentRtl() ? arText : enText;
}

QString I18n::formatNumberStr(const QString& raw) const {
    if (m_lang == QStringLiteral("ar") || m_lang == QStringLiteral("ur")) {
        static const QString kArDigits = QString::fromUtf8("٠١٢٣٤٥٦٧٨٩");
        QString out = raw;
        for (int i = 0; i < 10; ++i) {
            out.replace(QChar('0' + i), kArDigits[i]);
        }
        return out;
    }
    if (m_lang == QStringLiteral("fa")) {
        static const QString kFaDigits = QString::fromUtf8("۰۱۲۳۴۵۶۷۸۹");
        QString out = raw;
        for (int i = 0; i < 10; ++i) {
            out.replace(QChar('0' + i), kFaDigits[i]);
        }
        return out;
    }
    return raw;
}

QString I18n::formatNumberInt(int val) const {
    return formatNumberStr(QString::number(val));
}

QString I18n::formatNumberDouble(double val, int decimals) const {
    return formatNumberStr(QString::number(val, 'f', decimals));
}

} // namespace vmacro
