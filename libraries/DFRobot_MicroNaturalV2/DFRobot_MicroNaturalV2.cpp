#include <DFRobot_MicroNaturalV2.h>

DFRobot_MicroNaturalV2 *NaturalV2pt = NULL;
#define MAXTOPICNUMBER 5
MsgHandleCb mqttMsgHandleCbV2[MAXTOPICNUMBER] = {NULL, NULL, NULL, NULL, NULL};

static void IoTTask();

DFRobot_MicroNaturalV2::DFRobot_MicroNaturalV2()
{
    NaturalV2pt = this;
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
    Wire.begin();
    memset(data, 0, 22);
}

void DFRobot_MicroNaturalV2::request(void)
{
    uint8_t _data[26] = {0};
    uint8_t val[1] = {8};
    i2cWrite(0x10, val, 1);
    i2cReadBuffer(0x10, _data, 22);
    // for (uint8_t i = 0; i < 22; i ++){
    //     Serial.print(data[i]);
    //     Serial.print(" ");
    // }
    // Serial.println((data[2] << 8) | data[3]);
    for (uint8_t i = 0; i < 22; i++)
    {
        data[i] = _data[i];
    }
}

String DFRobot_MicroNaturalV2::readUltraviolet(void)
{
    String str = ".";
    str = data[0] + str + data[1];
    return str;
}

uint16_t DFRobot_MicroNaturalV2::readNaturalLight(void)
{
    return (data[2] << 8) | data[3];
}

uint16_t DFRobot_MicroNaturalV2::readSound(void)
{
    return (data[4] << 8) | data[5];
}

float DFRobot_MicroNaturalV2::readWaterTemp(void)
{
    String str = ".";
    str = data[6] + str + data[7];
    return str.toFloat();
}

int32_t DFRobot_MicroNaturalV2::readBME280(BME280 mode)
{
    String str = ".";
    if (mode == 1){
        if (data[8] == 1){
            str = data[9] + str + data[10];
            return str.toFloat();
        }else{
            str = "-" + data[9] + str + (255 - data[10]);
            return str.toFloat();
        }
    }else if (mode == 2){
        str = data[11] + str + data[12];
        return str.toInt();
    }else{
        String str1 = "";
        uint32_t number = (data[13] << 16) | (data[14] << 8) | data[15];
        str1 += number;
        return str1.toInt();
    }
}

uint16_t DFRobot_MicroNaturalV2::readTDS(void)
{
    return (data[16] << 8) | data[17];
}

uint16_t DFRobot_MicroNaturalV2::readTVOC(uint8_t mode)
{
    if(mode == 1){
        return (data[18] << 8) | data[19];
    }else{
        return (data[20] << 8) | data[21];
    }
}

void DFRobot_MicroNaturalV2::setBaseline(uint32_t number){
    uint8_t buf[3] = {0};
    buf[0] = 0x20;
    buf[1] = (number >> 8) & 0xff;
    buf[2] = number & 0xff;
    i2cWrite(0x10, buf, 3);
}

void DFRobot_MicroNaturalV2::setTDS(float k){
    uint8_t buf[3] = {0};
    unsigned char integer = floor(k);
    unsigned char decimal = floor(k*100 - integer*100);
    buf[0] = 0x1E;
    buf[1] = integer;
    buf[2] = decimal;
    i2cWrite(0x10, buf, 3);
}

