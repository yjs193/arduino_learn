#ifndef DFROBOT_UDPCLIENT_h
#define DFROBOT_UDPCLIENT_h

#include "Arduino.h"
#include <lwip/sockets.h>
#include <MPython.h>

typedef void (*CBUdpClientFunc)(String message); 

class DFRobot_UDPClient
{
  public:
    DFRobot_UDPClient();

    void connectToServer(String server,uint32_t port);
    void connectToServer(String server,String port);
    void stop();
    void sendUdpMsg(char* msg);
    void sendUdpMsg(String msg);
    void sendUdpMsg(double msg);

    void setCallback(CBUdpClientFunc cb);

    static void recv(void *param){
      DFRobot_UDPClient *self = (DFRobot_UDPClient *)param;
      char buffer[255]; //缓冲区
      int nSize = sizeof(sockaddr_in);
      while(1){
        memset(buffer,'\0',255);
        int strLen = lwip_recvfrom(self->fd, buffer, 255, 0, (sockaddr *)&self->clientAddr, (socklen_t*)&nSize);
        //int strLen = lwip_sendto(self->fd, buffer, strlen(buffer), 0, (sockaddr *)&self->clientAddr, sizeof(clientAddr));  
        if(strLen < 0){
          delay(100);
          continue;
        }
        //Serial.println(inet_ntoa(self->clientAddr.sin_addr));
		if(self->myserver == inet_ntoa(self->clientAddr.sin_addr)){
			String back = buffer;
            self->recvCb(buffer);
		}
      }
    }

  private:
    int fd;
    sockaddr_in clientAddr;
	sockaddr_in clientSend;
    CBUdpClientFunc recvCb;
	String myserver;

};

#endif