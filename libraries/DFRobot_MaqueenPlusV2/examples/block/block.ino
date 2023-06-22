#include <DFRobot_MaqueenPlus.h>

DFRobot_MaqueenPlus maqueen;
IRremote_Receive remoteReceive;
void onIRReceive(uint8_t data);

void setup() {
  Serial.begin(9600);
  maqueen.MotorRun(LEFT,CW,255);
  maqueen.MotorRun(RIGHT,CCW,100);
  //maqueen.MotorStop(LEFT);
  maqueen.MostotCompensation(eLEFT, 100);
  Serial.println(maqueen.ReadSpeed(eLEFT));
  Serial.println(maqueen.ReadDirection(eLEFT));
  Serial.println(maqueen.ReadPatrol(L1));
  Serial.println(maqueen.ReadVersion());
  remoteReceive.begin(16);
  remoteReceive.setCallback(onIRReceive);
}

void loop() {
  maqueen.ServoRun(S1,  0 );
  maqueen.SetRGBLight(RGBL, RED);
  Serial.println(maqueen.ReadPatrolVoltage(L1));
  Serial.println(maqueen.getDistanceCM(1, 2));
}

void onIRReceive(uint8_t data) {
  Serial.println(data);
}