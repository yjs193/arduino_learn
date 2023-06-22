#ifndef __DFROBOT_SERIALPORT_H__
#define __DFROBOT_SERIALPORT_H__

#include <ArduinoJson.h>
#include <Arduino.h>

class DFRobot_SerialPort
{
private:
  String data;
  void storage(JsonObject &obj);
  String get(String key);

public:
  DFRobot_SerialPort();
  ~DFRobot_SerialPort();

  void begin(uint32_t baud);
  int available(void);
  String waitData(String message);
  void writeData(String message, String value);

};




#endif