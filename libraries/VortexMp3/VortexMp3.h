//
//  VortexMp3
//  
//
//  Created by kelvin on 14-10-30.
//
//

#ifndef _VortexMp3_H_
#define _VortexMp3_H_
#include "Arduino.h"
#include "SoftwareSerial1.h"
#define TX 11
#define RX 2
static SoftwareSerial mySerial(RX, TX);// RX, TX

class VortexMp3
{
public:
    void init();
    void repeatPlayer(byte data);
    void play(byte data);
    void playDanceMusic(byte data);
    void stop();
    void setVolume(byte vol);
    void setMode(byte isRepeat);

};
extern VortexMp3 player;
#endif




