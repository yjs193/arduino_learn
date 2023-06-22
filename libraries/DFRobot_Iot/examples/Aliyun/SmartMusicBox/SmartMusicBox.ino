 /**
  * file SmartMusicBox.ino
  *
  * Connect the speakers. 
  * You can choose to play music according to the instructions sent by the cloud platform.
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
#include "Tone32.h"
#include "music_joy.h"

#define SPEARKER_PIN  A4

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

/*TOPIC that need to be subscribed*/
const char * subTopic = "you sub Topic";//****set

DFRobot_Iot myIot;
WiFiClient espClient;
PubSubClient client(espClient);

uint8_t beginPlay = 0;
uint8_t playWitchOne = 0;

/*Play LiangLiang*/
static void playLiangLiang(){
  for (int i = 0; i < sizeof(liangliangData)/sizeof(liangliangData[0]); i++) { 
    int noteDuration = liangliangDurations[i] *600;
    tone(SPEARKER_PIN, liangliangData[i],noteDuration); 
  }
  noTone(SPEARKER_PIN);
} 

/*Play Castle In The Sky*/
static void playCastleInTheSky(){
  for (int i = 0; i < sizeof(CastleInTheSkyData)/sizeof(CastleInTheSkyData[0]); i++) { 
    int noteDuration = CastleInTheSkyDurations[i] *600;
    tone(SPEARKER_PIN, CastleInTheSkyData[i],noteDuration); 
  }
  noTone(SPEARKER_PIN);
}

static void playMusic(){
  if(playWitchOne == 0){
    playLiangLiang();
  }else{
    playCastleInTheSky();
  }
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
  playWitchOne = root["params"][Identifier];
  Serial.print("playWitchOne=");
  Serial.println(playWitchOne);
  beginPlay = 1;
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

void loop(){
  if(!client.connected()){
    ConnectCloud();
  }
  if(beginPlay == 1){
    playMusic();
    beginPlay =0;
  }
  client.loop();
}