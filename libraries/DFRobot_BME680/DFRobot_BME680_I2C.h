#ifndef DFRobot_BME680_I2C_H
#define DFRobot_BME680_I2C_H

#include "DFRobot_BME680.h"

#define CALIBRATE_PRESSURE

class DFRobot_BME680_I2C : public DFRobot_BME680
{
  public:
    DFRobot_BME680_I2C();

    void        setConvertAndUpdate(void);
};

#endif