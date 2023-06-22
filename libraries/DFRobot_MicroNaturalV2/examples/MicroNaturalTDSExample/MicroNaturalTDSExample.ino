#include <DFRobot_MicroNatural.h>

MicroNatural_TDS mn_tds;


void setup() {
  Serial.begin(9600);
}

void loop() {
  mn_tds.setK(2.68);
  Serial.println(mn_tds.readK());
  Serial.println(mn_tds.read());
  delay(500);
}