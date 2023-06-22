#ifndef _MICROBIT_MICROPHONE_H_
#define _MICROBIT_MICROPHONE_H_

#include <Arduino.h>

class Microbit_Microphone
{
    public:

    Microbit_Microphone(); 
    ~Microbit_Microphone();
    //private:
    NRF52ADCChannel* microphone = NULL;
    LevelDetectorSPL* level = NULL;
	void init();
	int getSoundLevel();
        
};
extern Microbit_Microphone microphone;
#endif

