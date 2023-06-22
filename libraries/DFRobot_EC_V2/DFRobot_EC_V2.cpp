/*
 * file DFRobot_EC_V2.cpp
 * @ https://github.com/DFRobot/DFRobot_EC_V2
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

#include "DFRobot_EC_V2.h"

#if defined(__AVR__)
#include <EEPROM.h>
#define EEPROM_write(address, p) {int i = 0; byte *pp = (byte*)&(p);for(; i < sizeof(p); i++) EEPROM.write(address+i, pp[i]);}
#define EEPROM_read(address, p)  {int i = 0; byte *pp = (byte*)&(p);for(; i < sizeof(p); i++) pp[i]=EEPROM.read(address+i);}
#endif

#if defined(NRF5) || defined(NRF52833)
#include "basic.h"
#endif

#define KVALUEADDR 0x0A    //the start address of the K value stored in the EEPROM
#define RES2 820.0
#define ECREF 200.0

DFRobot_EC_V2::DFRobot_EC_V2()
{
    this->_ecvalue = 0.0;
    this->_kvalue = 1.0;
    this->_kvalueLow = 1.0;
    this->_kvalueHigh = 1.0;
    this->_cmdReceivedBufferIndex = 0;
    this->_voltage = 0.0;
    this->_temperature = 25.0;
}

DFRobot_EC_V2::~DFRobot_EC_V2()
{

}

void DFRobot_EC_V2::begin()
{
	#if defined(__AVR__)
    EEPROM_read(KVALUEADDR, this->_kvalueLow);  //read the calibrated K value from EEPROM
    if(EEPROM.read(KVALUEADDR)==0xFF && EEPROM.read(KVALUEADDR+1)==0xFF && EEPROM.read(KVALUEADDR+2)==0xFF && EEPROM.read(KVALUEADDR+3)==0xFF)
    {
      this->_kvalueLow = 1.0;   // For new EEPROM, write default value( K = 1.0) to EEPROM
      EEPROM_write(KVALUEADDR, this->_kvalueLow);
    }else{
      this->_kvalueLow = 1.0;
	}
    
    EEPROM_read(KVALUEADDR+4, this->_kvalueHigh);  //read the calibrated K value from EEPRM
    if(EEPROM.read(KVALUEADDR+4)==0xFF && EEPROM.read(KVALUEADDR+5)==0xFF && EEPROM.read(KVALUEADDR+6)==0xFF && EEPROM.read(KVALUEADDR+7)==0xFF)
    {
      this->_kvalueHigh = 1.0;  // For new EEPROM, write default value( K = 1.0) to EEPROM
      EEPROM_write(KVALUEADDR+4, this->_kvalueHigh);
    }else{
      this->_kvalueHigh = 1.0; 
	}
    
    
	#endif
	#if defined(NRF5) || defined(NRF52833)
	KeyValuePair *kvalueLow = uBit.storage.get("kvalueLow");
	if(kvalueLow != NULL){
		memcpy(&this->_kvalueLow, kvalueLow->value, sizeof(float));
        delete kvalueLow;
	}else{
		this->_kvalueLow = 1.0;  // new EEPROM, write typical voltage
		uBit.storage.put("kvalueLow",(uint8_t *)&(this->_kvalueLow),sizeof(float));
	}
	uBit.serial.printf("_kvalueLow:%d\n",this->_kvalueLow);
	
	KeyValuePair *kvalueHigh = uBit.storage.get("kvalueHigh");
	if(kvalueHigh != NULL){
		memcpy(&this->_kvalueHigh, kvalueHigh->value, sizeof(float));
        delete kvalueHigh;
	}else{
		this->_kvalueHigh = 1.0;  // new EEPROM, write typical voltage
		uBit.storage.put("kvalueHigh",(uint8_t *)&(this->_kvalueHigh),sizeof(float));
	}
	uBit.serial.printf("_kvalueHigh:%d\n",this->_kvalueHigh);
	#endif
	this->_kvalue =  this->_kvalueLow;  // set default K value: K = kvalueLow  
}

float DFRobot_EC_V2::readEC(int pin, float temperature)
{
    float value = 0,valueTemp = 0;
#if defined(NRF5) || defined(NRF52833)
	float voltage = analogRead(pin)/1024.0*3300;  // read the voltage
#else
  float voltage = analogRead(pin)/1024.0*5000;  // read the voltage
#endif
    this->_rawEC = 1000*voltage/RES2/ECREF;
    valueTemp = this->_rawEC * this->_kvalue;
    //automatic shift process
    //First Range:(0,2); Second Range:(2,20)
	
	//Serial.print("==");
	//Serial.println(valueTemp);
	//Serial.println(this->_kvalue);
	//Serial.println(voltage);
    if (valueTemp > 2.5)
    {
      this->_kvalue = this->_kvalueHigh;
    }
    else if(valueTemp < 2.0)
    {
      this->_kvalue = this->_kvalueLow;
    }
    this->_voltage = voltage;
    this->_temperature = temperature;
    value = this->_rawEC * this->_kvalue;  //calculate the EC value after automatic shift
    value = value / (1.0+0.0185*(temperature-25.0));  //temperature compensation
    this->_ecvalue = value;  //store the EC value for Serial CMD calibration
    return value;
}


void DFRobot_EC_V2::calibration()
{      
    if(cmdSerialDataAvailable() > 0)
    {
        ecCalibration(cmdParse());  // if received Serial CMD from the serial monitor, enter into the calibration mode
    }
}

boolean DFRobot_EC_V2::cmdSerialDataAvailable()
{
    char cmdReceivedChar;
    static unsigned long cmdReceivedTimeOut = millis();
    while (
	#if defined(NRF5) || defined(NRF52833)
	uBit.serial.rxBufferedSize()>0
	#else
	Serial.available()>0
	#endif
	) 
    {   
      if (millis() - cmdReceivedTimeOut > 500U) 
      {
        this->_cmdReceivedBufferIndex = 0;
        memset(this->_cmdReceivedBuffer,0,(ReceivedBufferLength));
      }
      cmdReceivedTimeOut = millis();
      #if defined(NRF5) || defined(NRF52833)
	  cmdReceivedChar = uBit.serial.read(MicroBitSerialMode::ASYNC);
	  #else
	  cmdReceivedChar = Serial.read();
	  #endif
      if (cmdReceivedChar == '\n' || this->_cmdReceivedBufferIndex==ReceivedBufferLength-1){
      this->_cmdReceivedBufferIndex = 0;
      strupr(this->_cmdReceivedBuffer);
      return true;
      }else{
        this->_cmdReceivedBuffer[this->_cmdReceivedBufferIndex] = cmdReceivedChar;
        this->_cmdReceivedBufferIndex++;
      }
    }
    return false;
}



byte DFRobot_EC_V2::cmdParse()
{
  byte modeIndex = 0;
  if(strstr(this->_cmdReceivedBuffer, "ENTER") != NULL) 
      modeIndex = 1;
  else if(strstr(this->_cmdReceivedBuffer, "EXIT") != NULL) 
      modeIndex = 3;
  else if(strstr(this->_cmdReceivedBuffer, "CAL") != NULL)
      modeIndex = 2;
  return modeIndex;
}

void DFRobot_EC_V2::ecCalibration(byte mode)
{
    char *receivedBufferPtr;
    static boolean ecCalibrationFinish = 0;
    static boolean enterCalibrationFlag = 0;
    static float compECsolution;
    float KValueTemp;
    switch(mode)
    {
      case 0:
      if(enterCalibrationFlag){
		#if defined(NRF5) || defined(NRF52833)
		uBit.serial.printf(">>>Command Error<<<\n");
		#else
		Serial.println(F(">>>Command Error<<<"));
		#endif  
	  }
      break;
      
      case 1:
      enterCalibrationFlag = 1;
      ecCalibrationFinish = 0;
      
	  #if defined(NRF5) || defined(NRF52833)
	  uBit.serial.printf("\n>>>Enter Calibration Mode<<<\n");
	  uBit.serial.printf(">>>Please put the probe into the 1413us/cm or 12.88ms/cm buffer solution<<<\n\n");
	  #else
	  Serial.println();
      Serial.println(F(">>>Enter Calibration Mode<<<"));
      Serial.println(F(">>>Please put the probe into the 1413us/cm or 12.88ms/cm buffer solution<<<"));
      Serial.println();
	  #endif
      break;
     
      case 2:
      if(enterCalibrationFlag)
      {
          if((this->_rawEC>0.9)&&(this->_rawEC<1.9))    //recognize 1.413us/cm buffer solution
            compECsolution = 1.413*(1.0+0.0185*(this->_temperature-25.0));  //temperature compensation
          else if((this->_rawEC>9)&&(this->_rawEC<16.8))  //recognize 12.88ms/cm buffer solution
            compECsolution = 12.88*(1.0+0.0185*(this->_temperature-25.0));  //temperature compensation
          else{
            #if defined(NRF5) || defined(NRF52833)
			uBit.serial.printf(">>>Buffer Solution Error<<<   ");
			#else
			Serial.print(F(">>>Buffer Solution Error<<<   "));
			#endif
			ecCalibrationFinish = 0;
          }
          KValueTemp = RES2*ECREF*compECsolution/1000.0/this->_voltage;  //calibrate the k value
          if((KValueTemp>0.5) && (KValueTemp<1.5))
          {
              
              #if defined(NRF5) || defined(NRF52833)
			  uBit.serial.printf("\n>>>Successful,K:%d, Send EXIT to Save and Exit<<<\n",KValueTemp);
			  #else
			  Serial.println();
              Serial.print(F(">>>Successful,K:"));
              Serial.print(KValueTemp);
              Serial.println(F(", Send EXIT to Save and Exit<<<"));
			  #endif
			  if((this->_rawEC>0.9)&&(this->_rawEC<1.9))
                this->_kvalueLow =  KValueTemp;
              else if((this->_rawEC>9)&&(this->_rawEC<16.8))
                this->_kvalueHigh =  KValueTemp;
              ecCalibrationFinish = 1;
          }
          else{
            
			#if defined(NRF5) || defined(NRF52833)
			uBit.serial.printf("\n>>>Failed,Try Again<<<\n");
			#else
			Serial.println();
            Serial.println(F(">>>Failed,Try Again<<<"));
            Serial.println();
			#endif
            ecCalibrationFinish = 0;
          }        
      }
      break;

        case 3:
        if(enterCalibrationFlag)
        {
            #if defined(NRF5) || defined(NRF52833)
			uBit.serial.printf("\n");
			#else
			Serial.println(); 
			#endif
            if(ecCalibrationFinish)
            {   
              if((this->_rawEC>0.9)&&(this->_rawEC<1.9))
              {
				 #if defined(__AVR__)
				 EEPROM_write(KVALUEADDR, this->_kvalueLow);
				 #endif
				 #if defined(NRF5) || defined(NRF52833)
				 uBit.storage.put("kvalueLow",(uint8_t *)&(this->_kvalueLow),sizeof(float));
				 #endif
              }
              else if((this->_rawEC>9)&&(this->_rawEC<16.8))
              {              
                 
				 #if defined(__AVR__)
				 EEPROM_write(KVALUEADDR+4, this->_kvalueHigh);
				 #endif
				 #if defined(NRF5) || defined(NRF52833)
         Serial.println("2222222222222222222");
         Serial.println(this->_kvalueHigh);
				 uBit.storage.put("kvalueHigh",(uint8_t *)&(this->_kvalueHigh),sizeof(float));
				 #endif
              }
              
			  #if defined(NRF5) || defined(NRF52833)
			  uBit.serial.printf(">>>Calibration Successful");
			  #else
			  Serial.print(F(">>>Calibration Successful"));
			  #endif
            }
            else {
				#if defined(NRF5) || defined(NRF52833)
				uBit.serial.printf(">>>Calibration Failed\n");
				#else
				Serial.print(F(">>>Calibration Failed")); 
				#endif
			}      
            #if defined(NRF5) || defined(NRF52833)
			uBit.serial.printf(",Exit Calibration Mode<<<\n\n");
			#else
			Serial.println(F(",Exit Calibration Mode<<<"));
            Serial.println(); 
			#endif
            ecCalibrationFinish = 0;
            enterCalibrationFlag = 0;
        }
        break;
    }
}
