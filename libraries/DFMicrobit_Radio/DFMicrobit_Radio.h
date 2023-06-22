#ifndef DF_MICROBIT_RADIO_H
#define DF_MICROBIT_RADIO_H
#include "Arduino.h"

typedef void (*onDataStrPacketReceivedCb) (String message);


class DFMicrobit_Radio
{
  public:
  String      staticRadioMessage;

  DFMicrobit_Radio();
  ~DFMicrobit_Radio();

  int         turnOn(),
              turnOff();

  void        setGroup(uint8_t group),
              setCallback(onDataStrPacketReceivedCb cb),
              send(String msg),
              send(int32_t i),
              send(uint32_t i),
              send(double f, int precision = 10);
  void        send(int i){ send((int32_t)i); }

  String      getMessage();


  private:

  int         group;

};

extern DFMicrobit_Radio Radio;

#endif
