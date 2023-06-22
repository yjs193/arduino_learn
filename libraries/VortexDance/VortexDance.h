#include "Arduino.h"
#ifndef VortexDance_h
#define VortexDance_h

#include <VortexMp3.h>
#include <VortexRGB.h>
#include <VortexMotor.h>
#include <Vortex.h>
#include <DFRobot_NeoPixel.h>
#include <Wire.h>
#include "VortexTask.h"

#define DanceStateNone 0
#define DanceStateFirst1 0x11
#define DanceStateFirst2 0x12
#define DanceStateFirst3 0x13
#define DanceStateFirst4 0x14
#define DanceStateFirst5 0x15
#define DanceStateSecond1 0x21
#define DanceStateSecond2 0x22
#define DanceStateSecond3 0x23
#define DanceStateSecond4 0x24
#define DanceStateSecond5 0x25
#define DanceStateSecond6 0x26
#define DanceStateThird1 0x31
#define DanceStateThird2 0x32
#define DanceStateThird3 0x33
#define DanceStateThird4 0x34
#define DanceStateThird5 0x35
#define DanceStateThird6 0x36
#define DanceStateThird7 0x37
#define DanceStateThird8 0x38
#define DanceStateForth1 0x41
#define DanceStateForth2 0x42
#define DanceStateForth3 0x43



#define PINENL 5
#define PINENR 6
#define PINDIRL 9
#define PINDIRR 10
#define PINBLE 4
#define PIN_INPUT_LEFT 0
#define PIN_INPUT_RIGHT 1
#define IR_VALUE 100
#define IR_TIME 500
#define SWITCH_LEFT_LED 8
#define SWITCH_RIGHT_LED 12
#define FACE_NUMBER 35
#define LED_TIME 1300
#define MP3_VOLUME 80


void danceinit();
void danceupdate();
void dancestart(uint8_t role);
void dancestop();

#endif
