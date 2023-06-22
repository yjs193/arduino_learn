#include <IOBOX_Motor.h>

IOBOX_Motor motor;

void setup() {
}

void loop() {
  motor.motorRun(motor.M1,motor.CW,255);
}
