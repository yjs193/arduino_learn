/*
语音播放原始文字示例
*/
#include "DFRobot_SYN6288.h"

SoftwareSerial softSerialSyn6288(2, 3);

DFRobot_SYN6288 syn6288;

void setup()
{
    syn6288.begin(&Serial, 0, 1, 13);
    // syn6288.begin(&softSerialSyn6288, 13);
    syn6288.setVolume(true, 10);
}

String a = "这是语音合成示例";
float b = 3.141592;

void loop()
{
    syn6288.playText(a, 1);
    syn6288.playText(b, 2);
}