void DFRobot_MicroNaturalV2::setOLEDShowString(uint8_t scolumn, uint8_t sleng, uint8_t srow, String str){
    if(scolumn > sleng){
        uint8_t temp = scolumn;
        scolumn = sleng;
        sleng = temp;
    }
    if(str.length() < 17){
        if(str.length() < (sleng - scolumn) + 1){
            uint8_t buffer[str.length() + 3];
            buffer[0] = 0x28;
            buffer[1] = srow;
            buffer[2] = scolumn;
            for(uint8_t i = 0; i < str.length(); i++){
                buffer[3 + i] = str[i];
            }
            i2cWrite(0x10, buffer, str.length()+3);
            clearOLED(srow, str.length() + scolumn, sleng);
        }else{
            uint8_t buffer[(sleng - scolumn) + 4];
            buffer[0] = 0x28;
            buffer[1] = srow;
            buffer[2] = scolumn;
            for(uint8_t i = 0; i < (sleng - scolumn) + 1; i++){
                buffer[i + 3] = str[i];
            }
            i2cWrite(0x10, buffer, (sleng - scolumn) + 4);
        }
    }else{
        uint8_t buffer[(sleng - scolumn) + 4];
        buffer[0] = 0x28;
        buffer[1] = srow;
        buffer[2] = scolumn;
        for(uint8_t i = 0; i < (sleng - scolumn) + 1; i++){
            buffer[i + 3] = str[i];
        }
        i2cWrite(0x10, buffer, (sleng - scolumn) + 4);
        // uint8_t buffer[19];
        // buffer[0] = 0x28;
        // buffer[1] = srow;
        // buffer[2] = scolumn;
        // for(uint8_t i = 0; i < 16; i++){
        //     buffer[i + 3] = str[i];
        // }
        // i2cWrite(0x10, buffer, 19);
    }
}

void DFRobot_MicroNaturalV2::setOLEDShowString(uint8_t ncolumn, uint8_t nleng, uint8_t nrow, uint32_t number){
    String str = "";
    str += number;
    setOLEDShowString(ncolumn, nleng, nrow, str);
}

void DFRobot_MicroNaturalV2::setOLEDShowString(uint8_t ncolumn, uint8_t nleng, uint8_t nrow, uint16_t number){
    String str = "";
    str += number;
    setOLEDShowString(ncolumn, nleng, nrow, str);
}

void DFRobot_MicroNaturalV2::setOLEDShowString(uint8_t ncolumn, uint8_t nleng, uint8_t nrow, uint8_t number){
    String str = "";
    str += number;
    setOLEDShowString(ncolumn, nleng, nrow, str);
}

void DFRobot_MicroNaturalV2::setOLEDShowString(uint8_t ncolumn, uint8_t nleng, uint8_t nrow, float number){
    String str = "";
    str += number;
    setOLEDShowString(ncolumn, nleng, nrow, str);
}


void DFRobot_MicroNaturalV2::clearOLED(uint8_t valuecolumnstart, uint8_t valuecolumnstop, uint8_t valuerow){
    uint8_t datalength = (valuecolumnstop - valuecolumnstart) + 1;
    if(datalength < 0) return;
    uint8_t buffer[datalength + 3];
    buffer[0] = 0x28;
    buffer[1] = valuerow;
    buffer[2] = valuecolumnstart;
    for(uint8_t i = 0; i < datalength; i++){
        buffer[i + 3] = 32;
    }
    i2cWrite(0x10, buffer, datalength + 3);
}

void DFRobot_MicroNaturalV2::OLEDBegin(void){
    for(uint8_t i=1; i <= 8; i++){
        this->clearOLEDRow(i);
    }
}

void DFRobot_MicroNaturalV2::clearOLEDRow(uint8_t valuerow){
    uint8_t buffer[19];
    buffer[0] = 0x28;
    buffer[1] = valuerow;
    buffer[2] = 1;
    for(uint8_t i = 0; i < 16; i++){
        buffer[i + 3] = 32;
    }
    i2cWrite(0x10, buffer, 19);
}

void DFRobot_MicroNaturalV2::mototRun(DIRECTION dir, uint8_t speed){
    uint8_t buffer[3];
    buffer[0] = 0x00;
    buffer[1] = dir;
    buffer[2] = speed;
    i2cWrite(0x10, buffer, 3);
}

