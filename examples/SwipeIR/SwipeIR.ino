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
  g.setSwipeTimeout(500);
  g.onSwipeLeft([](){ Serial.println("Swipe Left"); });
  g.onSwipeRight([](){ Serial.println("Swipe Right"); });
  Serial.println("SwipeIR demo ready.");
}

void loop() {
  g.update();
}
