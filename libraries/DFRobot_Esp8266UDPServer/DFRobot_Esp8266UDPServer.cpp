#include "DFRobot_Esp8266UDPServer.h"
#include "Arduino.h"
DFRobot_Esp8266UDPServer *udppServer;

bool UdpServerStart = false;

static void recvServer(void)
{
    if(UdpServerStart){
        int packetSize = udppServer->Udp.parsePacket();
        char packetBuffer[UDP_TX_PACKET_MAX_SIZE + 1];
        if (packetSize) {
            int n = udppServer->Udp.read(packetBuffer, UDP_TX_PACKET_MAX_SIZE);
            packetBuffer[n] = 0;
            udppServer->recvCb(packetBuffer);
        }
    }
}

DFRobot_Esp8266UDPServer::DFRobot_Esp8266UDPServer(void)
{
    udppServer = this;
}

DFRobot_Esp8266UDPServer::~DFRobot_Esp8266UDPServer(void)
{
    udppServer = NULL;
    UdpServerStart = false;
}


void DFRobot_Esp8266UDPServer::setPort(uint32_t port)
{
    Udp.begin((uint16_t)port);
    UdpServerStart = true;
}
void DFRobot_Esp8266UDPServer::setPort(String port)
{
    Udp.begin((uint16_t)port.toInt());
    UdpServerStart = true;
}
void DFRobot_Esp8266UDPServer::sendUdpMsg(String msg)
{
    char sendData[255];
    int len = msg.length();
    memset(sendData,'\0',255);
    msg.toCharArray(sendData,len+1);
    Udp.beginPacket(Udp.remoteIP(), Udp.remotePort());
    Udp.write(sendData);
    Udp.endPacket();
}
void DFRobot_Esp8266UDPServer::sendUdpMsg(double msg)
{
    char msg2[10];
    sprintf(msg2,"%.2f",msg);
    Udp.beginPacket(Udp.remoteIP(), Udp.remotePort());
    Udp.write(msg2);
    Udp.endPacket();
}
void DFRobot_Esp8266UDPServer::sendUdpMsg(char* msg)
{
    Udp.beginPacket(Udp.remoteIP(), Udp.remotePort());
    Udp.write(msg);
    Udp.endPacket();
}

void DFRobot_Esp8266UDPServer::setCallback(CBUdpFunc cb)
{
    recvCb = cb;
    tarefa1.task(5, recvServer);   
}


