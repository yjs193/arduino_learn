 /**
  * file Bedroom_Light.ino
  *
  * Connect the relay. 
  * It can be selected to be turned on or off according to the instructions sent by the cloud platform. 
  * Implemented a remote control socket or switch
  *
  *
  * Copyright   [DFRobot](http://www.dfrobot.com), 2016
  * Copyright   GNU Lesser General Public License
  *
  * version  V1.0
  * date  2019-2-20
  */
#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include "DFRobot_Iot.h"

#define SOCKET_PIN  D2

/*Set WIFI name and password*/
const char * WIFI_SSID     = "WIFI_SSID";
const char * WIFI_PASSWORD = "WIFI_PASSWORD";

/*Configure device certificate information*/
String ProductKey = "you Product Key";
String ClientId = "12345";
String DeviceName = "you Device Name";
String DeviceSecret = "you Device Secret";

/*Configure the domain name and port number*/
String ALIYUN_SERVER = "iot-as-mqtt.cn-shanghai.aliyuncs.com";
uint16_t PORT = 1883;

/*Product identifier that needs to be operated*/
String Identifier = "you Identifier";

/*TOPIC that need to be published and subscribed*/
const char * subTopic = "you sub Topic";//****set
const char * pubTopic = "you pub Topic";//******post

DFRobot_Iot myIot;
WiFiClient espClient;
PubSubClient client(espClient);


static void openSocket(){
  digitalWrite(SOCKET_PIN, HIGH);
}

static void closeSocket(){
  digitalWrite(SOCKET_PIN, LOW);
}
void connectWiFi(){
  Serial.print("Connecting to ");
  Serial.println(WIFI_SSID);
  WiFi.begin(WIFI_SSID,WIFI_PASSWORD);
  while(WiFi.status() != WL_CONNECTED){
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.println("WiFi connected");
  Serial.print("IP Adderss: ");
  Serial.println(WiFi.localIP());
}

void callback(char * topic, byte * payload, unsigned int len){
  Serial.print("Recevice [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < len; i++){
    Serial.print((char)payload[i]);
  }
  Serial.println();
  StaticJsonBuffer<300> jsonBuffer;
  JsonObject& root = jsonBuffer.parseObject((const char *)payload);
  if(!root.success()){
    Serial.println("parseObject() failed");
    return;
  }
  uint8_t SocketStatus = root["params"][Identifier];
  Serial.print("SocketStatus=");
  Serial.print(SocketStatus);
  if(SocketStatus == 1){
    /*Turn on the socket*/
    openSocket();
  }else{
    /*Turn off the socket*/
    closeSocket();
  }
  String tempMseg = "{\"id\":"+ClientId+",\"params\":{\""+Identifier+"\":"+(String)SocketStatus+"},\"method\":\"thing.event.property.post\"}";
  char sendMseg[tempMseg.length()];
  strcpy(sendMseg,tempMseg.c_str());
  client.publish(pubTopic,sendMseg);
}

void ConnectCloud(){
  while(!client.connected()){
    Serial.print("Attempting MQTT connection...");
    /*A device connected to the cloud platform based on an automatically calculated username and password*/
    if(client.connect(myIot._clientId,myIot._username,myIot._password)){
      Serial.println("connected");
      client.subscribe(subTopic);
    }else{
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}
void setup(){
  Serial.begin(115200);
  pinMode(SOCKET_PIN,OUTPUT);
  /*Turn off the socket first*/
  closeSocket();
  
  /*/*Connect to WIFI*/*/
  connectWiFi();
  
  /*Initialize the configuration of Aliyun*/
  myIot.init(ALIYUN_SERVER,ProductKey,ClientId,DeviceName,DeviceSecret);
  
  client.setServer(myIot._mqttServer,PORT);
  
  /*Set the callback function to execute the callback function when receiving the subscription information*/
  client.setCallback(callback);
  
  /*Connect to the cloud platform*/
  ConnectCloud();
  
  /*Publish socket information*/
  client.publish(pubTopic,("{\"id\":"+ClientId+",\"params\":{\""+Identifier+"\":0},\"method\":\"thing.event.property.post\"}").c_str());
}

void loop(){
  if(!client.connected()){
    ConnectCloud();
  }
  client.loop();
}