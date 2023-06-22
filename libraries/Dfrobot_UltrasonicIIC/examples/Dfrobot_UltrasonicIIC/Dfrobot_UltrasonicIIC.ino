#include <Dfrobot_UltrasonicIIC.h>
Dfrobot_UltrasonicIIC ult;
void setup() {
Serial.begin(9600);
ult.begin(0x12);
//ult.changeAddr(0x11)   //改变地址后，需要
}
void loop() {
  float dist,temp;
  dist = ult.getDistanceCm();
  temp = ult.getTemperature();
  Serial.print((int)dist, DEC);
  Serial.print("cm");
  Serial.print("------");
  Serial.print((float)temp / 10, 1);
  Serial.println("℃");
}
