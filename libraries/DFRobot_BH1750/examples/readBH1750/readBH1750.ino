#include "DFRobot_BH1750.h"

DFRobot_BH1750 env;

void setup()
{
  Serial.begin(9600);
  env.begin();
}

void loop()
{
  uint16_t val;
  
  val = env.BH1750_Read();
  Serial.print(val,DEC);
  Serial.println("[lx]");
  
  delay(200);
}