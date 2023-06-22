#ifndef __MICROBIT_NATURAL
#define __MICROBIT_NATURAL

#if ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif
#include "Wire.h"


#include <DFRobot_NeoPixel.h>


#define IIC_ADDRESS  0x16

#define READmode     0x00
#define SET_PARA     0x01
#define RUN_COMMAND  0x02
/*set para*/
#define SETWIFI_NAME       0x01
#define SETWIFI_PASSWORLD  0x02
#define SETMQTT_SERVER     0x03
#define SETMQTT_PORT       0x04
#define SETMQTT_ID         0x05
#define SETMQTT_PASSWORLD  0x06
#define SETHTTP_IP         0x07
/*run command*/
#define CONNECT_WIFI  0x02
#define CONNECT_MQTT  0x05
#define SUB_TOPIC0    0x06
#define SUB_TOPIC1    0x07
#define SUB_TOPIC2    0x08
#define SUB_TOPIC3    0x09
#define SUB_TOPIC4    0x0A
#define PUB_TOPIC0    0x0B
#define PUB_TOPIC1    0x0C
#define PUB_TOPIC2    0x0D
#define PUB_TOPIC3    0x0E
#define PUB_TOPIC4    0x0F
#define GET_URL       0x10
#define POST_URL      0x11

/*read para value*/
#define READ_PING        0x01
#define READ_WIFISTATUS  0x02
#define READ_IP          0x03
#define READ_MQTTSTATUS  0x04
#define READ_SUBSTATUS   0x05
#define READ_TOPICDATA   0x06
#define HTTP_REQUEST     0x10
#define READ_VERSION     0x12

/*para status */
#define PING_OK            0x01
#define WIFI_DISCONNECT    0x00
#define WIFI_CONNECTING    0x02
#define WIFI_CONNECTED     0x03
#define MQTT_CONNECTED     0x01
#define MQTT_CONNECTERR    0x02
#define SUB_TOPIC_OK       0x01
#define SUB_TOPIC_Ceiling  0x02

#define OBLOQ_MQTT_EASY_IOT_SERVER_CHINA  "iot.dfrobot.com.cn"
#define OBLOQ_MQTT_EASY_IOT_SERVER_EN     "iot.dfrobot.com"
#define microIoT_WEBHOOKS_URL             "maker.ifttt.com"
#define OBLOQ_MQTT_EASY_IOT_SERVER_TK     "api.thingspeak.com"

typedef void (*MsgHandleCb) (String& message);

class DFRobot_MicroNaturalV2: public DFRobot_NeoPixel
{
    
public:
    enum BME280 {
    	TEMP=0X01,
        HUMI=0X02,
        PRESSURE=0X03
	};

    enum DIRECTION {
    	CW=0X01,
        CCW=0X02
	};

    enum TOPIC {
    	Topic_0,
        Topic_1,
        Topic_2,
        Topic_3,
        Topic_4
	};

    enum  SERVERS{
    	siot = 0X01,
        China = 0X02,
        Global = 0x03
	};

    enum CO2TVOC {
    	CO2=0X01,
        TVOC=0X02
	};

    String RECDATA;

    DFRobot_MicroNaturalV2();
    virtual ~DFRobot_MicroNaturalV2(){};

    
    void request(void);
    uint16_t readNaturalLight(void);
    String  readUltraviolet(void);
    uint16_t readSound(void);
    float readWaterTemp(void);
    int32_t readBME280(BME280 mode);
    uint16_t readTDS(void);
    uint16_t readTVOC(uint8_t mode);
    void setBaseline(uint32_t number);
    void setTDS(float k);
    
    void setOLEDShowString(uint8_t scolumn, uint8_t sleng, uint8_t srow, String str);
    void setOLEDShowString(uint8_t ncolumn, uint8_t nleng, uint8_t nrow, uint32_t number);
    void setOLEDShowString(uint8_t ncolumn, uint8_t nleng, uint8_t nrow, uint16_t number);
    void setOLEDShowString(uint8_t ncolumn, uint8_t nleng, uint8_t nrow, uint8_t number);
    void setOLEDShowString(uint8_t ncolumn, uint8_t nleng, uint8_t nrow, float number);
    void clearOLED(uint8_t valuecolumnstart, uint8_t valuecolumnstop, uint8_t valuerow);
    void clearOLEDRow(uint8_t valuerow);
    void OLEDBegin(void);

    void mototRun(DIRECTION dir, uint8_t speed);
    void mototStop(void);
    
    void setColor(uint16_t index, int rgb);
    void setColor(float index, int rgb);
    void setColor(int index, int rgb);
    void setColor(uint32_t index, int rgb);
    
    uint32_t RGBRange(uint8_t num1, uint8_t num2);

    void IoTWIFI(String ssid, String password);
    void microIoT_MQTT(String IOT_ID, String IOT_PWD, String IOT_TOPIC, SERVERS servers, String IP = "0.0.0.0");
    void inquireStatus(void);
    void registerMqttHandle(const MsgHandleCb handles[]);
    void microIoT_SendMessage(String Mess, TOPIC Topic);
    void microIoT_SendMessage(float Message, TOPIC Topic);
    void microIoT_add_topic(TOPIC top, String IOT_TOPIC);

    void microIoT_http_IFTTT(String EVENT, String KEY);
    void microIoT_http_post(String value1, String value2, String value3);

    void microIoT_ThingSpeak_configura(String KEY);
    void microIoT_http_TK_GET(String field1="", String field2="", String field3="", String field4="", String field5="", String field6="", String field7="", String field8="");
    void microIoT_http_send(void);
    void microIoT_http_Prep(uint8_t index, String value);

protected:
    void IoTSetPara(uint8_t cmd, String para);
    void IoTRunCommand(uint8_t cmd);
    void IoTGetData(uint8_t number);
    void IoTCheckStatus(String cmd);
    void IoTParaRunCommand(uint8_t cmd, String Mess);
    
    void i2cWrite(uint8_t addr, uint8_t *data, uint8_t len);
    uint8_t i2cReadBuffer(uint8_t addr, uint8_t *data, uint8_t len);
    uint8_t i2cRead(uint8_t addr, uint8_t *data, uint8_t len);
private:
    uint8_t data[22];
    String microIoTStatus;
    String microIoT_IP;
    uint8_t Wifimode;
    String topic_0;
    String topic_1;
    String topic_2;
    String topic_3;
    String topic_4;

    String microIoT_WEBHOOKS_EVENT;
    String microIoT_WEBHOOKS_KEY;

    String microIoT_THINGSPEAK_KEY;

    String prep_str;

    bool flag[8];
};


#endif // __MICROBIT_NATURAL



