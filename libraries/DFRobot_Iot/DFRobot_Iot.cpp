#include "DFRobot_Iot.h"
#include "HMAC_SHA1.h"
#include "token.h"


#if CONFIG_FREERTOS_UNICORE
#define ARDUINO_RUNNING_CORE_MQTT 0
#else
#define ARDUINO_RUNNING_CORE_MQTT 1
#endif
CHMAC_SHA1 MyHmac_Sha1;
DFRobot_Iot *mqttPt;
DF_TOKEN token;
volatile static bool firstRegisteredIotDebugtCallback;
volatile static bool firstRegisteredMqttCallback;
volatile static bool mqttCallbackBusy;
volatile static int8_t currentWifiStatus;
volatile static int8_t currentMqttStatus;
uint8_t mqtt_count;


String byteToHexStr(unsigned char byte_arr[], int arr_len)  
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

static void mqttCallback(char * topic, BYTE * payload, unsigned int len){
    mqttCallbackBusy = true;
    String _topic = topic;

    for(int i = 0; i < mqttPt->mqtt_topicArray.size(); i++){
        if(_topic == mqttPt->mqtt_topicArray[i]){
            String _payload = (char *)payload;
            _payload = _payload.substring(0,len);
            for(int j = 0; j < mqttPt->mqtt_handleCb[i].size(); j++){
                if(_payload == mqttPt->mqtt_delimiters[i][j]){
                    mqttPt->mqtt_handleCb[i][j]();
                    break;
                }
            }
            if(mqttPt->mqtt_msgHandleCb[i])
                mqttPt->mqtt_msgHandleCb[i](_payload);
            break;
        }
    }
    mqttCallbackBusy = false;
}

static void subscribe(){
    for(int i = 0; i < mqttPt->mqtt_topicArray.size(); i++){
        if(mqttPt->mqtt_topicArray[i] != "$dp"){
            //Serial.println(mqttPt->mqtt_topicArray[i].c_str());
            client.subscribe(mqttPt->mqtt_topicArray[i].c_str());
        }
    }
}

static void mqttTask(void *param){
    while(!client.connected()){
        delay(100);
        yield();
    }
    while(1){
        while(!client.connected()){
            delay(100);
            yield();
        }
        client.loop();
        yield();
    }
}

static void _executeCallbackTask(void *param){
    DFRobot_Iot *self = (DFRobot_Iot *)param;
    if(mqttPt->iot_debugMsgHandleCb) mqttPt->iot_debugMsgHandleCb(self->iotDebug.message, self->iotDebug.error);
    self->iotDebug.task = NULL;
    vTaskDelete(NULL);
}

static void executeCallbackTask()
{
    if(mqttPt->iot_debugMsgHandleCb) {
        if(mqttPt->iotDebug.task){
            vTaskDelete(mqttPt->iotDebug.task);
            mqttPt->iotDebug.task = NULL;
        }
        xTaskCreatePinnedToCore(_executeCallbackTask, "_executeCallbackTask", 2048, mqttPt, 1, &mqttPt->iotDebug.task, ARDUINO_RUNNING_CORE_MQTT);
    }
}

