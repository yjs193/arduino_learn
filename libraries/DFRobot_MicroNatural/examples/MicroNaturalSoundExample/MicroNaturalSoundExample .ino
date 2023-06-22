#include <DFRobot_MicroNatural.h>

MicroNatural_Sound mn_sound;


void setup() {
  Serial.begin(9600);
}

void loop() {
  Serial.println(mn_sound.read());
  delay(500);
}