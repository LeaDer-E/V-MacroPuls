[🌐 جميع اللغات](../README.md) | [🏠 الرئيسية](01-Home-and-Getting-Started.md) | ← [معمل التجربة و22 محرك إدخال](05-Test-Lab-and-22-Input-Methods.md) | [درايفرات الكيرنل وقطع الهاردوير (Pico و Arduino)](07-Hardware-Drivers-and-Microcontrollers.md) →

# 🐍 أكواد البايثون المصغّرة والمحاكاة البشرية (Python Mini-Scripting)

يدعم **V-MacroPlus** كتابة وتشغيل سكريبتات بايثون مصغّرة داخل **خطوة سكريبت بايثون في الأزرار الذكية** وداخل **وضع الفحص الرابع في معمل التجربة**، وذلك عبر مفسّر مدمج سريع ومكتبة قوالب مشتركة (`python_scripts.json`).

## القوالب الستة الجاهزة وحفظ السكريبتات
يأتي البرنامج بـ **6 قوالب جاهزة** يمكنك اختيارها فوراً من القائمة:
1. فحص الشات داخل الألعاب (`Enter` -> كتابة رقم واسم المحرك -> `Enter`).
2. حركة ألعاب 3D بإيقاع بشري (`WASD` + تبديل أسلحة `1/2/3` + قفز `Space`).
3. تصويب ونقر داخل ألعاب 3D باستخدام الحركة النسبية (`move_rel` و`in_game_3d=True`).
4. كتابة نصوص بإيقاع بشري طبيعي (`human=True`).
5. كومبو مهارات عشوائي بإيقاع بشري (`1 -> 2 -> 3 -> Tab`).
6. سكريبت الفحص القياسي لجميع المحركات.

كما يمكنك كتابة اسم للسكريبت والضغط على **+ حفظ السكريبت** ليصبح متاحاً فوراً في كلٍ من "الأزرار الذكية" و"معمل التجربة".

## مرجع الدوال المتاحة داخل السكريبت
| الدالة | الوظيفة |
| :--- | :--- |
| `press(key, hold_ms=65, human=False)` | يضغط الزر ويمسكه لمدة `hold_ms` (افتراضياً 65ms لتلتقطه ألعاب 60 FPS). تفعيل `human=True` يضيف عشوائية بشرية. |
| `human_press(key, min_hold=58, max_hold=95, min_wait=65, max_wait=130)` | ضغطة بمدة مسك عشوائية يتبعها انتظار عشوائي بشري. |
| `hold_key(key, duration_ms=300)` | يمسك زر الحركة (مثل `W` أو `Shift`) بشكل متواصل لمدة محددة. |
| `rand(min_val=60, max_val=110)` | يولّد رقماً عشوائياً بين الحد الأدنى والأقصى. |
| `wait(ms)` / `human_wait(min_ms, max_ms)` | انتظار ثابت أو عشوائي بإيقاع بشري طبيعي. |
| `print(text, enter=False, human=False)` | يكتب النص بالمحرك النشط، ومع `human=True` يكتب حرفاً بحرف بسرعة بشرية. |
| `type_text(text, enter=False, human=False)` | مطابقة لـ `print` مع دعم متغيرات `{method_num}` و`{method_id}`. |
| `click(button="left", x=None, y=None, hold_ms=68, human=False, in_game_3d=False)` | نقر أي من أزرار الماوس الخمسة. استخدم `in_game_3d=True` للنقر في منتصف التصويب داخل ألعاب 3D بدون تحريك المؤشر المطلق. |
| `move_rel(dx, dy, steps=6, duration_ms=60)` | تحريك كاميرا ألعاب 3D بدلتا نسبية ناعمة (`MOUSEEVENTF_MOVE`). |

## مثال عملي
```python
# مثال: كومبو حركة وتصويب بشري داخل لعبة 3D
for i in range(3):
    human_press("1", min_hold=65, max_hold=95, min_wait=80, max_wait=130)
    hold_key("W", duration_ms=rand(260, 380))
    move_rel(120, 0, steps=8, duration_ms=80)
    click("left", hold_ms=rand(68, 98), human=True, in_game_3d=True)
    human_wait(150, 250)
```

---

[🌐 جميع اللغات](../README.md) | [🏠 الرئيسية](01-Home-and-Getting-Started.md) | ← [معمل التجربة و22 محرك إدخال](05-Test-Lab-and-22-Input-Methods.md) | [درايفرات الكيرنل وقطع الهاردوير (Pico و Arduino)](07-Hardware-Drivers-and-Microcontrollers.md) →
