 /**
  * file SmartDHT11.ino
  *
  * Connect the temperature and humidity sensor to release the temperature and humidity information to Aliyun.
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
#include "DFRobot_DHT11.h"

#define DHT11_PIN  D2

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
String TempIdentifier = "you Temp Identifier";
String HumiIdentifier = "you Humi Identifier";

/*TOPIC that need to be published and subscribed*/
const char * subTopic = "you sub Topic";//****set
const char * pubTopic = "you pub Topic";//******post

DFRobot_Iot myIot;
WiFiClient espClient;
PubSubClient client(espClient);
DFRobot_DHT11 DHT;

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
  
  /*Connect to WIFI*/
  connectWiFi();
  
  /*Initialize the configuration of Aliyun*/
  myIot.init(ALIYUN_SERVER,ProductKey,ClientId,DeviceName,DeviceSecret);
  
  client.setServer(myIot._mqttServer,PORT);
  
  /*Set the callback function to execute the callback function when receiving the subscription information*/
  client.setCallback(callback);
  
  /*Connect to the cloud platform*/
  ConnectCloud();
}

uint8_t tempTime = 0;
void loop(){
  if(!client.connected()){
    ConnectCloud();
  }
  /*One minute publish twice temperature and humidity information*/
  if(tempTime > 60){
    tempTime = 0;
    DHT.read(DHT11_PIN);
    Serial.print("DHT.temperature=");
    Serial.println(DHT.temperature);
    Serial.print("DHT.humidity");
    Serial.println(DHT.humidity);
    client.publish(pubTopic,("{\"id\":"+ClientId+",\"params\":{\""+TempIdentifier+"\":"+DHT.temperature+",\""+HumiIdentifier+"\":"+DHT.humidity+"},\"method\":\"thing.event.property.post\"}").c_str());
  }else{
    tempTime++;
    delay(500);
  }
  client.loop();
}