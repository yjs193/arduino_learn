#include "Pico_Obloq.h"

#define MAXTOPICNUMBER 5
Pico_Obloq *myObloq;
int OBLOQ_CMD = OBLOQ_NONE;
String OBLOQ_MESSAGE = "";
MsgHandleDebug debugHandle = NULL;
ObloqErrorCode currentError = OBLOQ_OK;
ObloqStatusInfo currentStatus = OBLOQ_NONE;
vector<String> mqttTopicArray;
MsgHandleCb mqttMsgHandleCb[MAXTOPICNUMBER] = {NULL, NULL, NULL, NULL, NULL};
bool mqttMsgHandleCbBusy[MAXTOPICNUMBER] = {false, false, false, false, false};

static void TickTopic0(void *param);
static void TickTopic1(void *param);
static void TickTopic2(void *param);
static void TickTopic3(void *param);
static void TickTopic4(void *param);
static void debugTask(void *param);
static void obloqSerialTask(void *param);
static void argumentParsing(String cmd);
static void obloqTick();

void Pico_Obloq::registerMqttHandle(const MsgHandleCb handles[])
{
    if (handles == NULL)
        return;
    for (int i = 0; i < MAXTOPICNUMBER; i++)
    {
        if (handles[i])
        {
            mqttMsgHandleCb[i] = handles[i];
        }
    }
}
void Pico_Obloq::registerDubugHandle(const MsgHandleDebug handle)
{
    if (!handle)
        return;
    debugHandle = handle;
    xTaskCreate(debugTask, "debugTask", 256, nullptr, 1, nullptr);
}

void Pico_Obloq::startConnect(uint8_t receive,
                              uint8_t send,
                              const String &ssid,
                              const String &pwd,
                              const String &iotId,
                              const String &iotPwd,
                              const String iotTopics[],
                              const String &server,
                              uint32_t port)
{

    IPAddress = "0.0.0.0";
    myObloq = this;
    parameteReset();
    currentStatus = OBLOQ_NONE;
    // 串口初始化
    serialInit(receive, send);
    xTaskCreate(obloqSerialTask, "tick", 256, nullptr, 1, nullptr);
    // 建立串口通讯
    ping();
    // WiFi连接
    if (wifiConnect(ssid, pwd) != 0)
        return;
    // Mqtt连接
    if (mqttConnect(iotId, iotPwd, iotTopics, server, port) != 0)
        return;
    // 订阅Topic
    subTopic();
}
// void Pico_Obloq::startConnect(Stream *serial,
//                                   const String &ssid,
//                                   const String &pwd,
//                                   const String &iotId,
//                                   const String &iotPwd,
//                                   const String iotTopics[],
//                                   const String &server,
//                                   uint32_t port)
// {
//     isSoftwareSerial = true;
//     parameteReset();
//     currentStatus = OBLOQ_NONE;
//     // 串口初始化
//     obloqSerial = serial;
//     xTaskCreate(obloqSerialTask, "tick", 256, nullptr, 1, nullptr);
//     // 建立串口通讯
//     ping();
//     // WiFi连接
//     if (wifiConnect(ssid, pwd) != 0)
//         return;
//     // Mqtt连接
//     if (mqttConnect(iotId, iotPwd, iotTopics, server, port) != 0)
//         return;
//     // 订阅Topic
//     subTopic();
// }

void Pico_Obloq::serialInit(uint8_t receive, uint8_t send)
{
    if (receive == 1 && send == 0)
    {
        obloqSerial = &Serial1;
        obloqSerial->begin(9600);
    }
    else if (receive == 9 && send == 8)
    {
        obloqSerial = &Serial2;
        obloqSerial->begin(9600);
    }
}

