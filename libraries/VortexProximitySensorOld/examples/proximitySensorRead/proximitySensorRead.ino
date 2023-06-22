#include "VortexProximitySensorOld.h"

void setup() {
  proximitySensorInit();
  Serial.begin(9600);
}

void loop() {
  Serial.println(proximitySensorRead());
}