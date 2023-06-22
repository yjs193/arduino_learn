#include <Wire.h>
#include "DFRobot_RGBLCD.h"
#include <dht.h>
dht DHT;
#define DHT22_PIN 7

DFRobot_RGBLCD lcd(16,2);  //16 characters and 2 lines of show

void setup() {
    // initialize
    lcd.init();
     lcd.setRGB(255, 0, 0);
}

void loop() {
int chk = DHT.read22(DHT22_PIN);  //读取数据  
    lcd.print("wendu:");
    lcd.print(DHT.temperature, 1);
    delay(2000);
    //lcd.clear();
    lcd.home();
    lcd.print("shidu:");
    lcd.print(DHT.humidity,1);
    lcd.home();
    delay(2000);
}
