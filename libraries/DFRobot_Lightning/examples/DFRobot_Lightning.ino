#include "DFRobot_AS3935_I2C.h"
#include "Lib_I2C.h"

DFRobot_AS3935_I2C lightning;

void setup() {
  Serial.begin(9600);
  lightning.init(2,0x03);
  while (!lightning.IICInit()) {
  }
  lightning.manualCal(96,0,1);
}

void loop() {
  int a = lightning.getInterruptSrc();
  if(a==0){
	  Serial.println("没有新闪电");
  }else if(a==1){
	  Serial.println("发现闪电");
	  Serial.println(lightning.getLightningDistKm());
      Serial.println(lightning.getStrikeEnergyRaw());
  }else if(a==2){
	  Serial.println("发现干扰");
  }else if(a==3){
	  Serial.println("噪音太大");
  }else{
	  Serial.println("未知结果");
  }
  
  delay(1000);
}
