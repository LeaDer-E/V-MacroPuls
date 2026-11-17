/*
 * V-MacroPlus Hardware USB HID Bridge — Raspberry Pi Pico / Pico 2 (Arduino IDE C++ Sketch)
 * Board Core: Earle F. Philhower raspberrypi / Arduino Mbed OS RP2040
 * USB Stack: "Adafruit TinyUSB" or "Pico SDK USB (Keyboard + Mouse + CDC Serial)"
 * Baud Rate: 115200
 */

#include <Keyboard.h>
#include <Mouse.h>

static String inputBuffer = "";

uint8_t mapHidUsageToArduinoKey(uint8_t usage) {
  // Printable ASCII & Special Keys mapping for Arduino Keyboard.h
  if (usage >= 0x04 && usage <= 0x1D) return 'a' + (usage - 0x04);
  if (usage >= 0x1E && usage <= 0x26) return '1' + (usage - 0x1E);
  if (usage == 0x27) return '0';
  switch (usage) {
    case 0x28: return KEY_RETURN;
    case 0x29: return KEY_ESC;
    case 0x2A: return KEY_BACKSPACE;
    case 0x2B: return KEY_TAB;
    case 0x2C: return ' ';
    case 0x2D: return '-';
    case 0x2E: return '=';
    case 0x2F: return '[';
    case 0x30: return ']';
    case 0x31: return '\';
    case 0x33: return ';';
    case 0x34: return ''';
    case 0x35: return '`';
    case 0x36: return ',';
    case 0x37: return '.';
    case 0x38: return '/';
    case 0x39: return KEY_CAPS_LOCK;
    case 0x3A: return KEY_F1;
    case 0x3B: return KEY_F2;
    case 0x3C: return KEY_F3;
    case 0x3D: return KEY_F4;
    case 0x3E: return KEY_F5;
    case 0x3F: return KEY_F6;
    case 0x40: return KEY_F7;
    case 0x41: return KEY_F8;
    case 0x42: return KEY_F9;
    case 0x43: return KEY_F10;
    case 0x44: return KEY_F11;
    case 0x45: return KEY_F12;
    case 0x49: return KEY_INSERT;
    case 0x4A: return KEY_HOME;
    case 0x4B: return KEY_PAGE_UP;
    case 0x4C: return KEY_DELETE;
    case 0x4D: return KEY_END;
    case 0x4E: return KEY_PAGE_DOWN;
    case 0x4F: return KEY_RIGHT_ARROW;
    case 0x50: return KEY_LEFT_ARROW;
    case 0x51: return KEY_DOWN_ARROW;
    case 0x52: return KEY_UP_ARROW;
    case 0xE0: return KEY_LEFT_CTRL;
    case 0xE1: return KEY_LEFT_SHIFT;
    case 0xE2: return KEY_LEFT_ALT;
    case 0xE3: return KEY_LEFT_GUI;
    case 0xE4: return KEY_RIGHT_CTRL;
    case 0xE5: return KEY_RIGHT_SHIFT;
    case 0xE6: return KEY_RIGHT_ALT;
    case 0xE7: return KEY_RIGHT_GUI;
    default: return 0;
  }
}

uint8_t parseMouseButton(const String &btn) {
  if (btn == "right") return MOUSE_RIGHT;
  if (btn == "middle") return MOUSE_MIDDLE;
  return MOUSE_LEFT;
}

void applyModifiers(int modMask, bool down) {
  if (modMask & 0x01) { if (down) Keyboard.press(KEY_LEFT_CTRL); else Keyboard.release(KEY_LEFT_CTRL); }
  if (modMask & 0x02) { if (down) Keyboard.press(KEY_LEFT_SHIFT); else Keyboard.release(KEY_LEFT_SHIFT); }
  if (modMask & 0x04) { if (down) Keyboard.press(KEY_LEFT_ALT); else Keyboard.release(KEY_LEFT_ALT); }
  if (modMask & 0x08) { if (down) Keyboard.press(KEY_LEFT_GUI); else Keyboard.release(KEY_LEFT_GUI); }
}

void processCommand(String cmd) {
  cmd.trim();
  if (cmd.length() == 0) return;

  if (cmd == "PING") {
    Serial.println("PONG:PICO_CPP_HID");
    return;
  }
  if (cmd == "REL_ALL") {
    Keyboard.releaseAll();
    Mouse.release(MOUSE_LEFT | MOUSE_RIGHT | MOUSE_MIDDLE);
    Serial.println("OK");
    return;
  }
  if (cmd.startsWith("KD:")) {
    int c1 = cmd.indexOf(',', 3);
    int usage = (c1 > 0) ? cmd.substring(3, c1).toInt() : cmd.substring(3).toInt();
    int mod = (c1 > 0) ? cmd.substring(c1 + 1).toInt() : 0;
    applyModifiers(mod, true);
    uint8_t k = mapHidUsageToArduinoKey((uint8_t)usage);
    if (k) Keyboard.press(k);
    Serial.println("OK");
    return;
  }
  if (cmd.startsWith("KU:")) {
    int c1 = cmd.indexOf(',', 3);
    int usage = (c1 > 0) ? cmd.substring(3, c1).toInt() : cmd.substring(3).toInt();
    int mod = (c1 > 0) ? cmd.substring(c1 + 1).toInt() : 0;
    uint8_t k = mapHidUsageToArduinoKey((uint8_t)usage);
    if (k) Keyboard.release(k);
    applyModifiers(mod, false);
    Serial.println("OK");
    return;
  }
  if (cmd.startsWith("KT:")) {
    int c1 = cmd.indexOf(',', 3);
    int c2 = (c1 > 0) ? cmd.indexOf(',', c1 + 1) : -1;
    int usage = (c1 > 0) ? cmd.substring(3, c1).toInt() : cmd.substring(3).toInt();
    int holdMs = (c1 > 0) ? ((c2 > 0) ? cmd.substring(c1 + 1, c2).toInt() : cmd.substring(c1 + 1).toInt()) : 25;
    int mod = (c2 > 0) ? cmd.substring(c2 + 1).toInt() : 0;
    applyModifiers(mod, true);
    uint8_t k = mapHidUsageToArduinoKey((uint8_t)usage);
    if (k) Keyboard.press(k);
    delay(max(5, min(2000, holdMs)));
    if (k) Keyboard.release(k);
    applyModifiers(mod, false);
    Serial.println("OK");
    return;
  }
  if (cmd.startsWith("MD:")) {
    Mouse.press(parseMouseButton(cmd.substring(3)));
    Serial.println("OK");
    return;
  }
  if (cmd.startsWith("MU:")) {
    Mouse.release(parseMouseButton(cmd.substring(3)));
    Serial.println("OK");
    return;
  }
  if (cmd.startsWith("MC:")) {
    int c1 = cmd.indexOf(',', 3);
    String bStr = (c1 > 0) ? cmd.substring(3, c1) : cmd.substring(3);
    int holdMs = (c1 > 0) ? cmd.substring(c1 + 1).toInt() : 25;
    uint8_t b = parseMouseButton(bStr);
    Mouse.press(b);
    delay(max(5, min(2000, holdMs)));
    Mouse.release(b);
    Serial.println("OK");
    return;
  }
  if (cmd.startsWith("MM:")) {
    int c1 = cmd.indexOf(',', 3);
    int dx = (c1 > 0) ? cmd.substring(3, c1).toInt() : 0;
    int dy = (c1 > 0) ? cmd.substring(c1 + 1).toInt() : 0;
    Mouse.move((signed char)constrain(dx, -127, 127), (signed char)constrain(dy, -127, 127), 0);
    Serial.println("OK");
    return;
  }
  if (cmd.startsWith("MS:")) {
    int c1 = cmd.indexOf(',', 3);
    int wheel = (c1 > 0) ? cmd.substring(3, c1).toInt() : cmd.substring(3).toInt();
    Mouse.move(0, 0, (signed char)constrain(wheel, -127, 127));
    Serial.println("OK");
    return;
  }
}

void setup() {
  Serial.begin(115200);
  Keyboard.begin();
  Mouse.begin();
}

void loop() {
  while (Serial.available() > 0) {
    char c = (char)Serial.read();
    if (c == '
') {
      processCommand(inputBuffer);
      inputBuffer = "";
    } else if (c != '') {
      inputBuffer += c;
    }
  }
}
