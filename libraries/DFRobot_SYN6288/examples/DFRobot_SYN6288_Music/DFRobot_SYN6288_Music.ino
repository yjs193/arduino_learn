/*
播放音乐示例
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

void loop()
{
    int i;
    for (i = 1; i <= 25; i++) syn6288.playPrompt(i, true);
    for (i = 1; i <= 8; i++) syn6288.playChord(i, true);
    for (i = 1; i <= 15; i++) syn6288.playMusic(i, true);
}
