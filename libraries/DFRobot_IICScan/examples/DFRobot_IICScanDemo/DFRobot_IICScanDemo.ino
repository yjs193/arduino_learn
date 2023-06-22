#include "DFRobot_IICScan.h"

DFRobot_IICScan iicscan;

void setup() {

  Serial.begin(9600);

}

void loop() {

  Serial.println(iicscan.scan());
  delay(500);

}