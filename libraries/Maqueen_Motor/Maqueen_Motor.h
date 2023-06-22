#ifndef MAQUEEN_MOTOR_H
#define MAQUEEN_MOTOR_H
#include <Arduino.h>
#include <Wire.h>




class Maqueen_Motor
{
  public:

  uint8_t  S1 = 20;
  uint8_t  S2 = 21;
  uint8_t  LEFT  = 0;
  uint8_t  RIGHT = 1;
  uint8_t  CW    = 0;
  uint8_t  CCW   = 1;

  Maqueen_Motor();
  ~Maqueen_Motor();

  void    motorRun(int index, int direction, int speed),
          motorStop(int index),
          servo(int index, int angle);

  private:

  void   i2cWriteBuffer(int addr, unsigned char *p, int len);
};

#endif
