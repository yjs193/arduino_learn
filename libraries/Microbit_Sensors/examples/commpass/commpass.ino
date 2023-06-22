#include "Microbit_Sensors.h"
#include "Microbit_Matrix.h"

void setup() {
}

void loop() {
  int needle = 11 - ((Sensors.compassHeading() - 15) / 30) % 12;
  MMatrix.show(MMatrix.ALL_CLOCKS[needle]);
  delay(50);
}