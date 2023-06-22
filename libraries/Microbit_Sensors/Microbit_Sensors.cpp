#include "Microbit_Sensors.h"
#include <basic.h>

static gestureHandlePtr callbacks[NUMBER_OF_GES_EVT];
static int8_t gestureHandleMap[NUMBER_OF_GES_EVT];
Microbit_Sensors *pt = NULL;

void dispatchEvent(MicroBitEvent e)
{
  if(e.source = MICROBIT_ID_GESTURE) {
    switch(e.value) {
      case pt->Shake:       callbacks[0](); return;
      case pt->LogoUp:      callbacks[1](); return;
      case pt->LogoDown:    callbacks[2](); return;
      case pt->ScreenUp:    callbacks[3](); return;
      case pt->ScreenDown:  callbacks[4](); return;
      case pt->TiltLeft:    callbacks[5](); return;
      case pt->TiltRight:   callbacks[6](); return;
      case pt->FreeFall:    callbacks[7](); return;
      case pt->ThreeG:      callbacks[8](); return;
      case pt->SixG:        callbacks[9](); return;
      case pt->EightG:      callbacks[10](); return;
    }
  }
}

Microbit_Sensors::Microbit_Sensors()
{
  pt = this;
  this->begin();
}

Microbit_Sensors::~Microbit_Sensors(){}

void Microbit_Sensors::begin()
{
  memset(callbacks, 0, sizeof(callbacks));
  memset(gestureHandleMap, -1, sizeof(gestureHandleMap));
}

int Microbit_Sensors::compassHeading()
{
  return uBit.compass.heading();
}

int Microbit_Sensors::temperature()
{
  return uBit.thermometer.getTemperature();
}

int Microbit_Sensors::acceleration(Dimension dimension)
{
  switch (dimension) {
    case X: return uBit.accelerometer.getX();
    case Y: return uBit.accelerometer.getY();
    case Z: return uBit.accelerometer.getZ();
    case Strength: return getAccelerationStrength();
  }
  return 0;
}

int Microbit_Sensors::magneticForce(Dimension dimension)
{
  //if (!uBit.compass.isCalibrated())  //debug jh20.5.18
  //  uBit.compass.calibrate();
  switch (dimension) {
    case X: return uBit.compass.getX() / 1000;
    case Y: return uBit.compass.getY() / 1000;
    case Z: return uBit.compass.getZ() / 1000;
    case Strength: return uBit.compass.getFieldStrength() / 1000;
  }
  return 0;
}

void Microbit_Sensors::onGesture(Gesture gesture, gestureHandlePtr body) {
  int gi = (int)gesture;
  int num = getNum(gi);
  callbacks[num] = body;
  gestureHandleMap[num] = 1;

  if (gi == MICROBIT_ACCELEROMETER_EVT_3G && uBit.accelerometer.getRange() < 3)
    uBit.accelerometer.setRange(4);
  else if ((gi == MICROBIT_ACCELEROMETER_EVT_6G || gi == MICROBIT_ACCELEROMETER_EVT_8G) && uBit.accelerometer.getRange() < 6)
    uBit.accelerometer.setRange(8);

  uBit.messageBus.listen(MICROBIT_ID_GESTURE, gi, dispatchEvent);
}

int Microbit_Sensors::getNum(int g)
{
    Gesture gi = (Gesture)g;
    switch(gi) {
      case Shake:      return 0;
      case LogoUp:     return 1;
      case LogoDown:   return 2; 
      case ScreenUp:   return 3;
      case ScreenDown: return 4;
      case TiltLeft:   return 5;
      case TiltRight:  return 6;
      case FreeFall:   return 7;
      case ThreeG:     return 8;
      case SixG:       return 9;
      case EightG:     return 10;
    }
}

int Microbit_Sensors::getAccelerationStrength() {
  double x = uBit.accelerometer.getX();
  double y = uBit.accelerometer.getY();
  double z = uBit.accelerometer.getZ();
  return (int)sqrt(x*x+y*y+z*z);
}

void Microbit_Sensors::updateSample(){//debugjh
	uBit.accelerometer.updateSample();
}


static int shakeGesture = 0;
static uint64_t timer = 0;
bool Microbit_Sensors::getGesture(Gesture gesture)
{
  if(gesture == Shake){
    static uint64_t shakeTimer = millis();
    if(millis() - shakeTimer >= 10) shakeTimer = millis();
    else return false;
  }

  uBit.accelerometer.updateSample(); 
  int _gesture = uBit.accelerometer.getGesture();

  if(gesture == 11 && (_gesture == 11 || shakeGesture == 11)){shakeGesture = 0; return true;}
  
  if(_gesture == 11 && gesture != 11) {shakeGesture = _gesture; timer = millis();}
  if(millis() - timer > 2000){ timer = millis(); shakeGesture = 0;}

  if(gesture == _gesture)
    return true;
  else
    return false;
}

int Microbit_Sensors::calibrate()
{
    // if(uBit.compass.isCalibrated()) return MICROBIT_OK;   //debug wsq
    return uBit.compass.calibrate();
}

Microbit_Sensors Sensors;