void Pico_Obloq::publish(Topicnum topic_num, const String &message)
{
    if (topic_num >= mqttTopicArray.size() || currentStatus != OBLOQ_SUB_OK)
        return;
    sendMessage("Publish", "|4|1|3|" + mqttTopicArray[topic_num] + "|" + message + "|");
    long time = millis();
    while (OBLOQ_CMD != OBLOQ_PULISH_OK)
    {
        delay(1);
        if (OBLOQ_CMD == OBLOQ_PULISH_FAILURE)
        {
            currentError = OBLOQ_PULISH_FAILURE;
            // debugTask(void);
            return;
        }
        if (millis() - time > 3000)
            return;
    }
    while (OBLOQ_CMD != OBLOQ_MQTT_RECEIVE_OK)
    {
        delay(1);
        if (millis() - time > 3000)
            return;
    }
}
void Pico_Obloq::publish(Topicnum topic_num, double f, int precision)
{
    if (topic_num >= mqttTopicArray.size() || currentStatus != OBLOQ_SUB_OK)
        return;
    String str = String(f, 5);
    int len = str.length() + 1;
    char buffer[len];
    str.toCharArray(buffer, len);
    buffer[len - 1] = '\0';
    char *p = buffer + len - 1;
    while (*p == '\0' || *p == '0')
    {
        *p = '\0';
        p--;
    }
    if (*p == '.')
        *p = '\0';
    if (str == "-0")
        str = "0";
    str = String(buffer);
    publish(topic_num, (const String &)str);
}
void Pico_Obloq::publish(Topicnum topic_num, int32_t i)
{
    if (topic_num >= mqttTopicArray.size() || currentStatus != OBLOQ_SUB_OK)
        return;
    char buffer[34];
    memset(buffer, 0, 34);
    itoa(i, buffer, 10);
    publish(topic_num, (const String &)buffer);
}

void Pico_Obloq::startConnect(uint8_t receive,
                                  uint8_t send,
                                  const String &ssid,
                                  const String &pwd,
                                  const String &ip,
                                  uint32_t port)
{
    IPAddress = "0.0.0.0";
    myObloq = this;
    parameteReset();
    currentStatus = OBLOQ_NONE;
    
    if (isIP(ip.c_str()))
        http_host = ip + ":" + port;
    else
        http_host = ip;
    // 建立串口通讯
    xTaskCreate(obloqSerialTask, "tick", 256, nullptr, 1, nullptr);
    // 串口初始化
    serialInit(receive, send);
    ping();
    // // WiFi连接
    wifiConnect(ssid, pwd);
}
// void Pico_Obloq::startConnect(Stream *serial,
//                                   const String &ssid,
//                                   const String &pwd,
//                                   const String &ip,
//                                   uint32_t port)
// {
//     isSoftwareSerial = true;
//     parameteReset();
//     currentStatus = OBLOQ_NONE;
//     if (isIP(ip.c_str()))
//         http_host = ip + ":" + port;
//     else
//         http_host = ip;
//     // 串口初始化
//     obloqSerial = serial;
//     xTaskCreate(obloqSerialTask, "tick", 256, nullptr, 1, nullptr);
//     // 建立串口通讯
//     ping();
//     // WiFi连接
//     wifiConnect(ssid, pwd);
// }
String Pico_Obloq::get(const String &url, int timeOut)
{
    OBLOQ_MESSAGE = "";
    if (currentStatus != OBLOQ_WIFI_CONNECT_OK)
        return OBLOQ_MESSAGE;
    sendMessage("Http get", "|3|1|http://" + http_host + "/" + url + "|");

    uint32_t time = millis();
    while (OBLOQ_CMD != OBLOQ_HTTP_RECEIVE_OK)
    {
        delay(1);
        if (millis() - time >= timeOut)
        {
            return "time out!";
        }
    }
    return OBLOQ_MESSAGE;
}
String Pico_Obloq::post(const String &url, const String &content, int timeOut)
{
    OBLOQ_MESSAGE = "";
    if (currentStatus != OBLOQ_WIFI_CONNECT_OK)
        return OBLOQ_MESSAGE;
    sendMessage("Http get", "|3|2|http://" + http_host + "/" + url + "," + content + "|");

    uint32_t time = millis();
    while (OBLOQ_CMD != OBLOQ_HTTP_RECEIVE_OK)
    {
        delay(1);
        if (millis() - time >= timeOut)
        {
            return "time out!";
        }
    }
    return OBLOQ_MESSAGE;
}
String Pico_Obloq::put(const String &url, const String &content, int timeOut)
{
    OBLOQ_MESSAGE = "";
    if (currentStatus != OBLOQ_WIFI_CONNECT_OK)
        return OBLOQ_MESSAGE;
    sendMessage("Http get", "|3|3|http://" + http_host + "/" + url + "," + content + "|");

    uint32_t time = millis();
    while (OBLOQ_CMD != OBLOQ_HTTP_RECEIVE_OK)
    {
        delay(1);
        if (millis() - time >= timeOut)
        {
            return "time out!";
        }
    }
    return OBLOQ_MESSAGE;
}

