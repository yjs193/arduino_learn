#include "UNO_Obloq.h"

/*
void UNO_Obloq::registerDubugHandle(const MsgHandleDebug handle)
{
    if(!handle) return;
    debugHandle = handle;
}
*/
void UNO_Obloq::startConnect(uint8_t receive,
                             uint8_t send,
                             const String &ssid,
                             const String &pwd,
                             const String &iotId,
                             const String &iotPwd,
                             const String iotTopics[],
                             const String &server,
                             uint32_t port)
{
    if (currentError != OBLOQ_OK)
    {
        return;
    }
    parameteReset();
    currentStatus = OBLOQ_NONE;
    // 串口初始化
    //#if defined(ARDUINO_AVR_UNO) || defined(ARDUINO_AVR_PRO)
    Serial.begin(9600);
    obloqSerial = &Serial;
    /*#elif defined(ARDUINO_AVR_LEONARDO)
    Serial1.begin(9600);
    obloqSerial = &Serial1;
#elif defined(ARDUINO_AVR_MEGA2560)
    if(receive == 19 && send == 18){
        Serial1.begin(9600);
        obloqSerial = &Serial1;
    }else if(receive == 17 && send == 16){
        Serial2.begin(9600);
        obloqSerial = &Serial2;
    }else if(receive == 15 && send == 14){
        Serial3.begin(9600);
        obloqSerial = &Serial3;
    }else{
        Serial1.begin(9600);
        obloqSerial = &Serial1;
    }
#endif*/
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
void UNO_Obloq::startConnect(Stream *serial,
                             const String &ssid,
                             const String &pwd,
                             const String &iotId,
                             const String &iotPwd,
                             const String iotTopics[],
                             const String &server,
                             uint32_t port)
{
    if (currentError != OBLOQ_OK)
    {
        return;
    }
    parameteReset();
    currentStatus = OBLOQ_NONE;
    // 串口初始化
    obloqSerial = serial;
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
void UNO_Obloq::publish(Topicnum topic_num, const String &message)
{
    if (topic_num >= mqttTopicArray.size() || currentStatus != OBLOQ_SUB_OK)
        return;
    sendMessage(/*"Publish", */ "|4|1|3|" + mqttTopicArray[topic_num] + "|" + message + "|\r");
    while (OBLOQ_CMD != OBLOQ_PULISH_OK)
    {
        obloqTick();
        if (OBLOQ_CMD == OBLOQ_PULISH_FAILURE)
        {
            currentError = OBLOQ_PULISH_FAILURE;
            //debugTask();
            return;
        }
    }
    long time = millis();
    while (OBLOQ_CMD != OBLOQ_MQTT_RECEIVE_OK)
    {
        obloqTick();
        if (millis() - time > 3000)
            return;
    }
}
void UNO_Obloq::publish(Topicnum topic_num, double f, int precision)
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
void UNO_Obloq::publish(Topicnum topic_num, int32_t i)
{
    if (topic_num >= mqttTopicArray.size() || currentStatus != OBLOQ_SUB_OK)
        return;
    char buffer[34];
    memset(buffer, 0, 34);
    itoa(i, buffer, 10);
    publish(topic_num, (const String &)buffer);
}
String UNO_Obloq::getMessage(Topicnum topic_num)
{
    obloqTick();
    if (mqtt_message[topic_num].available)
    {
        mqtt_message[topic_num].available = false;
        return mqtt_message[topic_num].message;
    }
    else
    {
        return "";
    }
    delay(1);
}
bool UNO_Obloq::available(Topicnum topic_num)
{
    obloqTick();
    return mqtt_message[topic_num].available;
}

void UNO_Obloq::startConnect(uint8_t receive,
                             uint8_t send,
                             const String &ssid,
                             const String &pwd,
                             const String &ip,
                             uint32_t port)
{
    if (currentError != OBLOQ_OK)
    {
        return;
    }
    parameteReset();
    currentStatus = OBLOQ_NONE;
    if (isIP(ip.c_str()))
        http_host = ip + ":" + port;
    else
        http_host = ip;
    // 串口初始化
    //#if defined(ARDUINO_AVR_UNO) || defined(ARDUINO_AVR_PRO)
    Serial.begin(9600);
    obloqSerial = &Serial;
    /*#elif defined(ARDUINO_AVR_LEONARDO)
    Serial1.begin(9600);
    obloqSerial = &Serial1;
#elif defined(ARDUINO_AVR_MEGA2560)
    if(receive == 19 && send == 18){
        Serial1.begin(9600);
        obloqSerial = &Serial1;
    }else if(receive == 17 && send == 16){
        Serial2.begin(9600);
        obloqSerial = &Serial2;
    }else if(receive == 15 && send == 14){
        Serial3.begin(9600);
        obloqSerial = &Serial3;
    }else{
        Serial1.begin(9600);
        obloqSerial = &Serial1;
    }
#endif*/
    // 建立串口通讯
    ping();
    // WiFi连接
    wifiConnect(ssid, pwd);
}
void UNO_Obloq::startConnect(Stream *serial,
                             const String &ssid,
                             const String &pwd,
                             const String &ip,
                             uint32_t port)
{
    if (currentError != OBLOQ_OK)
    {
        return;
    }
    parameteReset();
    currentStatus = OBLOQ_NONE;
    if (isIP(ip.c_str()))
        http_host = ip + ":" + port;
    else
        http_host = ip;
    // 串口初始化
    obloqSerial = serial;
    // 建立串口通讯
    ping();
    // WiFi连接
    wifiConnect(ssid, pwd);
}
String UNO_Obloq::get(const String &url, int timeOut)
{
    OBLOQ_MESSAGE = "";
    if (currentStatus != OBLOQ_WIFI_CONNECT_OK)
        return OBLOQ_MESSAGE;
    sendMessage(/*"Http get", */ "|3|1|http://" + http_host + "/" + url + "|\r");

    uint32_t time = millis();
    while (OBLOQ_CMD != OBLOQ_HTTP_RECEIVE_OK)
    {
        obloqTick();
        if (millis() - time >= timeOut)
        {
            return "time out!";
        }
    }
    return OBLOQ_MESSAGE;
}
String UNO_Obloq::post(const String &url, const String &content, int timeOut)
{
    OBLOQ_MESSAGE = "";
    if (currentStatus != OBLOQ_WIFI_CONNECT_OK)
        return OBLOQ_MESSAGE;
    sendMessage(/*"Http post", */ "|3|2|http://" + http_host + "/" + url + "," + content + "|\r");

    uint32_t time = millis();
    while (OBLOQ_CMD != OBLOQ_HTTP_RECEIVE_OK)
    {
        obloqTick();
        if (millis() - time >= timeOut)
        {
            return "time out!";
        }
    }
    return OBLOQ_MESSAGE;
}
String UNO_Obloq::put(const String &url, const String &content, int timeOut)
{
    OBLOQ_MESSAGE = "";
    if (currentStatus != OBLOQ_WIFI_CONNECT_OK)
        return OBLOQ_MESSAGE;
    sendMessage(/*"Http put", */ "|3|3|http://" + http_host + "/" + url + "," + content + "|\r");

    uint32_t time = millis();
    while (OBLOQ_CMD != OBLOQ_HTTP_RECEIVE_OK)
    {
        obloqTick();
        if (millis() - time >= timeOut)
        {
            return "time out!";
        }
    }
    return OBLOQ_MESSAGE;
}

UNO_Obloq::UNO_Obloq() : IPAddress("0.0.0.0")
{
    parameteReset();
}
UNO_Obloq::~UNO_Obloq()
{
    parameteReset();
}

ObloqErrorCode UNO_Obloq::ping()
{
    while (OBLOQ_CMD != OBLOQ_PING_OK)
    {
        sendMessage(/*"Ping", */ "|1|1|\r");
        obloqTick();
        delay(100);
    }
    while (OBLOQ_CMD != OBLOQ_GET_VERSION_OK)
    {
        sendMessage(/*"Get version", */ "|1|2|\r");
        obloqTick();
        delay(100);
    }
    currentStatus = OBLOQ_PING_OK;
    //debugPeek("Successful connection\nFirmware version: V" + OBLOQ_MESSAGE, 0);
    return OBLOQ_OK;
}
ObloqErrorCode UNO_Obloq::wifiConnect(const String &ssid, const String &pwd)
{
    if (currentStatus != OBLOQ_PING_OK)
        return OBLOQ_STEPS_ERROR;
    sendMessage(/*"Wifi connect", */ "|2|1|" + ssid + "," + pwd + "|\r");
    while (OBLOQ_CMD != OBLOQ_WIFI_CONNECT_OK)
    {
        obloqTick();
        if (OBLOQ_CMD == OBLOQ_WIFI_CONNECT_FAILURE)
        {
            currentError = OBLOQ_WIFI_CONNECT_FAILURE;
            //debugTask();
            return OBLOQ_WIFI_CONNECT_FAILURE;
        }
    }
    currentStatus = OBLOQ_WIFI_CONNECT_OK;
    IPAddress = OBLOQ_MESSAGE;
    //debugPeek("WIFI Connection Successful", 0);
    return OBLOQ_OK;
}
ObloqErrorCode UNO_Obloq::mqttConnect(const String &iotId,
                                      const String &iotPwd,
                                      const String iotTopics[],
                                      const String &server,
                                      uint32_t port)
{
    if (currentStatus != OBLOQ_WIFI_CONNECT_OK)
        return OBLOQ_STEPS_ERROR;
    for (uint8_t i = 0; i < MAXTOPICNUMBER; i++)
    {
        if (iotTopics[i] != "")
            mqttTopicArray.push_back(iotTopics[i]);
    }
    sendMessage(/*"Mqtt connect", */ "|4|1|1|" + server + "|" + port + "|" + iotId + "|" + iotPwd + "|\r");
    while (OBLOQ_CMD != OBLOQ_MQTT_CONNECT_OK)
    {
        obloqTick();
        if (OBLOQ_CMD == OBLOQ_MQTT_CONNECT_FAILURE)
        {
            currentError = OBLOQ_MQTT_CONNECT_FAILURE;
            //debugTask();
            return OBLOQ_MQTT_CONNECT_FAILURE;
        }
    }
    currentStatus = OBLOQ_MQTT_CONNECT_OK;
    //debugPeek("MQTT Connection Successful", 0);
    return OBLOQ_OK;
}
ObloqErrorCode UNO_Obloq::subTopic()
{
    if (currentStatus != OBLOQ_MQTT_CONNECT_OK)
        return OBLOQ_STEPS_ERROR;
    for (int i = 0; i < mqttTopicArray.size(); i++)
    {
        sendMessage(/*"Subtopic", */ "|4|1|2|" + mqttTopicArray[i] + "|\r");
        while (OBLOQ_CMD != OBLOQ_SUB_OK)
        {
            obloqTick();
            if (OBLOQ_CMD == OBLOQ_SUB_FAILURE)
            {
                currentError = OBLOQ_SUB_FAILURE;
                //debugTask();
                return OBLOQ_SUB_FAILURE;
            }
        }
        delay(500);
    }
    delay(5000);
    currentStatus = OBLOQ_SUB_OK;
    //debugPeek("Subscribe to the success", 0);
    return OBLOQ_OK;
}

void UNO_Obloq::parameteReset()
{
    obloqSerial = NULL;
    http_host = "";
    mqttTopicArray.clear();
    OBLOQ_CMD = OBLOQ_NONE;
    OBLOQ_MESSAGE = "";
    //currentError = OBLOQ_OK;
    currentStatus = OBLOQ_NONE;
    for (uint8_t i = 0; i < MAXTOPICNUMBER; i++)
    {
        mqtt_message[i].available = false;
        mqtt_message[i].message = "";
    }
}
void UNO_Obloq::sendMessage(/*const String& steps, */ String message)
{
    OBLOQ_CMD = OBLOQ_NONE;
    //debugPeek("send:" + message + "\\r [" + steps + "]", 0);
    obloqSerial->print(message);
}
/*
void UNO_Obloq::debugPeek(const String& debugMessage, int8_t errorCode){
    if(debugHandle)
        debugHandle(debugMessage, errorCode);
}
void UNO_Obloq::debugTask()
{
    switch(currentError){
        case OBLOQ_MQTT_CONNECT_FAILURE:
            debugPeek("receive:|4|1|1|2|\\r  [Mqtt connect failure]", OBLOQ_MQTT_CONNECT_FAILURE);
            //currentError = OBLOQ_OK;
        break;
        case OBLOQ_WIFI_CONNECT_FAILURE:
            debugPeek("receive:|2|4|\\r  [Check that the SSID and PWD are entered correctly]", OBLOQ_WIFI_CONNECT_FAILURE);
            //currentError = OBLOQ_OK;
        break;
        case OBLOQ_SUB_FAILURE:
            debugPeek("receive:|4|1|2|2|2|\\r  [Topic subscription failed]", OBLOQ_SUB_FAILURE);
            //currentError = OBLOQ_OK;
        break;
        case OBLOQ_PULISH_FAILURE:
            debugPeek("receive:|4|1|3|2|\\r  [Pulish failure]", OBLOQ_PULISH_FAILURE);
            //currentError = OBLOQ_OK;
        break;
        }
}
*/
int UNO_Obloq::isIP(char str[])
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
void UNO_Obloq::obloqTick()
{
    static String Obloq_message_str;

    while (obloqSerial && obloqSerial->available() > 0)
    {
        int c;
        while (obloqSerial->available())
        {
            c = obloqSerial->read();
            if (c >= 0)
                Obloq_message_str += (char)c;
            if (c == '\r')
            {
                int size = Obloq_message_str.length();
                if (Obloq_message_str[1] == '4')
                {
                    if (Obloq_message_str[3] == '1')
                    {
                        if (Obloq_message_str[5] == '1')
                        {
                            if (Obloq_message_str[7] == '1')
                            { //  |4|1|1|1|
                                OBLOQ_CMD = OBLOQ_MQTT_CONNECT_OK;
                                OBLOQ_MESSAGE = "";
                            }
                            else if (Obloq_message_str[7] == '2')
                            { //  |4|1|1|2|
                                OBLOQ_CMD = OBLOQ_MQTT_CONNECT_FAILURE;
                                OBLOQ_MESSAGE = Obloq_message_str.substring(9, size - 2 - 9);
                            }
                        }
                        else if (Obloq_message_str[5] == '2')
                        {
                            if (Obloq_message_str[7] == '1')
                            { //  |4|1|2|1|
                                OBLOQ_CMD = OBLOQ_SUB_OK;
                                OBLOQ_MESSAGE = "";
                            }
                            else if (Obloq_message_str[7] == '2' && Obloq_message_str[9] == '2')
                            { //  |4|1|2|2|2|
                                OBLOQ_CMD = OBLOQ_SUB_FAILURE;
                                OBLOQ_MESSAGE = "";
                            }
                        }
                        else if (Obloq_message_str[5] == '3')
                        {
                            if (Obloq_message_str[7] == '1')
                            { //  |4|1|3|1|
                                OBLOQ_CMD = OBLOQ_PULISH_OK;
                                OBLOQ_MESSAGE = "";
                            }
                            else if (Obloq_message_str[7] == '2')
                            { //  |4|1|3|2|
                                OBLOQ_CMD = OBLOQ_PULISH_FAILURE;
                                currentError = OBLOQ_PULISH_FAILURE;
                                OBLOQ_MESSAGE = "";
                            }
                        }
                        else if (Obloq_message_str[5] == '4')
                        {
                            if (Obloq_message_str[7] == '1')
                            { //  |4|1|4|1|
                                OBLOQ_CMD = OBLOQ_MQTT_DISCONNECT_OK;
                                OBLOQ_MESSAGE = "";
                            }
                        }
                        else if (Obloq_message_str[5] == '5')
                        { //  |4|1|5|
                            String str = Obloq_message_str.substring(7, size - 1);
                            int num = str.indexOf("|");
                            String topic = str.substring(0, num);
                            OBLOQ_MESSAGE = str.substring(num + 1, str.length() - 1);
                            for (int i = 0; i < mqttTopicArray.size(); i++)
                            {
                                if (topic == mqttTopicArray[i])
                                {
                                    mqtt_message[i].message = OBLOQ_MESSAGE;
                                    mqtt_message[i].available = true;
                                }
                            }
                            OBLOQ_CMD = OBLOQ_MQTT_RECEIVE_OK;
                        }
                    }
                }
                else if (Obloq_message_str[1] == '1')
                {
                    if (Obloq_message_str[3] == '1')
                    { // |1|1|
                        OBLOQ_CMD = OBLOQ_PING_OK;
                        OBLOQ_MESSAGE = "";
                    }
                    else if (Obloq_message_str[3] == '2')
                    { // |1|2|
                        OBLOQ_CMD = OBLOQ_GET_VERSION_OK;
                        OBLOQ_MESSAGE = Obloq_message_str.substring(5, size - 2); //version
                    }
                }
                else if (Obloq_message_str[1] == '2')
                {
                    if (Obloq_message_str[3] == '3')
                    { // |2|3|
                        OBLOQ_CMD = OBLOQ_WIFI_CONNECT_OK;
                        OBLOQ_MESSAGE = Obloq_message_str.substring(5, size - 2); //IP addr
                    }
                    else if (Obloq_message_str[3] == '4')
                    { // |2|4|
                        OBLOQ_CMD = OBLOQ_WIFI_CONNECT_FAILURE;
                        IPAddress = "0.0.0.0";
                        OBLOQ_MESSAGE = "";
                    }
                }
                else if (Obloq_message_str[1] == '3')
                { // |3|errcode|message|
                    String str = Obloq_message_str.substring(3, size - 1);
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
                }
                else
                {
                }
                Obloq_message_str = "";
                break;
            }
        }
    }
}
