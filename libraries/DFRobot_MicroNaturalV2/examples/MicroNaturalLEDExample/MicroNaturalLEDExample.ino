#include <DFRobot_MicroNatural.h>

MicroNatural_LED mn_led;


void setup() {
}

void loop() {
  mn_led.set(true);
  delay(1000);
  mn_led.set(false);
  delay(1000);
}