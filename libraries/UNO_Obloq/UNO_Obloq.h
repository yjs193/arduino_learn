#ifndef _MICROBIT_OBLOQ_H
#define _MICROBIT_OBLOQ_H
#include "Arduino.h"
#include "Stream.h"
#include <SimpleList.h>

#define MAXTOPICNUMBER 5


enum ObloqErrorCode {
    OBLOQ_OK                     =  0,
    OBLOQ_SERIAL_CONNECT_FAILURE = -1,
    OBLOQ_MQTT_CONNECT_FAILURE   = -2,
    OBLOQ_WIFI_CONNECT_FAILURE   = -3,
    OBLOQ_SUB_FAILURE            = -4,
    OBLOQ_PULISH_FAILURE         = -5,
    OBLOQ_STEPS_ERROR            = -6
};
enum ObloqStatusInfo {
    OBLOQ_NONE               = 1,
    OBLOQ_MQTT_CONNECT_OK    = 2,
    OBLOQ_SUB_OK             = 3,
    OBLOQ_PING_OK            = 4,
    OBLOQ_PULISH_OK          = 5,
    OBLOQ_MQTT_DISCONNECT_OK = 6,
    OBLOQ_GET_VERSION_OK     = 7,
    OBLOQ_WIFI_CONNECT_OK    = 8,
    OBLOQ_HTTP_RECEIVE_OK    = 9,
    OBLOQ_MQTT_RECEIVE_OK    = 10,
};

//typedef void (*MsgHandleDebug) (const String& debugMessage, int8_t errorCode);
typedef struct mqtt_message_info_t{
    bool available;
    String message;
} message_info_t;


class UNO_Obloq
{
public:

    enum Topicnum
    {
        topic_0 = 0,
        topic_1,
        topic_2,
        topic_3,
        topic_4
    };

    String IPAddress;

    //void registerDubugHandle(const MsgHandleDebug handle=NULL);

    void startConnect(uint8_t receive,
                      uint8_t send,
                      const String& ssid,
                      const String& pwd,
                      const String& iotId,
                      const String& iotPwd,
                      const String iotTopics[],
                      const String& serve,
                      uint32_t      port);
    void startConnect(Stream *      serial,
                      const String& ssid,
                      const String& pwd,
                      const String& iotId,
                      const String& iotPwd,
                      const String iotTopics[],
                      const String& server,
                      uint32_t      port);
    void publish(Topicnum topic_num, const String& message);
    void publish(Topicnum topic_num, int32_t i);
    void publish(Topicnum topic_num, double f, int precision = 10);
    void publish(Topicnum topic_num, const char *pCh){publish(topic_num, (const String&)pCh);}
    void publish(Topicnum topic_num, float f){publish(topic_num, (double)f);}
    void publish(Topicnum topic_num, int i){publish(topic_num, (int32_t)i);}
    void publish(Topicnum topic_num, byte i){publish(topic_num, (int32_t)i);}
    void publish(Topicnum topic_num, uint32_t i){publish(topic_num, (int32_t)i);}
    void publish(Topicnum topic_num, uint16_t i){publish(topic_num, (int32_t)i);}
    void publish(Topicnum topic_num, uint64_t i){publish(topic_num, (int32_t)i);}
    String getMessage(Topicnum topic_num);
    bool available(Topicnum topic_num);


    void startConnect(uint8_t receive,
                      uint8_t send,
                      const String&  ssid,
                      const String&  pwd,
                      const String&  ip,
                      uint32_t       port);
    void startConnect(Stream *      serial,
                      const String&  ssid,
                      const String&  pwd,
                      const String&  ip,
                      uint32_t       port);
    String get(const String& url, int timeOut);
    String get(const char *url, int timeOut){return get((String)url, timeOut);};

    String post(const String& url, const String& content, int timeOut);
    String post(const char *url, const char *content, int timeOut){return post((String)url,(String)content,timeOut);};
    String post(const char *url, const String& content, int timeOut){return post((String)url,(String)content,timeOut);};
    String post(const String& url, const char *content, int timeOut){return post((String)url,(String)content,timeOut);};

    String put(const String& url, const String& content, int timeOut);
    String put(const char *url, const char *content, int timeOut){return put((String)url,(String)content,timeOut);};
    String put(const char *url, const String& content, int timeOut){return put((String)url,(String)content,timeOut);};
    String put(const String& url, const char *content, int timeOut){return put((String)url,(String)content,timeOut);};

    UNO_Obloq();
    ~UNO_Obloq();

private:
    String http_host;
    Stream *obloqSerial;
    SimpleList<String> mqttTopicArray;
    volatile int OBLOQ_CMD;
    String OBLOQ_MESSAGE;
    //MsgHandleDebug debugHandle;
    ObloqErrorCode currentError = OBLOQ_OK;
    ObloqStatusInfo currentStatus;
    message_info_t mqtt_message[MAXTOPICNUMBER];

    ObloqErrorCode ping();
    ObloqErrorCode wifiConnect(const String& ssid, const String& pwd);
    ObloqErrorCode mqttConnect(const String& iotId,
                               const String& iotPwd,
                               const String iotTopics[],
                               const String& server,
                               uint32_t      port);
    ObloqErrorCode subTopic();
    void obloqTick();
    void parameteReset();
    void sendMessage(/*const String& steps, */String message);
    //void debugTask();
    int isIP(char str[]);
    //void debugPeek(const String& debugMessage, int8_t errorCode);
};

#endif //_MICROBIT_OBLOQ_H

