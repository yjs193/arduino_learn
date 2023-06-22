  #include <Wire.h>
#include "DFRobot_VEML7700.h"

DFRobot_VEML7700 als;

void setup()
{
  Serial.begin(9600);
  als.begin();
}

void loop()
{
  float lux;
  
  lux = als.getALSLux();
  Serial.print("Lux:");
  Serial.print(lux);
  Serial.println(" lx");
  
  delay(200);
}
