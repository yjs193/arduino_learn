#include <DFRobot_MicroNatural.h>

MicroNatural_UV    mn_uv;
MicroNatural_RGBC  mn_rgbc;
MicroNatural_LED   mn_led;
MicroNatural_Sound mn_sound;
MicroNatural_TDS   mn_tds;
bool k=true;

void setup() {
  Serial.begin(9600);
}

void loop() {
  Serial.println("==============tds==============");
  Serial.println("set k: 2.68"); mn_tds.setK(2.68);
  Serial.print("read k: "); Serial.println(mn_tds.readK());
  Serial.print("read ppm: ");Serial.println(mn_tds.read());
  Serial.println("=============rgbc==============");
  Serial.print("red: "); Serial.println(mn_rgbc.red());
  Serial.print("green: "); Serial.println(mn_rgbc.green());
  Serial.print("blue: "); Serial.println(mn_rgbc.blue());
  Serial.print("naturalLight: "); Serial.println(mn_rgbc.naturalLight());
  Serial.println("=============sound=============");
  Serial.print("read sound: "); Serial.println(mn_sound.read());
  Serial.println("==============led==============");
  if(k) {Serial.println("led on"); mn_led.set(true);}
  else  {Serial.println("led off"); mn_led.set(false);}
  Serial.println("==============UV===============");
  Serial.print("read uv: "); Serial.println(mn_uv.read());
  Serial.println("\r\n\r\n");
  k = !k;
  delay(500);
}