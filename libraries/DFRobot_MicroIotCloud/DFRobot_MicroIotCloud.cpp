#include <DFRobot_MicroIotCloud.h>


DFRobot_MicroIotCloud *activeObjectV2 = NULL;
#define MAXTOPICNUMBER 5
MicroIotCloudCb microIotCloudCb[MAXTOPICNUMBER] = {NULL,NULL,NULL,NULL,NULL};

static void IoTTask();



DFRobot_MicroIotCloud::DFRobot_MicroIotCloud()
{
    activeObjectV2 = this;
    i2cEnable = false;
    memset(flag, true, 8);
    prep_str = "";

    microIoT_THINGSPEAK_KEY= "";

    microIoT_WEBHOOKS_EVENT = "";
    microIoT_WEBHOOKS_KEY = "";

    microIoTStatus = "";
    RECDATA = "";
    microIoT_IP = "0.0.0.0";
    Wifimode = 0x00;
    topic_0 = "";
    topic_1 = "";
    topic_2 = "";
    topic_3 = "";
    topic_4 = "";

    // Wire.begin();
    // memset(data, 0, 22);
}

DFRobot_MicroIotCloud::~DFRobot_MicroIotCloud()
{
    
}


void DFRobot_MicroIotCloud::servo(Servo index, uint32_t angle)
{
    angle = constrain(angle, 0, 180);
    uint8_t buf[2];
    switch(index){
        case DFRobot_MicroIotCloud::S1:
            buf[0] = 0x14;
        break;
        case DFRobot_MicroIotCloud::S2:
            buf[0] = 0x15;
        break;
        default:
            buf[0] = 0x14;
        break;
    }
    buf[1] = angle;
    i2cWrite(0x10, buf, 2);
}

void DFRobot_MicroIotCloud::motorRun(uint32_t speed, Direction dir)
{
    uint8_t buf[3];
    buf[0] = 0x00;
    buf[1] = dir == DFRobot_MicroIotCloud::Forward ? 0x01 : 0x00;
    buf[2] = speed;
    i2cWrite(0x10, buf, 3);
}

void DFRobot_MicroIotCloud::motorStop()
{
    motorRun(0);
}


void DFRobot_MicroIotCloud::mqttRegister(const MicroIotCloudCb handles[])
{
    if(handles == NULL) return;
    for(int i = 0; i < MAXTOPICNUMBER; i++){
        if(handles[i]){
            microIotCloudCb[i] = handles[i];
        }
    }
}

void DFRobot_MicroIotCloud::wifiConnect(String ssid, String password){
    create_fiber(IoTTask);
    IotSetPara(SETWIFI_NAME, ssid);
    IotSetPara(SETWIFI_PASSWORLD, password);
    IotRunCommand(CONNECT_WIFI);
    checkStatus("WiFiConnected");
    Wifimode = WIFI_CONNECTED;

}



void DFRobot_MicroIotCloud::mqttConnect(String IOT_ID, String IOT_PWD, String IOT_TOPIC, SERVERS servers, String IP)
{
    if (servers == China){
        IotSetPara(SETMQTT_SERVER, OBLOQ_MQTT_EASY_IOT_SERVER_CHINA);
    }else if (servers == Global){
        IotSetPara(SETMQTT_SERVER, OBLOQ_MQTT_EASY_IOT_SERVER_EN);
    }else{
        IotSetPara(SETMQTT_SERVER, IP);
    }
    IotSetPara(SETMQTT_PORT, "1883");
    IotSetPara(SETMQTT_ID, IOT_ID);
    IotSetPara(SETMQTT_PASSWORLD, IOT_PWD);
    Serial.println("wifi conneced ok");
    IotRunCommand(CONNECT_MQTT);
    checkStatus("MQTTConnected");
    Serial.println("mqtt connected");
    
    delay(100);
    topic_0 = IOT_TOPIC;
    mqttParaRunCommand(SUB_TOPIC0, IOT_TOPIC);
    checkStatus("SubTopicOK");
    Serial.println("sub topic ok");
    
}

void DFRobot_MicroIotCloud::mqttAddTopic(TOPIC top, String IOT_TOPIC){
    delay(3000);
    mqttParaRunCommand(((uint8_t)top + 0x06), IOT_TOPIC);
    checkStatus("SubTopicOK");
    delay(3000);
}

