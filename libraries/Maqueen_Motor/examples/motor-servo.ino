#include <Maqueen_Motor.h>

Maqueen_Motor motor;

void setup() {
  motor.motorRun(motor.LEFT,motor.CW,255);
  motor.motorRun(motor.RIGHT,motor.CW,100);
}

void loop() {
  motor.servo(motor.S1,  0 );
  motor.servo(motor.S2, 180);
  delay(1000);
  motor.servo(motor.S1,  90);
  motor.servo(motor.S2,  90);
  delay(1000);
  motor.servo(motor.S1, 180);
  motor.servo(motor.S2,  0 );
  delay(1000);
  motor.servo(motor.S1,  90);
  motor.servo(motor.S2,  90);
  delay(1000);
}
