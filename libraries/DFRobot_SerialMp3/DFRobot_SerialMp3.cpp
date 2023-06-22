#include "DFRobot_SerialMp3.h"
#if defined(NRF5) || defined(NRF52833)
#include <basic.h>
uart SerialMp3(0, 0);
#endif



DFRobot_SerialMp3::DFRobot_SerialMp3()
{
    this->serialMp3First = true;
}
DFRobot_SerialMp3::~DFRobot_SerialMp3()
{
    
}

#if defined(NRF5)
void DFRobot_SerialMp3::begin(int rx, int tx){
  rpin = rx;
  tpin = tx;
  rx = g_PinID[rx];
  tx = g_PinID[tx];
  MicroBitPin *rxpin = getMicroBitPin(rx);
  MicroBitPin *txpin = getMicroBitPin(tx);
  uBit.serial.redirect(txpin->name, rxpin->name);
  _serialH = &SerialMp3;
  _serialS = NULL;
  _serialH->begin(9600);
}
#elif defined(NRF52833)
void DFRobot_SerialMp3::begin(int rx, int tx){
  rpin = rx;
  tpin = tx;
  rx = g_PinID[rx];
  tx = g_PinID[tx];
  MicroBitPin *rxpin = getMicroBitPin(rx);
  MicroBitPin *txpin = getMicroBitPin(tx);
  uBit.serial.redirect(*txpin, *rxpin);
  _serialH = &SerialMp3;
  _serialS = NULL;
  _serialH->begin(9600);
}
#else
void DFRobot_SerialMp3::begin(HardwareSerial *serial, int rx, int tx){
  rpin = rx;
  tpin = tx;
  _serialH = serial;
  _serialS = NULL;
#if defined(ESP_PLATFORM)
  _serialH->begin(9600,rx,tx);
#else
  _serialH->begin(9600);
#endif
}
#endif
void DFRobot_SerialMp3::begin(SoftwareSerial *serial){
  _serialH = NULL;
  _serialS = serial;
  _serialS->begin(9600);
  rpin = _serialS->getReceivePin();
  tpin = _serialS->getTransmitPin();
}

void DFRobot_SerialMp3::volume(uint8_t value)
{
    //digitalWrite(tpin,1);
    delay(1);
    if(value>100) value = 100;
    value = map(value, 0, 100, 0, 30);
    uint8_t data[5];
    data[0] = 0xAA;
    data[1] = 0x13;
    data[2] = 0x01;
    data[3] = (uint8_t)value;
    data[4] = data[0] + data[1] + data[2] + data[3];
    write(data,5);
    this->serialMp3First = false;
    //digitalWrite(tpin,0);
    delay(1);
}

void DFRobot_SerialMp3::playList(uint8_t value)
{
    //digitalWrite(tpin,1);
    delay(1);
    if(this->serialMp3First) {volume(50); this->serialMp3First = false;}
    delay(100); uint8_t data[6];
    data[0] = 0xAA;
    data[1] = 0x07;
    data[2] = 0x02;
    data[3] = (value>>8) & 0xFF;
    data[4] = value & 0xFF;
    data[5] = data[0] + data[1] + data[2] + data[3] + data[4];
    write(data,6);
    //digitalWrite(tpin,0);
    delay(1);
}

void DFRobot_SerialMp3::insertTracks(uint16_t value)
{
    //digitalWrite(tpin,1);
    delay(1);
    if(this->serialMp3First) {volume(50); this->serialMp3First = false;}
    uint8_t data[7];
    data[0] = 0xAA;
    data[1] = 0x16;
    data[2] = 0x03;
    data[3] = 0x00;
    data[4] = (uint8_t)((value>>8)&0xff);
    data[5] = (uint8_t)(value&0xff);
    data[6] = data[0] + data[1] + data[2] + data[3] + data[4] + data[5];
    write(data,7);
    //digitalWrite(tpin,0);
    delay(1);
}

void DFRobot_SerialMp3:: play()
{
    //digitalWrite(tpin,1);
    delay(1);
    if(this->serialMp3First) {volume(50); this->serialMp3First = false;}
    uint8_t data[4];
    data[0] = 0xAA;
    data[1] = 0x02;
    data[2] = 0x00;
    data[3] = data[0] + data[1] + data[2];
    write(data,4);
    //digitalWrite(tpin,0);
    delay(1);
}

