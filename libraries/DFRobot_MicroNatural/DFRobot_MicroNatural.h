#ifndef __MICROBIT_NATURAL
#define __MICROBIT_NATURAL

#if ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif
#include "Wire.h"



class DFRobot_MicroNatural
{
public:

    DFRobot_MicroNatural(){};
    virtual ~DFRobot_MicroNatural(){};

protected:

    bool                  init = false;

    virtual unsigned char getAddress() = 0;
    virtual unsigned long getClock() = 0;

    void                writeReg(unsigned char reg, unsigned char data);
    unsigned char       readReg(unsigned char reg);
    unsigned short int  readRegUint16(unsigned char reg);

private:
    volatile unsigned char iicAddress = 0x00;
};


class MicroNatural_TDS : public DFRobot_MicroNatural
{

#define REG_STM32_K_INTEGER  0X04
#define REG_SEM32_K_DECIMAL  0X05
#define REG_STM32_TDS_H      0X06
#define REG_SEM32_TDS_L      0X07

public:

    MicroNatural_TDS(){};
    ~MicroNatural_TDS(){};

    float          readK();
    void           setK(float k=2.68);
    uint32_t       read();

private:

    const unsigned char iicAddress = 0X1F;
    const unsigned char iicFrequency = 100000;

    unsigned char getAddress() override;
    unsigned long getClock() override;
};


class MicroNatural_Sound : public DFRobot_MicroNatural
{

#define REG_SEM32_NOISE_H  0X08
#define REG_STM32_NOISE_L  0X09

public:

    MicroNatural_Sound(){};
    ~MicroNatural_Sound(){};

    uint32_t   read();

private:

    const unsigned char iicAddress = 0X1F;
    const unsigned char iicFrequency = 100000;

    unsigned char getAddress() override;
    unsigned long getClock() override;
};


class MicroNatural_LED : public DFRobot_MicroNatural
{

#define REG_SEM32_LED_CONTROL  0X03
#define REG_SEM32_LED_ON       0X01
#define REG_SEM32_LED_OFF      0X00

public:

    MicroNatural_LED(){};
    ~MicroNatural_LED(){};

    void   set(bool sw = true);

private:

    const unsigned char iicAddress = 0X1F;
    const unsigned char iicFrequency = 100000;

    unsigned char getAddress() override;
    unsigned long getClock() override;
};


class MicroNatural_UV : public DFRobot_MicroNatural
{

#define REG_STM32_UV_INTEGER  0X0A
#define REG_SEM32_UV_DECIMAL  0X0B

public:

    MicroNatural_UV(){};
    ~MicroNatural_UV(){};

    float   read();

private:

    const unsigned char iicAddress = 0X1F;
    const unsigned char iicFrequency = 100000;

    unsigned char getAddress() override;
    unsigned long getClock() override;
};


class MicroNatural_RGBC : public DFRobot_MicroNatural
{

#define REG_TCS34725_ID           0x12
#define REG_TCS34725_COMMAND_BIT  0x80
#define REG_TCS34725_ENABLE       0X00
#define REG_TCS34725_ATIME        0X01
#define REG_TCS34725_GAIN         0x0F
#define REG_CLEAR_CHANNEL_L       0X14
#define REG_RED_CHANNEL_L         0X16
#define REG_GREEN_CHANNEL_L       0X18
#define REG_BLUE_CHANNEL_L        0X1A
#define TCS34725_ENABLE_AIEN      0X10

public:

    MicroNatural_RGBC();
    ~MicroNatural_RGBC();

    uint32_t   red();
    uint32_t   green();
    uint32_t   blue();
    uint32_t   naturalLight();

private:

    struct rgbcRaw_t
    {
        uint16_t r;
        uint16_t g;
        uint16_t b;
        uint16_t c;
    } rgbcRaw;

    bool                init;
    const unsigned char iicAddress;
    const unsigned char iicFrequency;

    unsigned char getAddress() override;
    unsigned long getClock() override;

    bool config();
    void flush();
};

#endif // __MICROBIT_NATURAL



