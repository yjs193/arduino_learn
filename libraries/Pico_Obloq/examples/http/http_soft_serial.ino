#include "Microbit_Obloq.h"
#include <SoftwareSerial.h>

Microbit_Obloq olq;
SoftwareSerial softwareSerial(0, 1);

void obloqDebugEvent(String& debugMessage, int8_t errorCode){
  Serial.println(debugMessage);
  Serial.println(errorCode);
}

void setup() {
  Serial.begin(9600);
  softwareSerial.begin(9600);
  olq.registerDubugHandle(obloqDebugEvent);
  olq.startConnect(&softwareSerial, "dfrobotYanfa", "hidfrobot", "192.168.0.101", 8080);
}

void loop() {
  Serial.println("----start----");
  Serial.println(olq.get("input?id=1&val="+String(millis()), 10000));
  Serial.println(olq.post("input?name=admin","{\"id\":\"1\",\"val\":\""+String(millis())+"\"}", 10000));
  delay(1000);
}

