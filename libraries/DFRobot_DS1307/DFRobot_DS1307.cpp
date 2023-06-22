#include "Wire.h"
#include "DFRobot_DS1307.h"

DFRobot_DS1307::DFRobot_DS1307()
{
#if !defined(ARDUINO_ARCH_RP2040)
  Wire.begin();
#endif
}

// PRIVATE FUNCTIONS

// Aquire data from the RTC chip in BCD format
// refresh the buffer
void DFRobot_DS1307::read(void)
{
  // use the Wire lib to connect to tho rtc
  // reset the resgiter pointer to zero
  Wire.beginTransmission(DS1307_CTRL_ID);
  Wire.write((uint8_t)0x00);
  Wire.endTransmission();

  // request the 7 bytes of data    (secs, min, hr, dow, date. mth, yr)
  Wire.requestFrom(DS1307_CTRL_ID, 7);
  for(int i=0; i<7; i++)
  {
    // store data in raw bcd format
    rtc_bcd[i]=Wire.read();
  }
}

// update the data on the IC from the bcd formatted data in the buffer
void DFRobot_DS1307::save(void)
{
  Wire.beginTransmission(DS1307_CTRL_ID);
  Wire.write((uint8_t)0x00); // reset register pointer
  for(int i=0; i<7; i++)
  {
    Wire.write(rtc_bcd[i]);
  }
  Wire.endTransmission();
}


// PUBLIC FUNCTIONS
void DFRobot_DS1307::get(int *rtc, boolean refresh)   // Aquire data from buffer and convert to int, refresh buffer if required
{
  if(refresh) read();
  for(int i=0;i<7;i++)  // cycle through each component, create array of data
  {
    rtc[i]=get(i, 0);
  }
}

int DFRobot_DS1307::get(int c, boolean refresh)  // aquire individual RTC item from buffer, return as int, refresh buffer if required
{
  if(refresh) read();
  int v=-1;
  switch(c)
  {
  case DS1307_SEC:
    v=(10*((rtc_bcd[DS1307_SEC] & DS1307_HI_SEC)>>4))+(rtc_bcd[DS1307_SEC] & DS1307_LO_BCD);
    break;
  case DS1307_MIN:
    v=(10*((rtc_bcd[DS1307_MIN] & DS1307_HI_MIN)>>4))+(rtc_bcd[DS1307_MIN] & DS1307_LO_BCD);
    break;
  case DS1307_HR:
    v=(10*((rtc_bcd[DS1307_HR] & DS1307_HI_HR)>>4))+(rtc_bcd[DS1307_HR] & DS1307_LO_BCD);
    break;
  case DS1307_DOW:
    v=rtc_bcd[DS1307_DOW] & DS1307_LO_DOW;
    break;
  case DS1307_DATE:
    v=(10*((rtc_bcd[DS1307_DATE] & DS1307_HI_DATE)>>4))+(rtc_bcd[DS1307_DATE] & DS1307_LO_BCD);
    break;
  case DS1307_MTH:
    v=(10*((rtc_bcd[DS1307_MTH] & DS1307_HI_MTH)>>4))+(rtc_bcd[DS1307_MTH] & DS1307_LO_BCD);
    break;
  case DS1307_YR:
    v=(10*((rtc_bcd[DS1307_YR] & DS1307_HI_YR)>>4))+(rtc_bcd[DS1307_YR] & DS1307_LO_BCD)+DS1307_BASE_YR;
    break;
  } // end switch
  return v;
}

void DFRobot_DS1307::set(int c, int v)  // Update buffer, then update the chip
{
  switch(c)
  {
  case DS1307_SEC:
    if(v<60 && v>-1)
    {
    //preserve existing clock state (running/stopped)
    int state=rtc_bcd[DS1307_SEC] & DS1307_CLOCKHALT;
    rtc_bcd[DS1307_SEC]=state | ((v / 10)<<4) + (v % 10);
    }
    break;
  case DS1307_MIN:
    if(v<60 && v>-1)
    {
    rtc_bcd[DS1307_MIN]=((v / 10)<<4) + (v % 10);
    }
    break;
  case DS1307_HR:
  // TODO : AM/PM  12HR/24HR
    if(v<24 && v>-1)
    {
    rtc_bcd[DS1307_HR]=((v / 10)<<4) + (v % 10);
    }
    break;
  case DS1307_DOW:
    if(v<8 && v>-1)
    {
    rtc_bcd[DS1307_DOW]=v;
    }
    break;
  case DS1307_DATE:
    if(v<32 && v>-1)
    {
    rtc_bcd[DS1307_DATE]=((v / 10)<<4) + (v % 10);
    }
    break;
  case DS1307_MTH:
    if(v<13 && v>-1)
    {
    rtc_bcd[DS1307_MTH]=((v / 10)<<4) + (v % 10);
    }
    break;
  case DS1307_YR:
    if(v<100 && v>-1)
    {
    rtc_bcd[DS1307_YR]=((v / 10)<<4) + (v % 10);
    }
    break;
  } // end switch
  save();
}

