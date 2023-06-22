#ifndef DFROBOT_MAQUEENPLUS_H
#define DFROBOT_MAQUEENPLUS_H
#include <Arduino.h>
#include <Wire.h>
#include <DFRobot_URM10.h>
#include <DFRobot_IRremote.h>


class DFRobot_MaqueenPlus : public IRremote_Receive, public DFRobot_URM10
{
public:

    enum Dir {
        LEFT = 1,
        RIGHT = 2,
        ALL = 3
    };

    enum SpeedDir {
        CW = 1,
        CCW = 2
    };

    enum Servos {
        S1 = 1,
        S2 = 2,
        S3 = 3,
        S1_3 = 4
    };

    enum Patrol {
        L1 = 1,
        L2 = 2,
        R1 = 3,
        R2 = 4,
        L3 = 5,
        R3 = 6
    };

    enum Sonicunit {
        Centimeters,
        MicroSeconds
    };

    enum PID {
        OFF = 0,
        ON = 1
    };

    enum Color {
        RED = 1,
        GREEN = 2,
        YELLOW = 3,
        BLUE = 4,
        PINK = 5,
        CYAN = 6,
        WHITH = 7,
        PUT = 8
    };

    void hardwareReset();
    void writePid(PID pid);
    void motorRun(Dir index, SpeedDir direction, int speed);
    void motorStop(Dir index);
    void mostotCompensation(Dir motor, int speed);
    void servoRun(Servos index, uint8_t angle);
    void setRGB(Dir rgb, Color color);
    uint32_t readSpeed(Dir index);
    uint32_t readDirection(Dir index);
    uint32_t readPatrol(Patrol patrol);
    uint32_t readPatrolVoltage(Patrol patrol);
    String readVersion();
	float readeDistance(Dir index);
	void clearDistance(Dir index);

    DFRobot_MaqueenPlus();
    ~DFRobot_MaqueenPlus();

protected:
    bool first;
	uint8_t _speedLeft;
	uint8_t _speedRight;
    void i2cWriteBuffer(int addr, unsigned char *p, int len);
    void i2cReadBuffer(uint8_t reg, unsigned char* p, int len);
};

#endif

