#include "Microbit_Obloq.h"
#include "basic.h"

#define MAXTOPICNUMBER 5
Microbit_Obloq *myObloq;
int OBLOQ_CMD = OBLOQ_NONE;
String OBLOQ_MESSAGE = "";
MsgHandleDebug debugHandle = NULL;
ObloqErrorCode currentError = OBLOQ_OK;
ObloqStatusInfo currentStatus = OBLOQ_NONE;
vector<String> mqttTopicArray;
MsgHandleCb mqttMsgHandleCb[MAXTOPICNUMBER] = {NULL, NULL, NULL, NULL, NULL};
bool mqttMsgHandleCbBusy[MAXTOPICNUMBER] = {false, false, false, false, false};

static void TickTopic0();
static void TickTopic1();
static void TickTopic2();
static void TickTopic3();
static void TickTopic4();
static void debugTask();
static void obloqSoftwareSerialTask();
static void mqttTick(MicroBitEvent e);
static void argumentParsing(String cmd);
static void obloqTick();

void Microbit_Obloq::registerMqttHandle(const MsgHandleCb handles[])
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
void Microbit_Obloq::registerDubugHandle(const MsgHandleDebug handle)
{
    if (!handle)
        return;
    debugHandle = handle;
    create_fiber(debugTask);
}

void Microbit_Obloq::startConnect(uint8_t receive,
                                  uint8_t send,
                                  const String &ssid,
                                  const String &pwd,
                                  const String &iotId,
                                  const String &iotPwd,
                                  const String iotTopics[],
                                  const String &server,
                                  uint32_t port)
{
    isSoftwareSerial = false;
    parameteReset();
    currentStatus = OBLOQ_NONE;
    // 串口初始化
    serialInit(receive, send);
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
void Microbit_Obloq::startConnect(Stream *serial,
                                  const String &ssid,
                                  const String &pwd,
                                  const String &iotId,
                                  const String &iotPwd,
                                  const String iotTopics[],
                                  const String &server,
                                  uint32_t port)
{
    isSoftwareSerial = true;
    parameteReset();
    currentStatus = OBLOQ_NONE;
    // 串口初始化
    obloqSoftwareSerial = serial;
    create_fiber(obloqSoftwareSerialTask);
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
void Microbit_Obloq::publish(Topicnum topic_num, const String &message)
{
    if (topic_num >= mqttTopicArray.size() || currentStatus != OBLOQ_SUB_OK)
        return;
    sendMessage("Publish", "|4|1|3|" + mqttTopicArray[topic_num] + "|" + message + "|");
    while (OBLOQ_CMD != OBLOQ_PULISH_OK)
    {
        delay(1);
        if (OBLOQ_CMD == OBLOQ_PULISH_FAILURE)
        {
            currentError = OBLOQ_PULISH_FAILURE;
            debugTask();
            return;
        }
    }
    long time = millis();
    while (OBLOQ_CMD != OBLOQ_MQTT_RECEIVE_OK)
    {
        delay(1);
        if (millis() - time > 3000)
            return;
    }
}
void Microbit_Obloq::publish(Topicnum topic_num, double f, int precision)
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
void Microbit_Obloq::publish(Topicnum topic_num, int32_t i)
{
    if (topic_num >= mqttTopicArray.size() || currentStatus != OBLOQ_SUB_OK)
        return;
    char buffer[34];
    memset(buffer, 0, 34);
    itoa(i, buffer, 10);
    publish(topic_num, (const String &)buffer);
}

void Microbit_Obloq::startConnect(uint8_t receive,
                                  uint8_t send,
                                  const String &ssid,
                                  const String &pwd,
                                  const String &ip,
                                  uint32_t port)
{
    isSoftwareSerial = false;
    parameteReset();
    currentStatus = OBLOQ_NONE;
    if (isIP(ip.c_str()))
        http_host = ip + ":" + port;
    else
        http_host = ip;
    // 串口初始化
    serialInit(receive, send);
    // 建立串口通讯
    ping();
    // WiFi连接
    wifiConnect(ssid, pwd);
}
void Microbit_Obloq::startConnect(Stream *serial,
                                  const String &ssid,
                                  const String &pwd,
                                  const String &ip,
                                  uint32_t port)
{
    isSoftwareSerial = true;
    parameteReset();
    currentStatus = OBLOQ_NONE;
    if (isIP(ip.c_str()))
        http_host = ip + ":" + port;
    else
        http_host = ip;
    // 串口初始化
    obloqSoftwareSerial = serial;
    create_fiber(obloqSoftwareSerialTask);
    // 建立串口通讯
    ping();
    // WiFi连接
    wifiConnect(ssid, pwd);
}
String Microbit_Obloq::get(const String &url, int timeOut)
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
    OBLOQ_CMD = OBLOQ_NONE;
    return OBLOQ_MESSAGE;
}
String Microbit_Obloq::post(const String &url, const String &content, int timeOut)
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
String Microbit_Obloq::put(const String &url, const String &content, int timeOut)
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

Microbit_Obloq::Microbit_Obloq() : IPAddress("0.0.0.0")
{
    myObloq = this;
    parameteReset();
}

Microbit_Obloq::~Microbit_Obloq()
{
    parameteReset();
}

ObloqErrorCode Microbit_Obloq::ping()
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
ObloqErrorCode Microbit_Obloq::wifiConnect(const String &ssid, const String &pwd)
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
ObloqErrorCode Microbit_Obloq::mqttConnect(const String &iotId,
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
ObloqErrorCode Microbit_Obloq::subTopic()
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
        delay(1500);
    }
    currentStatus = OBLOQ_SUB_OK;
    debugPeek("Subscribe to the success", 0);
    return OBLOQ_OK;
}

