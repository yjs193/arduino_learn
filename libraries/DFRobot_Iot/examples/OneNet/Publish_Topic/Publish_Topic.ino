 /*!
  * file Publish_Topic.ino
  *
  * Publish "Hello World!" to the specified Topic for the created device.
  *
  * Copyright   [DFRobot](http://www.dfrobot.com), 2016
  * Copyright   GNU Lesser General Public License
  *
  * version  V1.0
  * date  2019-2-20
  */
#include <WiFi.h>
#include <PubSubClient.h>
#include "DFRobot_Iot.h"

bool SwitchStatus = false;

/*Set WIFI name and password*/
const char * WIFI_SSID     = "dfrobotYanfa";
const char * WIFI_PASSWORD = "hidfrobot";

/*Configure device certificate information*/
String ProductID = "323474";
String DeviceId  = "587014017";
String ApiKey    = "2hyl5N=8nXd39LnolnsoQMimn50=";

/*Configure the domain name and port number*/
String ONENET_SERVER = "mqtt.heclouds.com";
uint16_t PORT = 6002;

/*Set the Topic you need to publish to*/
const char * pubTopic = "sys/587014017/test111/dp/post/json/+";

DFRobot_Iot myIot;
WiFiClient espClient;
PubSubClient client(espClient);

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
    if(client.connect(myIot._clientId, myIot._username, myIot._password)){
      Serial.println("connected");
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
  
  /*Initialize the configuration of OneNet*/
  myIot.init(ONENET_SERVER, ProductID, DeviceId, ApiKey);
  
  client.setServer(myIot._mqttServer,PORT);
  
  /*Set the callback function to execute the callback function when receiving the subscription information*/
  client.setCallback(callback);
  
  /*Connect to the cloud platform*/
  ConnectCloud();
  
  client.publish(pubTopic,"Hello World !");
}

void loop(){
  if(!client.connected()){
    ConnectCloud();
  }
  client.loop();
}