void DFRobot_MicroIotCloud::mqttParaRunCommand(uint8_t cmd, String data)
{
    uint8_t buf[data.length() + 4];
    buf[0] = 0x1E;
    buf[1] = RUN_COMMAND;
    buf[2] = cmd;
    buf[3] = data.length();
    for (int i = 0; i < data.length(); i++)
        buf[i + 4] = data.charAt(i);
    i2cWrite(IIC_ADDRESS, buf, data.length() + 4);
}


void DFRobot_MicroIotCloud::mqttSendMessage(String Mess, TOPIC Topic){
    uint8_t topic = 0;
    switch (Topic) {
         case Topic_0:
            topic = PUB_TOPIC0;
            break;
        case Topic_1:
            topic = PUB_TOPIC1;
            break;
        case Topic_2:
            topic = PUB_TOPIC2;
            break;
        case Topic_3:
            topic = PUB_TOPIC3;
            break;
        case Topic_4:
            topic = PUB_TOPIC4;
            break;
        default:
            break;
        }
    mqttParaRunCommand(topic, Mess);
    delay(150);
}

void DFRobot_MicroIotCloud::mqttSendMessage(float Message, TOPIC Topic){
    String Mess = (String)Message;
    uint8_t topic = 0;
    switch (Topic) {
         case Topic_0:
            topic = PUB_TOPIC0;
            break;
        case Topic_1:
            topic = PUB_TOPIC1;
            break;
        case Topic_2:
            topic = PUB_TOPIC2;
            break;
        case Topic_3:
            topic = PUB_TOPIC3;
            break;
        case Topic_4:
            topic = PUB_TOPIC4;
            break;
        default:
            break;
        }
    mqttParaRunCommand(topic, Mess);
    delay(150);
}


static void TickTopic0()
{
    String mess = activeObjectV2->RECDATA;
    microIotCloudCb[0](mess);
}

static void TickTopic1()
{
    String mess = activeObjectV2->RECDATA;
    microIotCloudCb[1](mess);
}

static void TickTopic2()
{
    String mess = activeObjectV2->RECDATA;
    microIotCloudCb[2](mess);
}

static void TickTopic3()
{
    String mess = activeObjectV2->RECDATA;
    microIotCloudCb[3](mess);
}

static void TickTopic4()
{
    String mess = activeObjectV2->RECDATA;
    microIotCloudCb[4](mess);
}

static void IoTTask()
{
    while (true)
    {
        activeObjectV2->IotInquireStatus();
        fiber_sleep(2);
    }
}

void DFRobot_MicroIotCloud::checkStatus(String cmd){
    while (true) {
        if (microIoTStatus == cmd) {
            // Serial.println("OKOK");
            return;
        }
        delay(50);
    }
}


void DFRobot_MicroIotCloud::IotInquireStatus(void)
{
    uint8_t buf[3] = {0x1E, READmode, 0x06};
    uint8_t tempId;
    uint8_t tempStatus;
    uint8_t recbuf[2];
    i2cWrite(IIC_ADDRESS, buf, 3);
    i2cRead(IIC_ADDRESS, recbuf, 2);
    tempId = recbuf[0];
    tempStatus = recbuf[1];
    switch (tempId) {
        case READ_PING:
            if (tempStatus == PING_OK) {
                microIoTStatus = "PingOK";
            }else{
                microIoTStatus = "PingERR";
            }
            break;
        case READ_WIFISTATUS:
            if (tempStatus == WIFI_CONNECTING) {
                 microIoTStatus = "WiFiConnecting";
            }else if (tempStatus == WIFI_CONNECTED) {
                microIoTStatus = "WiFiConnected";
            } else if (tempStatus == WIFI_DISCONNECT) {
                microIoTStatus = "WiFiDisconnect"; 
            }
            break;
        case READ_MQTTSTATUS:
            if (tempStatus == MQTT_CONNECTED) {
                microIoTStatus = "MQTTConnected";
            } else if (tempStatus == MQTT_CONNECTERR) {
                microIoTStatus = "MQTTConnectERR";
            }
            break;
        case READ_SUBSTATUS:
            if (tempStatus == SUB_TOPIC_OK) {
                microIoTStatus = "SubTopicOK";
            } else if (tempStatus == SUB_TOPIC_Ceiling) {
                microIoTStatus = "SubTopicCeiling";
            } else {
                microIoTStatus = "SubTopicERR";
            }
            break;
        case READ_IP:
            microIoTStatus = "READ_IP";
            IotGetData(tempStatus);
            microIoT_IP = RECDATA;
            microIoTStatus = "WiFiConnected";
            break;
        case SUB_TOPIC0:
            microIoTStatus = "READ_TOPICDATA";
            IotGetData(tempStatus);
            if(NULL != microIotCloudCb[0]){
                create_fiber(TickTopic0);
            }
            break;
        case SUB_TOPIC1:
            microIoTStatus = "READ_TOPICDATA";
            IotGetData(tempStatus);
            if(NULL != microIotCloudCb[1]){
                create_fiber(TickTopic1);
            }
            break;
        case SUB_TOPIC2:
            microIoTStatus = "READ_TOPICDATA";
            IotGetData(tempStatus);
            if(NULL != microIotCloudCb[2]){
                create_fiber(TickTopic2);
            }
            break;
        case SUB_TOPIC3:
            microIoTStatus = "READ_TOPICDATA";
            IotGetData(tempStatus);
            if(NULL != microIotCloudCb[3]){
                create_fiber(TickTopic3);
            }
            break;
        case SUB_TOPIC4:
            microIoTStatus = "READ_TOPICDATA";
            IotGetData(tempStatus);
            if(NULL != microIotCloudCb[4]){
                create_fiber(TickTopic4);
            }
            break;
        case HTTP_REQUEST:
            microIoTStatus = "HTTP_REQUEST";
            IotGetData(tempStatus);
            break;
        case READ_VERSION:
            microIoTStatus = "READ_VERSION";
            IotGetData(tempStatus);
            break;
        default:
            break;
    }
    delay(200);
}


