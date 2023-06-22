#ifndef DFROBOT_MICROIOTCLOUD_H
#define DFROBOT_MICROIOTCLOUD_H

#if ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include <Wire.h>

#define OBLOQ_MQTT_EASY_IOT_SERVER_CHINA        "iot.dfrobot.com.cn"
#define OBLOQ_MQTT_EASY_IOT_SERVER_GLOBAL       "mqtt.beebotte.com"
#define OBLOQ_MQTT_EASY_IOT_SERVER_EN           "iot.dfrobot.com"
#define microIoT_WEBHOOKS_URL                   "maker.ifttt.com"
#define OBLOQ_MQTT_EASY_IOT_SERVER_TK           "api.thingspeak.com" 


#define IIC_ADDRESS                             0x16
#define IIC_ADDRESS1                            0x10

#define Wifi_Status                             0x00

#define READmode                                0x00

#define SET_PARA                                0x01
#define RUN_COMMAND                             0x02
#define HTTP_Mode                               0x00

/*set para*/
#define SETWIFI_NAME                            0x01
#define SETWIFI_PASSWORLD                       0x02
#define SETMQTT_SERVER                          0x03
#define SETMQTT_PORT                            0x04
#define SETMQTT_ID                              0x05
#define SETMQTT_PASSWORLD                       0x06
#define SETHTTP_IP                              0x07
#define SETHTTP_PORT                            0x08    

/*run command*/
#define SEND_PING                               0x01
#define CONNECT_WIFI                            0x02
#define RECONNECT_WIFI                          0x03
#define DISCONECT_WIFI                          0x04
#define CONNECT_MQTT                            0x05
#define SUB_TOPIC0                              0x06
#define SUB_TOPIC1                              0x07
#define SUB_TOPIC2                              0x08
#define SUB_TOPIC3                              0x09
#define SUB_TOPIC4                              0x0A
#define PUB_TOPIC0                              0x0B
#define PUB_TOPIC1                              0x0C
#define PUB_TOPIC2                              0x0D
#define PUB_TOPIC3                              0x0E
#define PUB_TOPIC4                              0x0F
#define GET_URL                                 0x10
#define POST_URL                                0x11
#define PUT_URL                                 0x12
#define GET_VERSION                             0x13

/*read para value*/
#define READ_PING                               0x01
#define READ_WIFISTATUS                         0x02
#define READ_IP                                 0x03
#define READ_MQTTSTATUS                         0x04
#define READ_SUBSTATUS                          0x05
#define READ_TOPICDATA                          0x06
#define HTTP_REQUEST                            0x10
#define READ_VERSION                            0x12

/*para status */
#define PING_OK                                 0x01
#define WIFI_DISCONNECT                         0x00
#define WIFI_CONNECTING                         0x02
#define WIFI_CONNECTED                          0x03
#define MQTT_CONNECTED                          0x01
#define MQTT_CONNECTERR                         0x02
#define SUB_TOPIC_OK                            0x01
#define SUB_TOPIC_Ceiling                       0x02


typedef void (*MicroIotCloudCb) (String& message);


class DFRobot_MicroIotCloud 
{
  public:
    enum Servo {
        S1 = 0,
        S2
    };
    // enum Motor {
    //     M1 = 0,
    //     M2,
    //     ALL
    // };
    enum Direction {
        Forward = 0,
        Reverse,
    };
    enum TOPIC {
        Topic_0 = 0,
        Topic_1,
        Topic_2,
        Topic_3,
        Topic_4,
    };

    enum  SERVERS{
    	siot = 0X01,
        China = 0X02,
        Global = 0x03
	};

    enum Time  {
        year = 0,
        month,
        day,
        hour,
        minute,
        second
    };
    
    String RECDATA;

    void servo(Servo index, uint32_t angle);
    void motorRun(uint32_t speed, Direction dir = Forward);
    void motorStop();

    void mqttRegister(const MicroIotCloudCb handles[]);
    void wifiConnect(String ssid, String password);
    void mqttConnect(String IOT_ID, String IOT_PWD, String IOT_TOPIC, SERVERS servers, String IP = "0.0.0.0");
    void mqttAddTopic(TOPIC top, String IOT_TOPIC);
    void mqttSendMessage(String Mess, TOPIC Topic);
    void mqttSendMessage(float Message, TOPIC Topic);
    void IotInquireStatus(void);
    void IotIFTTTConfigure(String EVENT, String KEY);
    void IotIFTTTSend(String value1, String value2, String value3);
    void IotThingSpeakConfigure(String KEY);
    void IotThingSpeakSend(String field1="", String field2="", String field3="", String field4="", String field5="", String field6="", String field7="", String field8="");
    void IotThingSpeakSendbuffer(void);
    void IotThingSpeakSendPrep(uint8_t index, String value);
    
    void setTime(uint16_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t minute, uint8_t second);
    void setYear(uint16_t year);
    void setMonth(uint8_t month);
    void setDay(uint8_t day);
    void setHour(uint8_t hour);
    void setMinute(uint8_t minute);
    void setSecond(uint8_t second);
    uint16_t readTime(Time time);

    DFRobot_MicroIotCloud();
    ~DFRobot_MicroIotCloud();
    void microIotInquireStatus();
  private:

    uint8_t Wifimode;

    String microIoT_WEBHOOKS_KEY;                   
    String microIoT_WEBHOOKS_EVENT;               
    String microIoT_THINGSPEAK_KEY;               
    String microIoTStatus;                                      
    String microIoT_IP;                             
    String prep_str;

    String topic_0;
    String topic_1;
    String topic_2;
    String topic_3;
    String topic_4;

    bool flag[8];
    bool i2cEnable;
    
    void checkStatus(String cmd);
    void IotGetData(uint8_t len);
    void IotRunCommand(uint8_t cmd);
    void IotSetPara(uint8_t cmd, String para);
    void mqttParaRunCommand(uint8_t cmd, String data);

    void i2cWrite(uint8_t addr, uint8_t* buf, uint16_t length);
    void i2cRead(uint8_t addr, uint8_t* buf, uint16_t length);
};

extern DFRobot_MicroIotCloud microIotCloud;


#endif // __MICROBIT_IOT