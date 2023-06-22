#include "DFRobot_Esp8266MQTT.h"
#include "esp8266HMAC_SHA1.h"
#include "esp8266token.h"

#if CONFIG_FREERTOS_UNICORE
#define ARDUINO_RUNNING_CORE_MQTT 0
#else
#define ARDUINO_RUNNING_CORE_MQTT 1
#endif
CHMAC_SHA1 esp8266_MyHmac_Sha1;
DFRobot_Esp8266MQTT *esp8266_mqttPt;
DF_TOKEN esp8266_token;
volatile static bool firstRegisteredIotDebugtCallback;
volatile static bool firstRegisteredMqttCallback;
volatile static bool mqttCallbackBusy;
volatile static int8_t currentMqttStatus;
uint8_t esp8266_mqtt_count;

String esp8266_byteToHexStr(unsigned char byte_arr[], int arr_len)  
{  
    String hexstr;  
    for (int i=0;i<arr_len;i++)  
    {
        char hex1;  
        char hex2;  
        int value=byte_arr[i]; 
        int v1=value/16;
        int v2=value % 16;
        if (v1>=0&&v1<=9)
            hex1=(char)(48+v1);
        else
            hex1=(char)(55+v1);
        if (v2>=0&&v2<=9)
            hex2=(char)(48+v2);
        else
            hex2=(char)(55+v2);

        hexstr=hexstr+hex1+hex2;
    }  
    return hexstr;
}

static void esp8266_mqttCallback(char * topic, BYTE * payload, unsigned int len){
    mqttCallbackBusy = true;
    String _topic = topic;

    for(int i = 0; i < esp8266_mqttPt->mqtt_topicArray.size(); i++){
        if(_topic == esp8266_mqttPt->mqtt_topicArray[i]){
            String _payload = (char *)payload;
            _payload = _payload.substring(0,len);
            for(int j = 0; j < esp8266_mqttPt->mqtt_handleCb[i].size(); j++){
                if(_payload == esp8266_mqttPt->mqtt_delimiters[i][j]){
                    esp8266_mqttPt->mqtt_handleCb[i][j]();
                    break;
                }
            }
            if(esp8266_mqttPt->mqtt_msgHandleCb[i])
                esp8266_mqttPt->mqtt_msgHandleCb[i](_payload);
            break;
        }
    }
    mqttCallbackBusy = false;
}

static void esp8266_subscribe(){
    for(int i = 0; i < esp8266_mqttPt->mqtt_topicArray.size(); i++){
        if(esp8266_mqttPt->mqtt_topicArray[i] != "$dp"){
            //Serial.println(esp8266_mqttPt->mqtt_topicArray[i].c_str());
            esp8266_client.subscribe(esp8266_mqttPt->mqtt_topicArray[i].c_str());
        }
    }
}

static void esp8266_mqttTask(){
    if(esp8266_client.connected())
        esp8266_client.loop();
}

