#include "VortexTaskOld.h"
#include <FlexiTimer2.h>
#include <VortexRGBOld.h>
#include <VortexMotor.h>
#include "VortexDanceOld.h"

static void vortexOldTask() {
    static unsigned long timeout20msOld= millis();
    danceupdateOld();
    if (millis() - timeout20msOld >= 20) {
        timeout20msOld = millis();
        LED_OLD.update();
        motor.update();
    }
}

void vortexBeginOld()
{
    static bool enableOld=false;
    if(!enableOld){
        danceinitOld();
        FlexiTimer2::set(20, vortexOldTask);
        FlexiTimer2::start();
        enableOld = true;
    }
}

void vortexEndOld()
{
    FlexiTimer2::stop();
}