void DFRobot_MicroNaturalV2::mototStop(void){
    uint8_t buffer[3];
    buffer[0] = 0x00;
    buffer[1] = 0;
    buffer[2] = 0;
    i2cWrite(0x10, buffer, 3);
}


void DFRobot_MicroNaturalV2::setColor(int index, int rgb){
    uint8_t index1 = (index >> 8) & 0xff;
    uint8_t index2 = index & 0xff;
    setRangeColor(index1, index2, rgb);
}

void DFRobot_MicroNaturalV2::setColor(uint16_t index, int rgb){
    uint8_t index1 = (index >> 8) & 0xff;
    uint8_t index2 = index & 0xff;
    setRangeColor(index1, index2, rgb);
}

void DFRobot_MicroNaturalV2::setColor(uint32_t index, int rgb){
    uint8_t index1 = (index >> 8) & 0xff;
    uint8_t index2 = index & 0xff;
    setRangeColor(index1, index2, rgb);
}

void DFRobot_MicroNaturalV2::setColor(float index, int rgb){
    
    uint8_t index1 = (uint8_t)index - 1;
    if(index1 >= 0 && index1 <= 3){
        setRangeColor(index1, index1, rgb);
    }
}

uint32_t DFRobot_MicroNaturalV2::RGBRange(uint8_t num1, uint8_t num2){
    return ((num1 - 1) << 8 ) | (num2 - 1);
}

// i2c
void DFRobot_MicroNaturalV2::i2cWrite(uint8_t addr, uint8_t *data, uint8_t len)
{
    // Wire.setClock(400000);
    Wire.beginTransmission(addr);
    while (len--)
    {
        Wire.write(*data);
        data++;
    }
    Wire.endTransmission();
}

uint8_t DFRobot_MicroNaturalV2::i2cReadBuffer(uint8_t addr, uint8_t *data, uint8_t len)
{
    // Wire.setClock(100000);
    Wire.beginTransmission(addr);
    // Wire.write(reg);
    Wire.endTransmission();
    Wire.requestFrom(addr, len);
    if(Wire.available() || false){
        for (uint8_t i = 0; i < len; i++)
        {
            data[i] = Wire.read();
        }
    }
    return 0;
}

uint8_t DFRobot_MicroNaturalV2::i2cRead(uint8_t addr, uint8_t *data, uint8_t len){
    Wire.beginTransmission(addr);
    Wire.write(0x1E);
    Wire.write(READmode);
    Wire.write(0x06);
    Wire.endTransmission();
    Wire.requestFrom(addr, len);
    if(Wire.available() || false){
        for (uint8_t i = 0; i < len; i++)
        {
            data[i] = Wire.read();
        }
    }
    return 0;
}

void DFRobot_MicroNaturalV2::registerMqttHandle(const MsgHandleCb handles[])
{
    for (int i = 0; i < MAXTOPICNUMBER; i++)
    {
        if (handles[i])
        {
            mqttMsgHandleCbV2[i] = handles[i];
        }
    }
}

void DFRobot_MicroNaturalV2::IoTWIFI(String ssid, String password){
    create_fiber(IoTTask);
    IoTSetPara(SETWIFI_NAME, ssid);
    IoTSetPara(SETWIFI_PASSWORLD, password);
    IoTRunCommand(CONNECT_WIFI);
    IoTCheckStatus("WiFiConnected");
    Wifimode = WIFI_CONNECTED;
}