void DFRobot_MicroIotCloud::IotIFTTTConfigure(String EVENT, String KEY){
    microIoT_WEBHOOKS_EVENT  = EVENT;
    microIoT_WEBHOOKS_KEY = KEY;
}


void DFRobot_MicroIotCloud::IotIFTTTSend(String value1, String value2, String value3){
    IotSetPara(SETHTTP_IP, microIoT_WEBHOOKS_URL);
    // String tempStr = "";
     String tempStr = "trigger/" + microIoT_WEBHOOKS_EVENT + "/with/key/" + microIoT_WEBHOOKS_KEY + ",{\"value1\":\"" + value1 + "\",\"value2\":\"" + value2 + "\",\"value3\":\"" + value3 + "\" }" + "\r";
    mqttParaRunCommand(POST_URL, tempStr);
}

void DFRobot_MicroIotCloud::IotThingSpeakConfigure(String KEY){
    microIoT_THINGSPEAK_KEY = KEY;
}


void DFRobot_MicroIotCloud::IotThingSpeakSend(String field1, String field2, String field3, String field4, String field5, String field6, String field7, String field8){
    IotSetPara(SETHTTP_IP, OBLOQ_MQTT_EASY_IOT_SERVER_TK);
    String tempStr = "update?api_key=" + microIoT_THINGSPEAK_KEY + "&field1=" + field1 ;
    if(field2 != ""){
        tempStr += "&field2=" + field2;
    }
    if(field3 != ""){
        tempStr += "&field3=" + field3;
    }
    if(field4 != ""){
        tempStr += "&field4=" + field4;
    }
    if(field5 != ""){
        tempStr += "&field5=" + field5;
    }
    if(field6 != ""){
        tempStr += "&field6=" + field6;
    }
    if(field7 != ""){
        tempStr += "&field7=" + field7;
    }
    if(field8 != ""){
        tempStr += "&field8=" + field8;
    }
    mqttParaRunCommand(GET_URL, tempStr);
}

void DFRobot_MicroIotCloud::IotThingSpeakSendbuffer(void){
    mqttParaRunCommand(GET_URL, prep_str);
    memset(flag, true, 8);
    prep_str = "";
}


void DFRobot_MicroIotCloud::IotThingSpeakSendPrep(uint8_t index, String value){
    if(prep_str == ""){
        prep_str = "update?api_key=" + microIoT_THINGSPEAK_KEY;
    }
    if(index == 1){
        if(flag[index]){
            flag[index] = false;
            prep_str += "&field1=" + value;
        }
    }
    if(index == 2){
        if(flag[index]){
            flag[index] = false;
            prep_str += "&field2=" + value;
        }
    }
    if(index == 3){
        if(flag[index]){
            flag[index] = false;
            prep_str += "&field3=" + value;
        }
    }
    if(index == 4){
        if(flag[index]){
            flag[index] = false;
            prep_str += "&field4=" + value;
        }
    }
    if(index == 5){
        if(flag[index]){
            flag[index] = false;
            prep_str += "&field5=" + value;
        }
    }
    if(index == 6){
        if(flag[index]){
            flag[index] = false;
            prep_str += "&field6=" + value;
        }
    }
    if(index == 7){
        if(flag[index]){
            flag[index] = false;
            prep_str += "&field7=" + value;
        }
    }
    if(index == 8){
        if(flag[index]){
            flag[index] = false;
            prep_str += "&field8=" + value;
        }
    }
}

