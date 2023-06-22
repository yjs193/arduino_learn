/*
  DS1307.h - library for DS1307 rtc
*/

// ensure this library description is only included once
#ifndef __DFROBOT_DS1307_H__
#define __DFROBOT_DS1307_H__

// include types & constants of Wiring core API
#include "Arduino.h"
#include "Wire.h"


#define DS1307_BASE_YR 2000

#define DS1307_SQW1HZ  7
#define DS1307_SQW4KHZ 8
#define DS1307_SQW8KHZ 9
#define DS1307_SQW32KHZ 10

#define DS1307_CTRL_ID B1101000  //DS1307

 // Define register bit masks
#define DS1307_CLOCKHALT B10000000

#define DS1307_LO_BCD  B00001111
#define DS1307_HI_BCD  B11110000

#define DS1307_HI_SEC  B01110000
#define DS1307_HI_MIN  B01110000
#define DS1307_HI_HR   B00110000
#define DS1307_LO_DOW  B00000111
#define DS1307_HI_DATE B00110000
#define DS1307_HI_MTH  B00110000
#define DS1307_HI_YR   B11110000


// Prescaler
#define DS1307_LOW_BIT		B00000000
#define DS1307_HIGH_BIT		B10000000
#define DS1307_SQW1HZ_BIT	B00010000
#define DS1307_SQW4KHZ_BIT	B00010001
#define DS1307_SQW8KHZ_BIT	B00010010
#define DS1307_SQW32KHZ_BIT	B00010011

// library interface description
class DFRobot_DS1307
{
public:
  enum TYPE{
    DS1307_SEC = 0,
    DS1307_MIN ,
    DS1307_HR ,
    DS1307_DOW ,
    DS1307_DATE ,
    DS1307_MTH ,
    DS1307_YR
    };
  // user-accessible "public" interface
  public:
    DFRobot_DS1307();
    void get(int *, boolean);
    int get(int, boolean);
    void set(int, int);
    void start(void);
    void stop(void);
    void SetOutput(uint8_t);
    uint8_t GetOutput(void);

  void begin();
  void setTime(int year, int month, int date, int hour, int minute, int seconds);
  int getTime(TYPE node);
  // library-accessible "private" interface
  private:
    int getWeek(int iY, int iM, int iD); // 基姆拉尔森计算公式
    int getDaysOfMonth(int year, int month);
    uint8_t rtc_bcd[7]; // used prior to read/set ds1307 registers;
    void read(void);
    void save(void);
};

#endif