void DFRobot_MicroNaturalV2::microIoT_MQTT(String IOT_ID, String IOT_PWD, String IOT_TOPIC, SERVERS servers, String IP){
    if (servers == China){
        IoTSetPara(SETMQTT_SERVER, OBLOQ_MQTT_EASY_IOT_SERVER_CHINA);
    }else if (servers == Global){
        IoTSetPara(SETMQTT_SERVER, OBLOQ_MQTT_EASY_IOT_SERVER_EN);
    }else{
        IoTSetPara(SETMQTT_SERVER, IP);
    }
    IoTSetPara(SETMQTT_PORT, "1883");
    IoTSetPara(SETMQTT_ID, IOT_ID);
    IoTSetPara(SETMQTT_PASSWORLD, IOT_PWD);
    Serial.println("wifi conneced ok");
    IoTRunCommand(CONNECT_MQTT);
    IoTCheckStatus("MQTTConnected");
    Serial.println("mqtt connected");
    topic_0 = IOT_TOPIC;
    IoTParaRunCommand(SUB_TOPIC0, IOT_TOPIC);
    IoTCheckStatus("SubTopicOK");
    Serial.println("sub topic ok");
}

void DFRobot_MicroNaturalV2::microIoT_add_topic(TOPIC top, String IOT_TOPIC){
    delay(2000);
    IoTParaRunCommand(((uint8_t)top + 0x06), IOT_TOPIC);
    IoTCheckStatus("SubTopicOK");
    delay(2000);
}

void DFRobot_MicroNaturalV2::IoTParaRunCommand(uint8_t cmd, String para){
    uint8_t buffer[para.length() + 4];
    buffer[0] = 0x1E;
    buffer[1] = RUN_COMMAND;
    buffer[2] = cmd;
    buffer[3] = para.length();
    for(uint8_t i = 0; i < para.length(); i++){
        buffer[4 + i] = para[i];
    }
    i2cWrite(IIC_ADDRESS, buffer, para.length()+4);
}

void DFRobot_MicroNaturalV2::microIoT_SendMessage(String Mess, TOPIC Topic){
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
    IoTParaRunCommand(topic, Mess);
    delay(150);
}

void DFRobot_MicroNaturalV2::microIoT_SendMessage(float Message, TOPIC Topic){
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
    IoTParaRunCommand(topic, Mess);
    delay(150);
}

static void TickTopic0()
{
    String mess = NaturalV2pt->RECDATA;
    mqttMsgHandleCbV2[0](mess);
}

static void TickTopic1()
{
    String mess = NaturalV2pt->RECDATA;
    mqttMsgHandleCbV2[1](mess);
}

static void TickTopic2()
{
    String mess = NaturalV2pt->RECDATA;
    mqttMsgHandleCbV2[2](mess);
}

static void TickTopic3()
{
    String mess = NaturalV2pt->RECDATA;
    mqttMsgHandleCbV2[3](mess);
}

static void TickTopic4()
{
    String mess = NaturalV2pt->RECDATA;
    mqttMsgHandleCbV2[4](mess);
}


static void IoTTask()
{
    while (true)
    {
        NaturalV2pt->inquireStatus();
        fiber_sleep(2);
    }
}


void DFRobot_MicroNaturalV2::IoTCheckStatus(String cmd){
    while (true) {
        if (microIoTStatus == cmd) {
            Serial.println("OKOK");
            return;
        }
        delay(50);
    }
}

void DFRobot_MicroNaturalV2::inquireStatus(void){
    // uint8_t buffer1[3];
    uint8_t tempId;
    uint8_t tempStatus;
    uint8_t recbuf[2];
    i2cRead(0x16, recbuf, 2);
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
            IoTGetData(tempStatus);
            microIoT_IP = RECDATA;
            break;
        case SUB_TOPIC0:
            microIoTStatus = "READ_TOPICDATA";
            IoTGetData(tempStatus);
            // create_fiber(TickTopic0);
            // String mess = RECDATA;
            if(NULL != mqttMsgHandleCbV2[0]){
                // mqttMsgHandleCbV2[0](RECDATA);
                create_fiber(TickTopic0);
            }
            break;
        case SUB_TOPIC1:
            microIoTStatus = "READ_TOPICDATA";
            IoTGetData(tempStatus);
            if(NULL != mqttMsgHandleCbV2[1]){
                create_fiber(TickTopic1);
            }
            break;
        case SUB_TOPIC2:
            microIoTStatus = "READ_TOPICDATA";
            IoTGetData(tempStatus);
            if(NULL != mqttMsgHandleCbV2[2]){
                create_fiber(TickTopic2);
            }
            break;
        case SUB_TOPIC3:
            microIoTStatus = "READ_TOPICDATA";
            IoTGetData(tempStatus);
            if(NULL != mqttMsgHandleCbV2[3]){
                create_fiber(TickTopic3);
            }
            break;
        case SUB_TOPIC4:
            microIoTStatus = "READ_TOPICDATA";
            IoTGetData(tempStatus);
            if(NULL != mqttMsgHandleCbV2[4]){
                create_fiber(TickTopic4);
            }
            break;
        case HTTP_REQUEST:
            microIoTStatus = "HTTP_REQUEST";
            IoTGetData(tempStatus);
            break;
        case READ_VERSION:
            microIoTStatus = "READ_VERSION";
            IoTGetData(tempStatus);
            break;
        default:
            break;
    }
    delay(200);
}