void DFRobot_MicroIotCloud::IotGetData(uint8_t len)
{
    RECDATA = "";
    uint8_t tempbuf[1] = {0x22};
    i2cWrite(IIC_ADDRESS, tempbuf, 1);
    uint8_t tempRecbuf[len + 1];
    i2cRead(IIC_ADDRESS, tempRecbuf, len);
    for (int i = 0; i < len; i++) {
        RECDATA += tempRecbuf[i];
    }
    tempRecbuf[len] = '\0';
    RECDATA = (const char *)tempRecbuf;
}



void DFRobot_MicroIotCloud::IotRunCommand(uint8_t cmd)
{
    uint8_t buf[3];
    buf[0] = 0x1E;
    buf[1] = RUN_COMMAND;
    buf[2] = cmd;
    i2cWrite(IIC_ADDRESS, buf, 3);
}

void DFRobot_MicroIotCloud::IotSetPara(uint8_t cmd, String para)
{
    uint8_t buf[para.length() + 4];
    buf[0] = 0x1E;
    buf[1] = SET_PARA;
    buf[2] = cmd;
    buf[3] = para.length();
    for (int i = 0; i < para.length(); i++)
        buf[i + 4] = para.charAt(i);
    i2cWrite(IIC_ADDRESS, buf, para.length() + 4);
}

void DFRobot_MicroIotCloud::i2cWrite(uint8_t addr, uint8_t* buf, uint16_t length)
{
    if(!i2cEnable){
        Wire.begin();
        i2cEnable = true;
    }
    Wire.setClock(100000);
    Wire.beginTransmission(addr);
    while(length--) {
        Wire.write(*buf);
        buf++;
    }
    Wire.endTransmission();
    Wire.setClock(100000);
}

void DFRobot_MicroIotCloud::i2cRead(uint8_t addr, uint8_t* buf, uint16_t length)
{
    if(!i2cEnable){
        Wire.begin();
        i2cEnable = true;
    }
    Wire.setClock(100000);
    Wire.requestFrom(addr, length);
    for(int i=0; i<length; i++){
        buf[i] = Wire.read();
    }
    Wire.setClock(100000);
}



void DFRobot_MicroIotCloud::setTime(uint16_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t minute, uint8_t second){
    uint8_t buf[7];
    uint8_t cmd = 0x20;
    uint8_t buffer[7];
    uint16_t data;
    buf[0] = 0x16;
    buf[1] = year-2000;
    buf[2] = month;
    buf[3] = day;
    buf[4] = hour;
    buf[5] = minute;
    buf[6] = second;
    i2cWrite(0x10, buf, 7);
    while(1){
        i2cWrite(0x10, &cmd, 1);
        i2cRead(0x10, buffer, 7);
        data = buffer[0] + 2000;
        delay(50);
        if(data == year){
            return;
        }
    }
}

void DFRobot_MicroIotCloud::setYear(uint16_t year){
    
    uint8_t buf[7];
    uint8_t cmd = 0x20;
    uint8_t buffer[7];
    uint16_t data;
    i2cWrite(0x10, &cmd, 1);
    i2cRead(0x10, buf, 7);
    buffer[0] = 0x16;
    buffer[1] = year-2000;
    buffer[2] = buf[1];
    buffer[3] = buf[2];
    buffer[4] = buf[3];
    buffer[5] = buf[4];
    buffer[6] = buf[5];
    i2cWrite(0x10, buffer, 7);
    while(1){
        i2cWrite(0x10, &cmd, 1);
        i2cRead(0x10, buffer, 7);
        data = buffer[0] + 2000;
        delay(50);
        if(data == year){
            return;
        }
    }
}

void DFRobot_MicroIotCloud::setMonth(uint8_t month){

    uint8_t buf[7];
    uint8_t cmd = 0x20;
    uint8_t buffer[7];
    uint16_t data;
    i2cWrite(0x10, &cmd, 1);
    i2cRead(0x10, buf, 7);
    buffer[0] = 0x16;
    buffer[1] = buf[0];
    buffer[2] = month;
    buffer[3] = buf[2];
    buffer[4] = buf[3];
    buffer[5] = buf[4];
    buffer[6] = buf[5];
    i2cWrite(0x10, buffer, 7);
    while(1){
        i2cWrite(0x10, &cmd, 1);
        i2cRead(0x10, buffer, 7);
        data = buffer[1];
        delay(50);
        if(data == month){
            return;
        }
    }
}

