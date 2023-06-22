//武汉套件、解释执行MP3模块
#include <DFRobot_PlayerMini.h>
#include <SoftwareSerial.h>

SoftwareSerial softSerialmp3_9w(9, 'w');
DFRobot_PlayerMini mp3_9w;

void setup() {
    mp3_9w.begin(&softSerialmp3_9w);
    mp3_9w.volume(50);
}

void loop() {

}


// cpp模式 软串口
#include <DFRobot_PlayerMini.h>
#include <SoftwareSerial.h>

SoftwareSerial softSerialmp3(9, 2);
DFRobot_PlayerMini mp3;

void setup() {
    mp3.begin(&softSerialmp3);
    mp3.volume(50);
}

void loop() {

}


// cpp模式 microbit硬串口
#include <DFRobot_PlayerMini.h>
DFRobot_PlayerMini mp3;

void setup() {
    mp3.begin(0, 1);
    mp3.volume(50);
}

void loop() {

}

// cpp模式 除了microbit硬串口
#include <DFRobot_PlayerMini.h>
DFRobot_PlayerMini mp3;

void setup() {
    mp3.begin(&Serial1, 0, 1);
    mp3.volume(50);
}

void loop() {

}
