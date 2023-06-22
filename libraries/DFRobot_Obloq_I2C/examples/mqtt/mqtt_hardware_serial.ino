#include "UNO_Obloq.h"
#include <SoftwareSerial.h>

UNO_Obloq olq;
SoftwareSerial softwareSerial(2, 8);
const String topics[5] = {"HyA8nDhMQ","S18ofd2mm","SkTDbjp7Q","HJlO-iaQQ","Bkt_-i6mm"};

void obloqDebugEvent(String& debugMessage, int8_t errorCode){
  softwareSerial.println(debugMessage);
  softwareSerial.println(errorCode);
}

void setup() {
  softwareSerial.begin(9600);
  olq.registerDubugHandle(obloqDebugEvent);
  olq.startConnect(0, 1, "dfrobotYanfa", "hidfrobot", "HJZTNhw3fm", "HyGp4hD2zm", topics, "iot.dfrobot.com.cn", 1883);
}

void loop() {
  softwareSerial.println("----start----");
  olq.publish(olq.topic_0, "hello11");
  olq.publish(olq.topic_1, "hello22");
  olq.publish(olq.topic_2, "hello33");
  olq.publish(olq.topic_3, "hello44");
  olq.publish(olq.topic_4, "hello55");
  if ((olq.available(olq.topic_0))) {
    softwareSerial.println(olq.getMessage(olq.topic_0));
  }
  if ((olq.available(olq.topic_1))) {
    softwareSerial.println(olq.getMessage(olq.topic_1));
  }
  if ((olq.available(olq.topic_2))) {
    softwareSerial.println(olq.getMessage(olq.topic_2));
  }
  if ((olq.available(olq.topic_3))) {
    softwareSerial.println(olq.getMessage(olq.topic_3));
  }
  if ((olq.available(olq.topic_4))) {
    softwareSerial.println(olq.getMessage(olq.topic_4));
  }
  delay(1000);
}

