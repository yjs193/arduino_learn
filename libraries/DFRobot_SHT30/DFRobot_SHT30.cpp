#include "DFRobot_SHT30.h"

DFRobot_SHT30::DFRobot_SHT30()
{
    this->tempPin = -1;
    this->humyPin = -1;
}

DFRobot_SHT30::~DFRobot_SHT30()
{
    this->tempPin = -1;
    this->humyPin = -1;
}

void DFRobot_SHT30::begin(uint8_t tp,uint8_t hp)
{
    this->tempPin = tp;
    this->humyPin = hp;
}

float DFRobot_SHT30::getTemperatureC()
{
    if (tempPin<0) return 0;
    float analogVolt = (float)analogRead(this->tempPin);
#ifdef ESP_PLATFORM
    analogVolt = (-0.000000000000016 * pow(analogVolt,4) + 0.000000000118171 * pow(analogVolt,3)- 0.000000301211691 * pow(analogVolt,2)+ 0.001109019271794 * analogVolt + 0.034143524634089);
#else
    analogVolt =  analogVolt / map * vref;
#endif
    return (float)((-66.875 + 72.917 * analogVolt));
}

float DFRobot_SHT30::getTemperatureF()
{
    if (tempPin<0) return 0;
    float analogVolt = (float)analogRead(this->tempPin);
#ifdef ESP_PLATFORM
    analogVolt = (-0.000000000000016 * pow(analogVolt,4) + 0.000000000118171 * pow(analogVolt,3)- 0.000000301211691 * pow(analogVolt,2)+ 0.001109019271794 * analogVolt + 0.034143524634089);
#else
	analogVolt =  analogVolt / map * vref;
#endif
    return (float)(-88.375 + 131.25 * analogVolt);
}

float DFRobot_SHT30::getHumidity()
{
    if (humyPin<0) return 0;
    float analogVolt = (float)analogRead(this->humyPin);
#ifdef ESP_PLATFORM
    analogVolt = (-0.000000000000016 * pow(analogVolt,4) + 0.000000000118171 * pow(analogVolt,3)- 0.000000301211691 * pow(analogVolt,2)+ 0.001109019271794 * analogVolt + 0.034143524634089);
#else
	analogVolt = analogVolt / map * vref;
#endif
    return (float)((-12.5 + 41.667 * analogVolt));
}