static void iotDebugTask(void *param){
    int wifiStatus, mqttStatus;
    long timeOut;
    static uint32_t wifiConnectingTime = 0;
    while(1){
        wifiStatus = WiFi.status();
        mqttStatus = client.state();
        //if(wifiStatus != WL_DISCONNECTED && wifiStatus != WL_CONNECTED && wifiStatus != WL_IDLE_STATUS && wifiStatus != WL_NO_SHIELD) Serial.println(wifiStatus);
        //if(mqttStatus != MQTT_DISCONNECTED && mqttStatus != MQTT_CONNECTED) Serial.println(mqttStatus);
        switch(currentWifiStatus){
            case iotStatusProtocol::WifiConnectStart:
                mqttPt->iotDebug.message = "Start connecting to " + mqttPt->account;
                mqttPt->iotDebug.error = IOT_SUCCESSFULLY;
                currentWifiStatus = iotStatusProtocol::WifiConnecting;
                executeCallbackTask();
                wifiConnectingTime = millis();
                break;
            case iotStatusProtocol::WifiConnecting:
                if(millis() - wifiConnectingTime > 15000) {
                    mqttPt->iotDebug.message = "Unable to access the set SSID network [" + mqttPt->account + "], Wifi password error.";
                    mqttPt->iotDebug.error = IOT_WL_CONNECTION_LOST;
                    currentWifiStatus = iotStatusProtocol::NoneStatus;
                    executeCallbackTask();
                }else if(wifiStatus == WL_NO_SSID_AVAIL) {
                    mqttPt->iotDebug.message = "Unable to access the set SSID network [" + mqttPt->account + "], Wifi does not exist.";
                    mqttPt->iotDebug.error = IOT_WL_NO_SSID_AVAIL;
                    currentWifiStatus = iotStatusProtocol::NoneStatus;
                    executeCallbackTask();
                }else if(wifiStatus == WL_CONNECTED) {
                    String mode;
                    switch(WiFi.getMode()){
                        case WIFI_MODE_NULL:
                            mode = "NULL";
                            break;
                        case WIFI_MODE_STA:
                            mode = "STA";
                            break;
                        case WIFI_MODE_AP:
                            mode = "AP";
                            break;
                        case WIFI_MODE_APSTA:
                            mode = "APSTA";
                            break;
                    }
                    mqttPt->iotDebug.message = "Wifi connect to [" + WiFi.SSID() + "] successfully" + "\n" + \
                            " [ Mode: " + mode + " ]\n" + \
                            " [ IP: " + mqttPt->getWiFiLocalIP() + " ]\n" + \
                            " [ NetMask: " + mqttPt->getWiFiNetmask() + " ]\n" + \
                            " [ GateWay: " + mqttPt->getGateway() + " ]\n" + \
                            " [ DNS: " + mqttPt->getDNS() + " ]";
                    mqttPt->iotDebug.error = IOT_SUCCESSFULLY;
                    currentWifiStatus = iotStatusProtocol::WifiConnectEnd;
                    executeCallbackTask();
                }
                break;
            case iotStatusProtocol::WifiConnectEnd:
                currentWifiStatus = iotStatusProtocol::WifiConnected;
                break;
            case iotStatusProtocol::WifiConnected:
                if(wifiStatus == WL_NO_SSID_AVAIL) {
                    WiFi.disconnect(true);
                    mqttPt->iotDebug.message = "Unable to access the set SSID network [" + mqttPt->account + "], Wifi does not exist.";
                    mqttPt->iotDebug.error = IOT_WL_NO_SSID_AVAIL;
                    currentWifiStatus = iotStatusProtocol::NoneStatus;
                    executeCallbackTask();
                }else if(wifiStatus == WL_CONNECT_FAILED) {
                    WiFi.disconnect(true);
                    mqttPt->iotDebug.message = "Wifi connection failed!";
                    mqttPt->iotDebug.error = IOT_WL_CONNECT_FAILED;
                    currentWifiStatus = iotStatusProtocol::NoneStatus;
                    executeCallbackTask();
                }else if(wifiStatus == WL_CONNECTION_LOST) {
                    WiFi.disconnect(true);
                    mqttPt->iotDebug.message = "Wifi lost connection!";
                    mqttPt->iotDebug.error = IOT_WL_CONNECTION_LOST;
                    currentWifiStatus = iotStatusProtocol::NoneStatus;
                    executeCallbackTask();
                }else if(wifiStatus == WL_NO_SHIELD) {
                    WiFi.disconnect(true);
                    mqttPt->iotDebug.message = "Wifi disconnect!";
                    mqttPt->iotDebug.error = IOT_WL_DISCONNECTED;
                    currentWifiStatus = iotStatusProtocol::NoneStatus;
                    executeCallbackTask();
                }
            break;
        }
        switch(currentMqttStatus){
            case iotStatusProtocol::MqttConnectStart:
                mqttPt->iotDebug.message = "Start connecting mqtt";
                mqttPt->iotDebug.error = 0;
                currentMqttStatus = iotStatusProtocol::MqttConnecting;
                executeCallbackTask();
                timeOut = millis();
                while(!client.connect(mqttPt->_clientId,mqttPt->_username,mqttPt->_password)) {
                    if(millis() - timeOut > 3000)
                        break;
                    delay(10);
                }
                subscribe();
                if(firstRegisteredMqttCallback) {
                    xTaskCreatePinnedToCore(mqttTask, "mqttTask", 2048, mqttPt, 1, NULL, ARDUINO_RUNNING_CORE_MQTT);
                    firstRegisteredMqttCallback = false;
                }
                break;
            case iotStatusProtocol::MqttConnecting:
                if(mqttStatus == MQTT_CONNECTION_TIMEOUT) {
                    mqttPt->iotDebug.message = "The server didn’t respond within the keepalive time, please check the parameters!";
                    mqttPt->iotDebug.error = IOT_MQTT_CONNECTION_TIMEOUT;
                    currentMqttStatus = iotStatusProtocol::NoneStatus;
                    executeCallbackTask();
                }else if(mqttStatus == MQTT_CONNECTION_LOST){
                    mqttPt->iotDebug.message = "The network connection was broken, please check the parameters!";
                    mqttPt->iotDebug.error = IOT_MQTT_CONNECTION_LOST;
                    currentMqttStatus = iotStatusProtocol::NoneStatus;
                    executeCallbackTask();
                }else if(mqttStatus == MQTT_CONNECT_FAILED){
                    mqttPt->iotDebug.message = "The network connection failed! Please check the parameters!";
                    mqttPt->iotDebug.error = IOT_MQTT_CONNECT_FAILED;
                    currentMqttStatus = iotStatusProtocol::NoneStatus;
                    executeCallbackTask();
                }else if(mqttStatus == MQTT_CONNECTED){
                    mqttPt->iotDebug.message = String("Mqtt connect successfully\n") + \
                            " [ Server: " + String(mqttPt->_mqttServer) + " ]\n" + \
                            " [ Port: " + mqttPt->_port + " ]\n" + \
                            " [ ClientId: " + mqttPt->_clientId + " ]\n" + \
                            " [ Username: " + mqttPt->_username + " ]\n" + \
                            " [ Password: " + mqttPt->_password + " ]";
                    mqttPt->iotDebug.error = IOT_SUCCESSFULLY;
                    currentMqttStatus = iotStatusProtocol::MqttConnectEnd;
                    executeCallbackTask();
                }else if(mqttStatus == MQTT_CONNECT_BAD_PROTOCOL){
                    mqttPt->iotDebug.message = "The server doesn’t support the requested version of MQTT, please check the parameters!";
                    mqttPt->iotDebug.error = IOT_MQTT_CONNECT_BAD_PROTOCOL;
                    currentMqttStatus = iotStatusProtocol::NoneStatus;
                    executeCallbackTask();
                }else if(mqttStatus == MQTT_CONNECT_BAD_CLIENT_ID){
                    mqttPt->iotDebug.message = "The server rejected the client identifier, please check the parameters!";
                    mqttPt->iotDebug.error = IOT_MQTT_CONNECT_BAD_CLIENT_ID;
                    currentMqttStatus = iotStatusProtocol::NoneStatus;
                    executeCallbackTask();
                }else if(mqttStatus == MQTT_CONNECT_UNAVAILABLE){
                    mqttPt->iotDebug.message = "The server was unable to accept the connection, please check the parameters!";
                    mqttPt->iotDebug.error = IOT_MQTT_CONNECT_UNAVAILABLE;
                    currentMqttStatus = iotStatusProtocol::NoneStatus;
                    executeCallbackTask();
                }else if(mqttStatus == MQTT_CONNECT_BAD_CREDENTIALS){
                    mqttPt->iotDebug.message = "The username/password were rejected, please check the parameters!";
                    mqttPt->iotDebug.error = IOT_MQTT_CONNECT_BAD_CREDENTIALS;
                    currentMqttStatus = iotStatusProtocol::NoneStatus;
                    executeCallbackTask();
                }else if(mqttStatus == MQTT_CONNECT_UNAUTHORIZED){
                    mqttPt->iotDebug.message = "The client was not authorized to connect! please check the parameters!";
                    mqttPt->iotDebug.error = IOT_MQTT_CONNECT_UNAUTHORIZED;
                    currentMqttStatus = iotStatusProtocol::NoneStatus;
                    executeCallbackTask();
                }
                break;
            case iotStatusProtocol::MqttConnectEnd:
                currentMqttStatus = iotStatusProtocol::MqttConnected;
                break;
            case iotStatusProtocol::MqttConnected:
                if(mqttStatus == MQTT_CONNECTION_LOST){
                    mqttPt->iotDebug.message = "The network connection was broken!";
                    mqttPt->iotDebug.error = IOT_MQTT_CONNECTION_LOST;
                    currentMqttStatus = iotStatusProtocol::NoneStatus;
                    executeCallbackTask();
                }else if(mqttStatus == MQTT_DISCONNECTED){
                    mqttPt->iotDebug.message = "The client is disconnected cleanly!";
                    mqttPt->iotDebug.error = IOT_MQTT_DISCONNECTED;
                    currentMqttStatus = iotStatusProtocol::NoneStatus;
                    executeCallbackTask();
                }else if(mqttStatus == MQTT_CONNECT_FAILED){
                    mqttPt->iotDebug.message = "The network connection failed!";
                    mqttPt->iotDebug.error = IOT_MQTT_CONNECT_FAILED;
                    currentMqttStatus = iotStatusProtocol::NoneStatus;
                    executeCallbackTask();
                }else if(mqttStatus == MQTT_CONNECT_UNAVAILABLE){
                    mqttPt->iotDebug.message = "The client was not authorized to connect!";
                    mqttPt->iotDebug.error = IOT_MQTT_CONNECT_UNAVAILABLE;
                    currentMqttStatus = iotStatusProtocol::NoneStatus;
                    executeCallbackTask();
                }
                break;
        }
        delay(50);
        yield();
    }
}

