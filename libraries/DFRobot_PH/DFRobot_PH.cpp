#include "DFRobot_PH.h"
#if ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

//#define PHVALUEADDR 0x00    //the start address of the pH calibration parameters stored in the EEPROM


DFRobot_PH2::DFRobot_PH2()
{
    this->_temperature    = 25.0;
    this->_phValue        = 7.0;
    this->_acidVoltage    = 2032.44;    //buffer solution 4.0 at 25C
    this->_neutralVoltage = 1500.0;     //buffer solution 7.0 at 25C
    this->_voltage        = 1500.0;
}

DFRobot_PH2::~DFRobot_PH2()
{
}

float DFRobot_PH2::readPH(uint32_t pin)
{
    float voltage = analogRead(pin)/this->map*this->aref;
    float slope = (7.0-4.0)/((this->_neutralVoltage-1500.0)/3.0 - (this->_acidVoltage-1500.0)/3.0);
    float intercept =  7.0 - slope*(this->_neutralVoltage-1500.0)/3.0;
    this->_phValue = slope*(voltage-1500.0)/3.0+intercept;
    return _phValue;
}

void DFRobot_PH2::calibration(int32_t pin, float temperature)
{
    float voltage = analogRead(pin)/this->map*5000;
    this->_voltage = voltage;
    this->_temperature = temperature;
    if(cmdSerialDataAvailable() > 0){
        phCalibration(cmdParse());
    }
}

boolean DFRobot_PH2::cmdSerialDataAvailable()
{
    char cmdReceivedChar;
    static unsigned long cmdReceivedTimeOut = millis();
    while(Serial.available()>0){
        if(millis() - cmdReceivedTimeOut > 500U){
            this->_cmdReceivedBufferIndex = 0;
            memset(this->_cmdReceivedBuffer,0,(10));
        }
        cmdReceivedTimeOut = millis();
        cmdReceivedChar = Serial.read();
        if (cmdReceivedChar == '\n' || this->_cmdReceivedBufferIndex==9){
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

byte DFRobot_PH2::cmdParse()
{
    byte modeIndex = 0;
    if(strstr(this->_cmdReceivedBuffer, "ENTERPH")      != NULL){
        modeIndex = 1;
    }else if(strstr(this->_cmdReceivedBuffer, "EXITPH") != NULL){
        modeIndex = 3;
    }else if(strstr(this->_cmdReceivedBuffer, "CALPH")  != NULL){
        modeIndex = 2;
    }
    return modeIndex;
}

void DFRobot_PH2::phCalibration(byte mode)
{
    char *receivedBufferPtr;
    static boolean phCalibrationFinish  = 0;
    static boolean enterCalibrationFlag = 0;
    switch(mode){
        case 0:
        if(enterCalibrationFlag){
            Serial.println(F(">>>Command Error<<<"));
        }
        break;

        case 1:
        enterCalibrationFlag = 1;
        phCalibrationFinish  = 0;
        Serial.println();
        Serial.println(F(">>>Enter PH Calibration Mode<<<"));
        Serial.println(F(">>>Please put the probe into the 4.0 or 7.0 standard buffer solution<<<"));
        Serial.println();
        break;

        case 2:
        if(enterCalibrationFlag){
            if((this->_voltage>1322)&&(this->_voltage<1678)){        // buffer solution:7.0{
                Serial.println();
                Serial.print(F(">>>Buffer Solution:7.0"));
                this->_neutralVoltage =  this->_voltage;
                Serial.println(F(",Send EXITPH to Save and Exit<<<"));
                Serial.println();
                phCalibrationFinish = 1;
            }else if((this->_voltage>1854)&&(this->_voltage<2210)){  //buffer solution:4.0
                Serial.println();
                Serial.print(F(">>>Buffer Solution:4.0"));
                this->_acidVoltage =  this->_voltage;
                Serial.println(F(",Send EXITPH to Save and Exit<<<")); 
                Serial.println();
                phCalibrationFinish = 1;
            }else{
                Serial.println();
                Serial.print(F(">>>Buffer Solution Error Try Again<<<"));
                Serial.println();                                    // not buffer solution or faulty operation
                phCalibrationFinish = 0;
            }
        }
        break;

        case 3:
        if(enterCalibrationFlag){
            Serial.println();
            if(phCalibrationFinish){
                if((this->_voltage>1322)&&(this->_voltage<1678)){
                    //EEPROM_write(PHVALUEADDR, this->_neutralVoltage);
                }else if((this->_voltage>1854)&&(this->_voltage<2210)){
                    //EEPROM_write(PHVALUEADDR+4, this->_acidVoltage);
                }
                Serial.print(F(">>>Calibration Successful"));
            }else{
                Serial.print(F(">>>Calibration Failed"));
            }
            Serial.println(F(",Exit PH Calibration Mode<<<"));
            Serial.println();
            phCalibrationFinish  = 0;
            enterCalibrationFlag = 0;
        }
        break;
    }
}

DFRobot_PH::DFRobot_PH()
{
    this->pHArrayIndex = 0;
    this->enable = false;
    this->valuePrev = 0;
    this->first = true;
    pHArray = (int *)malloc(sizeof(int)*20);
    if(pHArray) this->enable = true;
}

DFRobot_PH::~DFRobot_PH()
{
    this->pHArrayIndex = 0;
    this->valuePrev = 0;
    this->first = true;
    if(pHArray) free(pHArray);
    this->enable = false;
}

float DFRobot_PH::readPH(uint32_t pin)
{
    if(!this->pHArray || !this->enable) 
        return 0;
    int count = 1;
    if(this->first){
        count = 20;
        this->first = false;
    }
    for(int i= 0; i<count; i++){
        pHArray[this->pHArrayIndex] = analogRead(pin);
        this->pHArrayIndex = (this->pHArrayIndex + 1) % 20;
        delay(20);
    }
    float voltage = avergeArray() * 5.0 / map;
    return 3.5 * voltage + 0.0; // 0.0: offset
}

float DFRobot_PH::avergeArray()
{
    if(!this->pHArray || !this->enable) 
        return 0;
    int i;
    int max,min;
    double avg;
    long amount=0;
    if(pHArray[0]<pHArray[1]){
        min = pHArray[0];max=pHArray[1];
    }
    else{
        min=pHArray[1];max=pHArray[0];
    }
    for(i=2;i<20;i++){
        if(pHArray[i]<min){
            amount+=min;        //pHArray<min
            min=pHArray[i];
        }else {
            if(pHArray[i]>max){
                amount+=max;    //pHArray>max
                max=pHArray[i];
            }else{
                amount+=pHArray[i]; //min<=pHArray<=max
            }
        }//if
    }//for
    avg = (double)amount/(20-2);
    return avg;
}

