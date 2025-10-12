#ifndef MULTIGESTURELIB_H
#define MULTIGESTURELIB_H

#include <Arduino.h>

#define MGL_MAX_SENSORS 6

typedef void (*GestureCallback)();

typedef enum {
  SENSOR_PROXIMITY_DIGITAL = 0,
  SENSOR_ULTRASONIC_HCSR04 = 1
} MGL_SensorType;

typedef enum {
  MGL_LAYOUT_HORIZONTAL = 0,
  MGL_LAYOUT_VERTICAL = 1,
  MGL_LAYOUT_QUAD = 2
} MGL_Layout;

class MultiGestureLib {
public:
  MultiGestureLib();

  // Add sensors
  void addProximitySensor(uint8_t pin, bool pullup=true);
  void addUltrasonicSensor(uint8_t trigPin, uint8_t echoPin);

  // Settings
  void setLayout(MGL_Layout layout);
  void setProximityActiveHigh(bool activeHigh);
  void setThresholdCm(uint8_t cm);   // ultrasonic "present" threshold
  void setDebounceMs(uint16_t ms);
  void setSwipeTimeout(uint16_t ms);

  // Callbacks
  void onSwipeLeft(GestureCallback cb);
  void onSwipeRight(GestureCallback cb);
  void onSwipeUp(GestureCallback cb);
  void onSwipeDown(GestureCallback cb);
  void onCircleCW(GestureCallback cb);
  void onCircleCCW(GestureCallback cb);

  // Main loop
  void update();

private:
  struct Sensor {
    MGL_SensorType type;
    uint8_t pin1;      // proximity: pin; ultrasonic: trig
    uint8_t pin2;      // ultrasonic: echo
    bool lastActive;
    uint32_t lastChange;
  };

  Sensor sensors[MGL_MAX_SENSORS];
  uint8_t sensorCount;

  MGL_Layout layout;
  bool proxActiveHigh;
  uint8_t thresholdCm;
  uint16_t debounceMs;
  uint16_t swipeTimeout;

  // event history (simple ring buffer of last 6 activations)
  uint8_t eventIdx[6];
  uint32_t eventTs[6];
  uint8_t eventCount;

  // callbacks
  GestureCallback cbSwipeLeft;
  GestureCallback cbSwipeRight;
  GestureCallback cbSwipeUp;
  GestureCallback cbSwipeDown;
  GestureCallback cbCircleCW;
  GestureCallback cbCircleCCW;

  // helpers
  bool readActive(uint8_t idx);
  uint16_t readDistanceCm(uint8_t idx);
  void pushEvent(uint8_t sensorIndex, uint32_t t);
  void detectGestures(uint32_t now);
};

#endif
