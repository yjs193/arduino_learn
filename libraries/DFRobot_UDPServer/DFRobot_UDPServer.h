#ifndef DFROBOT_UDPSERVER_h
#define DFROBOT_UDPSERVER_h

#include "Arduino.h"
#include <lwip/sockets.h>
#include <MPython.h>
#include <tcpip_adapter.h>



typedef struct node
{
  struct sockaddr_in addr;
  struct node *next;  
}listnode,*linklist;

typedef void (*CBUdpFunc)(String message); 

class DFRobot_UDPServer
{
  public:
    DFRobot_UDPServer();

    void stop();
    void setCallback(CBUdpFunc cb);
    void setPort(uint32_t port);
    void setPort(String port);
    void sendUdpMsg(String msg);
    void sendUdpMsg(double msg);
    void sendUdpMsg(char* msg);
        
    static void recv(void *param){
      DFRobot_UDPServer *self = (DFRobot_UDPServer *)param;
      int nSize = sizeof(sockaddr);
      char buffer[255]; 
     
      while(1){
        memset(buffer,'\0',255);
        //memset(&self->servRecv,0,sizeof(self->servRecv));
        int strLen = recvfrom(self->fd, buffer, 255, 0, (sockaddr *)&self->servRecv, (socklen_t*)&nSize);
        if(strLen < 0){
          delay(100);
          continue;
        }
        //Serial.println(inet_ntoa(self->servRecv.sin_addr));
        String back = buffer;
        if(back == "ImClientStartConnection!"){
            continue;
        }
        self->recvCb(buffer);
      }
    }
    
  private:
    int fd;
    sockaddr_in servAddr;
	sockaddr_in servSend;
	sockaddr_in servRecv;
    CBUdpFunc recvCb;
};

#endif