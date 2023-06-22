#include "DFRobot_UDPServer.h"
#if CONFIG_FREERTOS_UNICORE
#define ARDUINO_RUNNING_CORE 0
#else
#define ARDUINO_RUNNING_CORE 1
#endif
DFRobot_UDPServer::DFRobot_UDPServer()
{
}
void DFRobot_UDPServer::setPort(uint32_t port)
{
  tcpip_adapter_dhcpc_start(TCPIP_ADAPTER_IF_STA);
  fd = socket(PF_INET, SOCK_DGRAM, 0);
  
  int optval = 1;
  setsockopt(fd, SOL_SOCKET, SO_BROADCAST | SO_REUSEADDR, &optval, sizeof(int));
 
  //绑定套接字
  //sockaddr_in servAddr;
  memset(&servAddr, 0, sizeof(servAddr)); //每个字节都用0填充
  //servAddr.sin_family = PF_INET; //使用IPv4地址
  servAddr.sin_family = AF_INET;
  servAddr.sin_addr.s_addr = htonl(INADDR_ANY); //自动获取IP地址
  //servAddr.sin_addr.s_addr = inet_addr("255.255.255.255");
  servAddr.sin_port = htons(port); //端口
  bind(fd, (sockaddr*)&servAddr, sizeof(sockaddr));
  
  memset(&servSend,0,sizeof(servSend));
  servSend.sin_family = AF_INET;   
  servSend.sin_port = htons(port);    
  servSend.sin_addr.s_addr = inet_addr("255.255.255.255");

  memset(&servRecv,0,sizeof(servRecv));
  servRecv.sin_family = AF_INET;   
  servRecv.sin_port = htons(port);    
  servRecv.sin_addr.s_addr = INADDR_ANY;

}

void DFRobot_UDPServer::setPort(String port)
{
  fd = socket(PF_INET, SOCK_DGRAM, 0);
  
  int optval = 1;
  setsockopt(fd, SOL_SOCKET, SO_BROADCAST | SO_REUSEADDR, &optval, sizeof(int));
 
  //绑定套接字
  //sockaddr_in servAddr;
  memset(&servAddr, 0, sizeof(servAddr)); //每个字节都用0填充
  //servAddr.sin_family = PF_INET; //使用IPv4地址
  servAddr.sin_family = AF_INET;
  //servAddr.sin_addr.s_addr = htonl(INADDR_ANY); //自动获取IP地址
  servAddr.sin_addr.s_addr = inet_addr("255.255.255.255");
  servAddr.sin_port = htons(port.toInt()); //端口
  bind(fd, (sockaddr*)&servAddr, sizeof(sockaddr));
  
  memset(&servSend,0,sizeof(servSend));
  servSend.sin_family = AF_INET;   
  servSend.sin_port = htons(port.toInt());   
  servSend.sin_addr.s_addr = inet_addr("255.255.255.255");

  memset(&servRecv,0,sizeof(servRecv));
  servRecv.sin_family = AF_INET;   
  servRecv.sin_port = htons(port.toInt());  
  servRecv.sin_addr.s_addr = INADDR_ANY;
}

void DFRobot_UDPServer::sendUdpMsg(char* msg)
{	
  sendto(fd, msg, strlen(msg), 0, (struct sockaddr *)&servSend, sizeof(struct sockaddr));
}

void DFRobot_UDPServer::sendUdpMsg(String msg)
{
  char sendData[255];
  int len = msg.length();
  memset(sendData,'\0',255);
  msg.toCharArray(sendData,len+1);
  
  sendto(fd, sendData, strlen(sendData), 0, (struct sockaddr*)&servSend, sizeof(struct sockaddr));
}

void DFRobot_UDPServer::sendUdpMsg(double msg)
{
  char msg2[10];
  sprintf(msg2,"%.2f",msg);
  sendto(fd, msg2, strlen(msg2), 0, (struct sockaddr*)&servSend, sizeof(struct sockaddr));
}

void DFRobot_UDPServer::setCallback(CBUdpFunc cb)
{
  recvCb = cb;
  xTaskCreatePinnedToCore(DFRobot_UDPServer::recv, "aa", 2048, this, 1, NULL, ARDUINO_RUNNING_CORE);
  
}

void DFRobot_UDPServer::stop()
{
  closesocket(fd);
  //lwip_shutdown(fd,SHUT_RDWR);
  Serial.println("server over");
}
