#include <DFRobot_PM.h>

SoftwareSerial myPMSerial_23(2,3);
DFRobot_PM myPM_23;

void setup() {
  myPM_23.begin(&myPMSerial_23);
  Serial.begin(9600);
}

void loop() {
  Serial.println(myPM_23.readPM01());
}
