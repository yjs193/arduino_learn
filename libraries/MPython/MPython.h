#ifndef __MPYTHON_H
#define __MPYTHON_H

#if ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include <Wire.h>
#include <DFRobot_NeoPixel.h>
#include <MPython_Display_I2C.h>


#define MSA300_I2C_ADDR 38
#define QMI8658_I2C_ADDR 107

//To the beat
#define    Whole       1
#define    Half        2
#define    Quarter     4
#define    Eighth      8
#define    Sixteenth   16
#define    Double      32
#define    Breve       64

//Play beat
enum Beat {
    BEAT_1 = 4,
    BEAT_1_2 = 2,
    BEAT_1_4 = 1,
    BEAT_3_4 = 3,
    BEAT_3_2 = 6,
    BEAT_2 = 8,
    BEAT_3 = 12,
    BEAT_4 = 16
};
//music
enum Melodies {
    DADADADUM = 0,
    ENTERTAINER,
    PRELUDE,
    ODE,
    NYAN,
    RINGTONE,
    FUNK,
    BLUES,
    BIRTHDAY,
    WEDDING,
    FUNERAL,
    PUNCHLINE,
    BADDY,
    CHASE,
    BA_DING,
    WAWAWAWAA,
    JUMP_UP,
    JUMP_DOWN,
    POWER_UP,
    POWER_DOWN,
};
//Play mode
enum MelodyOptions {
    Once = 1,
    Forever = 2,
    OnceInBackground = 4,
    ForeverInBackground = 8
};


class MPython{
public:
    MPython();
    void begin(void);
    void setTouchThreshold(uint32_t threshold = 35);
};

typedef void (*mpythonGestureHandlePtr)(void);
class MSA300{
public:
    enum Gesture {
        /**
         * Raised when shaken
         */
        Shake = 0,
        /**
         * Raised when the screen is pointing down and the board is horizontal
         */
        ScreenUp = 1,
        /**
         * Raised when the screen is pointing up and the board is horizontal
         */
        ScreenDown = 2,
        /**
         * Raised when the screen is pointing left
         */
        TiltLeft = 3,
        /**
         * Raised when the screen is pointing right
         */
        TiltRight = 4,
        /**
         * Raised when the screen is pointing left
         */
        TiltForward = 5,
        /**
         * Raised when the screen is pointing right
         */
        TiltBack = 6,
        GestureNone = 7
    };

    MSA300();

    void init(void);
    float getX(void);
    float getY(void);
    float getZ(void);
    void setOffset(int x, int y, int z);
    float getStrength(void);
    void onGesture(Gesture gesture, mpythonGestureHandlePtr body);
    bool isGesture(Gesture gesture);

private:
    float rawX, rawY, rawZ;
    uint8_t i2c_addr;
    uint8_t chip;
    int offsetX, offsetY, offsetZ;
    mpythonGestureHandlePtr GestureHandle[7];
    Gesture currentGesture = GestureNone;
    bool isGestureEnable = false;
    bool _is_shaked = false;
    float getRawX(void);
    float getRawY(void);
    float getRawZ(void);
    static void taskLoop(void *param){
        MSA300 *self = (MSA300 *)param;
        char type = ' ';
        float _last_x, _last_y, _last_z, diff_x, diff_y, diff_z;
        int _count_shaked = 0;
        while(1){
			wait:
            delay(100);//delay放前面，让其他iic设备先行
            if(self->_is_shaked){
                _count_shaked += 1;
                if(_count_shaked==5)
                    _count_shaked = 0;
            }
            float x = 0.0;
            float y = 0.0;
            float z = 0.0;

            x = self->getRawX();
            y = self->getRawY();
            z = self->getRawZ();
            if(_last_x == 0 && _last_y == 0 && _last_z == 0){
                _last_x = x; _last_y = y; _last_z = z;
                goto yyy;
            }
            diff_x = x - _last_x; diff_y = y - _last_y; diff_z = z - _last_z;
            _last_x = x; _last_y = y; _last_z = z;
            if(_count_shaked > 0)
                goto yyy;
            self->_is_shaked = (diff_x * diff_x + diff_y * diff_y + diff_z * diff_z) > 1;
            yyy:
            if(self->_is_shaked){
                self->currentGesture = Shake;
                if(type == '0') goto wait;
                type = '0';
                if(self->GestureHandle[Shake]) self->GestureHandle[Shake]();
                goto wait;
            }
            else if(x < -0.5){
                self->currentGesture = TiltForward;
                if(type == '5') goto wait;
                type='5';
                if(self->GestureHandle[TiltForward]) self->GestureHandle[TiltForward]();
            }else if(x > 0.5){
                self->currentGesture = TiltBack;
                if(type == '6')  goto wait;
                type='6';
                if(self->GestureHandle[TiltBack]) self->GestureHandle[TiltBack]();
            }else if(y < -0.5){
                self->currentGesture = TiltRight;
                if(type == '4')  goto wait;
                type='4';
                if(self->GestureHandle[TiltRight]) self->GestureHandle[TiltRight]();
            }else if(y > 0.5){
                self->currentGesture = TiltLeft;
                if(type == '3')  goto wait;
                type='3';
                if(self->GestureHandle[TiltLeft]) self->GestureHandle[TiltLeft]();
            }else if(fabs(x)<0.2 && fabs(y)<0.2 && z>0){
                self->currentGesture = ScreenDown;
                if(type == '2')  goto wait;
                type='2';
                if(self->GestureHandle[ScreenDown]) self->GestureHandle[ScreenDown]();
            }else if(fabs(x)<0.2 && fabs(y)<0.2 && z<0){
                self->currentGesture = ScreenUp;
                if(type == '1')  goto wait;
                type='1';
                if(self->GestureHandle[ScreenUp]) self->GestureHandle[ScreenUp]();
            }else{
                self->currentGesture = GestureNone;
                type = ' ';
            }
        }
    }
};


