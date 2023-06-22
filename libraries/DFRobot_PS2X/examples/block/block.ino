#include "DFRobot_PS2X.h"

/*
possible key:
  PSB_SELECT
  PSB_L3
  PSB_R3
  PSB_START
  PSB_PAD_UP
  PSB_PAD_RIGHT
  PSB_PAD_DOWN
  PSB_PAD_LEFT
  PSB_L2
  PSB_R2
  PSB_L1
  PSB_R1
  PSB_GREEN
  PSB_RED
  PSB_BLUE
  PSB_PINK
  PSB_TRIANGLE
  PSB_CIRCLE
  PSB_CROSS
  PSB_SQUARE
*/

DFRobot_PS2X ps2x;

void setup(){
  Serial.begin(9600);
  #if ESP_PLATFORM
    ps2x.config_gamepad(P0,P15,P1,P8, true, true);                   //CLK  CMD  CS DATA
  #elif ARDUINO_BBC_MICROBIT
    ps2x.config_gamepad(0 , 15, 1, 8, true, true);
  #elif ARDUINO_AVR_UNO
    ps2x.config_gamepad(7 , 5 , 6, 4, true, true);
  #else
    ps2x.config_gamepad(7 , 5 , 6, 4, true, true);
  #endif
  delay(500);
  Serial.println("begin");
}

void loop(){
  ps2x.read_gamepad(false, 0);
   delay(100);
  if (ps2x.Button(PSB_GREEN)) {
    Serial.println("PSB_GREEN");
  }
  if (ps2x.Button(PSB_L1)) {
    Serial.println("PSB_L1");
  }
  if (ps2x.Button(PSB_PAD_UP)) {
    Serial.println("PSB_PAD_UP");
  }
  if (ps2x.Button(PSB_SELECT)) {
    Serial.println("PSB_SELECT");
  }
  if (ps2x.Analog(PSS_RX) < 120 || ps2x.Analog(PSS_RX) > 136) {
    Serial.println(ps2x.Analog(PSS_RX));
  }
  if (ps2x.Analog(PSS_RY) < 120 || ps2x.Analog(PSS_RY) > 136) {
    Serial.println(ps2x.Analog(PSS_RY));
  }
  if (ps2x.Analog(PSS_LX) < 120 || ps2x.Analog(PSS_LX) > 136) {
    Serial.println(ps2x.Analog(PSS_LX));
  }
  if (ps2x.Analog(PSS_LY) < 120 || ps2x.Analog(PSS_LY) > 136) {
    Serial.println(ps2x.Analog(PSS_LY));
  }
}
