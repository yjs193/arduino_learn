#include "VortexTask.h"
#include <FlexiTimer2.h>
#include <VortexRGB.h>
#include <VortexMotor.h>
#include "VortexDance.h"

static void vortexTask() {
    static unsigned long timeout20ms= millis();
    danceupdate();
    if (millis() - timeout20ms >= 20) {
        timeout20ms = millis();
        LED.update();
        motor.update();
    }
}

void vortexBegin()
{
    static bool enableNew=false;
    if(!enableNew){
        danceinit();
        FlexiTimer2::set(20, vortexTask);
        FlexiTimer2::start();
        enableNew = true;
    }
}

void vortexEnd()
{
    FlexiTimer2::stop();
}