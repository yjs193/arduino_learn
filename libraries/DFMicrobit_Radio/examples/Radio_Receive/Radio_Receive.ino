#include "SoftwareSerial.h"

SoftwareSerial softwareSerial1(2, 1);

void setup() {
  softwareSerial1.begin(9600);
  Serial.begin(9600);
}

void loop() {
  if (softwareSerial1.available()) {
    Serial.println(softwareSerial1.read());
  }
  if (Serial.available()) {
    softwareSerial1.println(Serial.read());
  }
}

