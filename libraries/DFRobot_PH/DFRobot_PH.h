#ifndef _DFROBOT_PH_H_
#define _DFROBOT_PH_H_

#if ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif


class DFRobot_PH2
{
public:
    DFRobot_PH2();
    ~DFRobot_PH2();
    void    calibration(int32_t pin, float temperature);
    float   readPH(uint32_t pin); // voltage to pH value, with temperature compensation

private:
    float  _phValue;
    float  _acidVoltage;
    float  _neutralVoltage;
    float  _voltage;
    float  _temperature;

    char   _cmdReceivedBuffer[10];  //store the Serial CMD
    byte   _cmdReceivedBufferIndex;
#if defined NRF5
    float aref = 3300;
#elif defined NRF52833
    float aref = 3300;
#else
    float aref = 5000;
#endif
    float map = 1024;

    boolean cmdSerialDataAvailable();
    void    phCalibration(byte mode); // calibration process, wirte key parameters to EEPROM
    byte    cmdParse();
};


class DFRobot_PH
{
public:
    DFRobot_PH();
    ~DFRobot_PH();
    float readPH(uint32_t pin);

private:
    int*  pHArray;
    bool  first;
    bool  enable;
    float valuePrev;
    int   pHArrayIndex;
    float map = 1024;
    float avergeArray();
};


#endif // _DFROBOT_PH_H_
