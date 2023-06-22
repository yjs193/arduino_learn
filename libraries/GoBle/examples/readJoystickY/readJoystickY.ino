#include "GoBLE.h"

void setup() {
  Serial.begin(9600);
  Goble.begin();
}

void loop() {
  Serial.println(Goble.readJoystickY());
}