#include "VortexMp3Old.h"

void VortexMp3Old::init()
{
    mySerial.begin (9600);
    delay(500);
}
void VortexMp3Old::setVolume(byte vol)
{
  //0x00~0x64 --> 0x00~0x26
  vol = (float)(vol/100.0)*38.0;
  vol = vol/1.2;
  mySerial.write((uint8_t)0x7e);
  mySerial.write((uint8_t)0xff);
  mySerial.write((uint8_t)0x06);
  mySerial.write((uint8_t)0x06);
  mySerial.write((uint8_t)0x00);
  mySerial.write((uint8_t)0x00);
  mySerial.write((uint8_t)vol);
  mySerial.write((uint8_t)0xef);
  delay(10);
}

void VortexMp3Old::repeatPlayer(byte data)
{
  mySerial.write((uint8_t)0x7e);
  mySerial.write((uint8_t)0xff);
  mySerial.write((uint8_t)0x06);
  mySerial.write((uint8_t)0x08);
  mySerial.write((uint8_t)0x00);
  mySerial.write((uint8_t)0x00);
  mySerial.write((uint8_t)data);
  mySerial.write((uint8_t)0xef);
}
void VortexMp3Old::play(byte data)
{
  mySerial.write((uint8_t)0x7e);
  mySerial.write((uint8_t)0xff);
  mySerial.write((uint8_t)0x06);
  mySerial.write((uint8_t)0x03);
  mySerial.write((uint8_t)0x00);
  mySerial.write((uint8_t)0x00);
  mySerial.write((uint8_t)data);
  mySerial.write((uint8_t)0xef);
}

void VortexMp3Old::stop()
{
  mySerial.write((uint8_t)0x7e);
  mySerial.write((uint8_t)0xff);
  mySerial.write((uint8_t)0x06);
  mySerial.write((uint8_t)0x16);
  mySerial.write((uint8_t)0x00);
  mySerial.write((uint8_t)0x00);
  mySerial.write((uint8_t)0x00);
  mySerial.write((uint8_t)0xef);
}

