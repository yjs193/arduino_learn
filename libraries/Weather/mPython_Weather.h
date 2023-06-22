#ifndef WEATHER_H
#define WEATHER_H
#include <vector>
#include <string>
#include <iostream>
#include <ArduinoJson.h>
#include "DFRobot_CHfont.h"

#if defined(ARDUINO_ARCH_ESP8266)
#include "ESP8266WiFi.h"
#else
#include <WiFi.h>
#endif

class mPython_Weather
{
  public:
    mPython_Weather();

    void init(String serverAddr, String _appid, String _appsecret);
    bool connectServer();
    String getWeather(String type,String city);
    int UnicodeToUtf8(unsigned long unic, unsigned char *pOutput, int outSize);

  private:
    String host;
    String appid;
    String appsecret;
    
    WiFiClient client;
    String split(const char* str, const char* spl);
    
  

  
};

#endif
