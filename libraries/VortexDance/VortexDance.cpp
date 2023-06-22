// Library header
#include "VortexDance.h"


uint32_t danceAllTimer = 0;
uint32_t danceTimer = 0;
uint32_t danceAllDuration = 500;
uint32_t danceDuration = 500;
int danceState = DanceStateNone;
unsigned long timer20ms;


void danceinit(){
    motor.pinENL = PINENL;
    motor.pinENR = PINENR;
    motor.pinDirL = PINDIRL;
    motor.pinDirR = PINDIRR;
    motor.Init();

    LED.Init();
    LED.faceNumber = FACE_NUMBER;

    player.init();
    player.setVolume(MP3_VOLUME);
}

void dancestart(uint8_t role)
{
    noInterrupts();
    danceAllTimer = millis();
    danceTimer = millis();
    
    switch (role) {
        case 0:
            player.playDanceMusic(1);
            danceDuration = 522;
            danceState = DanceStateFirst1;
            danceAllDuration = 33000UL;
            break;
        case 1:
            player.playDanceMusic(7);
            danceDuration = 365;
            danceState = DanceStateSecond1;
            danceAllDuration = 17500UL;
            break;
        case 2:
            player.playDanceMusic(13);
            danceDuration = 729;
            danceState = DanceStateThird1;
            danceAllDuration = 35000UL;
            break;
        case 3:
            player.playDanceMusic(19);
            danceDuration = 356;
            danceState = DanceStateForth1;
            danceAllDuration = 23000UL;
            break;
        default:
            break;
    }
    interrupts();
}

void dancestop()
{
    player.stop();
    danceState = DanceStateNone;
    motor.AccCount(0x00,0x00,0);
    LED.skirtCount(0xff,0x00, 0x70, 0x80, 100);
    LED.topCount(0xf7,0x00, 0x70, 0x80, 100);
    LED.topCount(0x08,0xff, 0x00, 0x00, 100);
}


void danceupdate()
{
    if (danceState != DanceStateNone) {
        
        if (millis() - danceAllTimer > danceAllDuration) {
            motor.AccCount(0x00,0x00,0);
            return;
        }
        if (millis() + danceDuration - danceTimer > danceDuration) {
            
            if (danceState != DanceStateSecond2) {
                LED.skirtCount(0xff,0x00, 0x70, 0x80, 0);
                LED.topCount(0xf7,0x00, 0x70, 0x80, 0);
                LED.topCount(0x08,0xff, 0x00, 0x00, 0);
                
                LED.skirtCount(0xff,0x00, 0x00, 0x00, danceDuration/20);
                LED.topCount(0xff,0x00, 0x00, 0x00, danceDuration/20);
            }
            
            danceTimer += danceDuration;
            switch (danceState) {
                case DanceStateFirst1:
                    motor.AccCount(0x00,0xcf,0);
                    danceState++;
                    break;
                case DanceStateFirst2:
                    motor.AccCount(0xcf,0x00,0);
                    danceState++;
                    break;
                case DanceStateFirst3:
                    motor.AccCount(0x4f,0x00,0);
                    danceState++;
                    break;
                case DanceStateFirst4:
                    motor.AccCount(0x00,0x4f,0);
                    danceState++;
                    break;
                case DanceStateFirst5:
                    motor.AccCount(0x00,0xcf,0);
                    danceState = DanceStateFirst1;
                    break;
                case DanceStateSecond1:
                    motor.AccCount(0xcf,0x4f,0);
                    motor.AccCount(0x8f,0x0f,150);
                    danceState = DanceStateSecond2;
                    
                    LED.skirtCount(0xff,0x00, 0x70, 0x80, 0);
                    LED.topCount(0xf7,0x00, 0x70, 0x80, 0);
                    LED.topCount(0x08,0xff, 0x00, 0x00, 0);

                    LED.skirtCount(0xff,0x00, 0x00, 0x00, 4000/20);
                    LED.topCount(0xff,0x00, 0x00, 0x00, 4000/20);
                    break;
                case DanceStateSecond2:
                    if (millis()- danceAllTimer>4000) {
                        danceState = DanceStateSecond3;
                    }
                    break;
                case DanceStateSecond3:
                    motor.AccCount(0x00, 0xff, 0);
                    motor.AccCount(0x00, 0x8f, 18);
                    danceState++;
                    break;
                case DanceStateSecond4:
                    motor.AccCount(0xff, 0x00, 0);
                    motor.AccCount(0x8f, 0x00, 18);
                    danceState++;
                    break;
                case DanceStateSecond5:
                    motor.AccCount(0x00,0x7f,0);
                    motor.AccCount(0x00,0x0f,18);
                    danceState++;
                    break;
                case DanceStateSecond6:
                    motor.AccCount(0xff, 0x00,0);
                    motor.AccCount(0x8f, 0x00,18);
                    danceState=DanceStateSecond3;
                    break;
                case DanceStateThird1:
                    motor.AccCount(0x00,0xff, 0);
                    danceState++;
                    break;
                case DanceStateThird2:
                    motor.AccCount(0xff,0x00, 0);
                    danceState++;
                    break;
                case DanceStateThird3:
                    motor.AccCount(0x00,0x7f,0);
                    danceState++;
                    break;
                case DanceStateThird4:
                    motor.AccCount(0x7f,0x00,0);
                    danceState++;
                    break;
                case DanceStateThird5:
                    motor.AccCount(0x7f,0x00, 0);
                    danceState++;
                    break;
                case DanceStateThird6:
                    motor.AccCount(0x00,0x7f, 0);
                    danceState++;
                    break;
                case DanceStateThird7:
                    motor.AccCount(0xff,0x00, 0);
                    danceState++;
                    break;
                case DanceStateThird8:
                    motor.AccCount(0x00,0xff, 0);
                    danceState=DanceStateThird1;
                    break;
                case DanceStateForth1:
                    motor.AccCount(0xff,0xff, 0);
                    motor.AccCount(0x8f,0x8f, 18);
                    danceState++;
                    break;
                case DanceStateForth2:
                    motor.AccCount(0x7f,0x7f, 0);
                    motor.AccCount(0x0f,0x0f, 18);
                    danceState++;
                    break;
                case DanceStateForth3:
                    motor.AccCount(0xff,0x7f, 0);
                    danceState = DanceStateForth1;
                    break;
                default:
                    motor.AccCount(0x00,0x00,0);
                    break;
            }
        }
        
    }

}
