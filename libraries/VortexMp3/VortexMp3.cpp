//
//  VortexMp3
//  
//
//  Created by kelvin on 15/1/5.
//
//

#include "VortexMp3.h"

void VortexMp3::init()
{
    mySerial.begin (9600);
    delay(1000);
}
void VortexMp3::setVolume(byte vol)
{
  //0x00~0x64 --> 0x00~0x26
  vol = (float)(vol/100.0)*38.0;
  delay(20);
  vol = vol/1.2;
  uint8_t buffer[] = {0x7e, 0x04, 0xae,vol,byte(0x04+0xae + vol), 0xef};
  mySerial.write(buffer, 6);delay(500);
}
void mp3setVolume(byte vol)
{
    uint8_t buffer[] = {0x7e, 0x04, 0xae,vol,byte(0x04+0xae + vol), 0xef};
    mySerial.write(buffer, 8);
    delay(20);
}

void VortexMp3::repeatPlayer(byte data)
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
void VortexMp3::play(byte data)
{
    mySerial.write((uint8_t)0x7e);
    mySerial.write((uint8_t)0x05);
    mySerial.write((uint8_t)0xa0);
    mySerial.write((uint8_t)0x00);
    mySerial.write((uint8_t)data);
    mySerial.write((uint8_t)byte(0x05+0xa0+data));
    mySerial.write((uint8_t)0xef);
}


void VortexMp3::playDanceMusic(byte data)
{
  //setMode(0);
  //delay(20);
  char dataHigh = (data/10)+0x30;
  char dataLow = (data%10)+0x30;
  
  uint8_t buffer[] = {0x7e, 0x07, 0xa1, 0x30,0x30,dataHigh,dataLow,byte(0x07+0xa1+0x30+0x30+dataHigh+dataLow),0xef};
  mySerial.write(buffer, 9);
}



void VortexMp3::stop()
{
    mySerial.write((uint8_t)0x7e);
    mySerial.write((uint8_t)0x03);
    mySerial.write((uint8_t)0xab);
    mySerial.write((uint8_t)0xae);
    mySerial.write((uint8_t)0xef);
}

void VortexMp3::setMode(byte isRepeat)
{
//    static bool _isRepeat = false;
//    if(_isRepeat!=isRepeat)
//    {
      delay(20);
      uint8_t buffer[] = {0x7e, 0x04, 0xaf, isRepeat, byte(0x04+0xaf+isRepeat),0xef};
      mySerial.write(buffer, 6);
//    }
}