void DFRobot_MicroNaturalV2::microIoT_http_IFTTT(String EVENT, String KEY){
    microIoT_WEBHOOKS_EVENT  = EVENT;
    microIoT_WEBHOOKS_KEY = KEY;
}

void DFRobot_MicroNaturalV2::microIoT_http_post(String value1, String value2, String value3){
    IoTSetPara(SETHTTP_IP, microIoT_WEBHOOKS_URL);
    // String tempStr = "";
     String tempStr = "trigger/" + microIoT_WEBHOOKS_EVENT + "/with/key/" + microIoT_WEBHOOKS_KEY + ",{\"value1\":\"" + value1 + "\",\"value2\":\"" + value2 + "\",\"value3\":\"" + value3 + "\" }" + "\r";
    IoTParaRunCommand(POST_URL, tempStr);
}

void DFRobot_MicroNaturalV2::microIoT_ThingSpeak_configura(String KEY){
    microIoT_THINGSPEAK_KEY = KEY;
}

void DFRobot_MicroNaturalV2::microIoT_http_TK_GET(String field1, String field2, String field3, String field4, String field5, String field6, String field7, String field8){
    IoTSetPara(SETHTTP_IP, OBLOQ_MQTT_EASY_IOT_SERVER_TK);
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
    IoTParaRunCommand(GET_URL, tempStr);
}

void DFRobot_MicroNaturalV2::microIoT_http_send(void){
    IoTParaRunCommand(GET_URL, prep_str);
    memset(flag, true, 8);
    prep_str = "";
}

void DFRobot_MicroNaturalV2::microIoT_http_Prep(uint8_t index, String value){
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

void DFRobot_MicroNaturalV2::IoTGetData(uint8_t number){
    RECDATA = "";
    uint8_t tempbuf[1] = {0x22};
    i2cWrite(IIC_ADDRESS, tempbuf, 1);
    uint8_t temprecbuf[number];
    i2cReadBuffer(IIC_ADDRESS, temprecbuf, number);
    for (uint8_t i = 0; i < number; i++) {
        RECDATA += (char)temprecbuf[i];
    }
}

void DFRobot_MicroNaturalV2::IoTRunCommand(uint8_t cmd){
    uint8_t buffer[3];
    buffer[0] = 0x1E;
    buffer[1] = RUN_COMMAND;
    buffer[2] = cmd;
    i2cWrite(IIC_ADDRESS, buffer, 3);
}

void DFRobot_MicroNaturalV2::IoTSetPara(uint8_t cmd, String para){
    uint8_t buffer[para.length() + 4];
    buffer[0] = 0x1E;
    buffer[1] = SET_PARA;
    buffer[2] = cmd;
    buffer[3] = para.length();
    for(uint8_t i = 0; i < para.length(); i++){
        buffer[4 + i] = para[i];
    }
    i2cWrite(IIC_ADDRESS, buffer, para.length()+4);
}