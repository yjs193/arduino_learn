#include "DFRobot_Esp8266WIFI.h"


DFRobot_Esp8266WIFI *esp8266_wifiPt;
volatile static int8_t currentWifiStatus;

static void _esp8266_executeCallbackTask(){

    if(esp8266_wifiPt->iot_debugMsgHandleCb) esp8266_wifiPt->iot_debugMsgHandleCb(esp8266_wifiPt->iotDebug.message, esp8266_wifiPt->iotDebug.error);
    esp8266_wifiPt->iotDebug.tarefa2.detach();
}

void esp8266_executeCallbackTask(String message, int32_t error)
{
    esp8266_wifiPt->iotDebug.message = message;
    esp8266_wifiPt->iotDebug.error = error;
    if(esp8266_wifiPt->iot_debugMsgHandleCb) {
        if(esp8266_wifiPt->iotDebug.tarefa2._timer){
            esp8266_wifiPt->iotDebug.tarefa2.detach();
        }
        esp8266_wifiPt->iotDebug.tarefa2.task(2,_esp8266_executeCallbackTask);
    }
}

static void esp8266_wifiDebugTask(){
    int wifiStatus;
    wifiStatus = WiFi.status();
    switch(currentWifiStatus){
        case wifiStatusProtocol::WifiConnectStart:
            esp8266_wifiPt->iotDebug.message = "Start connecting to " + esp8266_wifiPt->account;
            esp8266_wifiPt->iotDebug.error = WIFI_SUCCESSFULLY;
            currentWifiStatus = wifiStatusProtocol::WifiConnecting;
            esp8266_executeCallbackTask(esp8266_wifiPt->iotDebug.message, esp8266_wifiPt->iotDebug.error);
            static uint32_t wifiConnectingTime = millis();
            break;
        case wifiStatusProtocol::WifiConnecting:
            if(millis() - wifiConnectingTime > 50000) {
                esp8266_wifiPt->iotDebug.message = "Unable to access the set SSID network [" + esp8266_wifiPt->account + "], Wifi password error.";
                esp8266_wifiPt->iotDebug.error = IOT_WL_CONNECTION_LOST;
                currentWifiStatus = wifiStatusProtocol::NoneStatus;
                esp8266_executeCallbackTask(esp8266_wifiPt->iotDebug.message, esp8266_wifiPt->iotDebug.error);
            }else if(wifiStatus == WL_NO_SSID_AVAIL) {
                esp8266_wifiPt->iotDebug.message = "Unable to access the set SSID network [" + esp8266_wifiPt->account + "], Wifi does not exist.";
                esp8266_wifiPt->iotDebug.error = IOT_WL_NO_SSID_AVAIL;
                currentWifiStatus = wifiStatusProtocol::NoneStatus;
                esp8266_executeCallbackTask(esp8266_wifiPt->iotDebug.message, esp8266_wifiPt->iotDebug.error);
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
                esp8266_wifiPt->iotDebug.message = "Wifi connect to [" + WiFi.SSID() + "] successfully" + "\n" + \
                        " [ Mode: " + mode + " ]\n" + \
                        " [ IP: " + WiFi.localIP().toString() + " ]\n" + \
                        " [ NetMask: " + WiFi.subnetMask().toString() + " ]\n" + \
                        " [ GateWay: " + WiFi.gatewayIP().toString() + " ]\n" + \
                        " [ DNS: " + WiFi.dnsIP().toString() + " ]";
                esp8266_wifiPt->iotDebug.error = WIFI_SUCCESSFULLY;
                currentWifiStatus = wifiStatusProtocol::WifiConnectEnd;
                esp8266_executeCallbackTask(esp8266_wifiPt->iotDebug.message, esp8266_wifiPt->iotDebug.error);
            }
            break;
        case wifiStatusProtocol::WifiConnectEnd:
            currentWifiStatus = wifiStatusProtocol::WifiConnected;
            break;
        case wifiStatusProtocol::WifiConnected:
            if(wifiStatus == WL_NO_SSID_AVAIL) {
                WiFi.disconnect(true);
                esp8266_wifiPt->iotDebug.message = "Unable to access the set SSID network [" + esp8266_wifiPt->account + "], Wifi does not exist.";
                esp8266_wifiPt->iotDebug.error = IOT_WL_NO_SSID_AVAIL;
                currentWifiStatus = wifiStatusProtocol::NoneStatus;
                esp8266_executeCallbackTask(esp8266_wifiPt->iotDebug.message, esp8266_wifiPt->iotDebug.error);
            }else if(wifiStatus == WL_CONNECT_FAILED) {
                WiFi.disconnect(true);
                esp8266_wifiPt->iotDebug.message = "Wifi connection failed!";
                esp8266_wifiPt->iotDebug.error = IOT_WL_CONNECT_FAILED;
                currentWifiStatus = wifiStatusProtocol::NoneStatus;
                esp8266_executeCallbackTask(esp8266_wifiPt->iotDebug.message, esp8266_wifiPt->iotDebug.error);
            }else if(wifiStatus == WL_CONNECTION_LOST) {
                WiFi.disconnect(true);
                esp8266_wifiPt->iotDebug.message = "Wifi lost connection!";
                esp8266_wifiPt->iotDebug.error = IOT_WL_CONNECTION_LOST;
                currentWifiStatus = wifiStatusProtocol::NoneStatus;
                esp8266_executeCallbackTask(esp8266_wifiPt->iotDebug.message, esp8266_wifiPt->iotDebug.error);
            }else if(wifiStatus == WL_NO_SHIELD || wifiStatus == WL_DISCONNECTED) {
                WiFi.disconnect(true);
                esp8266_wifiPt->iotDebug.message = "Wifi disconnect!";
                esp8266_wifiPt->iotDebug.error = IOT_WL_DISCONNECTED;
                currentWifiStatus = wifiStatusProtocol::NoneStatus;
                esp8266_executeCallbackTask(esp8266_wifiPt->iotDebug.message, esp8266_wifiPt->iotDebug.error);
            }
        break;
    }
}


