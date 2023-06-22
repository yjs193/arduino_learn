#include "DFRobot_Obloq_I2C.h"

DFRobot_Obloq_I2C::DFRobot_Obloq_I2C(bool dbg) : Version("0.0"), IPAddress("0.0.0.0"), wireInterval(20),
                                                 recordTime(millis()), timer(false), timerDuration(0), timerStart(0), debug(dbg),
                                                 returnMessage(""), debugHandle(NULL), runningNode(RUNNING_NODE_NONE), returnStatus(RETURN_STATUS_NONE)
{
    reset();
}

void DFRobot_Obloq_I2C::startConnect(const String &ssid, const String &pwd, const String &iotId, const String &iotPwd, const String iotTopics[], const String &server, uint32_t port)
{
#if defined(ARDUINO_ARCH_RP2040)
    Wire.begin();
#endif
    // newObloqInit();
    // 建立串口通讯
    ping();
    // WiFi连接
    wifiConnect(ssid, pwd);
    // Mqtt连接
    mqttConnect(iotId, iotPwd, iotTopics, server, port);
    // 订阅Topic
    subTopic();
    // 开启后台任务
#if defined(NRF5) || defined(NRF52833)
    background();
#elif defined(ARDUINO_ARCH_RP2040)
    background();
#endif
}

void DFRobot_Obloq_I2C::newObloqInit(){

    // uint8_t buf[3] = {0x1E, 0x02, 0x17};
    // Wire.beginTransmission(COMMAND_WIRE_ADDRESS);
    // for (size_t i = 0; i < 3; i++)
    // {
    //     Wire.write(buf[i]);
    // }
    // Wire.endTransmission();
    // delay(200);
}

void DFRobot_Obloq_I2C::startConnect(const String &ssid, const String &pwd, const String &ip, uint32_t port)
{
    startConnect(ssid, pwd, ip);
    WRITE_PARAMETER(COMMAND_WRITE_HTTP_PORT, String(port));
    runningNode = RUNNING_NODE_RUNING;
}

void DFRobot_Obloq_I2C::startConnect(const String &ssid, const String &pwd, const String &host)
{
#if defined(ARDUINO_ARCH_RP2040)
    Wire.begin();
#endif
    // 建立串口通讯
    ping();
    // WiFi连接
    wifiConnect(ssid, pwd);
    WRITE_PARAMETER(COMMAND_WRITE_HTTP_IP, host);
    runningNode = RUNNING_NODE_RUNING;
}

void DFRobot_Obloq_I2C::registerDubugHandle(const DebugHandle handle)
{
    if (!handle)
        return;
    debugHandle = handle;
}

void DFRobot_Obloq_I2C::registerMqttHandle(const MqttHandle handles[])
{
    if (handles == NULL)
        return;
    for (int i = 0; i < SUBSCRIBE_UPPER_LIMIT; ++i)
    {
        if (handles[i])
        {
            mqttHandle[i] = handles[i];
        }
    }
}

void DFRobot_Obloq_I2C::publish(Topicnum topic_num, const String &message)
{
    DEBUGGER_LOG(this, ">=====> publish");
    // if (!idle())
    //     return;
    CHECK_RUNNING_NODE(RUNNING_NODE_RUNING,
                       peek(String("Publish: " + message + String(" to ") + String(mqttTopicArray[topic_num])), 0),
                       peek(String("Please reinitialize"), -1);
                       return;)
    if (topic_num >= mqttTopicArray.size())
    {
        peek(String("The number of topics exceeds the maximum"), -1);
        return;
    }
    EXECUTE_CMD(COMMAND_EXECUTE_MQTT_PUBLISH0 + topic_num, RETURN_STATUS_MQTT_PUBLISH_SUCCESS, 200, 2000, false, message);
    CHECK_RETURN_STATUS(RETURN_STATUS_MQTT_PUBLISH_SUCCESS,
                        peek(String("Publish success"), 0),
                        peek(String("Publish failure"), -1));
    READ_DATA(RETURN_STATUS_MQTT_RECEIVE_MESSAGE, 100, 2000);
    delay(100);
}

