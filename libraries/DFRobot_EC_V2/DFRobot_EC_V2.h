/*
 * file DFRobot_EC_V2.h * @ https://github.com/DFRobot/DFRobot_EC_V2
 *
 * Arduino library for Gravity: Analog Electrical Conductivity Sensor / Meter Kit V2 (K=1), SKU: DFR0300
 *
 * Copyright   [DFRobot](http://www.dfrobot.com), 2018
 * Copyright   GNU Lesser General Public License
 *
 * version  V1.01
 * date  2018-06
 */

#ifndef _DFROBOT_EC_v2_H_
#define _DFROBOT_EC_v2_H_

#if ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#define ReceivedBufferLength 10  //length of the Serial CMD buffer

class DFRobot_EC_V2
{
public:
    DFRobot_EC_V2();
    ~DFRobot_EC_V2();
    void calibration();  //calibration by Serial CMD
    float readEC(int pin, float temperature); // voltage to EC value, with temperature compensation
    void begin();   //initialization

private:
    float _ecvalue;
    float _kvalue;
    float _kvalueLow;
    float _kvalueHigh;
    float _voltage;
    float _temperature;
    float _rawEC;

    char _cmdReceivedBuffer[ReceivedBufferLength];  //store the Serial CMD
    byte _cmdReceivedBufferIndex;

private:
    boolean cmdSerialDataAvailable();
    void ecCalibration(byte mode); // calibration process, wirte key parameters to EEPROM
    byte cmdParse();
};

#endif
