#include "DFRobot_Midi.h"
#if defined(NRF5)
#include <basic.h>
#endif

static SoftwareSerial* MIDISerial;

#if defined(NRF5)
void DFRobot_Midi::begin(int rx, int tx){
  rpin = rx;
  tpin = tx;
  rx = g_PinID[rx];
  tx = g_PinID[tx];
  MicroBitPin *rxpin = getMicroBitPin(rx);
  MicroBitPin *txpin = getMicroBitPin(tx);
  uBit.serial.redirect(txpin->name, rxpin->name);
  _serialH = new uart(0, 0);
  _serialS = NULL;
  _serialH->begin(31250);
}
#elif defined(NRF52833)
void DFRobot_Midi::begin(int rx, int tx){
  rpin = rx;
  tpin = tx;
  rx = g_PinID[rx];
  tx = g_PinID[tx];
  MicroBitPin *rxpin = getMicroBitPin(rx);
  MicroBitPin *txpin = getMicroBitPin(tx);
  uBit.serial.redirect(*txpin, *rxpin);
  _serialH = new uart(0, 0);
  _serialS = NULL;
  _serialH->begin(31250);
}
#else
void DFRobot_Midi::begin(HardwareSerial *serial, int rx, int tx){
  rpin = rx;
  tpin = tx;
  _serialH = serial;
  _serialS = NULL;
#if defined(ESP_PLATFORM)
  _serialH->begin(31250,rx,tx);
#else
  _serialH->begin(31250);
#endif
}
#endif

void DFRobot_Midi::begin(SoftwareSerial *serial){
  _serialH = NULL;
  _serialS = serial;
  _serialS->begin(31250);
  rpin = _serialS->getReceivePin();
  tpin = _serialS->getTransmitPin();
}

void DFRobot_Midi::noteOn(int channel, int pitch, int velocity)
{
  channel = constrain(channel, 1, 16) + 0x90 - 1;
  write(channel);
  write(pitch);
  write(velocity);
}

void DFRobot_Midi::write(int data)
{
    if(_serialH){
        _serialH->write(data);
    }else if(_serialS){
        _serialS->write(data);
    }
}