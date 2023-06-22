#include"DFRobot_NFC0231.h"
#include <Wire.h>

#if ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif
#include <Wire.h>
#if defined(NRF5) || defined(NRF52833)
#include <basic.h>
uart SerialNrf(0, 0);
#endif
#define PN532_PACKBUFFSIZ 64


bool DFRobot_PN532::passWordCheck(int block,uint8_t id[],uint8_t st[]) {
    if(!this->enable)
        return false;
    unsigned char cmdPassWord[22]={0x00,0x00,0xFF,0x0F,0xF1,0xD4,0x40,0x01,0x60,\
                0x07,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xD1,0xAA,0x40,0xEA,0xC2,0x00};
    unsigned char sum=0,count=0;
    cmdPassWord[9]=block;
    for(int i=10;i<16;i++) cmdPassWord[i]=st[i-10];// 密码
    for(int i=16;i<20;i++) cmdPassWord[i]=id[i-16];// UID
    for(int i=0;i<20;i++) sum+=cmdPassWord[i];
    cmdPassWord[20]=0xff-sum&0xff;
    if(!Clear()) return "unknown error";
    this->writecommand(cmdPassWord,22);
    if(!this->readAck(16))
        return false;
    //Serial.println(checkDCS(16));
    //Serial.println(receive_ACK[12]);
    //Serial.println(receive_ACK[13]);

    if(checkDCS(16)==1 && receive_ACK[12]==0x41 && receive_ACK[13]==0x00)
        return true;
    else
        return false; 
}
bool DFRobot_PN532::checkDCS(int x) { // NFC S50卡 DCS校验检测子函数
    if(!this->enable)
        return 0;
    unsigned char sum=0,dcs=0;
    for(int i=6;i<x-2;i++)
    {
        sum += this->receive_ACK[i];
    }
    dcs = 0xff - sum&0xff;
    if(dcs==this->receive_ACK[x-2])
        return 1;
    else
        return 0;
}
void DFRobot_PN532::writeData(int block, uint8_t data[]) {
    if(!this->enable)
        return;
    if(!this->scan())
        return;

    if(!this->passWordCheck(block,uid,passWord))
      return;
    unsigned char cmdWrite[]={0x00,0x00,0xff,0x15,0xEB,0xD4,0x40,0x01,0xA0, \
                            0x06,0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07, \
                            0x08,0x09,0x0A,0x0B,0x0C,0x0D,0x0E,0x0F,0xCD,0x00};
    unsigned char sum=0,count=0;
    cmdWrite[9]=block;
    for(int i=10;i<26;i++) cmdWrite[i]=data[i-10];// 待写入的数据
    for(int i=0;i<26;i++) sum+=cmdWrite[i];//加和
    cmdWrite[26]=0xff-sum&0xff;//  计算DCS

    if(!Clear()) return ;
    this->writecommand(cmdWrite,28);

    this->readAck(16);
}
void DFRobot_PN532::writeData(int block, uint8_t index, uint8_t data) {
    if(!this->enable)
        return;
    index = max(min((int)index,16),1);
    this->readData(block);
    this->blockData[index-1] = data;
    this->writeData(block, this->blockData);
}
int DFRobot_PN532::readData(int block, uint8_t index)
{
    if(!this->enable)
        return -1;
    String error = this->readData(block);
    if(error == "no card!" || error == "read errorr!" || error =="unknown errorr!"
                        || error == "read timeout!" || error == "weak up error!")
        return -1;
    return this->blockData[index-1];
}

bool DFRobot_PN532::scan(String uid) {
    if(!this->enable)
        return false;
    if(this->scan())
    {
        String uidSrt = "";
        for(int i=0; i<4; i++)
        {
            if(this->uid[i]<=0x0f){
                uidSrt += "0";
                uidSrt += String(this->uid[i],HEX);
            }
            else
                uidSrt += String(this->uid[i],HEX);
        }
        if(uid == uidSrt)
            return true;
    }
    return false;
}

bool DFRobot_PN532::scan() {
    if(!this->enable)
        return false;
    uint8_t cmdUID[11]={0x00, 0x00, 0xFF, 0x04, 0xFC, 0xD4, 0x4A, 0x01, 0x00, 0xE1, 0x00};
    int repeat = 1;
    while(repeat){
        if(!Clear()) return false;
        writecommand(cmdUID,11);
        if(readAck(25))
            break;
        repeat--;
    }
    if(repeat==0)
        return false;
    for(int i=0; i<4; i++)
      uid[i] = receive_ACK[i+19];
    if(uid[0] == 0xFF && uid[1] == 0xFF && uid[2] == 0xFF && uid[3] == 0xFF)
        return false;
    if(uid[0] == 0x80 && uid[1] == 0x80 && uid[2] == 0x80 && uid[3] == 0x80)
        return false;
    return true;
}

