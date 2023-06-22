#include "Microbit_Sensors.h"

void shakeEvent(){
  Serial.println("Shake");
}
void logoUpEvent(){
  Serial.println("LogoUp");
}
void logoDownEvent(){
  Serial.println("LogoDown");
}
void screenUpEvent(){
  Serial.println("ScreenUp");
}
void screenDownEvent(){
  Serial.println("ScreenDown");
}
void tiltLeftEvent(){
  Serial.println("TiltLeft");
}
void tiltRightEvent(){
  Serial.println("TiltRight");
}
void freeFallEvent(){
  Serial.println("FreeFall");
}
void threeGEvent(){
  Serial.println("ThreeG");
}
void sixGEvent(){
  Serial.println("SixG");
}
void eightGEvent(){
  Serial.println("EightG");
}


void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Sensors.onGesture(Shake,shakeEvent);
  Sensors.onGesture(LogoUp,logoUpEvent);
  Sensors.onGesture(LogoDown,logoDownEvent);
  Sensors.onGesture(ScreenUp,screenUpEvent);
  Sensors.onGesture(ScreenDown,screenDownEvent);
  Sensors.onGesture(TiltLeft,tiltLeftEvent);
  Sensors.onGesture(TiltRight,tiltRightEvent);
  Sensors.onGesture(FreeFall,freeFallEvent);
  Sensors.onGesture(ThreeG,threeGEvent);
  Sensors.onGesture(SixG,sixGEvent);
  Sensors.onGesture(EightG,eightGEvent);
}

void loop() {
  // put your main code here, to run repeatedly:
 // Serial.println(Sensors.compassHeading());
  delay(50);
  Sensors.compassHeading();

}