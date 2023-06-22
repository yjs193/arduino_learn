#include <DFRobot_SerialMp3.h>

SoftwareSerial serialMp3soft(9, 10);
DFRobot_SerialMp3 serialMp3;

void setup() {
  serialMp3.begin(&serialMp3soft);
}

void loop() {
  serialMp3.volume(50);
  serialMp3.playList(50);
  serialMp3.insertTracks(50);

  serialMp3.play();
  serialMp3.queryPlayStatus();
  serialMp3.pause();
  serialMp3.stop();
  serialMp3.end();
  serialMp3.prev();
  serialMp3.next();
  serialMp3.volumeUp();
  serialMp3.volumeDown();
}