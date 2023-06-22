/*********************************************************************
* DFRobot_DS0469.cpp
*
* Copyright (C)    2017   [DFRobot](http://www.dfrobot.com),
* GitHub Link :https://github.com/DFRobot/Gravity-I2C-SD2405-RTC-Module/
* This Library is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* Description:Get real-time clock data
*
* Product Links：https://www.dfrobot.com/wiki/index.php/Gravity:_I2C_SD2405_RTC_Module_SKU:_DFR0469
*
* Sensor driver pin：I2C
*
* author  :  Jason(jason.ling@dfrobot.com)
* version :  V1.0
* date    :  2017-04-18
**********************************************************************/

#include "DFRobot_DS0469.h"
#include "Arduino.h"
#include "Wire.h"

const uint8_t daysInMonth [] PROGMEM = { 31,28,31,30,31,30,31,31,30,31,30,31 };
//********************************************************************************************
// Function Name: setup()
// Function Declaration: Initializing sensor
//********************************************************************************************
void DFRobot_DS0469::begin()
{
	Wire.begin();
}


//********************************************************************************************
// Function Name: update()
// Function Declaration: Update the sensor value
//********************************************************************************************
void DFRobot_DS0469::read()
{
	readRtc();
	processRtc();	
}

void DFRobot_DS0469::setTime(const __FlashStringHelper* date, const __FlashStringHelper* time)
{
	char buff[11];
    memcpy_P(buff, date, 11);
    year = conv2d(buff + 9);
    // Jan Feb Mar Apr May Jun Jul Aug Sep Oct Nov Dec
    switch (buff[0]) {
        case 'J': month = buff[1] == 'a' ? 1 : month = buff[2] == 'n' ? 6 : 7; break;
        case 'F': month = 2; break;
        case 'A': month = buff[2] == 'r' ? 4 : 8; break;
        case 'M': month = buff[2] == 'r' ? 3 : 5; break;
        case 'S': month = 9; break;
        case 'O': month = 10; break;
        case 'N': month = 11; break;
        case 'D': month = 12; break;
    }
    day = conv2d(buff + 4);
    memcpy_P(buff, time, 8);
    hour = conv2d(buff);
    minute = conv2d(buff + 3);
    second = conv2d(buff + 6);

	week = dayOfTheWeek();

	setTime(year,month,day,hour,minute,second);
}

//********************************************************************************************
// Function Name: initRtc()
// Function Declaration: Initializing RTC Clock
// setTime(2017,6,19,1,12,7,0);  //Set Time: 2017/June/19th/Monday/12:07:0 am
//********************************************************************************************
void DFRobot_DS0469::setTime(uint16_t year,uint8_t month,uint8_t day,uint8_t hour,uint8_t minute,uint8_t second)
{
	year = constrain(year, 2000, 2099);
	WriteTimeOn();
	day = constrain(day, 1, getDaysOfMonth(year, month));
	int week = getWeek(year, month, day);
	year -= 2000;
	Wire.beginTransmission(RTC_Address);
	Wire.write(char(0));//Set the address for writing       
	Wire.write(this->decTobcd(second));
	Wire.write(this->decTobcd(minute));
	Wire.write(this->decTobcd(hour + 80));      // +80: sets 24 hours format
	Wire.write(this->decTobcd(week));       // days values come from 0 to 6: Sunday, Monday, Tuesday, Wednesday, Thursday, Friday, Saturday
	Wire.write(this->decTobcd(day));
	Wire.write(this->decTobcd(month));
	Wire.write(this->decTobcd(year));
	Wire.endTransmission();

	Wire.beginTransmission(RTC_Address);
	Wire.write(0x12);   //Set the address for writing       
	Wire.write(char(0));
	Wire.endTransmission();

	WriteTimeOff();
}

int DFRobot_DS0469::getDaysOfMonth(int year, int month)
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
int DFRobot_DS0469::getWeek(int iY, int iM, int iD) 
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

//********************************************************************************************
// Function Name: readRtc()
// Function Declaration: Read RTC Time
//********************************************************************************************
void DFRobot_DS0469::readRtc()
{
	unsigned char n = 0;

	Wire.requestFrom(RTC_Address, 7);
	while (Wire.available())
	{
		date[n++] = Wire.read();
	}
	delayMicroseconds(1);
	Wire.endTransmission();
}


//********************************************************************************************
// Function Name: processRtc()
// Function Declaration: Analysis RTC Data from readRtc
//********************************************************************************************
void DFRobot_DS0469::processRtc()
{
	unsigned char i;

	for (i = 0; i<7; i++)
	{
		if (i != 2)
			date[i] = (((date[i] & 0xf0) >> 4) * 10) + (date[i] & 0x0f);
		else
		{
			date[2] = (date[2] & 0x7f);
			date[2] = (((date[2] & 0xf0) >> 4) * 10) + (date[2] & 0x0f);
		}
	}
	year   = date[6] + 2000;
	month  = date[5];
	day    = date[4];
	week   = date[3];
	hour   = date[2];
	minute = date[1];
	second = date[0];

}


//********************************************************************************************
// Function Name: decTobcd()
// Function Declaration: Decimal turn to BCD
//********************************************************************************************
uint8_t DFRobot_DS0469::decTobcd(uint8_t num)
{
	return ((num / 10 * 16) + (num % 10));
}


void DFRobot_DS0469::WriteTimeOn(void)
{
	Wire.beginTransmission(RTC_Address);
	Wire.write(0x10);//Set the address for writing as 10H       
	Wire.write(0x80);//Set WRTC1=1      
	Wire.endTransmission();

	Wire.beginTransmission(RTC_Address);
	Wire.write(0x0F);//Set the address for writing as OFH       
	Wire.write(0x84);//Set WRTC2=1,WRTC3=1      
	Wire.endTransmission();
}

void DFRobot_DS0469::WriteTimeOff(void)
{
	Wire.beginTransmission(RTC_Address);
	Wire.write(0x0F);   //Set the address for writing as OFH        
	Wire.write(0);//Set WRTC2=0,WRTC3=0      
	Wire.write(0);//Set WRTC1=0  
	Wire.endTransmission();
}

uint8_t DFRobot_DS0469::conv2d(const char* p)
{
	uint8_t v = 0;
    if ('0' <= *p && *p <= '9')
        v = *p - '0';
    return 10 * v + *++p - '0';
}

uint16_t DFRobot_DS0469::date2days(uint16_t y, uint8_t m, uint8_t d)
{
	if (y >= 2000)
        y -= 2000;
    uint16_t days = d;
    for (uint8_t i = 1; i < m; ++i)
        days += pgm_read_byte(daysInMonth + i - 1);
    if (m > 2 && y % 4 == 0)
        ++days;
    return days + 365 * y + (y + 3) / 4 - 1;
}

uint8_t DFRobot_DS0469::dayOfTheWeek()
{
	uint16_t day = date2days(this->year, this->month, this->day);
    return (day + 6) % 7; // Jan 1, 2000 is a Saturday, i.e. returns 6
}

int DFRobot_DS0469::getTime(TYPE node)
{
    this->read();
    if(YEAR==node)
    {
        return this->year;
    }
    if(MONTH==node)
    {
        return this->month;
    }
    if(DATE==node)
    {
        return this->day;
    }
    if(WEEK==node)
    {
        return this->week;
    }
    if(HOUR==node)
    {
        return this->hour;
    }
    if(MINUTE==node)
    {
        return this->minute;
    }
    if(SECONDS==node)
    {
        return this->second;
    }
    return -1;
}