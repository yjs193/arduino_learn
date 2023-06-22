#include "DFRobot_ESP32_Radio.h"

DFRobot_ESP32Radio Radio;

void setup() {
  Serial.begin(9600);
  Radio.setCallback(onRadioReceive);
  Radio.turnOn();
  Radio.setGroup(13);
}
void loop() {
}

void onRadioReceive(String message) {
  Serial.println(message);
}
