/*
  DS1307.h - library for DS1307 rtc
*/

// ensure this library description is only included once
#ifndef __DFROBOT_SHIFTOUT_H__
#define __DFROBOT_SHIFTOUT_H__

// include types & constants of Wiring core API
#include "Arduino.h"

typedef uint8_t   BYTE; 

// library interface description
class DFRobot_ShiftOut
{
public:
  DFRobot_ShiftOut();
  void begin(uint8_t data, uint8_t clk, uint8_t latch, uint8_t bitorder);
  void write(String segment);
  void write(uint8_t segment);
  void led(int value);
  void dot(bool on);
  uint8_t shiftParseOne(uint8_t a);
  uint8_t shiftParseDouble(uint8_t a, uint8_t b);
private:
  BYTE Tab[11];
  uint8_t _data;
  uint8_t _clk;
  uint8_t _latch;
  uint8_t _segment;
  uint8_t _bitorder;
};
#endif
