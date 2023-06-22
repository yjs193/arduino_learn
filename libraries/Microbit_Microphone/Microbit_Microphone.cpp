#include <Microbit_Microphone.h>
#include <basic.h>

Microbit_Microphone microphone;
Microbit_Microphone::Microbit_Microphone(){
    
}

void Microbit_Microphone::init(){
    if(microphone == NULL){
        microphone = uBit.adc.getChannel(uBit.io.microphone);
        level = new LevelDetectorSPL((new StreamNormalizer(microphone->output, 1.0f, true, DATASTREAM_FORMAT_UNKNOWN, 10))->output, 75.0, 60.0, 9, 52, DEVICE_ID_MICROPHONE);
        uBit.io.runmic.setDigitalValue(1);
        uBit.io.runmic.setHighDrive(true); 
        microphone->setGain(7,0);
    }
}

int Microbit_Microphone::getSoundLevel(){
    int soundlevel = level->getValue();
    int scaled = max(52,min(soundlevel,120))-52;
    int soundlevelSensor = min(0xff,scaled*0xff/(120-52));
    return soundlevelSensor;
}

Microbit_Microphone::~Microbit_Microphone(){
    
}