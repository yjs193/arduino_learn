//Dfrobot_UltrasonicIIC.h
#ifndef __Dfrobot_UltrasonicIIC__
#define __Dfrobot_UltrasonicIIC__
#include <Arduino.h>
#include <Wire.h>
#define MEASURE_MODE_PASSIVE (0x00)
#define MEASURE_RANG_500 (0x20)
#define CMD_DISTANCE_MEASURE (0x01)
typedef enum
{
	SLAVEADDR_INDEX = 0,
	PID_INDEX,
	VERSION_INDEX,
	DIST_H_INDEX,
	DIST_L_INDEX,
	TEMP_H_INDEX,
	TEMP_L_INDEX,
	CFG_INDEX,
	CMD_INDEX,
	REG_NUM
} regindexTypedef;
class Dfrobot_UltrasonicIIC
{
public:
	Dfrobot_UltrasonicIIC();
	uint8_t begin(uint8_t addr);

	float getTemperature();
	float getDistanceCm();

	uint8_t i2cWriteBytes(unsigned char Reg, unsigned char *pdata);
	void i2cReadBytes(unsigned char Reg);
	void changeAddr(unsigned char addr1);

	uint8_t scan();
	bool checkProductID();

private:
	unsigned char txbuf[10];
	unsigned char rxbuf[10];
	unsigned char _Reg;
	unsigned char addr0;
	unsigned char _addr1;
};
#endif