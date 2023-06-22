#include "DFRobot_UDPClient.h"
#if CONFIG_FREERTOS_UNICORE
#define ARDUINO_RUNNING_CORE 0
#else
#define ARDUINO_RUNNING_CORE 1
#endif
DFRobot_UDPClient::DFRobot_UDPClient()
{
  
}

void DFRobot_UDPClient::connectToServer(String server,uint32_t port)
{
  fd = lwip_socket(AF_INET,SOCK_DGRAM,0);
  int set = 1;
  setsockopt(fd, SOL_SOCKET, SO_REUSEADDR|SO_BROADCAST, &set, sizeof(int));
  clientAddr.sin_family = AF_INET;   
  clientAddr.sin_port = htons(port);    
  clientAddr.sin_addr.s_addr = INADDR_ANY; 
  bind(fd, (struct sockaddr *)&clientAddr, sizeof(struct sockaddr));
  
  clientSend.sin_family = AF_INET;   
  clientSend.sin_port = htons(port);
  clientSend.sin_addr.s_addr = inet_addr(server.c_str()); 
  
  myserver = server;
}

void DFRobot_UDPClient::connectToServer(String server,String port)
{
  fd = lwip_socket(AF_INET,SOCK_DGRAM,0);
  int set = 1;
  setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &set, sizeof(int));
  clientAddr.sin_family = AF_INET;   
  clientAddr.sin_port = htons(port.toInt());    
  clientAddr.sin_addr.s_addr = INADDR_ANY; 
  bind(fd, (struct sockaddr *)&clientAddr, sizeof(struct sockaddr));
  
  clientSend.sin_family = AF_INET;   
  clientSend.sin_port = htons(port.toInt());   
  clientSend.sin_addr.s_addr = inet_addr(server.c_str()); 
  
  myserver = server;
}

void DFRobot_UDPClient::sendUdpMsg(String msg)
{
  int len = sizeof(clientSend);   
  char sendData[255];
  int len2 = msg.length();
  memset(sendData,'\0',255);
  msg.toCharArray(sendData,len2+1);
  

  
  if(sendto(fd, sendData, strlen(sendData), 0, (sockaddr *)&clientSend, len)<0){
    delay(10); 
  }else{
    delay(10); 
  }
}

void DFRobot_UDPClient::sendUdpMsg(char* msg)
{
  int len = sizeof(clientSend);   
  if(sendto(fd, msg, strlen(msg), 0, (sockaddr *)&clientSend, len)<0){
    delay(10); 
  }else{
    delay(10); 
  }
}

void DFRobot_UDPClient::sendUdpMsg(double msg)
{
  int len = sizeof(clientSend);   
  char msg2[10];
  
  sprintf(msg2,"%.2f",msg);
  if(sendto(fd, msg2, strlen(msg2), 0, (sockaddr *)&clientSend, len)<0){
    delay(10); 
  }else{
    delay(10); 
  }
}

void DFRobot_UDPClient::setCallback(CBUdpClientFunc cb)
{
  recvCb = cb;
  xTaskCreatePinnedToCore(DFRobot_UDPClient::recv, "aa", 2048, this, 1, NULL, ARDUINO_RUNNING_CORE);
  
}

void DFRobot_UDPClient::stop()
{
  closesocket(fd);
  //lwip_shutdown(fd,SHUT_RDWR);
  //Serial.println("server over");
}