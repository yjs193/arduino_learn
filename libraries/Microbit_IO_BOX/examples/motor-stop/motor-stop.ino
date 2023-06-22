#include <IOBOX_Motor.h>

IO_BOX_Motor motor;

void setup() {
}

void loop() {
  motor.motorStop(motor.M1);
}
