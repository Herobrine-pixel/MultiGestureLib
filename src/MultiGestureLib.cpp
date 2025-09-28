#include "MultiGestureLib.h"

MultiGestureLib::MultiGestureLib()
: sensorCount(0),
  layout(MGL_LAYOUT_HORIZONTAL),
  proxActiveHigh(false),
  thresholdCm(25),
  debounceMs(50),
  swipeTimeout(500),
  eventCount(0),
  cbSwipeLeft(nullptr),
  cbSwipeRight(nullptr),
  cbSwipeUp(nullptr),
  cbSwipeDown(nullptr),
  cbCircleCW(nullptr),
  cbCircleCCW(nullptr)
{
  for (uint8_t i=0;i<MGL_MAX_SENSORS;i++){
    sensors[i].type = SENSOR_PROXIMITY_DIGITAL;
    sensors[i].pin1 = 0;
    sensors[i].pin2 = 0;
    sensors[i].lastActive = false;
    sensors[i].lastChange = 0;
  }
}

void MultiGestureLib::addProximitySensor(uint8_t pin, bool pullup) {
  if (sensorCount >= MGL_MAX_SENSORS) return;
  Sensor &s = sensors[sensorCount++];
  s.type = SENSOR_PROXIMITY_DIGITAL;
  s.pin1 = pin;
  s.pin2 = 0;
  pinMode(pin, pullup ? INPUT_PULLUP : INPUT);
}

void MultiGestureLib::addUltrasonicSensor(uint8_t trigPin, uint8_t echoPin) {
  if (sensorCount >= MGL_MAX_SENSORS) return;
  Sensor &s = sensors[sensorCount++];
  s.type = SENSOR_ULTRASONIC_HCSR04;
  s.pin1 = trigPin;
  s.pin2 = echoPin;
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  digitalWrite(trigPin, LOW);
}

void MultiGestureLib::setLayout(MGL_Layout l){ layout = l; }
void MultiGestureLib::setProximityActiveHigh(bool ah){ proxActiveHigh = ah; }
void MultiGestureLib::setThresholdCm(uint8_t cm){ thresholdCm = cm; }
void MultiGestureLib::setDebounceMs(uint16_t ms){ debounceMs = ms; }
void MultiGestureLib::setSwipeTimeout(uint16_t ms){ swipeTimeout = ms; }

void MultiGestureLib::onSwipeLeft(GestureCallback cb){ cbSwipeLeft = cb; }
void MultiGestureLib::onSwipeRight(GestureCallback cb){ cbSwipeRight = cb; }
void MultiGestureLib::onSwipeUp(GestureCallback cb){ cbSwipeUp = cb; }
void MultiGestureLib::onSwipeDown(GestureCallback cb){ cbSwipeDown = cb; }
void MultiGestureLib::onCircleCW(GestureCallback cb){ cbCircleCW = cb; }
void MultiGestureLib::onCircleCCW(GestureCallback cb){ cbCircleCCW = cb; }

bool MultiGestureLib::readActive(uint8_t idx){
  Sensor &s = sensors[idx];
  if (s.type == SENSOR_PROXIMITY_DIGITAL){
    int val = digitalRead(s.pin1);
    bool active = proxActiveHigh ? (val == HIGH) : (val == LOW);
    return active;
  } else {
    uint16_t d = readDistanceCm(idx);
    return (d > 0 && d <= thresholdCm);
  }
}

uint16_t MultiGestureLib::readDistanceCm(uint8_t idx){
  Sensor &s = sensors[idx];
  if (s.type != SENSOR_ULTRASONIC_HCSR04) return 0;
  // HC-SR04 pulse (blocking but short)
  digitalWrite(s.pin1, LOW);
  delayMicroseconds(2);
  digitalWrite(s.pin1, HIGH);
  delayMicroseconds(10);
  digitalWrite(s.pin1, LOW);
  unsigned long duration = pulseIn(s.pin2, HIGH, 30000UL); // 30ms timeout ~ 5m
  if (duration == 0) return 0;
  // speed of sound: 343 m/s -> 29.1 us/cm (round trip), so /58 for cm
  uint16_t cm = (uint16_t)(duration / 58UL);
  return cm;
}

