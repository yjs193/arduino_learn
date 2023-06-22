#include "Microbit_Pwm.h"
Microbit_Pwm mpwm;

void setup() {
}

void loop() {
  mpwm.pwmWrite(0,0);//引脚0 占空比为0
  delay(1000);
  mpwm.pwmWrite(0,200);//引脚0 占空比为200/1023
  delay(1000);
  mpwm.pwmWrite(0,512);//引脚0 占空比为512/1023
  delay(1000);
  mpwm.pwmWrite(0,800);//引脚0 占空比为800/1023
  delay(1000);
  mpwm.pwmWrite(0,1023);//引脚0 占空比为1
  delay(1000);
}