#include "DFRobot_Stepper.h"

DFRobot_Stepper myStepper;

void setup()
{
}

void loop()
{
  myStepper.stepperConfig(7,8,6,200,450); //X轴
  myStepper.step(200,CW);
  delay(1000);
  myStepper.stepperConfig(7,8,6,200,100); //X轴
  myStepper.step(200,CCW);
  delay(1000);

  myStepper.stepperConfig(4,12,5,200,450);//Y轴
  myStepper.step(200,CW);
  delay(1000);
  myStepper.stepperConfig(4,12,5,200,100);//Y轴
  myStepper.step(200,CCW);
  delay(1000);
}
