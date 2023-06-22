#include <DFRobot_Obloq.h>
#include <SoftwareSerial.h>

SoftwareSerial softSerial(1,2);
DFRobot_Obloq olq(&softSerial, "dfrobotYanfa", "hidfrobot", "192.168.0.106","8080");

void httpMsgHandle(const String& message)
{
    Serial.println(message);
}

void setup() {
  olq.setHttpMsgHandle(httpMsgHandle);
  softSerial.begin(9600);
  olq.startConnect();
  Serial.begin(9600);
}

void loop() {
  olq.get((String)"input?id=1&val=" +(String) "12.5", 10000);
  olq.post("input?name=admin", (String)"{\"id\":\"1\",\"val\":\"" + (String)"12.5" + "\"}", 10000);
  Serial.println("++++++");
}