static void esp8266_iotDebugTask(){
    int mqttStatus;
    long timeOut;
    mqttStatus = esp8266_client.state();
    if(WiFi.status() == WL_CONNECTED){
        switch(currentMqttStatus){
            case iotStatusProtocol::MqttConnectStart:
                esp8266_mqttPt->iotDebug.message = "Start connecting mqtt";
                esp8266_mqttPt->iotDebug.error = 0;
                esp8266_executeCallbackTask(esp8266_mqttPt->iotDebug.message, esp8266_mqttPt->iotDebug.error);
                timeOut = millis();
                if(!esp8266_client.connect(esp8266_mqttPt->_clientId,esp8266_mqttPt->_username,esp8266_mqttPt->_password))
                {
                    delay(10);
                    esp8266_mqtt_count--;
                    if(esp8266_mqtt_count)
                        break;
                }
                currentMqttStatus = iotStatusProtocol::MqttConnecting;
                esp8266_subscribe();
                if(firstRegisteredMqttCallback) {
                    esp8266_mqttPt->iotDebug.tarefa2.task(50,esp8266_mqttTask);
                    firstRegisteredMqttCallback = false;
                }
                break;
            case iotStatusProtocol::MqttConnecting:
                if(mqttStatus == MQTT_CONNECTION_TIMEOUT) {
                    esp8266_mqttPt->iotDebug.message = "The server didn’t respond within the keepalive time, please check the parameters!";
                    esp8266_mqttPt->iotDebug.error = IOT_MQTT_CONNECTION_TIMEOUT;
                    currentMqttStatus = iotStatusProtocol::NoneStatus;
                    esp8266_executeCallbackTask(esp8266_mqttPt->iotDebug.message, esp8266_mqttPt->iotDebug.error);
                }else if(mqttStatus == MQTT_CONNECTION_LOST){
                    esp8266_mqttPt->iotDebug.message = "The network connection was broken, please check the parameters!";
                    esp8266_mqttPt->iotDebug.error = IOT_MQTT_CONNECTION_LOST;
                    currentMqttStatus = iotStatusProtocol::NoneStatus;
                    esp8266_executeCallbackTask(esp8266_mqttPt->iotDebug.message, esp8266_mqttPt->iotDebug.error);
                }else if(mqttStatus == MQTT_CONNECT_FAILED){
                    esp8266_mqttPt->iotDebug.message = "The network connection failed! Please check the parameters!";
                    esp8266_mqttPt->iotDebug.error = IOT_MQTT_CONNECT_FAILED;
                    currentMqttStatus = iotStatusProtocol::NoneStatus;
                    esp8266_executeCallbackTask(esp8266_mqttPt->iotDebug.message, esp8266_mqttPt->iotDebug.error);
                }else if(mqttStatus == MQTT_CONNECTED){
                    esp8266_mqttPt->iotDebug.message = String("Mqtt connect successfully\n") + \
                            " [ Server: " + String(esp8266_mqttPt->_mqttServer) + " ]\n" + \
                            " [ Port: " + esp8266_mqttPt->_port + " ]\n" + \
                            " [ ClientId: " + esp8266_mqttPt->_clientId + " ]\n" + \
                            " [ Username: " + esp8266_mqttPt->_username + " ]\n" + \
                            " [ Password: " + esp8266_mqttPt->_password + " ]";
                    esp8266_mqttPt->iotDebug.error = IOT_SUCCESSFULLY;
                    currentMqttStatus = iotStatusProtocol::MqttConnectEnd;
                    esp8266_executeCallbackTask(esp8266_mqttPt->iotDebug.message, esp8266_mqttPt->iotDebug.error);
                }else if(mqttStatus == MQTT_CONNECT_BAD_PROTOCOL){
                    esp8266_mqttPt->iotDebug.message = "The server doesn’t support the requested version of MQTT, please check the parameters!";
                    esp8266_mqttPt->iotDebug.error = IOT_MQTT_CONNECT_BAD_PROTOCOL;
                    currentMqttStatus = iotStatusProtocol::NoneStatus;
                    esp8266_executeCallbackTask(esp8266_mqttPt->iotDebug.message, esp8266_mqttPt->iotDebug.error);
                }else if(mqttStatus == MQTT_CONNECT_BAD_CLIENT_ID){
                    esp8266_mqttPt->iotDebug.message = "The server rejected the client identifier, please check the parameters!";
                    esp8266_mqttPt->iotDebug.error = IOT_MQTT_CONNECT_BAD_CLIENT_ID;
                    currentMqttStatus = iotStatusProtocol::NoneStatus;
                    esp8266_executeCallbackTask(esp8266_mqttPt->iotDebug.message, esp8266_mqttPt->iotDebug.error);
                }else if(mqttStatus == MQTT_CONNECT_UNAVAILABLE){
                    esp8266_mqttPt->iotDebug.message = "The server was unable to accept the connection, please check the parameters!";
                    esp8266_mqttPt->iotDebug.error = IOT_MQTT_CONNECT_UNAVAILABLE;
                    currentMqttStatus = iotStatusProtocol::NoneStatus;
                    esp8266_executeCallbackTask(esp8266_mqttPt->iotDebug.message, esp8266_mqttPt->iotDebug.error);
                }else if(mqttStatus == MQTT_CONNECT_BAD_CREDENTIALS){
                    esp8266_mqttPt->iotDebug.message = "The username/password were rejected, please check the parameters!";
                    esp8266_mqttPt->iotDebug.error = IOT_MQTT_CONNECT_BAD_CREDENTIALS;
                    currentMqttStatus = iotStatusProtocol::NoneStatus;
                    esp8266_executeCallbackTask(esp8266_mqttPt->iotDebug.message, esp8266_mqttPt->iotDebug.error);
                }else if(mqttStatus == MQTT_CONNECT_UNAUTHORIZED){
                    esp8266_mqttPt->iotDebug.message = "The client was not authorized to connect! please check the parameters!";
                    esp8266_mqttPt->iotDebug.error = IOT_MQTT_CONNECT_UNAUTHORIZED;
                    currentMqttStatus = iotStatusProtocol::NoneStatus;
                    esp8266_executeCallbackTask(esp8266_mqttPt->iotDebug.message, esp8266_mqttPt->iotDebug.error);
                }
                break;
            case iotStatusProtocol::MqttConnectEnd:
                currentMqttStatus = iotStatusProtocol::MqttConnected;
                break;
            case iotStatusProtocol::MqttConnected:
                if(mqttStatus == MQTT_CONNECTION_LOST){
                    esp8266_mqttPt->iotDebug.message = "The network connection was broken!";
                    esp8266_mqttPt->iotDebug.error = IOT_MQTT_CONNECTION_LOST;
                    currentMqttStatus = iotStatusProtocol::NoneStatus;
                    esp8266_executeCallbackTask(esp8266_mqttPt->iotDebug.message, esp8266_mqttPt->iotDebug.error);
                }else if(mqttStatus == MQTT_DISCONNECTED){
                    esp8266_mqttPt->iotDebug.message = "The client is disconnected cleanly!";
                    esp8266_mqttPt->iotDebug.error = IOT_MQTT_DISCONNECTED;
                    currentMqttStatus = iotStatusProtocol::NoneStatus;
                    esp8266_executeCallbackTask(esp8266_mqttPt->iotDebug.message, esp8266_mqttPt->iotDebug.error);
                }else if(mqttStatus == MQTT_CONNECT_FAILED){
                    esp8266_mqttPt->iotDebug.message = "The network connection failed!";
                    esp8266_mqttPt->iotDebug.error = IOT_MQTT_CONNECT_FAILED;
                    currentMqttStatus = iotStatusProtocol::NoneStatus;
                    esp8266_executeCallbackTask(esp8266_mqttPt->iotDebug.message, esp8266_mqttPt->iotDebug.error);
                }else if(mqttStatus == MQTT_CONNECT_UNAVAILABLE){
                    esp8266_mqttPt->iotDebug.message = "The client was not authorized to connect!";
                    esp8266_mqttPt->iotDebug.error = IOT_MQTT_CONNECT_UNAVAILABLE;
                    currentMqttStatus = iotStatusProtocol::NoneStatus;
                    esp8266_executeCallbackTask(esp8266_mqttPt->iotDebug.message, esp8266_mqttPt->iotDebug.error);
                }
                break;
        }

    }
    
}