Pico_Obloq::Pico_Obloq()
{
}

Pico_Obloq::~Pico_Obloq()
{
}

ObloqErrorCode Pico_Obloq::ping()
{
    while (OBLOQ_CMD != OBLOQ_PING_OK)
    {
        sendMessage("Ping", "|1|1|");
        delay(100);
    }
    while (OBLOQ_CMD != OBLOQ_GET_VERSION_OK)
    {
        sendMessage("Get version", "|1|2|");
        delay(100);
    }
    currentStatus = OBLOQ_PING_OK;
    debugPeek("Successful connection\nFirmware version: V" + OBLOQ_MESSAGE, 0);
    return OBLOQ_OK;
}

ObloqErrorCode Pico_Obloq::wifiConnect(const String &ssid, const String &pwd)
{
    if (currentStatus != OBLOQ_PING_OK)
        return OBLOQ_OK;
    sendMessage("Wifi connect", "|2|1|" + ssid + "," + pwd + "|");
    while (OBLOQ_CMD != OBLOQ_WIFI_CONNECT_OK)
    {
        delay(1);
        if (OBLOQ_CMD == OBLOQ_WIFI_CONNECT_FAILURE)
        {
            currentError = OBLOQ_WIFI_CONNECT_FAILURE;
            return OBLOQ_WIFI_CONNECT_FAILURE;
        }
    }
    currentStatus = OBLOQ_WIFI_CONNECT_OK;
    IPAddress = OBLOQ_MESSAGE;
    debugPeek("WIFI Connection Successful", 0);
    return OBLOQ_OK;
}

ObloqErrorCode Pico_Obloq::mqttConnect(const String &iotId,
                                           const String &iotPwd,
                                           const String iotTopics[],
                                           const String &server,
                                           uint32_t port)
{
    if (currentStatus != OBLOQ_WIFI_CONNECT_OK)
        return OBLOQ_OK;
    for (uint8_t i = 0; i < MAXTOPICNUMBER; i++)
    {
        if (iotTopics[i] != "")
            mqttTopicArray.push_back(iotTopics[i]);
    }
    sendMessage("Mqtt connect", "|4|1|1|" + server + "|" + port + "|" + iotId + "|" + iotPwd + "|");
    while (OBLOQ_CMD != OBLOQ_MQTT_CONNECT_OK)
    {
        delay(1);
        if (OBLOQ_CMD == OBLOQ_MQTT_CONNECT_FAILURE)
        {
            currentError = OBLOQ_MQTT_CONNECT_FAILURE;
            return OBLOQ_MQTT_CONNECT_FAILURE;
        }
    }
    currentStatus = OBLOQ_MQTT_CONNECT_OK;
    debugPeek("MQTT Connection Successful", 0);
    return OBLOQ_OK;
}

ObloqErrorCode Pico_Obloq::subTopic()
{
    if (currentStatus != OBLOQ_MQTT_CONNECT_OK)
        return OBLOQ_OK;
    for (int i = 0; i < mqttTopicArray.size(); i++)
    {
        sendMessage("Subtopic", "|4|1|2|" + mqttTopicArray[i] + "|");
        while (OBLOQ_CMD != OBLOQ_SUB_OK)
        {
            delay(1);
            if (OBLOQ_CMD == OBLOQ_SUB_FAILURE)
            {
                currentError = OBLOQ_SUB_FAILURE;
                return OBLOQ_SUB_FAILURE;
            }
        }
        delay(3000);
    }
    currentStatus = OBLOQ_SUB_OK;
    debugPeek("Subscribe to the success", 0);
    delay(2000);
    return OBLOQ_OK;
}

void Pico_Obloq::parameteReset()
{
    OBLOQ_CMD = OBLOQ_NONE;
    OBLOQ_MESSAGE = "";
    http_host = "";
    mqttTopicArray.clear();
    currentError = OBLOQ_OK;
    ObloqStatusInfo currentStatus = OBLOQ_NONE;
}

void Pico_Obloq::sendMessage(String steps, String message)
{
    String msg = message + "\r";
    debugPeek("send:" + message + "\\r " + "[" + steps + "]", 0);
    obloqSerial->print(msg);
}

