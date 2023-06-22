#include <DFRobot_MicroNatural.h>

MicroNatural_UV mn_uv;


void setup() {
  Serial.begin(9600);
}

void loop() {
  Serial.println(mn_uv.read());
  delay(500);
}