void DFRobot_MicroIotCloud::setDay(uint8_t day){
   
    uint8_t buf[7];
    uint8_t cmd = 0x20;
    uint8_t buffer[7];
    uint16_t data;
    i2cWrite(0x10, &cmd, 1);
    i2cRead(0x10, buf, 7);
    buffer[0] = 0x16;
    buffer[1] = buf[0];
    buffer[2] = buf[1];
    buffer[3] = day;
    buffer[4] = buf[3];
    buffer[5] = buf[4];
    buffer[6] = buf[5];
    i2cWrite(0x10, buffer, 7);
    while(1){
        i2cWrite(0x10, &cmd, 1);
        i2cRead(0x10, buffer, 7);
        data = buffer[2];
        delay(50);
        if(data == day){
            return;
        }
    }
}

void DFRobot_MicroIotCloud::setHour(uint8_t hour){
  
    uint8_t buf[7];
    uint8_t cmd = 0x20;
    uint8_t buffer[7];
    uint16_t data;
    i2cWrite(0x10, &cmd, 1);
    i2cRead(0x10, buf, 7);
    buffer[0] = 0x16;
    buffer[1] = buf[0];
    buffer[2] = buf[1];
    buffer[3] = buf[2];
    buffer[4] = hour;
    buffer[5] = buf[4];
    buffer[6] = buf[5];
    i2cWrite(0x10, buffer, 7);
    while(1){
        i2cWrite(0x10, &cmd, 1);
        i2cRead(0x10, buffer, 7);
        data = buffer[3];
        delay(50);
        if(data == hour){
            return;
        }
    }
}

void DFRobot_MicroIotCloud::setMinute(uint8_t minute){
   
    uint8_t buf[7];
    uint8_t cmd = 0x20;
    uint8_t buffer[7];
    uint16_t data;
    i2cWrite(0x10, &cmd, 1);
    i2cRead(0x10, buf, 7);
    buffer[0] = 0x16;
    buffer[1] = buf[0];
    buffer[2] = buf[1];
    buffer[3] = buf[2];
    buffer[4] = buf[3];
    buffer[5] = minute;
    buffer[6] = buf[5];
    i2cWrite(0x10, buffer, 7);
    while(1){
        i2cWrite(0x10, &cmd, 1);
        i2cRead(0x10, buffer, 7);
        data = buffer[4];
        delay(50);
        if(data == minute){
            return;
        }
    }
}

void DFRobot_MicroIotCloud::setSecond(uint8_t second){
    uint8_t buf[7];
    uint8_t cmd = 0x20;
    uint8_t buffer[7];
    uint16_t data;
    i2cWrite(0x10, &cmd, 1);
    i2cRead(0x10, buf, 7);
    buffer[0] = 0x16;
    buffer[1] = buf[0];
    buffer[2] = buf[1];
    buffer[3] = buf[2];
    buffer[4] = buf[3];
    buffer[5] = buf[4];
    buffer[6] = second;
    i2cWrite(0x10, buffer, 7);
    while(1){
        i2cWrite(0x10, &cmd, 1);
        i2cRead(0x10, buffer, 7);
        data = buffer[5];
        delay(50);
        if(data != 0){
            return;
        }
    }
}

uint16_t DFRobot_MicroIotCloud::readTime(Time time){
    uint8_t buf[7];
    uint8_t cmd = 0x20;
    uint8_t buffer[7];
    uint16_t data;
    i2cWrite(0x10, &cmd, 1);
    i2cRead(0x10, buffer, 7);
    switch (time)
    {
        case DFRobot_MicroIotCloud::year:
            data = buffer[0]+2000;
            break;
        case DFRobot_MicroIotCloud::month:
            data = buffer[1];
            break;
        case DFRobot_MicroIotCloud::day:
            data = buffer[2];
            break;
        case DFRobot_MicroIotCloud::hour:
            data = buffer[3];
            break;
         case DFRobot_MicroIotCloud::minute:
            data = buffer[4];
            break;
        default:
            data = buffer[5];
            break;
    }
    return data;

}


DFRobot_MicroIotCloud microIotCloud;