uint8_t DFRobot_SerialMp3::queryPlayStatus()
{
    //digitalWrite(tpin,1);
    delay(1);
    if(this->serialMp3First) {volume(50); this->serialMp3First = false;}
    uint8_t dataIn[5];
    uint8_t data[4];
    data[0] = 0xAA;
    data[1] = 0x01;
    data[2] = 0x00;
    data[3] = data[0] + data[1] + data[2];
    write(data,4);

    long timeOut = millis();
    while(!available())
    {
        if((millis() - timeOut) > 1000 && !available())
            return 0;
        delay(1);
    }
    uint8_t num = available();
    if(num != 5) return 0;
    while(num){
        dataIn[5-num] = read();
        num--;
    }
    if(dataIn[4] != (dataIn[0] + dataIn[1] + dataIn[2] + dataIn[3]))
        return 0;
    //digitalWrite(tpin,0);
    delay(1);
    return dataIn[3];
}

void DFRobot_SerialMp3::pause()
{
    //digitalWrite(tpin,1);
    delay(1);
    if(this->serialMp3First) {volume(50); this->serialMp3First = false;}
    uint8_t data[4];
    data[0] = 0xAA;
    data[1] = 0x03;
    data[2] = 0x00;
    data[3] = data[0] + data[1] + data[2];
    write(data,4);
    //digitalWrite(tpin,0);
    delay(1);
}

void DFRobot_SerialMp3::stop()
{
    //digitalWrite(tpin,1);
    delay(1);
    if(this->serialMp3First) {volume(50); this->serialMp3First = false;}
    uint8_t data[4];
    data[0] = 0xAA;
    data[1] = 0x04;
    data[2] = 0x00;
    data[3] = data[0] + data[1] + data[2];
    write(data,4);
    //digitalWrite(tpin,0);
    delay(1);
}

void DFRobot_SerialMp3::end()
{
    //digitalWrite(tpin,1);
    delay(1);
    if(this->serialMp3First) {volume(50); this->serialMp3First = false;}
    uint8_t data[4];
    data[0] = 0xAA;
    data[1] = 0x10;
    data[2] = 0x00;
    data[3] = data[0] + data[1] + data[2];
    write(data,4);
    //digitalWrite(tpin,0);
    delay(1);
}

void DFRobot_SerialMp3::prev()
{
    //digitalWrite(tpin,1);
    delay(1);
    if(this->serialMp3First) {volume(50); this->serialMp3First = false;}
    uint8_t data[4];
    data[0] = 0xAA;
    data[1] = 0x05;
    data[2] = 0x00;
    data[3] = data[0] + data[1] + data[2];
    write(data,4);
    //digitalWrite(tpin,0);
    delay(1);
}

void DFRobot_SerialMp3::next()
{
    //digitalWrite(tpin,1);
    delay(1);
    if(this->serialMp3First) {volume(50); this->serialMp3First = false;}
    uint8_t data[4];
    data[0] = 0xAA;
    data[1] = 0x06;
    data[2] = 0x00;
    data[3] = data[0] + data[1] + data[2];
    write(data,4);
    //digitalWrite(tpin,0);
    delay(1);
}

void DFRobot_SerialMp3::volumeUp()
{
    //digitalWrite(tpin,1);
    delay(1);
    if(this->serialMp3First) {volume(50); this->serialMp3First = false;}
    uint8_t data[4];
    data[0] = 0xAA;
    data[1] = 0x14 ;
    data[2] = 0x00;
    data[3] = data[0] + data[1] + data[2];
    write(data,4);
    //digitalWrite(tpin,0);
    delay(1);
}

void DFRobot_SerialMp3::volumeDown()
{
    //digitalWrite(tpin,1);
    delay(1);
    if(this->serialMp3First) {volume(50); this->serialMp3First = false;}
    uint8_t data[4];
    data[0] = 0xAA;
    data[1] = 0x15 ;
    data[2] = 0x00;
    data[3] = data[0] + data[1] + data[2];
    write(data,4);
    //digitalWrite(tpin,0);
    delay(1);
}

void DFRobot_SerialMp3::write(uint8_t data[], uint16_t len)
{
    if(_serialH){
        _serialH->write(data, len);
    }else if(_serialS){
        _serialS->write(data, len);
    }
}
int DFRobot_SerialMp3::available(){
    if(_serialH){
        return _serialH->available();
    }else if(_serialS){
        return _serialS->available();
    }
    return 0;
}
int DFRobot_SerialMp3::read()
{
    if(_serialH){
        return _serialH->read();
    }else if(_serialS){
        return _serialS->read();
    }
    return -1;
}