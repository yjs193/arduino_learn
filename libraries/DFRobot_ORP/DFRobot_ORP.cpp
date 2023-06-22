#include "DFRobot_ORP.h"

DFRobot_ORP::DFRobot_ORP()
{
  
}

int DFRobot_ORP::readORP(int pin)
{
	int val[ArrayLenth] = {0};
	int orpValue;
	for(int i=0;i<ArrayLenth;i++){
		val[i] = analogRead(pin);
		delay(10);
	}
	for(int i=0;i<ArrayLenth-1;i++){
		int temp=0;
		for(int j=i+1;j<ArrayLenth;j++){
			if(val[i] > val[j]){
				temp = val[i];
				val[i] = val[j];
				val[j] = temp;
			}
		}
	}
	orpValue=((30*(double)VOLTAGE*1000)-(75*val[((ArrayLenth+1)/2)]*VOLTAGE*1000/1024))/75-OFFSET;   //convert the analog value to orp according the circuit
	return (int)orpValue;
}
