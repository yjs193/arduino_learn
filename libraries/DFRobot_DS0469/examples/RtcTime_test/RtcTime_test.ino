#include "GravityRtc.h"
#include "Wire.h"  

GravityRtc rtc;     //RTC Initialization

void setup() {
  Serial.begin(9600);
  
  
  rtc.setup();

  //Set the RTC time automatically: Calibrate RTC time by your computer time
  rtc.adjustRtc(F(__DATE__), F(__TIME__)); 
  
  //Set the RTC time manually
  //rtc.adjustRtc(2017,6,19,1,12,7,0);  //Set time: 2017/6/19, Monday, 12:07:00
}

void loop() {

  rtc.showTime("Year");
  delay(1000);

}