void Microbit_Obloq::parameteReset()
{
    OBLOQ_CMD = OBLOQ_NONE;
    OBLOQ_MESSAGE = "";
    http_host = "";
    mqttTopicArray.clear();
    currentError = OBLOQ_OK;
    ObloqStatusInfo currentStatus = OBLOQ_NONE;
}
void Microbit_Obloq::serialInit(uint8_t receive, uint8_t send)
{
    receive = g_PinID[receive];
    send = g_PinID[send];
    MicroBitPin *rxpin = getMicroBitPin(receive);
    MicroBitPin *txpin = getMicroBitPin(send);
#if defined(NRF5)
    uBit.serial.redirect(txpin->name, rxpin->name);
    uBit.serial.baud((int)9600);
#elif defined(NRF52833)
    uBit.serial.redirect(*txpin, *rxpin);
    uBit.serial.setBaudrate((int)9600);
#endif

    uBit.serial.setTxBufferSize(300);
    uBit.serial.setRxBufferSize(300);
    uBit.serial.eventOn(ManagedString('\r'), MicroBitSerialMode::ASYNC);
#if defined(NRF5)
    uBit.messageBus.listen(32, 1, mqttTick);
#elif defined(NRF52833)
    uBit.messageBus.listen(DEVICE_ID_SERIAL, CODAL_SERIAL_EVT_DELIM_MATCH, mqttTick);
#endif
    uBit.serial.clearRxBuffer();
    uBit.serial.clearTxBuffer();
}
void Microbit_Obloq::sendMessage(String steps, String message)
{
    String msg = message + "\r";
    debugPeek("send:" + message + "\\r " + "[" + steps + "]", 0);
    if (isSoftwareSerial)
    {
        obloqSoftwareSerial->print(msg);
    }
    else
    {
        uBit.serial.send(msg.c_str());
    }
}
void Microbit_Obloq::debugPeek(String debugMessage, int8_t errorCode)
{
    if (debugHandle)
        debugHandle(debugMessage, errorCode);
}
int Microbit_Obloq::isIP(const char str[])
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

///////////////////// static function /////////////////////

static void TickTopic0()
{
    String mess = OBLOQ_MESSAGE;
    mqttMsgHandleCb[0](mess);
    mqttMsgHandleCbBusy[0] = false;
}

static void TickTopic1()
{
    String mess = OBLOQ_MESSAGE;
    mqttMsgHandleCb[1](mess);
    mqttMsgHandleCbBusy[1] = false;
}

static void TickTopic2()
{
    String mess = OBLOQ_MESSAGE;
    mqttMsgHandleCb[2](mess);
    mqttMsgHandleCbBusy[2] = false;
}

static void TickTopic3()
{
    String mess = OBLOQ_MESSAGE;
    mqttMsgHandleCb[3](mess);
    mqttMsgHandleCbBusy[3] = false;
}

static void TickTopic4()
{
    String mess = OBLOQ_MESSAGE;
    mqttMsgHandleCb[4](mess);
    mqttMsgHandleCbBusy[4] = false;
}

static void debugTask()
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
        delay(5);
    }
}

static void obloqSoftwareSerialTask()
{
    while (true)
    {
        obloqTick();
        fiber_sleep(1);
    }
}

static void mqttTick(MicroBitEvent e)
{
    obloqTick();
}

static void obloqTick()
{
    static String Obloq_message_str;
    if (myObloq->isSoftwareSerial)
    {
        while (myObloq->obloqSoftwareSerial && myObloq->obloqSoftwareSerial->available() > 0)
        {
            int c;
            while (myObloq->obloqSoftwareSerial->available())
            {
                c = myObloq->obloqSoftwareSerial->read();
                if (c >= 0)
                    Obloq_message_str += (char)c;
                if (c == '\r')
                {
                    argumentParsing(Obloq_message_str);
                    Obloq_message_str = "";
                    break;
                }
                if(Obloq_message_str.length()>=1000){
                    argumentParsing(Obloq_message_str);
                    break;
                }
                delay(1);
            }
            delay(1);
        }
    }
    else
    {
        int n = uBit.serial.getRxBufferSize();
        ManagedString s = uBit.serial.read(n, MicroBitSerialMode::ASYNC);
        Obloq_message_str = s.toCharArray();
        argumentParsing(Obloq_message_str);
        Obloq_message_str = "";
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
                    case Microbit_Obloq::topic_0:
                        create_fiber(TickTopic0);
                        break;
                    case Microbit_Obloq::topic_1:
                        create_fiber(TickTopic1);
                        break;
                    case Microbit_Obloq::topic_2:
                        create_fiber(TickTopic2);
                        break;
                    case Microbit_Obloq::topic_3:
                        create_fiber(TickTopic3);
                        break;
                    case Microbit_Obloq::topic_4:
                        create_fiber(TickTopic4);
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
        }else{
            OBLOQ_MESSAGE = "unknow error!";
        }
        OBLOQ_CMD = OBLOQ_HTTP_RECEIVE_OK;
        return;
    }
    else
    {
        return;
    }
}