void DFRobot_Obloq_I2C::publish(Topicnum topic_num, double f, int precision)
{
    if (topic_num >= mqttTopicArray.size())
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

void DFRobot_Obloq_I2C::publish(Topicnum topic_num, int32_t i)
{
    if (topic_num >= mqttTopicArray.size())
        return;
    char buffer[34];
    memset(buffer, 0, 34);
    itoa(i, buffer, 10);
    publish(topic_num, (const String &)buffer);
}

String DFRobot_Obloq_I2C::getMessage(Topicnum topic_num)
{
    MUTEX_TICK(this);
    if (mqtt_message[topic_num].available)
    {
        mqtt_message[topic_num].available = false;
        return mqtt_message[topic_num].message;
    }
    else
    {
        return "";
    }
}

bool DFRobot_Obloq_I2C::available(Topicnum topic_num)
{
    MUTEX_TICK(this);
    return mqtt_message[topic_num].available;
}

String DFRobot_Obloq_I2C::get(const String &url, int timeOut)
{
    DEBUGGER_LOG(this, ">=====> http get");
    CHECK_RUNNING_NODE(RUNNING_NODE_RUNING,
                       peek(String("GET ") + String(url), 0),
                       peek(String("Please reinitialize"), -1);
                       return "";)
    EXECUTE_CMD(COMMAND_EXECUTE_HTTP_GET, RETURN_STATUS_HTTP_SUCCESS, 200, timeOut, false, url);
    CHECK_RETURN_STATUS(RETURN_STATUS_HTTP_SUCCESS,
                        peek(String("GET success"), 0),
                        peek(String("GET failure"), -1);
                        return "";);
    return returnMessage;
}

String DFRobot_Obloq_I2C::post(const String &url, const String &content, int timeOut)
{
    DEBUGGER_LOG(this, ">=====> http post");
    CHECK_RUNNING_NODE(RUNNING_NODE_RUNING,
                       peek(String("POST ") + String(url) + String(" ") + String(content), 0),
                       peek(String("Please reinitialize"), -1);
                       return "";)
    String posturl = url + "," + content;
    EXECUTE_CMD(COMMAND_EXECUTE_HTTP_POST, RETURN_STATUS_HTTP_SUCCESS, 200, timeOut, false, posturl);
    CHECK_RETURN_STATUS(RETURN_STATUS_HTTP_SUCCESS,
                        peek(String("POST success"), 0),
                        peek(String("POST failure"), -1);
                        return "";);
    return returnMessage;
}

String DFRobot_Obloq_I2C::put(const String &url, const String &content, int timeOut)
{
    DEBUGGER_LOG(this, ">=====> http put");
    CHECK_RUNNING_NODE(RUNNING_NODE_RUNING,
                       peek(String("PUT ") + String(url) + String(" ") + String(content), 0),
                       peek(String("Please reinitialize"), -1);
                       return "";)
    String puturl = url + "," + content;
    EXECUTE_CMD(COMMAND_EXECUTE_HTTP_PUT, RETURN_STATUS_HTTP_SUCCESS, 200, timeOut, false, puturl);
    CHECK_RETURN_STATUS(RETURN_STATUS_HTTP_SUCCESS,
                        peek(String("PUT success"), 0),
                        peek(String("PUT failure"), -1);
                        return "";);
    return returnMessage;
}

void DFRobot_Obloq_I2C::ping()
{
    // if (!idle())
    //     return;
    // DEBUGGER_LOG(this, ">=====> ping");
    // EXECUTE_CMD(COMMAND_EXECUTE_PING, RETURN_STATUS_PING_SUCCESS, 100, 20000, true);
    // CHECK_RETURN_STATUS(RETURN_STATUS_PING_SUCCESS,
    //                     peek(String("Ping success"), 0),
    //                     peek(String("Ping failure"), -1);
    //                     return;);
    // if (!idle())
    //     return;
    DEBUGGER_LOG(this, ">=====> get version");
    EXECUTE_CMD(COMMAND_EXECUTE_VERSION, RETURN_STATUS_VERSION_SUCCESS, 100, 2000, false);
    CHECK_RETURN_STATUS(RETURN_STATUS_VERSION_SUCCESS,
                        peek(String("Get version success"), 0),
                        peek(String("Get version failure"), -1);
                        return;);
    runningNode = RUNNING_NODE_PING;
    return;
}

void DFRobot_Obloq_I2C::wifiConnect(const String &ssid, const String &pwd)
{
    DEBUGGER_LOG(this, ">=====> wifi connect");
    CHECK_RUNNING_NODE(RUNNING_NODE_PING,
                       peek(String("Wifi connect ") + String(ssid) + String(" ") + String(pwd), 0),
                       peek(String("Please reinitialize"), -1);
                       return;)
    // if (!idle())
    //     return;
    WRITE_PARAMETER(COMMAND_WRITE_WIFI_NAME, ssid);
    WRITE_PARAMETER(COMMAND_WRITE_WIFI_PASSWORD, pwd);
    EXECUTE_CMD(COMMAND_EXECUTE_WIFI_CONNECT, RETURN_STATUS_WIFI_CONNECTED, 200, 10000, false);
    CHECK_RETURN_STATUS(
        RETURN_STATUS_WIFI_CONNECTED,
        peek(String("Wifi connect success"), 0),
        switch (returnStatus) {
            case RETURN_STATUS_WIFI_IDLE:
                peek(String("Wifi connect not responding"), -1);
                break;
            case RETURN_STATUS_WIFI_CONNECTING:
                peek(String("Wifi connect timeout"), -1);
                break;
        } return;);
    READ_DATA(RETURN_STATUS_WIFI_GETIP_SUCCESS, 200, 2000);
    runningNode = RUNNING_NODE_WIFI_CONNECT;
    return;
}

void DFRobot_Obloq_I2C::mqttConnect(const String &iotId,
                                    const String &iotPwd,
                                    const String iotTopics[],
                                    const String &server,
                                    uint32_t port)
{
    DEBUGGER_LOG(this, ">=====> mqtt connect");
    CHECK_RUNNING_NODE(RUNNING_NODE_WIFI_CONNECT,
                       peek(String("MQTT connect ") + String(server) + String(" ") + String(port) + String(" ") + String(iotId) + String(" ") + String(iotPwd), 0),
                       peek(String("Please reinitialize"), -1);
                       return )
    // if (!idle())
    //     return;
    for (uint8_t i = 0; i < SUBSCRIBE_UPPER_LIMIT; ++i)
    {
        if (iotTopics[i] != ""){
            delay(300);
            mqttTopicArray.push_back(iotTopics[i]);
        }
    }
    WRITE_PARAMETER(COMMAND_WRITE_MQTT_SERVER, server);
    WRITE_PARAMETER(COMMAND_WRITE_MQTT_PORT, String(port));
    WRITE_PARAMETER(COMMAND_WRITE_MQTT_ID, iotId);
    WRITE_PARAMETER(COMMAND_WRITE_MQTT_PWD, iotPwd);
    EXECUTE_CMD(COMMAND_EXECUTE_MQTT_CONNECT, RETURN_STATUS_MQTT_CONNECT_SUCCESS, 200, 2000, false);
    CHECK_RETURN_STATUS(
        RETURN_STATUS_MQTT_CONNECT_SUCCESS,
        peek(String("MQTT connect success"), 0),
        peek(String("MQTT connect failure"), -1);
        return;);
    runningNode = RUNNING_NODE_MQTT_CONNECT;
    return;
}

void DFRobot_Obloq_I2C::subTopic()
{
    DEBUGGER_LOG(this, ">=====> subscribe");
    CHECK_RUNNING_NODE(RUNNING_NODE_MQTT_CONNECT,
                       peek(String("Subscribe"), 0),
                       peek(String("Please reinitialize"), -1);
                       return;)
    // if (!idle())
    //     return;
    for (int i = 0; i < mqttTopicArray.size(); i++)
    {
        EXECUTE_CMD(COMMAND_EXECUTE_MQTT_SUBSCRIBE0 + i, RETURN_STATUS_MQTT_SUBSCRIBE_SUCCESS, 100, 5000, false, mqttTopicArray[i]);
        CHECK_RETURN_STATUS(
            RETURN_STATUS_MQTT_SUBSCRIBE_SUCCESS,
            peek(String("Subscribe") + String(" ") + String(mqttTopicArray[i]) + String(" success"), 0),
            switch (returnStatus) {
                case RETURN_STATUS_MQTT_SUBSCRIBE_IDLE:
                    peek(String("Subscribe") + String(" ") + String(mqttTopicArray[i]) + String(" no response"), -1);
                    break;
                case RETURN_STATUS_MQTT_SUBSCRIBE_LIMIT:
                    peek(String("Subscribe") + String(" ") + String(mqttTopicArray[i]) + String(" limit"), -1);
                    break;
                case RETURN_STATUS_MQTT_SUBSCRIBE_FAILURE:
                    peek(String("Subscribe") + String(" ") + String(mqttTopicArray[i]) + String(" failure"), -1);
                    break;
            } return;);
        delay(1500);
    }
    delay(1000);
    runningNode = RUNNING_NODE_RUNING;
}
#if defined(NRF5) || defined(NRF52833)
void DFRobot_Obloq_I2C::background()
{
    DEBUGGER_LOG(this, ">=====> background program");
    CHECK_RUNNING_NODE(RUNNING_NODE_RUNING,
                       peek(String("Start background program"), 0),
                       peek(String("Please reinitialize"), -1);
                       return;)

    CREATE_TASK(background_task, this);
}
#elif defined(ARDUINO_ARCH_RP2040)

void DFRobot_Obloq_I2C::background()
{
    DEBUGGER_LOG(this, ">=====> background program");
    CHECK_RUNNING_NODE(RUNNING_NODE_RUNING,
                       peek(String("Start background program"), 0),
                       peek(String("Please reinitialize"), -1);
                       return;)

    CREATE_TASK(background_task, "background_task", this);
}
#endif
void DFRobot_Obloq_I2C::reset()
{
    mqttTopicArray.clear();
    for (uint8_t i = 0; i < SUBSCRIBE_UPPER_LIMIT; ++i)
    {
        mqtt_message[i].available = false;
        mqtt_message[i].message = "";
        mqttHandle[i] = NULL;
        mqttCbBusy[i] = false;
    }
    for (uint8_t i = 0; i < RETURN_STATUS_UNKNOWN; ++i)
    {
        returnStatusList[i] = false;
    }
#if !defined(ARDUINO_ARCH_RP2040)
    Wire.begin();
#endif
}

void DFRobot_Obloq_I2C::peek(String message, int8_t code)
{
    if (debugHandle)
        debugHandle(message, code);
    DEBUGGER_LOG(this, message);
}

void DFRobot_Obloq_I2C::tick()
{
    uint8_t cmdbuffer[2];
    if (read(cmdbuffer, 2, COMMAND_READTYPE_REG) != 2)
    {
        DEBUGGER_LOG(this, "Read reg error");
        return;
    }
    uint8_t command = cmdbuffer[0];
    uint8_t status = cmdbuffer[1];
    returnStatus = RETURN_STATUS_UNKNOWN;

    if (!CHECK_RETURN_COMMAND(command))
    {
        DEBUGGER_LOG(this, "Unknown command");
        return;
    }

    switch (command)
    {
    case RETURN_COMMAND_NONE:
    {
        returnStatus = RETURN_STATUS_NONE;
    }
    break;
    case RETURN_COMMAND_PING:
    {
        if (status == 0x00)
            returnStatus = RETURN_STATUS_PING_SUCCESS;
        if (status == 0x01)
            returnStatus = RETURN_STATUS_PING_SUCCESS;
    }
    break;
    case RETURN_COMMAND_WIFI:
    {
        if (status == 0x00)
            returnStatus = RETURN_STATUS_WIFI_IDLE;
        if (status == 0x02)
            returnStatus = RETURN_STATUS_WIFI_CONNECTING;
        if (status == 0x03)
            returnStatus = RETURN_STATUS_WIFI_CONNECTED;
    }
    break;
    case RETURN_COMMAND_WIFI_IP:
    {
        uint8_t datalength = status;
        uint8_t databuffer[datalength+1];
        if (read(databuffer, datalength, COMMAND_READTYPE_DATA) != datalength)
        {
            DEBUGGER_LOG(this, "Read data error");
            return;
        }
        databuffer[datalength] = '\0';
        IPAddress = String((char *)databuffer);
        returnStatus = RETURN_STATUS_WIFI_GETIP_SUCCESS;
    }
    break;
    case RETURN_COMMAND_MQTT:
    {
        if (status == 0x00)
            returnStatus = RETURN_STATUS_MQTT_IDLE;
        if (status == 0x01)
            returnStatus = RETURN_STATUS_MQTT_CONNECT_SUCCESS;
        if (status == 0x02)
            returnStatus = RETURN_STATUS_MQTT_CONNECT_FAILURE;
    }
    break;
    case RETURN_COMMAND_MQTT_SUBSCRIBE:
    {
        if (status == 0x00)
            returnStatus = RETURN_STATUS_MQTT_SUBSCRIBE_IDLE;
        if (status == 0x01)
            returnStatus = RETURN_STATUS_MQTT_SUBSCRIBE_SUCCESS;
        if (status == 0x02)
            returnStatus = RETURN_STATUS_MQTT_SUBSCRIBE_LIMIT;
        if (status == 0x03)
            returnStatus = RETURN_STATUS_MQTT_SUBSCRIBE_FAILURE;
    }
    break;
    case RETURN_COMMAND_MQTT_TOPIC0:
    case RETURN_COMMAND_MQTT_TOPIC1:
    case RETURN_COMMAND_MQTT_TOPIC2:
    case RETURN_COMMAND_MQTT_TOPIC3:
    case RETURN_COMMAND_MQTT_TOPIC4:
    {
        uint8_t datalength = status;
        uint8_t databuffer[datalength + 1];
        databuffer[datalength] = '\0';
        if (read(databuffer, datalength, COMMAND_READTYPE_DATA) != datalength)
        {
            DEBUGGER_LOG(this, "Read data error");
            return;
        }
        mqtt_message[command - RETURN_COMMAND_MQTT_TOPIC0].message = String((char *)databuffer);
        mqtt_message[command - RETURN_COMMAND_MQTT_TOPIC0].available = true;
        RUN_TOPIC_STATIC(command - RETURN_COMMAND_MQTT_TOPIC0, this);
        returnStatus = RETURN_STATUS_MQTT_RECEIVE_MESSAGE;
    }
    break;
    case RETURN_COMMAND_HTTP:
    {
        uint8_t datalength = status;
        uint8_t databuffer[datalength + 1];
        databuffer[datalength] = '\0';
        if (read(databuffer, datalength, COMMAND_READTYPE_DATA) != datalength)
        {
            DEBUGGER_LOG(this, "Read data error");
            return;
        }
        returnMessage = String((char *)databuffer);
        returnStatus = RETURN_STATUS_HTTP_SUCCESS;
    }
    break;
    case RETURN_COMMAND_HTTP_ERROR:
    {
        returnMessage = String(status);
        returnStatus = RETURN_STATUS_HTTP_FAILURE;
    }
    break;
    case RETURN_COMMAND_VERSION:
    {
        uint8_t datalength = status;
        uint8_t databuffer[datalength + 1];
        databuffer[datalength] = '\0';
        if (read(databuffer, datalength, COMMAND_READTYPE_DATA) != datalength)
        {
            DEBUGGER_LOG(this, "Read data error");
            return;
        }
        Version = String((char *)databuffer);
        returnStatus = RETURN_STATUS_VERSION_SUCCESS;
    }
    break;
    case RETURN_COMMAND_MQTT_PUBLISH:
    {
        // if (status == 0x00)
        //     returnStatus = RETURN_STATUS_MQTT_PUBLISH_FAILURE;
        // else
        returnStatus = RETURN_STATUS_MQTT_PUBLISH_SUCCESS;
    }
    break;
    }
    returnStatusList[returnStatus] = false;
    return;
}

void DFRobot_Obloq_I2C::timerInit(long time)
{
    timer = true;
    timerDuration = time;
    timerStart = millis();
}

bool DFRobot_Obloq_I2C::timerCount()
{
    if (timer)
    {
        if (millis() - timerStart >= timerDuration)
        {
            timer = false;
            return false;
        }
        return true;
    }
    return false;
}

bool DFRobot_Obloq_I2C::idle(long timeout)
{
    int count = 0;
    uint8_t cmdbuffer[2];
    timerInit(timeout);
    while (tickLock)
        delay(1);
    tickLock = true;
    while (1)
    {
        if (!timerCount())
        {
            peek(String("idle timeout"), -1);
            DEBUGGER_LOG(this, "idle timeout");
            tickLock = false;
            return false;
        }
        DEBUGGER_LOG(this, );
        if (read(cmdbuffer, 2, COMMAND_READTYPE_REG) != 2)
        {
            peek(String("idle error"), -1);
            DEBUGGER_LOG(this, "idle error");
            tickLock = false;
            return false;
        }
        {
            if (cmdbuffer[0] == RETURN_COMMAND_NONE && cmdbuffer[1] == RETURN_STATUS_NONE)
            // if (cmdbuffer[0] == 0x77 && cmdbuffer[1] == 0xFF)
                count++;
            else
                count = 0;
        }
        if (count >= 5)
        {
            tickLock = false;
            return true;
        }
    }
}

uint8_t DFRobot_Obloq_I2C::read(void *pBuf, size_t size, CommandReadTypeIndex type)
{
    delay(20);
    uint8_t result = readReg(COMMAND_WIRE_ADDRESS, type == COMMAND_READTYPE_REG ? COMMAND_WIRE_REGTSTER : COMMAND_WIRE_DATA, pBuf, size);
    if (DEBUGGER || debug)
    {
        String info = type == COMMAND_READTYPE_DATA ? "data:" : "reg:";
        for (int i = 0; i < size; ++i)
            info = info + String(" ") + String(((uint8_t *)pBuf)[i]);
        DEBUGGER_LOG(this, info);
    }
    return result;
}

void DFRobot_Obloq_I2C::write(CommandWriteIndex command, const String &data)
{
    uint8_t *buffer = (uint8_t *)malloc(sizeof(uint8_t) * (data.length() + 3));
    buffer[0] = COMMAND_TYPE_WRITE;
    buffer[1] = command;
    buffer[2] = data.length();
    memcpy(buffer + 3, data.c_str(), data.length());
    writeReg(COMMAND_WIRE_ADDRESS, COMMAND_WIRE_REGTSTER, buffer, data.length() + 3);
    free(buffer);
}

void DFRobot_Obloq_I2C::execute(CommandExecutEIndex command)
{
    uint8_t buffer[2] = {COMMAND_TYPE_EXECUTE, command};
    writeReg(COMMAND_WIRE_ADDRESS, COMMAND_WIRE_REGTSTER, buffer, 2);
    // delay(10);
}

void DFRobot_Obloq_I2C::execute(CommandExecutEIndex command, const String &data)
{
    uint8_t *buffer = (uint8_t *)malloc(sizeof(uint8_t) * (data.length() + 3));
    buffer[0] = COMMAND_TYPE_EXECUTE;
    buffer[1] = command;
    buffer[2] = data.length();
    memcpy(buffer + 3, data.c_str(), data.length());
    writeReg(COMMAND_WIRE_ADDRESS, COMMAND_WIRE_REGTSTER, buffer, data.length() + 3);
    free(buffer);
}

void DFRobot_Obloq_I2C::writeReg(uint8_t address, uint8_t reg, void *pBuf, size_t size)
{
    while (millis() - recordTime < wireInterval)
        delay(1);
    uint8_t *_pBuf = (uint8_t *)pBuf;
    Wire.beginTransmission(address);
    Wire.write(reg);
    if (size > 32)
    {
        uint16_t j = 1;
        for (uint8_t i = 0; i < size; i++)
        {
            if (i >= (31 * j))
            {
                Wire.endTransmission(false);
                Wire.beginTransmission(address);
                j++;
            }
            Wire.write(_pBuf[i]);
        }
    }
    else
    {
        for (size_t i = 0; i < size; i++)
        {
            Wire.write(_pBuf[i]);
        }
    }
    Wire.endTransmission();
    recordTime = millis();
}

uint8_t DFRobot_Obloq_I2C::readReg(uint8_t address, uint8_t reg, void *pBuf, size_t size)
{
    if (pBuf == NULL)
    {
        return 0;
    }
    uint8_t *_pBuf = (uint8_t *)pBuf;
    Wire.beginTransmission(address);
    Wire.write(reg);
    if (reg == COMMAND_WIRE_REGTSTER)
    {
        Wire.write(COMMAND_TYPE_READ);
        Wire.write(COMMAND_READ);
    }
    if (Wire.endTransmission() != 0)
    {
        return 0;
    }
    Wire.requestFrom(address, size);
    for (size_t i = 0; i < size; i++)
    {
        _pBuf[i] = Wire.read();
    }
    return size;
}
