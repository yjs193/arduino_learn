#include <DFRobot_VL53L0X.h>

DFRobot_VL53L0X vl53l0x;

void setup() {
  vl53l0x.begin(0x50);
  vl53l0x.setMode(vl53l0x.Low, vl53l0x.Single);
  Serial.begin(9600);
}

void loop() {
  Serial.println(vl53l0x.getDistance());
}