void MultiGestureLib::pushEvent(uint8_t sensorIndex, uint32_t t){
  if (eventCount < 6){
    eventIdx[eventCount] = sensorIndex;
    eventTs[eventCount] = t;
    eventCount++;
  } else {
    // shift left
    for (uint8_t i=1;i<6;i++){
      eventIdx[i-1] = eventIdx[i];
      eventTs[i-1] = eventTs[i];
    }
    eventIdx[5] = sensorIndex;
    eventTs[5] = t;
  }
}

void MultiGestureLib::detectGestures(uint32_t now){
  if (sensorCount < 2) return;

  // Find last two distinct sensor activations within timeout
  int8_t a = -1, b = -1;
  uint32_t ta = 0, tb = 0;
  for (int i=eventCount-1;i>=0;i--){
    if (b == -1){
      b = eventIdx[i]; tb = eventTs[i];
    } else if (eventIdx[i] != b){
      a = eventIdx[i]; ta = eventTs[i];
      break;
    }
  }
  if (a != -1 && b != -1){
    if (tb - ta <= swipeTimeout){
      if (layout == MGL_LAYOUT_HORIZONTAL && sensorCount >= 2){
        // added order: 0=left, 1=right
        if (a == 0 && b == 1 && cbSwipeRight) cbSwipeRight();
        if (a == 1 && b == 0 && cbSwipeLeft)  cbSwipeLeft();
      } else if (layout == MGL_LAYOUT_VERTICAL && sensorCount >= 2){
        // added order: 0=top, 1=bottom
        if (a == 0 && b == 1 && cbSwipeDown) cbSwipeDown();
        if (a == 1 && b == 0 && cbSwipeUp)   cbSwipeUp();
      }
    }
  }

  // Circle detection (requires 4 sensors, layout QUAD)
  if (layout == MGL_LAYOUT_QUAD && sensorCount >= 4 && eventCount >= 4){
    // look at last 4 unique activations
    int8_t seq[4]; uint8_t found=0;
    // walk backwards, keep unique consecutive
    int last = -1;
    for (int i=eventCount-1;i>=0 && found<4;i--){
      int si = eventIdx[i];
      if (si != last){
        seq[3-found] = si; // fill from end to keep chronological order
        last = si;
        found++;
      }
    }
    if (found == 4){
      uint32_t t0=0, t3=0;
      // get timestamps of the 1st and last of these four
      // (re-scan to match indices)
      int count=0;
      for (int i=0;i<eventCount;i++){
        if (eventIdx[i] == seq[0] || eventIdx[i] == seq[1] || eventIdx[i] == seq[2] || eventIdx[i] == seq[3]){
          if (count==0) t0 = eventTs[i];
          t3 = eventTs[i];
          count++;
        }
      }
      if (t3 - t0 <= (uint32_t)(swipeTimeout*2)){
        // Clockwise if 0->1->2->3, CCW if 0->3->2->1 (assuming CW order of added sensors: 0,1,2,3)
        bool cw = (seq[0]==0 && seq[1]==1 && seq[2]==2 && seq[3]==3);
        bool ccw= (seq[0]==0 && seq[1]==3 && seq[2]==2 && seq[3]==1);
        if (cw && cbCircleCW) cbCircleCW();
        if (ccw && cbCircleCCW) cbCircleCCW();
      }
    }
  }
}

void MultiGestureLib::update(){
  uint32_t now = millis();
  for (uint8_t i=0;i<sensorCount;i++){
    bool active = readActive(i);
    if (active != sensors[i].lastActive){
      if (now - sensors[i].lastChange >= debounceMs){
        sensors[i].lastActive = active;
        sensors[i].lastChange = now;
        if (active){
          pushEvent(i, now);
          detectGestures(now);
        }
      }
    }
  }

  // Clean up old events to keep buffer relevant
  // Remove events older than swipeTimeout*3
  uint8_t write=0;
  for (uint8_t i=0;i<eventCount;i++){
    if (now - eventTs[i] <= (uint32_t)(swipeTimeout*3)){
      eventIdx[write] = eventIdx[i];
      eventTs[write] = eventTs[i];
      write++;
    }
  }
  eventCount = write;
}
