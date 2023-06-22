#include "Microbit_Obloq.h"
#include <SoftwareSerial.h>

Microbit_Obloq olq;
SoftwareSerial softwareSerial(2, 8);
const String topics[5] = {"HyA8nDhMQ","S18ofd2mm","SkTDbjp7Q","HJlO-iaQQ","Bkt_-i6mm"};

void obloqDebugEvent(String& debugMessage, int8_t errorCode){
  softwareSerial.println(debugMessage);
  softwareSerial.println(errorCode);
}

void obloqMqttEventT0(String& message){
  softwareSerial.println(message);
}
void obloqMqttEventT1(String& message){
  softwareSerial.println(message);
}
void obloqMqttEventT2(String& message){
  softwareSerial.println(message);
}
void obloqMqttEventT3(String& message){
  softwareSerial.println(message);
}
void obloqMqttEventT4(String& message){
  softwareSerial.println(message);
}

const MsgHandleCb msgHandles[5] = {obloqMqttEventT0,obloqMqttEventT1,obloqMqttEventT2,obloqMqttEventT3,obloqMqttEventT4};

void setup() {
  softwareSerial.begin(9600);
  olq.registerDubugHandle(obloqDebugEvent);
  olq.registerMqttHandle(msgHandles);
  olq.startConnect(0, 1, "dfrobotYanfa", "hidfrobot", "HJZTNhw3fm", "HyGp4hD2zm", topics, "iot.dfrobot.com.cn", 1883);
}

void loop() {
  softwareSerial.println("----start----");
  olq.publish(olq.topic_0, "hello11");
  olq.publish(olq.topic_1, "hello22");
  olq.publish(olq.topic_2, "hello33");
  olq.publish(olq.topic_3, "hello44");
  olq.publish(olq.topic_4, "hello55");
  delay(1000);
}