String DFRobot_PN532::readUid() {
    if(!this->enable)
        return "weak up error!";
    if(!scan())
        return "no card!";
    String uidSrt = "";
    for(int i=0; i<4; i++)
    {
        if(uid[i]<=0x0f){
            uidSrt += "0";
            uidSrt += String(uid[i],HEX);
        }
        else
            uidSrt += String(uid[i],HEX);
    }
    return uidSrt;
}
String DFRobot_PN532::readData(int page) {
    if(!this->enable)
        return "weak up error!";
    if(!scan())
        return "no card!";
    if(!passWordCheck(page,uid,passWord))
        return "read errorr!";
    
    unsigned char cmdRead[12]={0x00,0x00,0xff,0x05,0xfb,0xD4,0x40,0x01,0x30,0x07,0xB4,0x00};
    unsigned char sum=0,count=0;
    cmdRead[9]=page;
    for(int i=0;i<10;i++) sum+=cmdRead[i];
    cmdRead[10]=0xff-sum&0xff;
    
    if(!this->Clear()) return "unknown error";
    writecommand(cmdRead,12);
    if(!readAck(32))
        return "read timeout!";

    String dataSrt = "0x";
    if(checkDCS(32)==1 && receive_ACK[12]==0x41 && receive_ACK[13]==0x00) {
        for(int i=0; i<16; i++)
        {
            blockData[i] = receive_ACK[i+14];
            if(receive_ACK[i+14]<=0x0f){
                dataSrt += "0";
                dataSrt += String(receive_ACK[i+14],HEX);
            }
            else
                dataSrt += String(receive_ACK[i+14],HEX);
            if(i<15) dataSrt += " 0x";
        }
    }
    return dataSrt;
}

bool DFRobot_PN532_IIC::Clear() {
    return 1;
}

bool DFRobot_PN532_IIC::wakeUp() {
    this->passWord[0] = 0xff;
    this->passWord[1] = 0xff;
    this->passWord[2] = 0xff;
    this->passWord[3] = 0xff;
    this->passWord[4] = 0xff;
    this->passWord[5] = 0xff;
    uint8_t pn532PacketBuffer[6]={0x00};
    Wire.begin();
    pn532PacketBuffer[0] = PN532_COMMAND_SAMCONFIGURATION;
    pn532PacketBuffer[1] = 0x01; // normal mode;
    pn532PacketBuffer[2] = 0x14; // timeout 50ms * 20 = 1 second
    pn532PacketBuffer[3] = 0x01; // use IRQ pin!
    enable = true;
    uint8_t checksum;
    uint8_t cmdlen=4;
    cmdlen++;
    delay(20);     // or whatever the delay is for waking up the board
    Wire.beginTransmission(PN532_I2C_ADDRESS);
    checksum = PN532_PREAMBLE + PN532_PREAMBLE + PN532_STARTCODE2;
    Wire.write(PN532_PREAMBLE);
    Wire.write(PN532_PREAMBLE);
    Wire.write(PN532_STARTCODE2);
    Wire.write(cmdlen);
    Wire.write(~cmdlen + 1);
    Wire.write(PN532_HOSTTOPN532);
    checksum += PN532_HOSTTOPN532;
    for (uint8_t i=0; i<4; i++){
        Wire.write(pn532PacketBuffer[i]);
        checksum += pn532PacketBuffer[i];
    }
    Wire.write((byte)~checksum);
    Wire.write((byte)PN532_POSTAMBLE);
    Wire.endTransmission();
    delay(10);
    if(readAck(14)!=1){
        return 0;
    }
    return (receive_ACK[12] == 0x15);
}

void DFRobot_PN532_IIC::writecommand(uint8_t* cmd, uint8_t cmdlen) {
    delay(20);
    Wire.beginTransmission(PN532_I2C_ADDRESS);
    for (uint8_t i=0; i<cmdlen-1; i++){
        Wire.write(cmd[i]);
    }
    Wire.endTransmission();
}

bool DFRobot_PN532_IIC::readAck(int x,long timeout ) {
    uint8_t pn532ack[6];
    pn532ack[0] = 0x00;
    pn532ack[1] = 0x00;
    pn532ack[2] = 0xFF;
    pn532ack[3] = 0x00;
    pn532ack[4] = 0xFF;
    pn532ack[5] = 0x00;
    delay(2);
    Wire.requestFrom(PN532_I2C_ADDRESS,8);
    Wire.read();
    for(int i = 0; i < 6; i++){
        delay(1);
        receive_ACK[i]= Wire.read();
    }
    delay(100);
    Wire.requestFrom(PN532_I2C_ADDRESS,x-4);
    Wire.read();
    for(int i = 0; i < x-6; i++){
        delay(1);
        receive_ACK[6+i] = Wire.read();
    }
    //for(int i=0;i<32;i++){
    //    Serial.print(receive_ACK[i],HEX);Serial.print("  ");
        
    //}
    if(strncmp((char *)pn532ack,(char *)receive_ACK, 6)!=0){
        return 0 ;
    }
    return 1;
}

