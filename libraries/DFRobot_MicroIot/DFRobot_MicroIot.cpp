#include <DFRobot_MicroIot.h>


String microIoTStatus = "";
String microIoT_IP = "0.0.0.0";
String RECDATA = "";
DFRobot_MicroIot *activeObject = NULL;
static void microIotEvent();
static void MicroIotTickTopic0();
static void MicroIotTickTopic1();
static void MicroIotTickTopic2();
static void MicroIotTickTopic3();
static void MicroIotTickTopic4();
bool mqttBusy[5] = {false,false,false,false,false};
MicroIotCb microIotCb[5] = {NULL,NULL,NULL,NULL,NULL};
Fiber *mqttRegisteredFiber[5];


DFRobot_MicroIot::DFRobot_MicroIot()
{
    activeObject = this;
    i2cEnable = false;
}

DFRobot_MicroIot::~DFRobot_MicroIot()
{
    
}


void DFRobot_MicroIot::servo(Servo index, uint32_t angle)
{
    angle = constrain(angle, 0, 180);
    uint8_t buf[2];
    switch(index){
        case DFRobot_MicroIot::S1:
            buf[0] = 0x14;
        break;
        case DFRobot_MicroIot::S2:
            buf[0] = 0x15;
        break;
        default:
            buf[0] = 0x14;
        break;
    }
    buf[1] = angle;
    i2cWrite(0x16, buf, 2);
}

void DFRobot_MicroIot::motorRun(Motor index, uint32_t speed, Direction dir)
{
    uint8_t buf[3];
    buf[2] = speed;
    switch(index){
        case DFRobot_MicroIot::M1:
            buf[0] = 0x00;
            buf[1] = dir == DFRobot_MicroIot::Forward ? 0x01 : 0x00;
        break;
        case DFRobot_MicroIot::M2:
            buf[0] = 0x02;
            buf[1] = dir == DFRobot_MicroIot::Forward ? 0x00 : 0x01;
        break;
        case DFRobot_MicroIot::ALL:
            buf[0] = 0x00;
            buf[1] = dir == DFRobot_MicroIot::Forward ? 0x01 : 0x00;
            i2cWrite(0x16, buf, 3);
            buf[0] = 0x02;
            buf[1] = dir == DFRobot_MicroIot::Forward ? 0x00 : 0x01;
        break;
        default:
            buf[0] = 0x00;
        break;
    }
    i2cWrite(0x16, buf, 3);
}

void DFRobot_MicroIot::motorStop(Motor index)
{
    motorRun(index, 0);
}


void DFRobot_MicroIot::mqttRegister(const MicroIotCb handles[])
{
    if(handles == NULL) return;
    for(int i = 0; i < 5; i++){
        if(handles[i]){
            microIotCb[i] = handles[i];
        }
    }
}

void DFRobot_MicroIot::mqttConnect(const String& ssid,
                                   const String& pwd,
                                   const String& iotId,
                                   const String& iotPwd,
                                   const String iotTopics[],
                                   const String& servers)
{
    for(uint8_t i=0; i<5; i++){
        if(iotTopics[i] != "")
            mqttTopicArray.push_back(iotTopics[i]);
    }
    create_fiber(microIotEvent);
    mqttSetPara(SETWIFI_NAME, ssid);
    mqttSetPara(SETWIFI_PASSWORLD, pwd);
    mqttSetPara(SETMQTT_SERVER, servers);
    mqttSetPara(SETMQTT_PORT, "1883");
    mqttSetPara(SETMQTT_ID, iotId);
    mqttSetPara(SETMQTT_PASSWORLD, iotPwd);
    mqttRunCommand(CONNECT_WIFI);
    checkStatus("WiFiConnected");
    mqttRunCommand(CONNECT_MQTT);
    checkStatus("MQTTConnected");
    for(uint8_t i=0; i<mqttTopicArray.size(); i++){
        mqttParaRunCommand(SUB_TOPIC_CMD[i], mqttTopicArray[i]);
        checkStatus("SubTopicOK");
    }
}

void DFRobot_MicroIot::mqttPublish(Topicnum topic, const String& message)
{
    mqttParaRunCommand(PUB_TOPIC_CMD[topic], message);
}