DFRobot_Iot::DFRobot_Iot(void){

    mqttPt = this;
    firstRegisteredIotDebugtCallback = true;
    firstRegisteredMqttCallback = true;
    mqttCallbackBusy = false;
    currentMqttStatus = iotStatusProtocol::NoneStatus;
    currentWifiStatus = iotStatusProtocol::NoneStatus;
    if(firstRegisteredIotDebugtCallback) {

        xTaskCreatePinnedToCore(iotDebugTask, "iotDebugTask", 2048, this, 1, NULL, ARDUINO_RUNNING_CORE_MQTT);
        firstRegisteredIotDebugtCallback = false;
    }

}

DFRobot_Iot::~DFRobot_Iot(void){
    firstRegisteredIotDebugtCallback = false;
    firstRegisteredMqttCallback = false;
    mqttPt = NULL;
    wifiDisconnect();
}

void DFRobot_Iot::wifiConnect(String account, String password){
    this->account = account;
    this->password = password;
    WiFi.disconnect(true);
    delay(100);
    currentWifiStatus = iotStatusProtocol::WifiConnectStart;
    WiFi.begin(account.c_str(),password.c_str());
}

boolean DFRobot_Iot::wifiStatus(){
    return currentWifiStatus == iotStatusProtocol::WifiConnected && WiFi.status() == WL_CONNECTED;
}

