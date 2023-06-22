#include "Maqueen_Motor.h"
#include "Arduino.h"
#include "Wire.h"



Maqueen_Motor::Maqueen_Motor(){
    Wire.begin();
}

Maqueen_Motor::~Maqueen_Motor() {}


void Maqueen_Motor::motorRun(int index, int direction, int speed)
{
  byte buf[3] = {0};
  speed = abs(speed);
  if (speed >= 255) {
    speed = 255;
  }

  if (index > 1 || index < 0)
    return;
  
  
  if(index == LEFT)
	buf[0] = 0x00;
  else
	buf[0] = 0x02;

  buf[1] = direction;
  buf[2] = speed;
  
  this->i2cWriteBuffer(0x10,buf,3);
}

void Maqueen_Motor::motorStop(int index)
{
  motorRun(index,CW,0);
}

void Maqueen_Motor::servo(int index, int angle)
{
  byte buf[2] = {0};
  angle = min(max(angle ,0), 180);

  if(index == S2)
    buf[0] = (uint8_t)S2;
  else
    buf[0] = (uint8_t)S1;
  buf[1] = (uint8_t)angle;

  this->i2cWriteBuffer(0x10,buf,2);
}

void Maqueen_Motor::i2cWriteBuffer(int addr, unsigned char *p, int len)
{
#ifdef ESP_PLATFORM
  Wire.setClock(100000);
#endif
  Wire.beginTransmission(addr);
  for(int i=0; i<len; i++)
    Wire.write((uint8_t)p[i]);
  Wire.endTransmission();
}

