# MultiGestureLib

Universal Arduino **gesture detection** using **low-cost sensors** (IR proximity and HC-SR04 ultrasonic).  
Detect **swipe left/right/up/down**, **tap/hover**, and **circle (CW/CCW)** with simple, affordable hardware.

## ✨ Features
- Supports **digital IR proximity** sensors and **HC-SR04 ultrasonic** sensors.
- Detects **Swipe L/R/U/D** (2 sensors) and **Circle CW/CCW** (4 sensors in a square).
- Adjustable **distance threshold**, **debounce**, and **gesture timeout**.
- **Callback API** for easy event handling.
- Works on Arduino Uno/Nano/MEGA, ESP32/ESP8266, STM32 and more.

## 🧰 Supported Sensors
- **IR Proximity (digital output)**: e.g., KY-032, Sharp digital modules, TCRT5000-style digital boards.
- **Ultrasonic HC-SR04**.

> You can mix sensor types in the same layout.

## 🔌 Wiring

### IR Proximity (Digital)
| Sensor | Pin on Sensor | Arduino Pin | Notes |
|-------:|----------------|-------------|-------|
| IR #1  | OUT            | D2          | Use any digital input pin |
|        | VCC            | 5V/3.3V     | As per module |
|        | GND            | GND         | Common ground |
| IR #2  | OUT            | D3          | For left/right or up/down swipe |
|        | VCC            | 5V/3.3V     |  |
|        | GND            | GND         |  |

### Ultrasonic HC-SR04
| Sensor | Pin on Sensor | Arduino Pin | Notes |
|-------:|----------------|-------------|-------|
| US #1  | TRIG           | D4          | Any digital output |
|        | ECHO           | D5          | Input pin (use voltage divider for 5V→3.3V boards) |
|        | VCC            | 5V          |  |
|        | GND            | GND         |  |
| US #2  | TRIG           | D6          | Second sensor |
|        | ECHO           | D7          |  |

### 4-Sensor Circle Layout (clockwise indexing)
Add sensors in **clockwise order**: `0=Top-Left`, `1=Top-Right`, `2=Bottom-Right`, `3=Bottom-Left`.

## 🧪 Quick Start (IR swipe left/right)

```cpp
#include <MultiGestureLib.h>

MultiGestureLib g;

void setup() {
  Serial.begin(9600);
  // Two IR digital sensors on pins 2 and 3 (added left then right)
  g.addProximitySensor(2); // Left
  g.addProximitySensor(3); // Right
  g.setLayout(MGL_LAYOUT_HORIZONTAL);
  g.setProximityActiveHigh(false); // most IR modules pull LOW when near
  g.setDebounceMs(50);
  g.setSwipeTimeout(500); // ms
  g.onSwipeLeft([](){ Serial.println("Swipe Left"); });
  g.onSwipeRight([](){ Serial.println("Swipe Right"); });
}

void loop() {
  g.update();
}
```

## 🧪 Quick Start (Ultrasonic swipe up/down)

```cpp
#include <MultiGestureLib.h>

MultiGestureLib g;

void setup() {
  Serial.begin(9600);
  // Two HC-SR04 sensors (added top then bottom)
  g.addUltrasonicSensor(4,5);  // TRIG=4, ECHO=5 (Top)
  g.addUltrasonicSensor(6,7);  // TRIG=6, ECHO=7 (Bottom)
  g.setLayout(MGL_LAYOUT_VERTICAL);
  g.setThresholdCm(25);  // consider "present" if within 25 cm
  g.setDebounceMs(70);
  g.setSwipeTimeout(600);
  g.onSwipeUp([](){ Serial.println("Swipe Up"); });
  g.onSwipeDown([](){ Serial.println("Swipe Down"); });
}

void loop() {
  g.update();
}
```

## 🧪 Quick Start (4-sensor circle)

```cpp
#include <MultiGestureLib.h>
MultiGestureLib g;

void setup() {
  Serial.begin(115200);
  // Add 4 sensors clockwise: TL, TR, BR, BL (mix types allowed)
  g.addProximitySensor(2);        // TL
  g.addProximitySensor(3);        // TR
  g.addUltrasonicSensor(4,5);     // BR
  g.addUltrasonicSensor(6,7);     // BL
  g.setLayout(MGL_LAYOUT_QUAD);
  g.setThresholdCm(25);
  g.onCircleCW([](){ Serial.println("Circle CW"); });
  g.onCircleCCW([](){ Serial.println("Circle CCW"); });
}

void loop() { g.update(); }
```

## 📚 API Reference

- `void addProximitySensor(uint8_t pin, bool pullup=true)`
- `void addUltrasonicSensor(uint8_t trigPin, uint8_t echoPin)`
- `void setLayout(MGL_Layout layout)` where `layout` is one of `MGL_LAYOUT_HORIZONTAL`, `MGL_LAYOUT_VERTICAL`, `MGL_LAYOUT_QUAD`
- `void setProximityActiveHigh(bool activeHigh)`
- `void setThresholdCm(uint8_t cm)` (ultrasonic presence threshold, default 25 cm)
- `void setDebounceMs(uint16_t ms)`
- `void setSwipeTimeout(uint16_t ms)`
- `void onSwipeLeft(void (*cb)())`, `onSwipeRight(...)`, `onSwipeUp(...)`, `onSwipeDown(...)`
- `void onCircleCW(void (*cb)())`, `void onCircleCCW(void (*cb)())`
- `void update()`

## 🔧 Implementation Notes
- Debounce prevents false toggles.
- Swipe is detected when **two distinct sensors** activate in order within `swipeTimeout`.
- Circle (CW/CCW) is detected when all four sensors activate sequentially clockwise or counter-clockwise within `swipeTimeout * 2`.

## 🧾 License
MIT — do anything, just keep the license.

---

**Made for makers. Built to be extended.** :)
