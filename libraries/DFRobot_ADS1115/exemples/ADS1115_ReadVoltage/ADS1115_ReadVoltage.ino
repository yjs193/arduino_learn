
#include <DFRobot_ADS1115.h>

DFRobot_ADS1115 ads;

void setup(void) 
{
    Serial.begin(115200);
    ads.begin(0x48);   // 0x48
}

void loop(void) 
{
    
     
      
        Serial.print("A0:");
        Serial.print(ads.readVoltage(0));
        Serial.print("mV,  ");
     
        Serial.print("A1:");
        Serial.print(ads.readVoltage(1));
        Serial.print("mV,  ");
     
        Serial.print("A2:");
        Serial.print(ads.readVoltage(2));
        Serial.print("mV,  ");
     
        Serial.print("A3:");
        Serial.print(ads.readVoltage(3));
        Serial.println("mV");
  
    delay(1000);
}
