#ifndef IOBOX_Motor_H
#define IOBOX_Motor_H
#include <Arduino.h>
#include <Wire.h>




class IOBOX_Motor
{
  public:

  uint8_t  M1  = 0;
  uint8_t  M2 = 1;
  uint8_t  ALL = 2;
  uint8_t  CW    = 0;
  uint8_t  CCW   = 1;

  IOBOX_Motor();
  ~IOBOX_Motor();

  void    motorRun(int index, int direction, int speed),
          motorStop(int index),
          servo(int index, int angle);

  private:

  void   i2cWriteBuf(int addr, unsigned char *p, int len);
};


#endif