DFRobot_Esp8266WIFI::DFRobot_Esp8266WIFI(void)
{
    esp8266_wifiPt = this;
    currentWifiStatus = wifiStatusProtocol::NoneStatus;
    esp8266_wifiPt->iotDebug.tarefa1.task(10,esp8266_wifiDebugTask);
}

DFRobot_Esp8266WIFI::~DFRobot_Esp8266WIFI(void)
{
    esp8266_wifiPt->iotDebug.tarefa1.detach();
    wifiDisconnect();
}


void DFRobot_Esp8266WIFI::wifiConnect(String account, String password)
{
    this->account = account;
    this->password = password;
    WiFi.disconnect(true);
    delay(100);
    currentWifiStatus = wifiStatusProtocol::WifiConnectStart;
    WiFi.mode(WIFI_STA);
    WiFi.begin(account.c_str(),password.c_str());
}

boolean DFRobot_Esp8266WIFI::wifiStatus()
{
    return currentWifiStatus == wifiStatusProtocol::WifiConnected && WiFi.status() == WL_CONNECTED;
}

void DFRobot_Esp8266WIFI::wifiDisconnect()
{
    WiFi.disconnect(true);
}

String DFRobot_Esp8266WIFI::getWiFiLocalIP()
{
    return WiFi.localIP().toString();
}

String DFRobot_Esp8266WIFI::getWiFiNetmask()
{
    return WiFi.subnetMask().toString();
}

String DFRobot_Esp8266WIFI::getGateway()
{
    return WiFi.gatewayIP().toString();
}

String DFRobot_Esp8266WIFI::getDNS()
{
    return WiFi.dnsIP().toString();
}

boolean DFRobot_Esp8266WIFI::setSoftAP(String ssid, String pass)
{
    this->account = ssid;
    this->password = pass;
    return WiFi.softAP(ssid.c_str(),pass.c_str());
}

String DFRobot_Esp8266WIFI::getWiFiSoftIP()
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

void DFRobot_Esp8266WIFI::setIotDebugCallback(const DebugMsgHandleCb handleCb){
    iot_debugMsgHandleCb = handleCb;
}