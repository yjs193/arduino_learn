#ifndef DFROBOT_MAQUEENPLUSV2_H
#define DFROBOT_MAQUEENPLUSV2_H
#include <Arduino.h>
#include <Wire.h>
#include <DFRobot_URM10.h>
#include <DFRobot_NeoPixel.h>

typedef void (*IRCall)(int8_t val);

class DFRobot_MaqueenPlusV2 : public DFRobot_URM10, public DFRobot_NeoPixel
{
public:

    enum Dir {
        LEFT = 1,
        RIGHT = 2,
        ALL = 3
    };

    enum SpeedDir {
        CW = 0,
        CCW = 1
    };

    enum Patrol {
        L1 = 1,
        M = 2,
        R1 = 3,
        L2 = 4,
        R2 = 5
    };

    enum Color {
        OFF = 0,
        ON = 1
    };

    uint8_t recvpin;
    bool enable;
    uint32_t irData;
    uint32_t irEventData;

    IRCall IRcb = NULL;
    void enableIRIn();
    void setCallback(IRCall cb);
    int8_t getData(void);


    void motorRun(Dir index, SpeedDir direction, int speed);
    void motorStop(Dir index);

    void setRGB(Dir rgb, Color color);

    uint32_t readPatrol(Patrol patrol);
    uint32_t readPatrolVoltage(Patrol patrol);
    String readVersion();

    void IRbegin(int recvpin1);
    void decode(uint32_t code);
    DFRobot_MaqueenPlusV2();
    ~DFRobot_MaqueenPlusV2();

protected:
    bool first;
	uint8_t _speedLeft;
	uint8_t _speedRight;
    void i2cWriteBuffer(int addr, unsigned char *p, int len);
    void i2cReadBuffer(uint8_t reg, unsigned char* p, int len);
};

#endif

