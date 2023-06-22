#ifndef __MICROBIT_TOUCH
#define __MICROBIT_TOUCH

#if ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include <Wire.h>
#include<vector>
#include <iostream>
using namespace std;


typedef void (*TouchKeyAction) (void);

class DFRobot_MicroTouch 
{
  public:
    enum Servo {
        S1 = 0,
        S2
    };
    enum Motor {
        M1 = 0,
        M2,
        ALL
    };
    enum Direction {
        Forward = 0,
        Backward,
    };
    enum KeyValue {
        //% block="key 1"
        key1 = 0x80,
        //% block="key 2"
        key2 = 0x40,
        //% block="key 3"
        key3 = 0x20,
        //% block="key +"
        keyplus = 0x10,
        //% block="key 4"
        key4 = 0x200,
        //% block="key 5"
        key5 = 0x100,
        //% block="key 6"
        key6 = 0x400,
        //% block="key -"
        keyminus = 0x800,
        //% block="key 7"
        key7 = 0x4000,
        //% block="key 8"
        key8 = 0x1000,
        //% block="key 9"
        key9 = 0x2000,
        //% block="key ×"
        keymul = 0x8000,
        //% block="key DF"
        keydf = 0x01,
        //% block="key 0"
        key0 = 0x02,
        //% block="key ="
        keyequal = 0x04,
        //% block="key ÷"
        keydiv = 0x08,
        //% block="key any"
        keyany = 0xFFFF
    };

    typedef struct
    {
        KeyValue         key;
        TouchKeyAction   action;
        bool             isTouch;
        Fiber*           fiber;
        uint32_t         time;
    } KV;
    vector<KV> kbCallback;
    int8_t eventCount = 0;

    void begin();

    void servo(Servo index, uint32_t angle);
    void motorRun(Motor index, uint32_t speed, Direction dir = Forward);
    void motorStop(Motor index);
    void vibrationMotor(bool sw);
    
    int32_t readKeyBasc();
    int32_t readNumKeyMath();
    String  readFunKeyMath();
    bool    keyPressed(KeyValue kv);
    void    touchKeyRegister(KeyValue kv, const TouchKeyAction action);

    uint32_t coorToPixel(uint32_t x, uint32_t y);
    uint16_t i2cRead16(uint8_t addr);
    void runCallback(uint8_t index);

    DFRobot_MicroTouch();
    ~DFRobot_MicroTouch();

  private:
    bool i2cEnable;
    int prevKey = -1;
    int key = -1;
    int keyRow = 0;
    int mathKeyNumber = -1;
    String mathKeyFunction = "none";
    bool newKeyFlag = true;

    void i2cWrite(uint8_t addr, uint8_t* buf, uint16_t length);
    void i2cRead(uint8_t addr, uint8_t* buf, uint16_t length);
};

extern DFRobot_MicroTouch microTouch;


#endif // __MICROBIT_TOUCH