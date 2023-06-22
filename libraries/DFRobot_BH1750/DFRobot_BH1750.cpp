#include "DFRobot_BH1750.h"
#include <math.h>

DFRobot_BH1750::DFRobot_BH1750()
{
}

void DFRobot_BH1750::begin(void)
{
  Wire.begin();
  BH1750_Init(0x23);
  delay(200);
}

uint16_t DFRobot_BH1750::BH1750_Read(void)
{
  uint16_t val = 0;
  BYTE buff[2];
  if (2 == rRead(0x23, buff))
  {
    val = ((buff[0] << 8) | buff[1]) / 1.2;
  }
  delay(150);
  return val;
}

uint8_t DFRobot_BH1750::rRead(int address, BYTE *p)
{
  int i = 0;
#ifndef ARDUINO_ARCH_RP2040
  Wire.beginTransmission(address);
#endif
  Wire.requestFrom(address, 2);
  while (Wire.available()) //
  {
    p[i] = Wire.read(); // receive one byte
    i++;
  }
#ifndef ARDUINO_ARCH_RP2040
  Wire.endTransmission();
#endif
  return i;
}

void DFRobot_BH1750::BH1750_Init(int address)
{
  Wire.beginTransmission(address);
  Wire.write(0x10); // 1lx reolution 120ms
  Wire.endTransmission();
}
