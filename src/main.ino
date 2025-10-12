#include <MultiGestureLib.h>

MultiGestureLib gestures;

void setup() {
  Serial.begin(9600);

  gestures.addUltrasonic(2, 3);
  gestures.addUltrasonic(4, 5);
  gestures.addUltrasonic(6, 7);
  gestures.addUltrasonic(8, 9);
  gestures.addCapacitive(12);

  gestures.setThreshold(20);
  gestures.setSwipeTimeout(500);
  gestures.setHoldTime(800);
  gestures.setHoverTime(1500);

  gestures.onGesture(SWIPE_LEFT, [](){ Serial.println("Swipe Left"); });
  gestures.onGesture(SWIPE_RIGHT, [](){ Serial.println("Swipe Right"); });
  gestures.onGesture(SWIPE_UP, [](){ Serial.println("Swipe Up"); });
  gestures.onGesture(SWIPE_DOWN, [](){ Serial.println("Swipe Down"); });
  gestures.onGesture(TAP, [](){ Serial.println("Tap"); });
  gestures.onGesture(HOLD, [](){ Serial.println("Hold"); });
  gestures.onGesture(HOVER, [](){ Serial.println("Hover"); });
  gestures.onGesture(CIRCLE_CW, [](){ Serial.println("Circle CW"); });
  gestures.onGesture(CIRCLE_CCW, [](){ Serial.println("Circle CCW"); });
}

void loop() {
  gestures.update();
}