void Pico_Obloq::debugPeek(String debugMessage, int8_t errorCode)
{
    if (debugHandle)
        debugHandle(debugMessage, errorCode);
}

int Pico_Obloq::isIP(const char str[])
{
    int a, b, c, d;
    char temp[100];
    if ((sscanf(str, "%d.%d.%d.%d", &a, &b, &c, &d)) != 4)
        return 0;
    sprintf(temp, "%d.%d.%d.%d", a, b, c, d);
    if (strcmp(temp, str) != 0)
        return 0;
    if (!((a <= 255 && a >= 0) && (b <= 255 && b >= 0) && (c <= 255 && c >= 0)))
        return 0;
    else
        return 1;
}

// ///////////////////// static function /////////////////////

static void TickTopic0(void *param)
{
    String mess = OBLOQ_MESSAGE;
    mqttMsgHandleCb[0](mess);
    mqttMsgHandleCbBusy[0] = false;
    vTaskDelete(NULL);
}

static void TickTopic1(void *param)
{
    String mess = OBLOQ_MESSAGE;
    mqttMsgHandleCb[1](mess);
    mqttMsgHandleCbBusy[1] = false;
    vTaskDelete(NULL);
}

static void TickTopic2(void *param)
{
    String mess = OBLOQ_MESSAGE;
    mqttMsgHandleCb[2](mess);
    mqttMsgHandleCbBusy[2] = false;
    vTaskDelete(NULL);
}

static void TickTopic3(void *param)
{
    String mess = OBLOQ_MESSAGE;
    mqttMsgHandleCb[3](mess);
    mqttMsgHandleCbBusy[3] = false;
    vTaskDelete(NULL);
}

static void TickTopic4(void *param)
{
    String mess = OBLOQ_MESSAGE;
    mqttMsgHandleCb[4](mess);
    mqttMsgHandleCbBusy[4] = false;
    vTaskDelete(NULL);
}

static void debugTask(void *param)
{
    while (true)
    {
        switch (currentError)
        {
        case OBLOQ_MQTT_CONNECT_FAILURE:
            myObloq->debugPeek("receive:|4|1|1|2|\\r  [Mqtt connect failure]", OBLOQ_MQTT_CONNECT_FAILURE);
            currentError = OBLOQ_OK;
            break;
        case OBLOQ_WIFI_CONNECT_FAILURE:
            myObloq->debugPeek("receive:|2|4|\\r  [Check that the SSID and PWD are entered correctly]", OBLOQ_WIFI_CONNECT_FAILURE);
            currentError = OBLOQ_OK;
            break;
        case OBLOQ_SUB_FAILURE:
            myObloq->debugPeek("receive:|4|1|2|2|2|\\r  [Topic subscription failed]", OBLOQ_SUB_FAILURE);
            currentError = OBLOQ_OK;
            break;
        case OBLOQ_PULISH_FAILURE:
            myObloq->debugPeek("receive:|4|1|3|2|\\r  [Pulish failure]", OBLOQ_PULISH_FAILURE);
            currentError = OBLOQ_OK;
            break;
        }
        yield();
        delay(50);
    }
}

static void obloqSerialTask(void *param)
{
    while (true)
    {
        obloqTick();
        // delay(1000);
        yield();
        delay(30);
    }
}

static void obloqTick()
{
    static String Obloq_message_str;

    while (myObloq->obloqSerial && myObloq->obloqSerial->available() > 0)
    {
        int c;
        while (myObloq->obloqSerial->available())
        {
            c = myObloq->obloqSerial->read();
            if (c >= 0)
                Obloq_message_str += (char)c;
            if (c == '\r')
            {
                argumentParsing(Obloq_message_str);
                Obloq_message_str = "";
                break;
            }
            delay(1);
        }
        delay(1);
    }
}

