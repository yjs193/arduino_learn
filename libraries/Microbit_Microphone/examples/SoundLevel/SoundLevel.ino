#include <Microbit_Microphone.h>

Microbit_Microphone *microphone = NULL;
void setup() {
  Serial.begin(9600);
  microphone = new Microbit_Microphone();
}
void loop() {
  delay(1000);
  int soundlevel = microphone->level->getValue();
  int scaled = max(52,min(soundlevel,120))-52;
  int soundlevelSensor = min(0xff,scaled*0xff/(120-52));
  Serial.println(soundlevelSensor);
}
