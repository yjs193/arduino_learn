#include "DFRobot_SEN0170.h"

#define PIN A0

DFRobot_SEN0170 Wind;

void setup()
{
  Serial.begin(115200);  
}

void loop()
{
	int value = Wind.readSpeed(PIN);
	Serial.print("wind speed is");
	Serial.print(value);
	Serial.println(" level now");
	delay(1000);
}

