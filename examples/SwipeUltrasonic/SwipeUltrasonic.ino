#include <MultiGestureLib.h>

MultiGestureLib g;

void setup() {
  Serial.begin(115200);
  // Two HC-SR04 sensors (added top then bottom)
  g.addUltrasonicSensor(4,5);  // TRIG=4, ECHO=5 (Top)
  g.addUltrasonicSensor(6,7);  // TRIG=6, ECHO=7 (Bottom)
  g.setLayout(MGL_LAYOUT_VERTICAL);
  g.setThresholdCm(25);
  g.setDebounceMs(70);
  g.setSwipeTimeout(600);
  g.onSwipeUp([](){ Serial.println("Swipe Up"); });
  g.onSwipeDown([](){ Serial.println("Swipe Down"); });
  Serial.println("SwipeUltrasonic demo ready.");
}

void loop() {
  g.update();
}
