/*
语音播放串口文字示例
*/
#include "DFRobot_SYN6288.h"

SoftwareSerial softSerialSyn6288(2, 3);

DFRobot_SYN6288 syn6288;

void setup()
{
    Serial.begin(9600);
    syn6288.begin(&softSerialSyn6288, 13);
    syn6288.setVolume(true, 10);
}

uint8_t music = 0;

void loop()
{
    if (Serial.available())
    {
        if (++music > 15) music = 1;
        syn6288.playText(Serial.readString(), music);
    }
}
