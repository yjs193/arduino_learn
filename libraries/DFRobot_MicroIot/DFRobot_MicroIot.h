#ifndef __MICROBIT_IOT
#define __MICROBIT_IOT

#if ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include <Wire.h>
#include <DFRobot_SSD1306_I2C.h>
#include<vector>
#include <iostream>
using namespace std;

#define SET_PARA           0x01
#define SETWIFI_NAME       0x01
#define SETWIFI_PASSWORLD  0x02
#define SETMQTT_SERVER     0x03
#define SETMQTT_PORT       0x04
#define SETMQTT_ID         0x05
#define SETMQTT_PASSWORLD  0x06
#define SETHTTP_IP         0x07
#define SETHTTP_PORT       0x08

#define SEND_PING          0x01
#define CONNECT_WIFI       0x02
#define RECONNECT_WIFI     0x03
#define DISCONECT_WIFI     0x04
#define CONNECT_MQTT       0x05

constexpr uint8_t SUB_TOPIC_CMD[5] = {0x06, 0x07, 0x08, 0x09, 0x0A};
constexpr uint8_t PUB_TOPIC_CMD[5] = {0x0B, 0x0C, 0x0D, 0x0E, 0x0F};

#define GET_URL            0x10
#define POST_URL           0x11
#define PUT_URL            0x12
#define GET_VERSION        0x13

#define READ_STATUS        0x00
#define SET_PARA           0x01
#define RUN_COMMAND        0x02

/*read para value*/
#define READ_PING          0x01
#define READ_WIFISTATUS    0x02
#define READ_IP            0x03
#define READ_MQTTSTATUS    0x04
#define READ_SUBSTATUS     0x05
#define READ_TOPICDATA     0x06
#define HTTP_REQUEST       0x10
#define READ_VERSION       0x12

/*para status */
#define PING_ERR           0x00
#define PING_OK            0x01
#define WIFI_DISCONNECT    0x00
#define WIFI_CONNECTING    0x02
#define WIFI_CONNECTED     0x03
#define MQTT_CONNECTED     0x01
#define MQTT_CONNECTERR    0x02
#define SUB_TOPIC_OK       0x01
#define SUB_TOPIC_Ceiling  0x02
#define SUB_TOPIC_ERR      0x03

typedef void (*MicroIotCb) (String& message);


class DFRobot_MicroIot 
{
  public:
    enum Servo {
        S1 = 0,
        S2
    };
    enum Motor {
        M1 = 0,
        M2,
        ALL
    };
    enum Direction {
        Forward = 0,
        Reverse,
    };
    enum Topicnum  {
        topic_0 = 0,
        topic_1,
        topic_2,
        topic_3,
        topic_4
    };

    void servo(Servo index, uint32_t angle);
    void motorRun(Motor index, uint32_t speed, Direction dir = Forward);
    void motorStop(Motor index);

    void mqttRegister(const MicroIotCb handles[]);
    void mqttConnect(const String& ssid,
                     const String& pwd,
                     const String& iotId,
                     const String& iotPwd,
                     const String iotTopics[],
                     const String& servers);
    void mqttPublish(Topicnum topic, const String& message);
    void mqttPublish(Topicnum topic, int32_t i);
    void mqttPublish(Topicnum topic, double f, int precision = 10);
    void mqttPublish(Topicnum topic, const char *pCh){mqttPublish(topic, (const String&)pCh);}
    void mqttPublish(Topicnum topic, float f){mqttPublish(topic, (double)f);}
    void mqttPublish(Topicnum topic, int i){mqttPublish(topic, (int32_t)i);}
    void mqttPublish(Topicnum topic, byte i){mqttPublish(topic, (int32_t)i);}
    void mqttPublish(Topicnum topic, uint32_t i){mqttPublish(topic, (int32_t)i);}
    void mqttPublish(Topicnum topic, uint16_t i){mqttPublish(topic, (int32_t)i);}
    void mqttPublish(Topicnum topic, uint64_t i){mqttPublish(topic, (int32_t)i);}

    DFRobot_MicroIot();
    ~DFRobot_MicroIot();
    void microIotInquireStatus();
  private:
    bool i2cEnable;
    vector<String> mqttTopicArray;

    void mqttSetPara(uint8_t cmd, const String& para);
    void mqttRunCommand(uint8_t cmd);
    void runCallback(Topicnum topic);
    void i2cWrite(uint8_t addr, uint8_t* buf, uint16_t length);
    void i2cRead(uint8_t addr, uint8_t* buf, uint16_t length);
    void checkStatus(String cmd);

    void mqttGetData(uint8_t len);
    void mqttParaRunCommand(uint8_t cmd, const String& data);
};

extern DFRobot_MicroIot microIot;


#endif // __MICROBIT_IOT