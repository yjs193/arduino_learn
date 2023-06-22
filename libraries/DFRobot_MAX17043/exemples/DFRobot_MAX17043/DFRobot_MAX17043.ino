
#include "DFRobot_MAX17043.h"



DFRobot_MAX17043        gauge;



void setup()
{
  Serial.begin(115200);
}

void loop()
{

    Serial.print("voltage: ");
    Serial.print(gauge.readVoltage());
    Serial.println(" mV");

    Serial.print("precentage: ");
    Serial.print(gauge.readPercentage());
    Serial.println(" %");
    delay(1000);
  }


