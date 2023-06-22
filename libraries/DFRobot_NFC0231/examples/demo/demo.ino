#include <DFrobot_nfc.h>

DFRobot_PN532_IIC  nfc;

void setup() {
  Serial.begin(115200);
  nfc.begin();
}
void loop() {

    if(nfc.scan()){

    nfc.writeData (1,1,6);                 //特定位置写入数据
    Serial.print("打印之前写入的数据:");
    Serial.println(nfc.readData(1));
    Serial.println(nfc.readData(1,1));
    Serial.println(nfc.readUid());
    
   if(nfc.scan("fb7c6083"))
   { Serial.println("card 1");
     Serial.print("数据块10所有数据:");
     Serial.println( nfc.readData(10));
     Serial.print("卡片id:");
     Serial.println(nfc.readUid());
     Serial.println("");
   }

    }
    delay(10000);
    }

/*
void setup() {
	nfc.begin(&Serial1);
	Serial.begin(9600);
}

void loop() {
	nfc.writeData(1, 1, 200);
	Serial.println(nfc.readUid());
	Serial.println(nfc.readData(2));
	if (nfc.scan()) {
		if (nfc.scan("4978ef9c")) {
		}
	}
}
*/