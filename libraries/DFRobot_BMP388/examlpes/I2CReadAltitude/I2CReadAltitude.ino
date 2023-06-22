#include "DFRobot_BMP388_I2C.h"

DFRobot_BMP388_I2C bmp388;

float seaLevel;

void setup(){
  Serial.begin(9600);
  bmp388.begin();
}

void loop(){
  Serial.println(bmp388.readAltitude());
  Serial.println(bmp388.readPressure());
  Serial.println(bmp388.readTemperature());
  Serial.println("==================");
  delay(500);
}