void DFRobot_MicroIot::mqttPublish(Topicnum topic, double f, int precision)
{
    String str;
    str = String(f,5);
    char *p = (char *)(str.c_str() + str.length());
    while(*p == '\0'|| *p == '0'){
        *p = '\0';
        p--;
        delay(1);
    }
    if(*p == '.')
        *p = '\0';
    if(str == "-0")
        str = "0";
    mqttPublish(topic, (const String&)str);
}

void DFRobot_MicroIot::mqttPublish(Topicnum topic, int32_t i)
{
    char buffer[34];
    memset(buffer, 0, 34);
    itoa(i, buffer, 10);
    mqttPublish(topic, (const String&)buffer);
}

void DFRobot_MicroIot::runCallback(Topicnum topic)
{
    if (topic == DFRobot_MicroIot::topic_0) {
        if(!microIotCb[0]) return;
        if(mqttBusy[0]){
            release_fiber(mqttRegisteredFiber[0]);
            mqttBusy[0] = false;
        }
        Fiber *f = create_fiber(MicroIotTickTopic0);
        if(!f) return;
        mqttRegisteredFiber[0] = f;
        mqttBusy[0] = true;
    } else if(topic == DFRobot_MicroIot::topic_1) {
        if(!microIotCb[1]) return;
        if(mqttBusy[1]){
            release_fiber(mqttRegisteredFiber[1]);
            mqttBusy[1] = false;
        }
        Fiber *f = create_fiber(MicroIotTickTopic1);
        if(!f) return;
        mqttRegisteredFiber[1] = f;
        mqttBusy[1] = true;
    } else if(topic == DFRobot_MicroIot::topic_2) {
        if(!microIotCb[2]) return;
        if(mqttBusy[2]){
            release_fiber(mqttRegisteredFiber[2]);
            mqttBusy[2] = false;
        }
        Fiber *f = create_fiber(MicroIotTickTopic2);
        if(!f) return;
        mqttRegisteredFiber[2] = f;
        mqttBusy[2] = true;
    } else if(topic == DFRobot_MicroIot::topic_3) {
        if(!microIotCb[3]) return;
        if(mqttBusy[3]){
            release_fiber(mqttRegisteredFiber[3]);
            mqttBusy[3] = false;
        }
        Fiber *f = create_fiber(MicroIotTickTopic3);
        if(!f) return;
        mqttRegisteredFiber[3] = f;
        mqttBusy[3] = true;
    } else if(topic == DFRobot_MicroIot::topic_4) {
        if(!microIotCb[4]) return;
        if(mqttBusy[4]){
            release_fiber(mqttRegisteredFiber[4]);
            mqttBusy[4] = false;
        }
        Fiber *f = create_fiber(MicroIotTickTopic4);
        if(!f) return;
        mqttRegisteredFiber[4] = f;
        mqttBusy[4] = true;
    }
}

static void microIotEvent(){
    while(true){
        activeObject->microIotInquireStatus();
        delay(20);
    }
}

void DFRobot_MicroIot::mqttGetData(uint8_t len)
{
    RECDATA = "";
    uint8_t tempbuf[1] = {0x22};
    i2cWrite(0x16, tempbuf, 1);
    uint8_t tempRecbuf[len + 1];
    i2cRead(0x16, tempRecbuf, len);
    for (int i = 0; i < len; i++) {
        RECDATA += tempRecbuf[i];
    }
    tempRecbuf[len] = '\0';
    RECDATA = (const char *)tempRecbuf;
}