typedef void (*CBFunc)(void); 

class Button
{
public:
    Button(uint8_t _io);
    Button(uint8_t _io1, uint8_t _io2);
    bool isPressed(void);
    void setPressedCallback(CBFunc _cb);
    void setUnPressedCallback(CBFunc _cb);
private:
    CBFunc pressedCb;
    CBFunc unpressedCb;
    uint8_t io;
    uint8_t io1;
    uint8_t io2;
    uint8_t pinNum;
    bool prevState;
    
    static void taskLoop(void *param){
        Button *self = (Button *)param;
        while(1){
            bool isPressed = self->isPressed();
            if(self->prevState != isPressed){
                //Serial.printf("button change prevState=%d current=%d\n",(int)self->prevState,(int)isPressed);
                if(isPressed && self->pressedCb){
                    self->pressedCb();
                }
                if((!isPressed) && self->unpressedCb){
                    self->unpressedCb();
                }
                self->prevState = isPressed;
                //Serial.printf("button change prevState=%d\n",(int)self->prevState);
            }
            yield();
        }
    }
};

class TouchPad
{
public:
    uint16_t threshold = 35;
    TouchPad(uint8_t _io);
    bool isTouched(void);
    void setTouchedCallback(CBFunc _cb);
    void setUnTouchedCallback(CBFunc _cb);
    uint32_t read(void);
private:
    CBFunc touchedCb;
    CBFunc untouchedCb;
    uint8_t io;
    int valuePrevPrev,valuePrev,value;
    static void taskLoop(void *param){
        bool prevState = false;
        TouchPad *self = (TouchPad *)param;
        while(1){
            bool isTouched = self->isTouched();
            if(prevState != isTouched){
                if(isTouched && self->touchedCb){
                    self->touchedCb();
                }
                if((!isTouched) && self->untouchedCb){
                    self->untouchedCb();
                }
                prevState = isTouched;
            }
            delay(10);
        }
    }
};

class AnalogPin
{
public:
    AnalogPin(uint8_t _io);
    uint16_t read();
private:
    uint8_t io;
};

class RGB
{
public:
    RGB();
    void write(int8_t index, uint8_t r, uint8_t g, uint8_t b);
    void write(int8_t index, uint32_t color){write(index, 0xff&(color>>16), 0xff&(color>>8), 0xff&color);}
    void brightness(uint8_t b);
    uint8_t brightness();
private:
    int _brightness;
    uint32_t c[3];
};

class Buzz
{
public:
    Buzz();
    void on(void);
    void off(void);
    bool isOn(void);
    void setTicksTempo(uint32_t _ticks, uint32_t _tempo);
    void freq(uint32_t _freq=500);
    void freq(int _freq=500){freq((uint32_t)_freq);};
    void freq(double _freq=500.0){freq((uint32_t)_freq);};

    void freq(uint32_t _freq, Beat beat);
    void freq(int _freq, Beat beat){freq((uint32_t)_freq, beat);};
    void freq(double _freq, Beat beat){freq((uint32_t)_freq, beat);};
    void freq(uint32_t _freq, double beat);
    void freq(uint32_t _freq, uint32_t beat){freq(_freq, (double)beat);};
    void freq(uint32_t _freq, int beat){freq(_freq, (double)beat);};

    void redirect(uint32_t pin);
    void stop();
    void play(Melodies melodies, MelodyOptions options = Once);
private:
    int16_t  _pin;
    uint16_t _freq;
    bool     _on;
    bool     _playing;
    float ticks;
    float tempo;

    struct buzzMelody_t
    {
        String melody;
        int currentDuration;
        int currentOctave;
        int beatsPerMinute;
        MelodyOptions opt;
        TaskHandle_t task;
    } buzzMelody;

    void     playNextNote(String tone);
    const char* const getMelody(Melodies melody);
    static void taskLoop(void *param){
        Buzz *self = (Buzz *)param;
        self->_playing = true;
        int head=0, trail=0, index=0;
        while (self->_playing) {
            for(index; index < self->buzzMelody.melody.length(); index++){
                if(self->buzzMelody.melody[index] == '|'){
                    trail = index;
                    self->playNextNote(self->buzzMelody.melody.substring(head, trail));
                    index++;
                    head = index;
                    break;
                }
            }
            if (head >= self->buzzMelody.melody.length()) {
                if(self->buzzMelody.opt == Forever || 
                    self->buzzMelody.opt == ForeverInBackground) {
                    head=0, trail=0, index=0;
                }else{
                    break;
                }
            }
        }
        self->_playing = false;
        self->buzzMelody.task = NULL;
        vTaskDelete(NULL); // 播放完成删除任务
    }
};

extern MPython mPython;
extern Button buttonA,buttonB,buttonAB;
extern DFRobot_NeoPixel pixels;
extern MPython_Display_I2C display;
extern TouchPad touchPadP,touchPadY,touchPadT,touchPadH,touchPadO,touchPadN;
extern Buzz buzz;
extern AnalogPin light,sound;
extern RGB rgb;
extern MSA300 accelerometer;
#endif
