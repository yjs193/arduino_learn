#include "DFRobot_DO.h"



float SaturationDoTemperature = 25.0;   //default temperature is 25^C
double SaturationDoVoltage = 1127.6;   //default voltage:1127.6mv

const float SaturationValueTab[41] = {      //saturation dissolved oxygen concentrations at various temperatures
14.46, 14.22, 13.82, 13.44, 13.09,
12.74, 12.42, 12.11, 11.81, 11.53,
11.26, 11.01, 10.77, 10.53, 10.30,
10.08, 9.86,  9.66,  9.46,  9.27,
9.08,  8.90,  8.73,  8.57,  8.41,
8.25,  8.11,  7.96,  7.82,  7.69,
7.56,  7.43,  7.30,  7.18,  7.07,
6.95,  6.84,  6.73,  6.63,  6.53,
6.41,
};


DFRobot_DO::DFRobot_DO()
{
  
}


int getMedianNum(int bArray[], int iFilterLen) 
{
      int bTab[iFilterLen];
      for (byte i = 0; i<iFilterLen; i++)
      {
	  bTab[i] = bArray[i];
      }
      int i, j, bTemp;
      for (j = 0; j < iFilterLen - 1; j++) 
      {
	  for (i = 0; i < iFilterLen - j - 1; i++) 
          {
	    if (bTab[i] > bTab[i + 1]) 
            {
		bTemp = bTab[i];
	        bTab[i] = bTab[i + 1];
		bTab[i + 1] = bTemp;
	     }
	  }
      }
      if ((iFilterLen & 1) > 0)
	bTemp = bTab[(iFilterLen - 1) / 2];
      else
	bTemp = (bTab[iFilterLen / 2] + bTab[iFilterLen / 2 - 1]) / 2;
      return bTemp;
}

float DFRobot_DO::readDO(int pin,float temperature)
{
	int val[ArrayLenth] = {0};
	float doValue;
	for(int i=0;i<ArrayLenth;i++){
		val[i] = analogRead(pin);
		delay(20);
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

	float averageVoltage = getMedianNum(val,ArrayLenth) * (float)VREF / 1024.0; // read the value more stable by the median filtering algorithm
	doValue =SaturationValueTab[0+ (int)(temperature+0.5)]* averageVoltage / SaturationDoVoltage;  //calculate the do value, doValue = Voltage / SaturationDoVoltage * SaturationDoValue(with temperature compensation)
	
	return doValue;
}
