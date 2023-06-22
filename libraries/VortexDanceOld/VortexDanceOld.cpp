// Library header
#include "VortexDanceOld.h"


uint32_t danceAllTimerOld = 0;
uint32_t danceTimerOld = 0;
uint32_t danceAllDurationOld = 500;
uint32_t danceDurationOld = 500;
int danceStateOld = DanceStateNone;
unsigned long timer20msOld;


void danceinitOld(){
    timer20msOld = millis();
    motor.pinENL = PINENL;
    motor.pinENR = PINENR;
    motor.pinDirL = PINDIRL;
    motor.pinDirR = PINDIRR;
    motor.Init();

    LED_OLD.Init();
    LED_OLD.faceNumber = FACE_NUMBER;

    player_old.init();
    player_old.setVolume(MP3_VOLUME);
}

void dancestartOld(uint8_t role)
{
    noInterrupts();
    danceAllTimerOld = millis();
    danceTimerOld = millis();
    
    switch (role) {
        case 0:
            player_old.play(1);
            danceDurationOld = 522;
            danceStateOld = DanceStateFirst1;
            danceAllDurationOld = 33000UL;
            break;
        case 1:
            player_old.play(7);
            danceDurationOld = 365;
            danceStateOld = DanceStateSecond1;
            danceAllDurationOld = 17500UL;
            break;
        case 2:
            player_old.play(13);
            danceDurationOld = 729;
            danceStateOld = DanceStateThird1;
            danceAllDurationOld = 35000UL;
            break;
        case 3:
            player_old.play(19);
            danceDurationOld = 356;
            danceStateOld = DanceStateForth1;
            danceAllDurationOld = 23000UL;
            break;
        default:
            break;
    }
    interrupts();
}

void dancestopOld()
{
    player_old.stop();
    danceStateOld = DanceStateNone;
    motor.AccCount(0x00,0x00,0);
    LED_OLD.skirtCount(0xff,0x00, 0x70, 0x80, 100);
    LED_OLD.topCount(0xf7,0x00, 0x70, 0x80, 100);
    LED_OLD.topCount(0x08,0xff, 0x00, 0x00, 100);
}


void danceupdateOld()
{
    if (danceStateOld != DanceStateNone) {
        
        if (millis() - danceAllTimerOld > danceAllDurationOld) {
            motor.AccCount(0x00,0x00,0);
            return;
        }
        if (millis() + danceDurationOld - danceTimerOld > danceDurationOld) {
            
            if (danceStateOld != DanceStateSecond2) {
                LED_OLD.skirtCount(0xff,0x00, 0x70, 0x80, 0);
                LED_OLD.topCount(0xf7,0x00, 0x70, 0x80, 0);
                LED_OLD.topCount(0x08,0xff, 0x00, 0x00, 0);
                
                LED_OLD.skirtCount(0xff,0x00, 0x00, 0x00, danceDurationOld/20);
                LED_OLD.topCount(0xff,0x00, 0x00, 0x00, danceDurationOld/20);
            }
            
            danceTimerOld += danceDurationOld;
            switch (danceStateOld) {
                case DanceStateFirst1:
                    motor.AccCount(0x00,0xcf,0);
                    danceStateOld++;
                    break;
                case DanceStateFirst2:
                    motor.AccCount(0xcf,0x00,0);
                    danceStateOld++;
                    break;
                case DanceStateFirst3:
                    motor.AccCount(0x4f,0x00,0);
                    danceStateOld++;
                    break;
                case DanceStateFirst4:
                    motor.AccCount(0x00,0x4f,0);
                    danceStateOld++;
                    break;
                case DanceStateFirst5:
                    motor.AccCount(0x00,0xcf,0);
                    danceStateOld = DanceStateFirst1;
                    break;
                case DanceStateSecond1:
                    motor.AccCount(0xcf,0x4f,0);
                    motor.AccCount(0x8f,0x0f,150);
                    danceStateOld = DanceStateSecond2;
                    
                    LED_OLD.skirtCount(0xff,0x00, 0x70, 0x80, 0);
                    LED_OLD.topCount(0xf7,0x00, 0x70, 0x80, 0);
                    LED_OLD.topCount(0x08,0xff, 0x00, 0x00, 0);

                    LED_OLD.skirtCount(0xff,0x00, 0x00, 0x00, 4000/20);
                    LED_OLD.topCount(0xff,0x00, 0x00, 0x00, 4000/20);
                    break;
                case DanceStateSecond2:
                    if (millis()- danceAllTimerOld>4000) {
                        danceStateOld = DanceStateSecond3;
                    }
                    break;
                case DanceStateSecond3:
                    motor.AccCount(0x00, 0xff, 0);
                    motor.AccCount(0x00, 0x8f, 18);
                    danceStateOld++;
                    break;
                case DanceStateSecond4:
                    motor.AccCount(0xff, 0x00, 0);
                    motor.AccCount(0x8f, 0x00, 18);
                    danceStateOld++;
                    break;
                case DanceStateSecond5:
                    motor.AccCount(0x00,0x7f,0);
                    motor.AccCount(0x00,0x0f,18);
                    danceStateOld++;
                    break;
                case DanceStateSecond6:
                    motor.AccCount(0xff, 0x00,0);
                    motor.AccCount(0x8f, 0x00,18);
                    danceStateOld=DanceStateSecond3;
                    break;
                case DanceStateThird1:
                    motor.AccCount(0x00,0xff, 0);
                    danceStateOld++;
                    break;
                case DanceStateThird2:
                    motor.AccCount(0xff,0x00, 0);
                    danceStateOld++;
                    break;
                case DanceStateThird3:
                    motor.AccCount(0x00,0x7f,0);
                    danceStateOld++;
                    break;
                case DanceStateThird4:
                    motor.AccCount(0x7f,0x00,0);
                    danceStateOld++;
                    break;
                case DanceStateThird5:
                    motor.AccCount(0x7f,0x00, 0);
                    danceStateOld++;
                    break;
                case DanceStateThird6:
                    motor.AccCount(0x00,0x7f, 0);
                    danceStateOld++;
                    break;
                case DanceStateThird7:
                    motor.AccCount(0xff,0x00, 0);
                    danceStateOld++;
                    break;
                case DanceStateThird8:
                    motor.AccCount(0x00,0xff, 0);
                    danceStateOld=DanceStateThird1;
                    break;
                case DanceStateForth1:
                    motor.AccCount(0xff,0xff, 0);
                    motor.AccCount(0x8f,0x8f, 18);
                    danceStateOld++;
                    break;
                case DanceStateForth2:
                    motor.AccCount(0x7f,0x7f, 0);
                    motor.AccCount(0x0f,0x0f, 18);
                    danceStateOld++;
                    break;
                case DanceStateForth3:
                    motor.AccCount(0xff,0x7f, 0);
                    danceStateOld = DanceStateForth1;
                    break;
                default:
                    motor.AccCount(0x00,0x00,0);
                    break;
            }
        }
        
    }

}