void DFRobot_Iot::wifiDisconnect(){
    if(client.connected())
        client.disconnect();
    WiFi.disconnect(true);
}

void DFRobot_Iot::connect(){
    boolean connectState = false;
    long timeOut = millis();
    while(!wifiStatus()){
        if(millis() - timeOut > 10000 && !wifiStatus())
            return;
        delay(10);
    }
    client.setServer(this->_mqttServer,this->_port);
    client.setCallback(mqttCallback);
    currentMqttStatus = iotStatusProtocol::MqttConnectStart;
}

boolean DFRobot_Iot::connected(){
    return currentMqttStatus == iotStatusProtocol::MqttConnected && client.connected();
}

String DFRobot_Iot::getWiFiLocalIP()
{
    return WiFi.localIP().toString();
}

String DFRobot_Iot::getWiFiNetmask()
{
    return WiFi.subnetMask().toString();
}

String DFRobot_Iot::getGateway()
{
    return WiFi.gatewayIP().toString();
}

String DFRobot_Iot::getDNS()
{
    return WiFi.dnsIP().toString();
}

boolean DFRobot_Iot::setSoftAP(String ssid, String pass)
{
    this->account = ssid;
    this->password = pass;
    return WiFi.softAP(ssid.c_str(),pass.c_str());
}

