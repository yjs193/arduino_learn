#ifndef __DFROBOT_IOT__
#define __DFROBOT_IOT__

#include "Arduino.h"
#include <PubSubClient.h>
#include <WiFi.h>


#include <ArduinoJson.h>

#include<vector>
#include <iostream>
using namespace std;


#define ALIYUN 0
#define ONENET 1
#define ONENET_NEW 2
#define MAXTOPICNUMBER 5

#define IOT_SUCCESSFULLY                     0;
#define IOT_WL_CONNECTION_LOST              -1;
#define IOT_WL_NO_SSID_AVAIL                -2;
#define IOT_WL_CONNECT_FAILED               -3;
#define IOT_WL_DISCONNECTED                 -4;
#define IOT_MQTT_CONNECTION_TIMEOUT         -5;
#define IOT_MQTT_CONNECTION_LOST            -6;
#define IOT_MQTT_CONNECT_FAILED             -7;
#define IOT_MQTT_CONNECT_BAD_PROTOCOL       -8;
#define IOT_MQTT_CONNECT_BAD_CLIENT_ID      -9;
#define IOT_MQTT_CONNECT_UNAVAILABLE        -10;
#define IOT_MQTT_CONNECT_BAD_CREDENTIALS    -11;
#define IOT_MQTT_CONNECT_UNAUTHORIZED       -12;
#define IOT_MQTT_DISCONNECTED               -13;

class iotStatusProtocol
{
public:
    static const int8_t NoneStatus          = 0;

    static const int8_t WifiConnectStart    = 1;
    static const int8_t WifiConnecting      = 2;
    static const int8_t WifiConnectEnd      = 3;
    static const int8_t WifiConnected       = 4;

    static const int8_t MqttConnectStart    = 5;
    static const int8_t MqttConnecting      = 6;
    static const int8_t MqttConnectEnd      = 7;
    static const int8_t MqttConnected       = 8;
};

typedef void (*DebugMsgHandleCb) (String& debug, int32_t error);
typedef void (*MsgHandleCb) (String& message);
typedef void (*HandleCb) (void);

enum Topicnum
{
    topic_0 = 0,
    topic_1,
    topic_2,
    topic_3,
    topic_4
};

class DFRobot_Iot{
  public:
    DFRobot_Iot(void);
    ~DFRobot_Iot(void);
    /*ONENET*/
    void init(String OneNetServer,
              String OneNetProductID, String OneNetDeviceID,
              String OneNetApiKey, const String iotTopics[],
              uint16_t OneNetPort = 6002);
    /*Aliyun*/
    void init(String AliyunServer, String AliProductKey, 
              String AliClientId, String AliDeviceName, 
              String AliDeviceSecret, const String iotTopics[],
              uint16_t AliPort = 1883);
    void setConfig();
    void connect();
    boolean connected();
    void disconnect();
    void setMqttCallback(const MsgHandleCb handles[]);
    void setMqttCallback(Topicnum topic_num, const String delimiters, const HandleCb handles);
    void publish(Topicnum topic_num, const String& message, uint8_t qos = 0);
    void publish(Topicnum topic_num, int64_t i, uint8_t qos = 0);
    void publish(Topicnum topic_num, uint64_t i, uint8_t qos = 0){publish(topic_num, (int64_t)i, qos);}
    void publish(Topicnum topic_num, int8_t i, uint8_t qos = 0){publish(topic_num, (int64_t)i, qos);}
    void publish(Topicnum topic_num, uint8_t i, uint8_t qos = 0){publish(topic_num, (int64_t)i, qos);}
    void publish(Topicnum topic_num, int16_t i, uint8_t qos = 0){publish(topic_num, (int64_t)i, qos);}
    void publish(Topicnum topic_num, uint16_t i, uint8_t qos = 0){publish(topic_num, (int64_t)i, qos);}
    void publish(Topicnum topic_num, int32_t i, uint8_t qos = 0){publish(topic_num, (int64_t)i, qos);}
    void publish(Topicnum topic_num, uint32_t i, uint8_t qos = 0){publish(topic_num, (int64_t)i, qos);}
    void publish(Topicnum topic_num, long unsigned int i, uint8_t qos = 0){publish(topic_num, (int64_t)i, qos);}
    void publish(Topicnum topic_num, double f, uint8_t qos = 0, int precision = 10);
    void publish(Topicnum topic_num, const char *pCh, uint8_t qos = 0){publish(topic_num, (const String&)pCh, qos);}
    void publish(Topicnum topic_num, char *pCh, uint8_t qos = 0){publish(topic_num, (const String&)pCh, qos);}  
    // void publishSave(Topicnum topic_num, const String& message, uint8_t qos = 0){publish(topic_num, message, qos);} 
    // void publishSave(Topicnum topic_num, char *pCh, uint8_t qos = 0){publish(topic_num, (const String&)pCh, qos);}
    void setIotDebugCallback(const DebugMsgHandleCb handleCb);
    void wifiConnect(String account, String password);
    boolean wifiStatus();
    void wifiDisconnect();
    String getWiFiLocalIP();
    String getWiFiNetmask();
    String getGateway();
    String getDNS();
    boolean setSoftAP(String ssid, String pass);
    String getWiFiSoftIP();

    uint8_t _UseServer = ALIYUN;
    /*wifi*/
    String account;
    String password;
    /*onenet*/
    String _ApiKey;
    String _ProductID;
    String _DeviceID;
    /*aliyun*/
    String _ProductKey;
    String _ClientId;
    String _DeviceName;
    String _DeviceSecret;
    /*public*/
    String _MQTTSERVER;
    uint16_t _port;
    char * _mqttServer;
    char * _clientId;
    char * _username;
    char * _password;
    MsgHandleCb    mqtt_msgHandleCb[5] = {NULL,NULL,NULL,NULL,NULL};
    DebugMsgHandleCb  iot_debugMsgHandleCb = NULL;
    vector<HandleCb> mqtt_handleCb[MAXTOPICNUMBER];
    vector<String> mqtt_delimiters[MAXTOPICNUMBER];
    vector<String> mqtt_topicArray;

    struct iotDebug_t
    {
        String message;
        int32_t error;
        TaskHandle_t task;
    } iotDebug;

  private:

};

extern WiFiClient espClient;

extern PubSubClient client;

#endif
