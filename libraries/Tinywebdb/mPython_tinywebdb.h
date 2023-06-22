#ifndef TINYWEBDB_H
#define TINYWEBDB_H
#if defined(ARDUINO_ARCH_ESP8266)
#include "ESP8266WiFi.h"
#include <ESP8266HTTPClient.h>
#else
#include <WiFi.h>
#include <HTTPClient.h>
#endif
#include <ArduinoJson.h>


class mPython_TinyWebDB
{
  public:
  mPython_TinyWebDB();  

  void setServerParameter(String apiAddr,String userName,String secretWord);
  void updateTagValue(String tag,String value);
  void updateTagValue(String tag,float value);
  void updateTagValue(String tag,double value);
  void updateTagValue(String tag,uint32_t value);

  String testInternetConnect();


  
  String getTag(String tag);
  void deleteTag(String tag);
  int count();
  String searchTags(String startNum,String num,String tag="",String type="both");
  String searchTags(double startNum,double num,String tag="",String type="both");
  String searchTags(String startNum,double num,String tag="",String type="both");
  String searchTags(double startNum,String num,String tag="",String type="both");
  
  //bool connectServer();
  String getMsg();

  private:
  String api;
  String user;
  String secret;
  String host;
  String url;
  HTTPClient client;
#if defined(ARDUINO_ARCH_ESP8266)
  WiFiClient _wificlient_tinywebdb;
#endif
  
};
#endif
