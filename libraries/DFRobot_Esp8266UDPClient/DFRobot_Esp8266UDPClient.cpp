#include "DFRobot_Esp8266UDPClient.h"
#include "Arduino.h"
DFRobot_Esp8266UDPClient *udppClient;

bool UdpClientStart = false;

static void recvClient(void)
{
    
    if(UdpClientStart){
        int packetSize = udppClient->Udp.parsePacket();
        char packetBuffer[UDP_TX_PACKET_MAX_SIZE + 1];
        if (packetSize) {
            
            int n = udppClient->Udp.read(packetBuffer, UDP_TX_PACKET_MAX_SIZE);
            packetBuffer[n] = 0;
            udppClient->recvCb(packetBuffer);
        }
    }
    
}


DFRobot_Esp8266UDPClient::DFRobot_Esp8266UDPClient(void)
{
    udppClient = this;
}

DFRobot_Esp8266UDPClient::~DFRobot_Esp8266UDPClient(void)
{
    udppClient = NULL;
    UdpClientStart = false;
}


void DFRobot_Esp8266UDPClient::connectToServer(String server,uint32_t port)
{
    Udp.begin(port);
    UdpClientStart = true;
    strcpy(ip, server.c_str());
    _port = port;
    _ip =  ip;
     
}
void DFRobot_Esp8266UDPClient::connectToServer(String server,String port)
{
    Udp.begin(port.toInt());
    UdpClientStart = true;
    strcpy(ip, server.c_str());
    _port = port.toInt();
    _ip =  ip;
}
void DFRobot_Esp8266UDPClient::stop()
{

}
void DFRobot_Esp8266UDPClient::sendUdpMsg(char* msg)
{
    Udp.beginPacket(_ip, _port);
    Udp.write(msg);
    Udp.endPacket();
}
void DFRobot_Esp8266UDPClient::sendUdpMsg(String msg)
{
    char sendData[255];
    int len = msg.length();
    memset(sendData,'\0',255);
    msg.toCharArray(sendData,len+1);

    Udp.beginPacket(_ip, _port);
    Udp.write(sendData);
    Udp.endPacket();
}
void DFRobot_Esp8266UDPClient::sendUdpMsg(double msg)
{
    char msg2[10];
    sprintf(msg2,"%.2f",msg);
    Udp.beginPacket(_ip, _port);
    Udp.write(msg2);
    Udp.endPacket();
}

void DFRobot_Esp8266UDPClient::setCallback(CBUdpClientFunc cb)
{
    recvCb = cb;
    tarefa1.task(10,recvClient);
}

