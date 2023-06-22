#include "DFRobot_UDPClient.h"
#include "DFRobot_Iot.h"


DFRobot_UDPClient myclient;
DFRobot_Iot myIot;

void onUdpClientRecvMsg(String message)
{
  Serial.println(message);
}


void setup() {
  
  Serial.begin(9600);
  WiFi.disconnect(true);
  myIot.wifiConnect("jhjhjhjh", "12345678");
  while (!myIot.wifiStatus()) {
    Serial.print(".");
    delay(100);
  }
  Serial.println(myIot.getWiFiLocalIP());
  myclient.connectToServer("192.168.4.1","1234");
  myclient.setCallback(onUdpClientRecvMsg);
  Serial.println("1111");
}

void loop() {
  delay(1000);
  myclient.sendUdpMsg("hello,I am client");
  myclient.sendUdpMsg(2222.345);
}