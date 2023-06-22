#include "DFRobot_BNO055.h"
#include "Wire.h"

typedef DFRobot_BNO055_IIC    BNO;

BNO   bno(&Wire, 0x28);

void setup()
{
  Serial.begin(115200);
  bno.reset();
  bno.begin();
  bno.setPowerMode(BNO::ePowerModeNormal);//ePowerModeNormal,ePowerModeLowPower,ePowerModeSuspend
  bno.getAxis(BNO::eAxisAcc).x;    // read acceleration
  bno.getAxis(BNO::eAxisMag).x;    // read geomagnetic
  bno.getAxis(BNO::eAxisGyr).x;    // read gyroscope
  bno.getAxis(BNO::eAxisLia).x;    // read linear acceleration
  bno.getAxis(BNO::eAxisGrv).x;    // read gravity vector
  bno.getEul().head;                  // read euler angle:head,roll,pitch
  bno.getQua().x;                  // read quaternion:w,x,y,z
}


void loop()
{
    
}