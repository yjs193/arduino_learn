
#include <DFRobot_LIS2DH12.h>


DFRobot_LIS2DH12 LIS; //Accelerometer

void setup(){
  Serial.begin(9600);
  LIS.init(LIS2DH12_RANGE_8GA);
}

void loop(){
  int16_t x, y, z;
  delay(100);
  x=LIS.getX();
  y=LIS.getY();
  z=LIS.getZ();
  Serial.print("Acceleration x: "); //print acceleration
  Serial.print(x);
  Serial.print(" mg \ty: ");
  Serial.print(y);
  Serial.print(" mg \tz: ");
  Serial.print(z);
  Serial.println(" mg");}
