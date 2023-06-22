
#include "DFRobot_PM.h"
#if defined(NRF5)
#include <basic.h>
uart SerialPm(0, 0);
#elif defined NRF52833
#include <basic.h>
uart SerialPm(0, 0);
#endif

DFRobot_PM::DFRobot_PM()
{
}

DFRobot_PM::~DFRobot_PM()
{
}

#if defined(NRF5)
void DFRobot_PM::begin(int rx, int tx){
  rx = g_PinID[rx];
  tx = g_PinID[tx];
  MicroBitPin *rxpin = getMicroBitPin(rx);
  MicroBitPin *txpin = getMicroBitPin(tx);
  uBit.serial.redirect(txpin->name, rxpin->name);
  _serialH = &SerialPm;
  _serialS = NULL;
  _serialH->begin(9600);
  _serialH->setTimeout(1500);
}
#elif defined NRF52833
void DFRobot_PM::begin(int rx, int tx){
  rx = g_PinID[rx];
  tx = g_PinID[tx];
  MicroBitPin *rxpin = getMicroBitPin(rx);
  MicroBitPin *txpin = getMicroBitPin(tx);
  uBit.serial.redirect(*txpin, *rxpin);
  _serialH = &SerialPm;
  _serialS = NULL;
  _serialH->begin(9600);
  _serialH->setTimeout(1500);
}
#else
void DFRobot_PM::begin(HardwareSerial *serial, int rx, int tx){
  _serialH = serial;
  _serialS = NULL;
#if defined(ESP_PLATFORM)
  _serialH->begin(9600,rx,tx);
#else
  _serialH->begin(9600);
#endif
  _serialH->setTimeout(1500);
}
#endif
void DFRobot_PM::begin(SoftwareSerial *serial){
  _serialH = NULL;
  _serialS = serial;
  _serialS->begin(9600);
  _serialS->setTimeout(1500);
}

int DFRobot_PM::readPM01()
{
	readBuf();
	return this->_PM01Value;
}

int DFRobot_PM::readPM2_5()
{
	readBuf();
	return this->_PM2_5Value;
}

int DFRobot_PM::readPM10()
{
	readBuf();
	return this->_PM10Value;
}

char DFRobot_PM::checkValue(unsigned char *buf,uint8_t len)
{
	char receiveflag = 0;
	int receiveSum = 0;
	for(int i = 0; i < (len-2); i++){
		receiveSum += buf[i];
	}
	receiveSum += 0x42;
	if(receiveSum == ((buf[len-2] << 8) + buf[len - 1])){
		receiveSum = 0;
		receiveflag = 1;
	}
	return receiveflag;
}

void DFRobot_PM::readBuf(){
	if(_serialH){
		if(_serialH->find(0x42)){
			_serialH->readBytes(this->_buf,31);
			if(this->_buf[0] == 0x4d){
				if(checkValue(this->_buf,31)){
					this->_PM01Value = transmitPM01(this->_buf);
					this->_PM2_5Value = transmitPM2_5(this->_buf);
					this->_PM10Value = transmitPM10(this->_buf);
				}
			}
		}
	}else if(_serialS){
		if(_serialS->find(0x42)){
			_serialS->readBytes(this->_buf,31);
			if(this->_buf[0] == 0x4d){
				if(checkValue(this->_buf,31)){
					this->_PM01Value = transmitPM01(this->_buf);
					this->_PM2_5Value = transmitPM2_5(this->_buf);
					this->_PM10Value = transmitPM10(this->_buf);
				}
			}
		}
	}
}

int DFRobot_PM::transmitPM01(unsigned char *buf){
	int PM01Val;
	PM01Val = ((buf[3]<<8) + buf[4]);
	return PM01Val;
}

int DFRobot_PM::transmitPM2_5(unsigned char *buf){
	int PM2_5Val;
	PM2_5Val = ((buf[5]<<8) + buf[6]);
	return PM2_5Val;
}

int DFRobot_PM::transmitPM10(unsigned char *buf){
	int PM10Val;
	PM10Val=((buf[7]<<8) + buf[8]);
	return PM10Val;
}