bool DFRobot_PN532_IIC::begin(void) {
    return wakeUp();
}

#if defined(ESP_PLATFORM)
void DFRobot_PN532_UART::begin(HardwareSerial *serial, int rx, int tx) {
    this->uartTimeout = 1000;
    this->_serialH = serial;
    this->_serialS = NULL;
    this->_serialH->begin(115200,rx,tx);
    wakeUp();
}
#elif defined(NRF5)
void DFRobot_PN532_UART::begin(int rx, int tx) {
    rx = g_PinID[rx];
    tx = g_PinID[tx];
    MicroBitPin *rxpin = getMicroBitPin(rx);
    MicroBitPin *txpin = getMicroBitPin(tx);
    uBit.serial.redirect(txpin->name, rxpin->name);
    this->uartTimeout = 1000;
    this->_serialH = &SerialNrf;
    this->_serialS = NULL;
    this->_serialH->begin(115200);
    wakeUp();
}
#elif defined(NRF52833)
void DFRobot_PN532_UART::begin(int rx, int tx) {
    rx = g_PinID[rx];
    tx = g_PinID[tx];
    MicroBitPin *rxpin = getMicroBitPin(rx);
    MicroBitPin *txpin = getMicroBitPin(tx);
    uBit.serial.redirect(*txpin, *rxpin);
    this->uartTimeout = 1000;
    this->_serialH = &SerialNrf;
    this->_serialS = NULL;
    this->_serialH->begin(115200);
    wakeUp();
}
#else
void DFRobot_PN532_UART::begin(HardwareSerial *serial)
{   this->uartTimeout = 1000;
    this->_serialH = serial;
    this->_serialS = NULL;
    this->_serialH->begin(115200);
    wakeUp();
}
void DFRobot_PN532_UART::begin(SoftwareSerial *serial)
{   this->uartTimeout = 1000;
    this->_serialH = NULL;
    this->_serialS = serial;
    this->_serialS->begin(115200);
    wakeUp();
}
#endif

bool DFRobot_PN532_UART::readAck(int x, long timeout) { //读取x个串口发来的数据
    unsigned char i = x;
    long _timeout = millis();
    if(_serialH){
        while(x)
        {
            if(this->_serialH->available()){
                receive_ACK[i-x]= _serialH->read();
             x--;  
            }
            if(((millis() - _timeout >= this->uartTimeout) || millis() - _timeout >= timeout) && !this->_serialH->available() && x>0)
                    return false;
        }
    }else if(_serialS){
        while(x)
        {
            if(this->_serialS->available()){
                receive_ACK[i-x]= _serialS->read();
             x--;  
            }
            if(((millis() - _timeout >= this->uartTimeout) || millis() - _timeout >= timeout) && !this->_serialS->available() && x>0)
                    return false;
        }
    }

    return true;
}

bool DFRobot_PN532_UART::wakeUp() {
    this->passWord[0] = 0xff;
    this->passWord[1] = 0xff;
    this->passWord[2] = 0xff;
    this->passWord[3] = 0xff;
    this->passWord[4] = 0xff;
    this->passWord[5] = 0xff;
    memset(this->receive_ACK,0,35);
    memset(this->blockData,0,16);
    memset(this->uid,0,4);
    unsigned char wake[24]={0x55, 0x55, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
           0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0x03, 0xfd, 0xd4, 0x14, 0x01, 0x17, 0x00};//wake up NFC module
    this->writecommand(wake, 24);            //wake up nfc modules

    if(!this->readAck(15))
        this->enable = false;
    this->enable = true;
    return true;
}

void DFRobot_PN532_UART::writecommand(uint8_t *command_data, uint8_t bytes) {
    if(_serialH){
        for(int i=0; i<bytes; i++)
        {
            this->_serialH->write(command_data[i]);
#if defined(ARDUINO) && ARDUINO >= 100
#ifndef ESP_PLATFORM && NRF5
            this->_serialH->flush();// complete the transmission of outgoing serial data
#endif
#endif
        }
    }else if(_serialS){
        for(int i=0; i<bytes; i++)
        {
            this->_serialS->write(command_data[i]);
#if defined(ARDUINO) && ARDUINO >= 100
#ifndef ESP_PLATFORM && NRF5
            this->_serialS->flush();// complete the transmission of outgoing serial data
#endif
#endif
        }
    }
}

bool DFRobot_PN532_UART::Clear() {
    if(!this->enable)
        return false;
    char residual;
    long timeout = millis();
    if(_serialH){
        while(this->_serialH->available()){
            residual=this->_serialH->read();
            if(millis() - timeout >= this->uartTimeout && this->_serialH->available())
                return false;
        }
    }else if(_serialS){
        while(this->_serialS->available()){
            residual=this->_serialS->read();
            if(millis() - timeout >= this->uartTimeout && this->_serialS->available())
                return false;
        }
    }
    return true;
}