static void argumentParsing(String cmd)
{
    int size = cmd.length();
    String item = cmd;
    if (item.indexOf("|4|1|1|1|", 0) != -1)
    {
        OBLOQ_CMD = OBLOQ_MQTT_CONNECT_OK;
        OBLOQ_MESSAGE = "";
        return;
    }
    else if (item.indexOf("|4|1|1|2|", 0) != -1)
    {
        OBLOQ_CMD = OBLOQ_MQTT_CONNECT_FAILURE;
        OBLOQ_MESSAGE = item.substring(9, size - 2 - 9);
        return;
    }
    else if (item.indexOf("|4|1|2|1|", 0) != -1)
    {
        OBLOQ_CMD = OBLOQ_SUB_OK;
        OBLOQ_MESSAGE = "";
        return;
    }
    else if (item.indexOf("|4|1|2|2|2|", 0) != -1)
    {
        OBLOQ_CMD = OBLOQ_SUB_FAILURE;
        OBLOQ_MESSAGE = "";
        return;
    }
    else if (item.indexOf("|4|1|3|1|", 0) != -1)
    {
        OBLOQ_CMD = OBLOQ_PULISH_OK;
        OBLOQ_MESSAGE = "";
        return;
    }
    else if (item.indexOf("|4|1|3|2|", 0) != -1)
    {
        OBLOQ_CMD = OBLOQ_PULISH_FAILURE;
        currentError = OBLOQ_PULISH_FAILURE;
        OBLOQ_MESSAGE = "";
        return;
    }
    else if (item.indexOf("|4|1|4|1|", 0) != -1)
    {
        OBLOQ_CMD = OBLOQ_MQTT_DISCONNECT_OK;
        OBLOQ_MESSAGE = "";
        return;
    }
    else if (item.indexOf("|4|1|5|") != -1)
    { //|4|1|5|topic|message|
        String str = item.substring(7, size - 1);
        int num = str.indexOf("|");
        String topic = str.substring(0, num);
        OBLOQ_MESSAGE = str.substring(num + 1, str.length() - 1);
        for (int i = 0; i < mqttTopicArray.size(); i++)
        {
            if (topic == mqttTopicArray[i])
            {
                if (mqttMsgHandleCb[i] && !mqttMsgHandleCbBusy[i])
                {
                    mqttMsgHandleCbBusy[i] = true;
                    switch (i)
                    {
                    case Pico_Obloq::topic_0:
                        xTaskCreate(TickTopic0, "TickTopic0", 256, nullptr, 1, nullptr);
                        break;
                    case Pico_Obloq::topic_1:
                        xTaskCreate(TickTopic1, "TickTopic1", 256, nullptr, 1, nullptr);
                        break;
                    case Pico_Obloq::topic_2:
                        xTaskCreate(TickTopic2, "TickTopic2", 256, nullptr, 1, nullptr);
                        break;
                    case Pico_Obloq::topic_3:
                        xTaskCreate(TickTopic3, "TickTopic3", 256, nullptr, 1, nullptr);
                        break;
                    case Pico_Obloq::topic_4:
                        xTaskCreate(TickTopic4, "TickTopic4", 256, nullptr, 1, nullptr);
                        break;
                    }
                }
            }
        }
        OBLOQ_CMD = OBLOQ_MQTT_RECEIVE_OK;
        return;
    }
    else if (item.indexOf("|1|1|", 0) != -1)
    {
        OBLOQ_CMD = OBLOQ_PING_OK;
        OBLOQ_MESSAGE = "";
        return;
    }
    else if (item.indexOf("|1|2|", 0) != -1)
    {
        OBLOQ_CMD = OBLOQ_GET_VERSION_OK;
        OBLOQ_MESSAGE = item.substring(5, size - 2); //version
        return;
    }
    else if (item.indexOf("|2|3|", 0) != -1)
    {
        OBLOQ_CMD = OBLOQ_WIFI_CONNECT_OK;
        OBLOQ_MESSAGE = item.substring(5, size - 2); //IP addr
        return;
    }
    else if (item.indexOf("|2|4|", 0) != -1)
    {
        OBLOQ_CMD = OBLOQ_WIFI_CONNECT_FAILURE;
        myObloq->IPAddress = "0.0.0.0";
        OBLOQ_MESSAGE = "";
        return;
    }
    else if (item.indexOf("|3|", 0) != -1)
    { //|3|errcode|message|
        String str = item.substring(3, size - 1);
        int num = str.indexOf("|");
        String error = str.substring(0, num);
        if (error == "200")
        {
            OBLOQ_MESSAGE = str.substring(num + 1, str.length() - 1);
        }
        else if (error == "-1")
        {
            OBLOQ_MESSAGE = "Data acquisition failed!";
        }
        else if (error == "1")
        {
            OBLOQ_MESSAGE = "Request field error!";
        }
        OBLOQ_CMD = OBLOQ_HTTP_RECEIVE_OK;
        return;
    }
    else
    {
        return;
    }
}
