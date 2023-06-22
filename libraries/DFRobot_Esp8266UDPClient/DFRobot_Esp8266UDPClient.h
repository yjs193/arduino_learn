
#ifndef __DFRobot_Esp8266UDPClient_H
#define __DFRobot_Esp8266UDPClient_H
#include "Arduino.h"
#include <WiFiUdp.h>
#include <ritos.h>

typedef void (*CBUdpClientFunc)(String message);

class DFRobot_Esp8266UDPClient
{
public:

    DFRobot_Esp8266UDPClient(void);
    ~DFRobot_Esp8266UDPClient(void);

    void connectToServer(String server,uint32_t port);
    void connectToServer(String server,String port);
    void stop();
    void sendUdpMsg(char* msg);
    void sendUdpMsg(String msg);
    void sendUdpMsg(double msg);

    void setCallback(CBUdpClientFunc cb);

    WiFiUDP Udp;
    CBUdpClientFunc recvCb;

private:
    Ritos tarefa1;
    char ip[20]; 
    const char *_ip;
    uint32_t _port;
};

#endif
