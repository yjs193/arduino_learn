#include "UNO_Obloq.h"
#include <SoftwareSerial.h>

UNO_Obloq olq;
SoftwareSerial softwareSerial(2, 8);

void obloqDebugEvent(String& debugMessage, int8_t errorCode){
  softwareSerial.println(debugMessage);
  softwareSerial.println(errorCode);
}

void setup() {
  softwareSerial.begin(9600);
  olq.registerDubugHandle(obloqDebugEvent);
  olq.startConnect(0, 1, "dfrobotYanfa", "hidfrobot", "192.168.0.101", 8080);
}

void loop() {
  softwareSerial.println("----start----");
  softwareSerial.println(olq.get("input?id=1&val="+String(millis()), 10000));
  softwareSerial.println(olq.post("input?name=admin","{\"id\":\"1\",\"val\":\""+String(millis())+"\"}", 10000));
  delay(1000);
}

