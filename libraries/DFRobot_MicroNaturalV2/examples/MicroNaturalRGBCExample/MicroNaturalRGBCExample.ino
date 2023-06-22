#include <DFRobot_MicroNatural.h>

MicroNatural_RGBC mn_rgbc;


void setup() {
  Serial.begin(9600);
}

void loop() {
  Serial.print("red: "); Serial.println(mn_rgbc.red());
  Serial.print("green: "); Serial.println(mn_rgbc.green());
  Serial.print("blue: "); Serial.println(mn_rgbc.blue());
  Serial.print("naturalLight: "); Serial.println(mn_rgbc.naturalLight());
  Serial.println("=============split line=============");
  delay(500);
}