


#include <DFRobot_ADS1115.h>

DFRobot_ADS1115 ads;

void setup(void) 
{
    Serial.begin(115200);

  
}

void loop(void) 
{
    ads.begin(0x48);   // 0x48
    
  
  
        Serial.print("ADC1 A0:");
        Serial.print(ads.readVoltage(0));
        Serial.print("mV,  ");
       
        Serial.print("ADC1 A1:");
        Serial.print(ads.readVoltage(1));
        Serial.print("mV,  ");
       
        Serial.print("ADC1 A2:");
        Serial.print(ads.readVoltage(2));
        Serial.print("mV,  ");
        
        Serial.print("ADC1 A3:");
        Serial.print(ads.readVoltage(3));
        Serial.println("mV");

    delay(1000);
    ads.begin(0x49);   // 0x49
  
        int16_t adc0, adc1, adc2, adc3;
     
        Serial.print("ADC2 A0:");
        Serial.print(ads.readVoltage(0));
        Serial.print("mV,  ");
  
        Serial.print("ADC2 A1:");
        Serial.print(ads.readVoltage(1));
        Serial.print("mV,  ");

        Serial.print("ADC2 A2:");
        Serial.print(ads.readVoltage(2));
        Serial.print("mV,  ");
 
        Serial.print("ADC2 A3:");
        Serial.print(ads.readVoltage(3));
        Serial.println("mV");
  
    delay(1000);
}
