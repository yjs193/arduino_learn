#include <Wire.h>
#include <DFRobot_DS1307.h>
DFRobot_DS1307 RTC;

#define TIME_YR    2017
#define TIME_MTH   9
#define TIME_DATE  6
#define TIME_DOW   16
#define TIME_HR    23
#define TIME_MIN   59
#define TIME_SEC   45
char* str[] = {"Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday", "Sunday"};

void showtime()
{
    Serial.print( RTC.getTime(RTC.DS1307_YR) );
    Serial.print("-");    Serial.print( RTC.getTime(RTC.DS1307_MTH));
    Serial.print("-");    Serial.println( RTC.getTime(RTC.DS1307_DATE));
    /*HOUR  MIN SEC */
    Serial.print(  RTC.getTime(RTC.DS1307_HR) );
    Serial.print(":");    Serial.print( RTC.getTime(RTC.DS1307_MIN));
    Serial.print(":");    Serial.println( RTC.getTime(RTC.DS1307_SEC));
    /********WEEK*****/
    Serial.println( str[RTC.getTime(RTC.DS1307_DOW) - 1]);
    Serial.println("**********");
}
void setup()
{ 
  Serial.begin(9600);
  RTC.begin();
  Serial.println(" OldTime:  ");
  showtime();
  RTC.setTime(TIME_YR,TIME_MTH,TIME_DATE, TIME_DOW, TIME_HR, TIME_MIN,TIME_SEC);
  Serial.println(" SetTime:  ");
  showtime();
}
void loop()
{
  showtime();
  delay(1000);
}

