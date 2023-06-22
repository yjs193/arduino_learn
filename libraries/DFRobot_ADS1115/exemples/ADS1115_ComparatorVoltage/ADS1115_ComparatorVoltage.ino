

#include <Wire.h>
#include <DFRobot_ADS1115.h>

DFRobot_ADS1115 ads;

void setup(void)
{
    Serial.begin(115200);
    ads.begin(0x48);   // 0x48
  
    delay(1000);
}

void loop(void)
{
   
        Serial.println("Get the voltage difference between two channels");
        Serial.println(" ");
     
        Serial.print("The voltage difference between A0 and A1 ");
        Serial.print(ads.ComparatorVoltage(1));
        Serial.println("mV");
   
        Serial.print("The voltage difference between A0 and A3 ");
        Serial.print(ads.ComparatorVoltage(2));
        Serial.println("mV");
     
        Serial.print("The voltage difference between A1 and A3 ");
        Serial.print(ads.ComparatorVoltage(3));
        Serial.println("mV");
    
        Serial.print("The voltage difference between A2 and A3 ");
        Serial.print(ads.ComparatorVoltage(4));
        Serial.println("mV ");
    
    delay(1000);
}
