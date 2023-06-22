/*
 * file DFRobot_EC.h * @ https://github.com/DFRobot/DFRobot_EC
 *
 * Arduino library for Gravity: Analog Electrical Conductivity Sensor / Meter Kit V2 (K=1), SKU: DFR0300
 *
 * Copyright   [DFRobot](http://www.dfrobot.com), 2018
 * Copyright   GNU Lesser General Public License
 *
 * version  V1.01
 * date  2018-06
 */

#ifndef _DFROBOT_EC_H_
#define _DFROBOT_EC_H_

#if ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif


class DFRobot_EC
{
public:
    DFRobot_EC();
    ~DFRobot_EC();
    float readEC(uint8_t pin, float temperature); // voltage to EC value, with temperature compensation
private:
	uint8_t   numReadings = 20;
	int       readings[20]={0};
	uint8_t   delaytime = 25;
	byte      index = 0; 
};

#endif
