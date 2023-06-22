#ifndef _VortexRGBOLD_H_
#define _VortexRGBOLD_H_
#include <Arduino.h>
#include "VortexTaskOld.h"
#define NUM_LEDS 12

#define I2C_LED_ADDRESS 0b1100000
#define I2C_WRITE   0x00
#define CONTROLPOWER  0x80
#define ENABLE_MOTO 0x20
#define ENABLE_GRAY 0x40
#define ENABLE_MP3  0x10

struct VortexRGBStoreOld {
    uint8_t topStore[6][3];
    uint8_t skirtStore[6][3];
    uint8_t faceColorStore;
    uint8_t faceNumberStore;
};

class VortexRGBOld
{
//    CRGB ledS[NUM_LEDS];
    float topCurrent[6][3],topLedAcc[6][3],topTarget[6][3];
    bool TopFastLed[6];

    float skirtCurrent[6][3],skirtLedAcc[6][3],skirtTarget[6][3];
    bool skirtFastLed[6];

    uint8_t currentFaceNumber;
    uint8_t currentFaceColor;

public:
    VortexRGBStoreOld stateStore;

    int faceNumber;
    unsigned long currentTime;
    bool isBegin;
//    void beginFace();
//    void stopFace();
//    void faceAction();

    void Init();
    void topCount(uint8_t date,uint8_t red,uint8_t green,uint8_t blue,uint8_t time);
    void topAction();
    void setPower();
    void setMotorPower(char isOpen);
    void setGrayPower(char isOpen);
    void setMp3Power(char isOpen);
    void setSwitchLeftPower(char isOpen);
    void setSwitchRightPower(char isOpen);

    void effectAction();

    void skirtCount(uint8_t date,uint8_t red,uint8_t green,uint8_t blue,uint8_t time);
    void skirtAction();
    void setUserFace(uint8_t color,void *faceBuf);
    void setFace(uint8_t color,uint8_t number);

    void update();

    VortexRGBOld();

};
extern VortexRGBOld LED_OLD;
#endif