String DFRobot_Iot::getWiFiSoftIP()
{
    unsigned  int IP_Addr = WiFi.softAPIP();
    int a1 = IP_Addr>>24;
    IP_Addr = IP_Addr<<8;
    int a2 = IP_Addr>>24;
    IP_Addr = IP_Addr<<8;
    int a3 = IP_Addr>>24;
    IP_Addr = IP_Addr<<8;
    int a4 = IP_Addr>>24;
    char myaddr[50];
    memset(myaddr,'\0',50);
    sprintf(myaddr,"%d.%d.%d.%d",a4,a3,a2,a1);
    
    String myLocalIP = myaddr;
    return myLocalIP;
}

void DFRobot_Iot::disconnect(){
    client.disconnect();
}

void DFRobot_Iot::publish(Topicnum topic_num, const String& message, uint8_t qos)
{
    if(!wifiStatus() || !connected() || mqttCallbackBusy)
        return;
    //Serial.println(this->mqtt_topicArray[topic_num].c_str());
    //Serial.println(message.c_str());
    if(topic_num >= mqtt_topicArray.size()) return;
    client.setPublishQos(qos > 0 ? MQTTQOS1: MQTTQOS0);
    client.publish(this->mqtt_topicArray[topic_num].c_str(),message.c_str());
    delay(200);
}

void DFRobot_Iot::publish(Topicnum topic_num, double f, uint8_t qos, int precision)
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
    while(!connected() || !wifiStatus()){
        if(millis() - timeOut > 15000 && (!connected() || !wifiStatus()))
            return;
        delay(10);
    }
    publish(topic_num, (const String&)str, qos);
}

void DFRobot_Iot::publish(Topicnum topic_num, int64_t i, uint8_t qos)
{
    char buffer[34];
    memset(buffer, 0, 34);
    itoa(i, buffer, 10);
    publish(topic_num, (const String&)buffer, qos);
}

/*Use ONENET cloud platform*/
void DFRobot_Iot::init(String OneNetServer,
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
void DFRobot_Iot::init(String AliyunServer, String AliProductKey, 
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

void DFRobot_Iot::setIotDebugCallback(const DebugMsgHandleCb handleCb){
    iot_debugMsgHandleCb = handleCb;
}

void DFRobot_Iot::setMqttCallback(const MsgHandleCb handles[]){
    for(int i = 0; i < MAXTOPICNUMBER; i++){
        if(handles[i]){
            this->mqtt_msgHandleCb[i] = handles[i];
        }
    }
}

void DFRobot_Iot::setMqttCallback(Topicnum topic_num, const String delimiters, const HandleCb handles){
    mqtt_handleCb[topic_num].push_back(handles);
    mqtt_delimiters[topic_num].push_back(delimiters);
}

/*alculate the connection username and password, etc.*/
void DFRobot_Iot::setConfig(){
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
        MyHmac_Sha1.HMAC_SHA1((BYTE * )tempData,Data.length(),(BYTE * )tempSecret,this->_DeviceSecret.length(),tempPassWord);
        String tempPass = byteToHexStr(tempPassWord,sizeof(tempPassWord));
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
        token.TOKEN_Authorization("2018-10-31",buf1,1767077900,buf2,authorization,128);
        String tempPass = String(authorization);
        if(this->_password == NULL){
            this->_password = (char *) malloc(tempPass.length());
        }
        strcpy(this->_password,tempPass.c_str());
    }
}

PubSubClient client(espClient);


WiFiClient espClient;
