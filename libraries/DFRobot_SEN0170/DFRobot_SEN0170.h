#ifndef _DFROBOT_SEN0170_H_
#define _DFROBOT_SEN0170_H_

#if ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif


class DFRobot_SEN0170
{
public:
    DFRobot_SEN0170();
    ~DFRobot_SEN0170();
    int readSpeed(uint8_t pin);
};

#endif
