#if ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include "DFRobot_SEN0170.h" 
DFRobot_SEN0170::DFRobot_SEN0170()
{

}

DFRobot_SEN0170::~DFRobot_SEN0170()
{
	
}
int DFRobot_SEN0170::readSpeed(uint8_t pin)
{
	int sensorValue = analogRead(pin);
#if defined(ARDUINO_ARCH_RP2040)
	float outvoltage = (float)sensorValue *(3.3/1023.0);
#else
	float outvoltage = (float)sensorValue *(5.0/1023.0);
#endif
	int speed = 6*outvoltage;
    return speed;
}