void DFRobot_DS1307::stop(void)
{
  // set the ClockHalt bit high to stop the rtc
  // this bit is part of the seconds byte
  rtc_bcd[DS1307_SEC]=rtc_bcd[DS1307_SEC] | DS1307_CLOCKHALT;
  save();
}

void DFRobot_DS1307::start(void)
{
  // unset the ClockHalt bit to start the rtc
  // TODO : preserve existing seconds
  rtc_bcd[DS1307_SEC]=rtc_bcd[DS1307_SEC] & (~DS1307_CLOCKHALT);
  save();
}


void DFRobot_DS1307::SetOutput(uint8_t c)  
{
  uint8_t out;
  switch(c)
  {
  case HIGH :
    out=DS1307_HIGH_BIT;
  break;
  case LOW :
    out=DS1307_LOW_BIT;
  break;
  case DS1307_SQW1HZ :
    out=DS1307_SQW1HZ_BIT;
  break;
  case DS1307_SQW4KHZ :
    out=DS1307_SQW4KHZ_BIT;
  break;
  case DS1307_SQW8KHZ :
    out=DS1307_SQW8KHZ_BIT;
  break;
  case DS1307_SQW32KHZ :
    out=DS1307_SQW32KHZ_BIT;
  break;
  default: 
    out=DS1307_LOW_BIT;
  }
  
  Wire.beginTransmission(DS1307_CTRL_ID);
  Wire.write((uint8_t)0x07); 
  Wire.write(out);
  Wire.endTransmission();
}

uint8_t DFRobot_DS1307::GetOutput(void)  
{
  Wire.beginTransmission(DS1307_CTRL_ID);
  Wire.write((uint8_t)0x07);
  Wire.endTransmission();

  Wire.requestFrom(DS1307_CTRL_ID, 1);
  uint8_t out=Wire.read();
  
  int c=-1;
  switch(out)
  {
  case DS1307_HIGH_BIT :
    c=HIGH;
  break;
  case DS1307_LOW_BIT :
    c=LOW;
  break;
  case DS1307_SQW1HZ_BIT :
    c=DS1307_SQW1HZ;
  break;
  case DS1307_SQW4KHZ_BIT :
    c=DS1307_SQW4KHZ;
  break;
  case DS1307_SQW8KHZ_BIT :
    c=DS1307_SQW8KHZ;
  break;
  case DS1307_SQW32KHZ_BIT :
    c=DS1307_SQW32KHZ;
  break;
  }
  return c;
}

void DFRobot_DS1307::begin(){
  Wire.begin();
}

void DFRobot_DS1307::setTime(int year, int month, int date, int hour, int minute, int seconds){
  year = constrain(year, 2000, 2099);
  date = constrain(date, 1, getDaysOfMonth(year, month));
  int week = getWeek(year, month, date);
  week = constrain(week, 0, 6);
  year -= 2000;
  month = constrain(month, 1, 12);
  hour = constrain(hour, 0, 23);
  minute = constrain(minute, 0, 59);
  seconds = constrain(seconds, 0, 59);
  stop();
  set(DS1307_SEC, seconds);
  set(DS1307_MIN, minute);
  set(DS1307_HR, hour);
  set(DS1307_DOW, week);
  set(DS1307_DATE, date);
  set(DS1307_MTH, month);
  set(DS1307_YR, year);
  start();
}

int DFRobot_DS1307::getTime(TYPE node){
  int ret[7];
  get(ret, true);

  if(DS1307_YR == node){
    return ret[6];
  }else if(DS1307_MTH == node){
    return ret[5];
  }else if(DS1307_DATE == node){
    return ret[4];
  }else if(DS1307_DOW == node){
    return ret[3];
  }else if(DS1307_HR == node){
    return ret[2];
  }else if(DS1307_MIN == node){
    return ret[1];
  }else if(DS1307_SEC == node){
    return ret[0];
  }
  return 0;
}

int DFRobot_DS1307::getDaysOfMonth(int year, int month)
{
  switch (month) {
    case 1:
    case 3:
    case 5:
    case 7:
    case 8:
    case 10:
    case 12:
      return 31;
    case 4:
    case 6:
    case 9:
    case 11:
      return 30;
    case 2:
      return ((year % 4 == 0 && year % 100 != 0) || year % 400 == 0) ? 29 : 28;
    default:
      return 0;
  }
}

// 基姆拉尔森计算公式
int DFRobot_DS1307::getWeek(int iY, int iM, int iD) 
{
  int iWeekDay = -1; 
  if (1 == iM || 2 == iM) 
  {
    iM += 12; 
    iY--;
  }
  iWeekDay = (iD + 1 + 2 * iM + 3 * (iM + 1) / 5 + iY + iY / 4 - iY / 100 + iY / 400) % 7;
  return iWeekDay;
}
