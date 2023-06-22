
#ifndef __DFRobot_Esp8266WIFI_H
#define __DFRobot_Esp8266WIFI_H

#include "ESP8266WiFi.h"
#include <ritos.h>

#define WIFI_SUCCESSFULLY                    0
#define IOT_WL_CONNECTION_LOST              -1
#define IOT_WL_NO_SSID_AVAIL                -2
#define IOT_WL_CONNECT_FAILED               -3
#define IOT_WL_DISCONNECTED                 -4


typedef enum {
    WIFI_MODE_NULL = 0,  /**< null mode */
    WIFI_MODE_STA,       /**< WiFi station mode */
    WIFI_MODE_AP,        /**< WiFi soft-AP mode */
    WIFI_MODE_APSTA,     /**< WiFi station + soft-AP mode */
    WIFI_MODE_MAX
} wifi_mode_t;

class wifiStatusProtocol
{
public:
    static const int8_t NoneStatus          = 0;

    static const int8_t WifiConnectStart    = 1;
    static const int8_t WifiConnecting      = 2;
    static const int8_t WifiConnectEnd      = 3;
    static const int8_t WifiConnected       = 4;
};


typedef void (*DebugMsgHandleCb) (String& debug, int32_t error);


class DFRobot_Esp8266WIFI
{
public:

  DFRobot_Esp8266WIFI(void);
  ~DFRobot_Esp8266WIFI(void);

  void wifiConnect(String account, String password);
  boolean wifiStatus();
  void wifiDisconnect();
  String getWiFiLocalIP();
  String getWiFiNetmask();
  String getGateway();
  String getDNS();
  boolean setSoftAP(String ssid, String pass);
  String getWiFiSoftIP();

  void setIotDebugCallback(const DebugMsgHandleCb handleCb);

  DebugMsgHandleCb iot_debugMsgHandleCb = NULL;

  String account;
  String password;
  struct iotDebug_t
  {
      String message;
      int32_t error;
      Ritos tarefa1;
      Ritos tarefa2;     
  } iotDebug;

private:
  
  
};

extern void esp8266_executeCallbackTask(String message, int32_t error);

#endif