void DFRobot_MicroIot::microIotInquireStatus()
{
    uint8_t buf[3];
    uint8_t tempId = 0;
    uint8_t tempStatus = 0;
    buf[0] = 0x1E;
    buf[1] = READ_STATUS;
    buf[2] = 0x06;
    i2cWrite(0x16, buf, 3);
    uint8_t recbuf[2];
    i2cRead(0x16, recbuf, 2);
    tempId = recbuf[0];
    tempStatus = recbuf[1];
    switch (tempId) {
        case READ_PING:
            if (tempStatus == PING_OK) {
                microIoTStatus = "PingOK";
            } else {
                microIoTStatus = "PingERR";
            }
            break;
        case READ_WIFISTATUS:
            if (tempStatus == WIFI_CONNECTING) {
                microIoTStatus = "WiFiConnecting";
            } else if (tempStatus == WIFI_CONNECTED) {
                microIoTStatus = "WiFiConnected";
            } else if (tempStatus == WIFI_DISCONNECT) {
                microIoTStatus = "WiFiDisconnect";
            } else {
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
            mqttGetData(tempStatus);
            microIoT_IP = RECDATA;
            break;
        case SUB_TOPIC_CMD[0]:
            microIoTStatus = "READ_TOPICDATA";
            mqttGetData(tempStatus);
            runCallback(DFRobot_MicroIot::topic_0);
            break;
        case SUB_TOPIC_CMD[1]:
            microIoTStatus = "READ_TOPICDATA";
            mqttGetData(tempStatus);
            runCallback(DFRobot_MicroIot::topic_1);
            break;
        case SUB_TOPIC_CMD[2]:
            microIoTStatus = "READ_TOPICDATA";
            mqttGetData(tempStatus);
            runCallback(DFRobot_MicroIot::topic_2);
            break;
        case SUB_TOPIC_CMD[3]:
            microIoTStatus = "READ_TOPICDATA";
            mqttGetData(tempStatus);
            runCallback(DFRobot_MicroIot::topic_3);
            break;
        case SUB_TOPIC_CMD[4]:
            microIoTStatus = "READ_TOPICDATA";
            mqttGetData(tempStatus);
            runCallback(DFRobot_MicroIot::topic_4);
            break;
        case HTTP_REQUEST:
            microIoTStatus = "HTTP_REQUEST";
            mqttGetData(tempStatus);
            break;
        case READ_VERSION:
            microIoTStatus = "READ_VERSION";
            mqttGetData(tempStatus);
            break;
        default:
            break;
    }
    delay(200);
}

void DFRobot_MicroIot::mqttParaRunCommand(uint8_t cmd, const String& data)
{
    uint8_t buf[data.length() + 4];
    buf[0] = 0x1E;
    buf[1] = RUN_COMMAND;
    buf[2] = cmd;
    buf[3] = data.length();
    for (int i = 0; i < data.length(); i++)
        buf[i + 4] = data.charAt(i);
    i2cWrite(0x16, buf, data.length() + 4);
}

void DFRobot_MicroIot::mqttRunCommand(uint8_t cmd)
{
    uint8_t buf[3];
    buf[0] = 0x1E;
    buf[1] = RUN_COMMAND;
    buf[2] = cmd;
    i2cWrite(0x16, buf, 3);
}

void DFRobot_MicroIot::mqttSetPara(uint8_t cmd, const String& para)
{
    uint8_t buf[para.length() + 4];
    buf[0] = 0x1E;
    buf[1] = SET_PARA;
    buf[2] = cmd;
    buf[3] = para.length();
    for (int i = 0; i < para.length(); i++)
        buf[i + 4] = para.charAt(i);
    i2cWrite(0x16, buf, para.length() + 4);
}

void DFRobot_MicroIot::i2cWrite(uint8_t addr, uint8_t* buf, uint16_t length)
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

void DFRobot_MicroIot::i2cRead(uint8_t addr, uint8_t* buf, uint16_t length)
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

void DFRobot_MicroIot::checkStatus(String cmd){
    while (true) {
        if (microIoTStatus == cmd) {
            //Serial.println(cmd + "ok");
            return;
        }
        delay(50);
    }
}

static void MicroIotTickTopic0()
{
    microIotCb[0](RECDATA);
    mqttBusy[0] = false;
}

static void MicroIotTickTopic1()
{
    microIotCb[1](RECDATA);
    mqttBusy[1] = false;
}

static void MicroIotTickTopic2()
{
    microIotCb[2](RECDATA);
    mqttBusy[2] = false;
}

static void MicroIotTickTopic3()
{
    microIotCb[3](RECDATA);
    mqttBusy[3] = false;
}

static void MicroIotTickTopic4()
{
    microIotCb[4](RECDATA);
    mqttBusy[4] = false;
}




DFRobot_MicroIot microIot;