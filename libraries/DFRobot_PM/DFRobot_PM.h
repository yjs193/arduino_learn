#ifndef _DFROBOT_PM_H_
#define _DFROBOT_PM_H_

#include "Arduino.h"
#include "SoftwareSerial.h"
#include "HardwareSerial.h"


class DFRobot_PM
{
  public:
    DFRobot_PM();
    ~DFRobot_PM();

#if defined(NRF5)
    void begin(int rx, int tx);
#elif defined NRF52833
    void begin(int rx, int tx);
#else
    void begin(HardwareSerial *serial, int rx, int tx);
#endif
    void begin(SoftwareSerial *serial);

    int readPM01();
	int readPM2_5();
	int readPM10();

	private:
    HardwareSerial *_serialH=NULL;
    SoftwareSerial *_serialS=NULL;
	int _PM01Value;
	int _PM2_5Value;
	int _PM10Value;
	unsigned char _buf[31];
    int   transmitPM01(unsigned char *buf);
    int   transmitPM2_5(unsigned char *buf);
    int   transmitPM10(unsigned char *buf);
	char  checkValue(unsigned char *buf,uint8_t len);
	void  readBuf();
};

#endif





















