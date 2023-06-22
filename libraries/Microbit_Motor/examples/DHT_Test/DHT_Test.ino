#include "DFRobot_DHT.h"

DHT dht(2);

void setup()
{
  Serial.begin(9600);
}

void loop()
{
  Serial.println(dht.getHumidity());
  Serial.println(dht.getTemperature());
}
