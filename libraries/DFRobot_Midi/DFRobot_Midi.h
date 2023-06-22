#ifndef __DFRobot_MIDI_H__
#define __DFRobot_MIDI_H__

#include <Arduino.h>
#include <stdint.h>
#include <Wire.h>
#include <stdio.h>
#include "SoftwareSerial.h"
#include <HardwareSerial.h>

class DFRobot_Midi 
{
  public:
#if defined(NRF5)
    void begin(int rx, int tx);
#elif defined(NRF52833)
    void begin(int rx, int tx);
#else
    void begin(HardwareSerial *serial, int rx, int tx);
#endif
    void begin(SoftwareSerial *serial);
    void noteOn(int channel, int pitch, int velocity);
  private:
    HardwareSerial *_serialH=NULL;
    SoftwareSerial *_serialS=NULL;
    int rpin = -1;
    int tpin = -1;
    void write(int data);
};

#endif