DFRobot_Esp8266MQTT::DFRobot_Esp8266MQTT(void)
{
    esp8266_mqttPt = this;
    firstRegisteredIotDebugtCallback = true;
    firstRegisteredMqttCallback = true;
    mqttCallbackBusy = false;
    currentMqttStatus = iotStatusProtocol::NoneStatus;
    if(firstRegisteredIotDebugtCallback) {
        esp8266_mqttPt->iotDebug.tarefa1.task(100,esp8266_iotDebugTask);
        firstRegisteredIotDebugtCallback = false;
    }
}

DFRobot_Esp8266MQTT::~DFRobot_Esp8266MQTT(void)
{
    firstRegisteredIotDebugtCallback = false;
    firstRegisteredMqttCallback = false;
    esp8266_mqttPt = NULL;
    esp8266_mqttPt->iotDebug.tarefa1.detach();
    esp8266_mqttPt->iotDebug.tarefa2.detach();
    if(esp8266_client.connected())
        esp8266_client.disconnect();
    WiFi.disconnect(true);
}

/*Use ONENET cloud platform*/
void DFRobot_Esp8266MQTT::init(String OneNetServer,
                            String OneNetProductID, String OneNetDeviceID,
                            String OneNetApiKey, const String iotTopics[],
                            uint16_t OneNetPort)
{
    if(OneNetServer == "183.230.40.96"){
        this->_MQTTSERVER    = OneNetServer;
        this->_ProductID     = OneNetProductID;
        this->_DeviceID      = OneNetDeviceID;
        this->_ApiKey        = OneNetApiKey;
        this->_port          = OneNetPort;
        this->_UseServer     = ONENET_NEW;
    }else{
        this->_MQTTSERVER    = OneNetServer;
        this->_ProductID     = OneNetProductID;
        this->_DeviceID      = OneNetDeviceID;
        this->_ApiKey        = OneNetApiKey;
        this->_port          = OneNetPort;
        this->_UseServer     = ONENET;
    }
    for(int i = 0; i < MAXTOPICNUMBER; i++){
        if(iotTopics[i] != ""){
            this->mqtt_topicArray.push_back(iotTopics[i]);
        }
    }
    setConfig();
}
/*Use Aliyun cloud platform*/
void DFRobot_Esp8266MQTT::init(String AliyunServer, String AliProductKey, 
                            String AliClientId, String AliDeviceName, 
                            String AliDeviceSecret, const String iotTopics[],
                            uint16_t AliPort)
                            {
    this->_MQTTSERVER    = AliyunServer;
    this->_ProductKey    = AliProductKey;
    this->_ClientId      = AliClientId;
    this->_DeviceName    = AliDeviceName;
    this->_DeviceSecret  = AliDeviceSecret;
    this->_port          = AliPort;
    this->_UseServer     = ALIYUN;

    for(int i = 0; i < MAXTOPICNUMBER; i++){
        if(iotTopics[i] != ""){
            this->mqtt_topicArray.push_back(iotTopics[i]);
        }
    }
    setConfig();
}
/*alculate the connection username and password, etc.*/
void DFRobot_Esp8266MQTT::setConfig(){
    if(this->_UseServer == ONENET){
        String tempSERVER = this->_MQTTSERVER;
        uint8_t len = tempSERVER.length();
        if(this->_mqttServer == NULL){
            this->_mqttServer = (char *) malloc(len);
        }
        strcpy(this->_mqttServer,tempSERVER.c_str());
        String tempID = this->_DeviceID;
        len = tempID.length();
        if(len == 0)
        {
            this->_clientId = "";
        }
        else{
            if(this->_clientId == NULL){
                this->_clientId = (char *) malloc(len);
            }
            strcpy(this->_clientId,tempID.c_str());
        }
        String tempName = this->_ProductID;
        len = tempName.length();
        this->_username = (char * )malloc(len);
        strcpy(this->_username,tempName.c_str());
    
        String tempPass = this->_ApiKey;
        if(this->_password == NULL){
            this->_password = (char *) malloc(tempPass.length());
        }
        strcpy(this->_password,tempPass.c_str());
    }else if(this->_UseServer == ALIYUN){
        String tempSERVER = (this->_ProductKey + "." + this->_MQTTSERVER);
        uint8_t len = tempSERVER.length();
        uint16_t timestamp = 49;
        if(this->_mqttServer == NULL){
            this->_mqttServer = (char *) malloc(len);
        }
        strcpy(this->_mqttServer,tempSERVER.c_str());
        String tempID = (this->_ClientId + 
                         "|securemode=3"+
                         ",signmethod=" + "hmacsha1"+
                         ",timestamp="+(String)timestamp+"|");
        len = tempID.length();
        if(this->_clientId == NULL){
            this->_clientId = (char *) malloc(len);
        }
        strcpy(this->_clientId,tempID.c_str());
        String Data = ("clientId" + this->_ClientId + 
                         "deviceName" + this->_DeviceName + 
                         "productKey" + this->_ProductKey + 
                         "timestamp" + (String)timestamp);
        BYTE tempPassWord[20];
        char tempSecret[this->_DeviceSecret.length()];
        char tempData[Data.length()];
        String tempName = (this->_DeviceName + "&" + this->_ProductKey);
        len = tempName.length();
        this->_username = (char * )malloc(len);
        strcpy(this->_username,tempName.c_str());
    
        strcpy(tempData,Data.c_str());
        strcpy(tempSecret,this->_DeviceSecret.c_str());
        esp8266_MyHmac_Sha1.HMAC_SHA1((BYTE * )tempData,Data.length(),(BYTE * )tempSecret,this->_DeviceSecret.length(),tempPassWord);
        String tempPass = esp8266_byteToHexStr(tempPassWord,sizeof(tempPassWord));
        if(this->_password == NULL){
            this->_password = (char *) malloc(tempPass.length());
        }
        strcpy(this->_password,tempPass.c_str());
    }else if(this->_UseServer == ONENET_NEW){
        String tempSERVER = this->_MQTTSERVER;
        uint8_t len = tempSERVER.length();
        if(this->_mqttServer == NULL){
            this->_mqttServer = (char *) malloc(len);
        }
        strcpy(this->_mqttServer,tempSERVER.c_str());
    
        String tempID = this->_DeviceID;
        len = tempID.length();
        if(this->_clientId == NULL){
            this->_clientId = (char *) malloc(len);
        }
        strcpy(this->_clientId,tempID.c_str());

        String tempName = this->_ProductID;
        len = tempName.length();
        this->_username = (char * )malloc(len);
        strcpy(this->_username,tempName.c_str());
        char authorization[128];
        String pid = this->_ProductID;
        pid += "/devices/";
        pid += this->_DeviceID;
        char buf1[pid.length()+1];
        pid.toCharArray(buf1, pid.length()+1);
        char buf2[this->_ApiKey.length()+1];
        this->_ApiKey.toCharArray(buf2, this->_ApiKey.length()+1);
        esp8266_token.TOKEN_Authorization("2018-10-31",buf1,1767077900,buf2,authorization,128);
        String tempPass = String(authorization);
        if(this->_password == NULL){
            this->_password = (char *) malloc(tempPass.length());
        }
        strcpy(this->_password,tempPass.c_str());
    }
}
void DFRobot_Esp8266MQTT::connect(){
    boolean connectState = false;
    long timeOut = millis();
    while(WiFi.status() != WL_CONNECTED){
        if(millis() - timeOut > 10000 && (WiFi.status() != WL_CONNECTED))
            return;
        delay(10);
    }
    esp8266_client.setServer(this->_mqttServer,this->_port);
    esp8266_client.setCallback(esp8266_mqttCallback);
    esp8266_client.connect(this->_clientId,this->_username,this->_password);
    esp8266_mqtt_count = 30;
    delay(100);
    currentMqttStatus = iotStatusProtocol::MqttConnectStart;
}
boolean DFRobot_Esp8266MQTT::connected(){
    return currentMqttStatus == iotStatusProtocol::MqttConnected && esp8266_client.connected();
}
void DFRobot_Esp8266MQTT::disconnect(){
    esp8266_client.disconnect();
}
void DFRobot_Esp8266MQTT::setMqttCallback(const MsgHandleCb handles[]){
    for(int i = 0; i < MAXTOPICNUMBER; i++){
        if(handles[i]){
            this->mqtt_msgHandleCb[i] = handles[i];
        }
    }
}
void DFRobot_Esp8266MQTT::setMqttCallback(Topicnum topic_num, const String delimiters, const HandleCb handles){
    mqtt_handleCb[topic_num].push_back(handles);
    mqtt_delimiters[topic_num].push_back(delimiters);
}
void DFRobot_Esp8266MQTT::publish(Topicnum topic_num, const String& message, uint8_t qos)
{
    if((WiFi.status() != WL_CONNECTED) || !connected() || mqttCallbackBusy)
        return;

    //Serial.println(this->mqtt_topicArray[topic_num].c_str());
    //Serial.println(message.c_str());
    if(topic_num >= mqtt_topicArray.size()) return;
    esp8266_client.setPublishQos(qos > 0 ? MQTTQOS1: MQTTQOS0);
    esp8266_client.publish(this->mqtt_topicArray[topic_num].c_str(),message.c_str());
    delay(200);
}

void DFRobot_Esp8266MQTT::publish(Topicnum topic_num, double f, uint8_t qos, int precision)
{
    String str;
    str = String(f,5);
    //Serial.println(str);

    char *p = (char *)(str.c_str() + str.length());
    while(*p == '\0'|| *p == '0'){
        *p = '\0';
        p--;
    }
    if(*p == '.')
        *p = '\0';
    if(str == "-0")
        str = "0";
    long timeOut = millis();
    while(!connected() || (WiFi.status() != WL_CONNECTED)){
        if(millis() - timeOut > 15000 && (!connected() || (WiFi.status() != WL_CONNECTED)))
            return;
        delay(10);
    }
    publish(topic_num, (const String&)str, qos);
}

void DFRobot_Esp8266MQTT::publish(Topicnum topic_num, int64_t i, uint8_t qos)
{
    char buffer[34];
    memset(buffer, 0, 34);
    itoa(i, buffer, 10);
    publish(topic_num, (const String&)buffer, qos);
}

PubSubClient esp8266_client(esp8266_espClient);
WiFiClient esp8266_espClient;

