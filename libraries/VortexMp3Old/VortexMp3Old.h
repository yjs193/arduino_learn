//
//  VortexMp3Old
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

class VortexMp3Old
{
public:
    void init();
    void repeatPlayer(byte data);
    void play(byte data);
    void stop();
    void setVolume(byte vol);
};
extern VortexMp3Old player_old;
#endif




