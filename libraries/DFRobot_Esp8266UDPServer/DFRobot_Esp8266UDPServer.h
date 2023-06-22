
#ifndef __DFRobot_Esp8266UDPServer_H
#define __DFRobot_Esp8266UDPServer_H

#include <WiFiUdp.h>
#include <ritos.h>
typedef void (*CBUdpFunc)(String message);


class DFRobot_Esp8266UDPServer
{
public:

    DFRobot_Esp8266UDPServer(void);
    ~DFRobot_Esp8266UDPServer(void);

    void setPort(uint32_t port);
    void setPort(String port);
    void sendUdpMsg(String msg);
    void sendUdpMsg(double msg);
    void sendUdpMsg(char* msg);
    
    void setCallback(CBUdpFunc cb);
    
    WiFiUDP Udp;
    CBUdpFunc recvCb;

private:
    Ritos tarefa1;
};

#endif
