#include "DFRobot_UDPServer.h"
#include "DFRobot_Iot.h"
#include "WiFiUdp.h"

DFRobot_UDPServer myserver;
DFRobot_Iot myIot;
WiFiUDP udp;


void onUdpServerRecvMsg(String message)
{
  Serial.println(message);
}


void setup() {
  Serial.begin(9600);
  WiFi.disconnect(true);
  myIot.setSoftAP("jhjhjhjh", "12345678");
  delay(1000);
  Serial.println(myIot.getWiFiSoftIP());
  myserver.setPort("1234");
  myserver.setCallback(onUdpServerRecvMsg);
}

void loop() {
  delay(1000);
  myserver.sendUdpMsg("hello,I am server");
  myserver.sendUdpMsg(1111);
}