/*
 * file DFRobot_EC.cpp
 * @ https://github.com/DFRobot/DFRobot_EC
 *
 * Arduino library for Gravity: Analog Electrical Conductivity Sensor / Meter Kit V2 (K=1), SKU: DFR0300
 *
 * Copyright   [DFRobot](http://www.dfrobot.com), 2018
 * Copyright   GNU Lesser General Public License
 *
 * version  V1.01
 * date  2018-06
 */


#if ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include "DFRobot_EC.h"

void bubble(int *a,uint8_t n){
	int i,j,temp;
	for(i = 0; i<n-1;i++){
		for(j=i+1;j<n;j++){
			if(a[i]>a[j]){
				temp = a[i];
				a[i] = a[j];
				a[j] = temp;
			}
		}
	}
} 
DFRobot_EC::DFRobot_EC()
{

}

DFRobot_EC::~DFRobot_EC()
{
	
}
float DFRobot_EC::readEC(uint8_t EC_PIN, float temperature)
{
	float value = 0;
	while(index < 20){
		static unsigned long timepoint = millis();
		while(millis()-timepoint<delaytime){
			delay(5);
		}
		timepoint = millis();
		readings[index] = analogRead(EC_PIN);
		index++;
		if(index > numReadings-1){
			index = 0;
			bubble(readings,numReadings);
			float voltage = (float)readings[numReadings/2]/1024*5000;
			float TempCoefficient=1.0+0.0185*(temperature-25.0);
			float CoefficientVolatge = (float)voltage/TempCoefficient;
			if(CoefficientVolatge<=448){
				value = 6.84 * CoefficientVolatge - 64.32;
			}else if(CoefficientVolatge<=1457){
				value = 6.98 * CoefficientVolatge - 127;
			}else{
				value = 5.3 * CoefficientVolatge + 2278;
			}
			value/=1000;
			break;
		}
	}
	if(value < 0){
		value = 0;
	}
	if(value > 20){
		value = 20;
	}
    return value;
}
