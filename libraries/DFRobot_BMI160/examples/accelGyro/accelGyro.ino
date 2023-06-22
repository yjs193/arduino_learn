#include <DFRobot_BMI160.h>

DFRobot_BMI160 bmi160;
void setup(){
  Serial.begin(115200);
  delay(100);
  bmi160.begin(DFRobot_BMI160::eAcc);
}

void loop(){  
  float x1,x2,y1,y2,z1,z2;
  x1=bmi160.getX();
  x2=bmi160.getAccX();
  y1=bmi160.getY();
  y2=bmi160.getAccY();
  z1=bmi160.getZ();
  z2=bmi160.getAccZ();
  Serial.print(x1);Serial.print("\t");
  Serial.print(y1);Serial.print("\t");
  Serial.print(z1);Serial.print("\t");
  Serial.print(x2);Serial.print("\t");
  Serial.print(y2);Serial.print("\t");
  Serial.print(z2);Serial.print("\t");
  Serial.println();
  delay(100);
}