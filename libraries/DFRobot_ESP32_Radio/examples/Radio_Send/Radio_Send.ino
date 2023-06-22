#include "DFRobot_ESP32_Radio.h"

DFRobot_ESP32Radio Radio;

void setup() {
  Radio.turnOn();
  Radio.setGroup(13);
}
void loop() {
  Radio.send("hello");
  delay(200);
}