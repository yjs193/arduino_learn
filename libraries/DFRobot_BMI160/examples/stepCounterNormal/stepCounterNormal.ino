#include <DFRobot_BMI160.h>
DFRobot_BMI160 bmi160;
void setup(){
  Serial.begin(115200);
  delay(100);
  bmi160.begin(DFRobot_BMI160::eStep);
}
void loop(){
  Serial.println(bmi160.getstep());
}


