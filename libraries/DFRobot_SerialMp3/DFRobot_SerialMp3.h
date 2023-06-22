#ifndef DF_SERIAL_MP3_H
#define DF_SERIAL_MP3_H
#include "SoftwareSerial.h"
#include <Arduino.h>


class DFRobot_SerialMp3
{
  public:

    DFRobot_SerialMp3();
    ~DFRobot_SerialMp3();

#if defined(NRF5)
    void begin(int rx, int tx);
#elif defined(NRF52833)
    void begin(int rx, int tx);
#else
    void begin(HardwareSerial *serial, int rx, int tx);
#endif
    void begin(SoftwareSerial *serial);
    void volume(uint8_t value);
    void playList(uint8_t value);
    void insertTracks(uint16_t value);

    void play();
    uint8_t queryPlayStatus();
    void pause();
    void stop();
    void end();
    void prev();
    void next();
    void volumeUp();
    void volumeDown();

  private:
    HardwareSerial *_serialH=NULL;
    SoftwareSerial *_serialS=NULL;
    bool serialMp3First;
    int rpin = -1;
    int tpin = -1;
    void write(uint8_t data[], uint16_t len);
    int available();
    int read();

};

#endif
