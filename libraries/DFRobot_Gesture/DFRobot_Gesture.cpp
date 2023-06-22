#include "DFRobot_Gesture.h"
#include <Arduino.h>

typedef struct _gesturEvents{
  long time = 0;
  GESTURE id = GESTURE_NoN;
  uint8_t len = 0;
  _gesturEvents *next = NULL;
}gesturEvents;
gesturEvents *events = NULL;


void DFRobot_Gesture::begin(unsigned char pin_xfer, unsigned char pin_reset){
  this->xfer = pin_xfer;
  this->rst  = pin_reset;
  this->addr = SW_ADDR;

  Wire.begin();

  pinMode(this->xfer, INPUT_PULLUP);
  pinMode(this->rst,  OUTPUT);
  digitalWrite(this->rst, LOW);
  delay(10);
  digitalWrite(this->rst, HIGH);
  delay(50);
}

bool DFRobot_Gesture::onGesture(GESTURE gesture)
{
  this->addEvent(gesture);
  int cmd = this->poll();
  GESTURE ges = (GESTURE)cmd;
  recordEvents(ges);
  return extractEvent(gesture);
}

int DFRobot_Gesture::poll(){
  int val = 0;
  if (!digitalRead(this->xfer)){
    pinMode(this->xfer, OUTPUT);
    digitalWrite(this->xfer, LOW);

    Wire.requestFrom(this->addr, (unsigned char)32);

    unsigned char d_size,d_flags,d_seq,d_ident;

    if( Wire.available() >= 4 ){
      d_size  = Wire.read();
      d_flags = Wire.read();
      d_seq   = Wire.read();
      d_ident = Wire.read();
    }
    else{
      return 0;
    }

    this->command_buffer[0] = '\0';

    unsigned char i = 0;
    while(Wire.available()){
      this->command_buffer[i] = Wire.read();
      i++;
    }
    this->command_buffer[i] = '\0';

    switch(d_ident){
      case 0x91:
        val = this->handle_sensor_data(this->command_buffer);
        break;
      case 0x15:
        // Status info - Unimplemented
        break;
      case 0x83:
        // Firmware data - Unimplemented
        break;
    }
    
    
    digitalWrite(this->xfer, HIGH);
    pinMode(this->xfer, INPUT);
  }
  return val;
}

void DFRobot_Gesture::onTouch(    void (*function)(unsigned char) ){this->handle_touch    = function;}
void DFRobot_Gesture::onAirwheel( void (*function)(int)           ){this->handle_airwheel = function;}
void DFRobot_Gesture::onGesture(  void (*function)(unsigned char) ){this->handle_gesture  = function;}
void DFRobot_Gesture::onXYZ(      void (*function)(unsigned int,unsigned int,unsigned int) ){this->handle_xyz  = function;}

int DFRobot_Gesture::handle_sensor_data(unsigned char* data){
  int retVal = 0;
/*
  | HEADER | PAYLOAD
  |        |  DataOutputConfigMask 2 | TimeStamp 1 | SystemInfo 1 | Content |
  
  DataOutputConfigMask
  Bit 0 - DSPStatus
  Bit 1 - GestureInfo
  Bit 2 - TouchInfo
  Bit 3 - AirWheelInfo
  Bit 4 - XYZ Position
  Bit 5 - NoisePower
  Bit 6-7 - Reserved
  Bit 8-10 - ElectrodeConfiguration
  Bit 11 - CICData
  Bit 12 - SDData
  Bit 13-15 - Reserved
  
  SystemInfo
  Bit 0 - PositionValid, indicates xyz pos data is valid
  Bit 1 - AirWheelValid, indicates AirWheel is active and AirWheelInfo is vaid
  Bit 2 - RawDataValid, indicates CICData and SDData fields are valid
  Bit 3 - NoisePowerValid, indicates NoisePower field is valid
  Bit 4 - EnvironmentalNoise, indicates that environmental noise has been detected
  Bit 5 - Clipping, indicates that the ADCs are clipping
  Bit 6 - Reserved
  Bit 7 - DSPRunning, indicates the system is currently running
  DSPStatus - 2 bytes -
  GestureInfo - 4 bytes
  TouchInfo - 4 bytes
  AirWheelInfo - 2 bytes - first byte indicates rotation, ++ = clockwise, 32 = 1 rotation
  xyzPosition - 6 bytes - 1+2 = x, 3-4 = y, 5-6 = z
  NoisePower
  CICData
  SDData
*/
  if( data[SW_PAYLOAD_HDR_CONFIGMASK] & SW_DATA_XYZ && data[SW_PAYLOAD_HDR_SYSINFO] & SW_SYS_POSITION ){
    // Valid XYZ position
    this->x = data[SW_PAYLOAD_X+1] << 8 | data[SW_PAYLOAD_X];
    this->y = data[SW_PAYLOAD_Y+1] << 8 | data[SW_PAYLOAD_Y];
    this->z = data[SW_PAYLOAD_Z+1] << 8 | data[SW_PAYLOAD_Z];

    if( handle_xyz != NULL ) handle_xyz(this->x,this->y,this->z);
  }

  if( data[SW_PAYLOAD_HDR_CONFIGMASK] & SW_DATA_GESTURE && data[SW_PAYLOAD_GESTURE] > 0){
    // Valid gesture
    this->last_gesture = data[SW_PAYLOAD_GESTURE];
    retVal = this->last_gesture;
    //if( this->handle_gesture != NULL ) this->handle_gesture(this->last_gesture);
  }

  if ( data[SW_PAYLOAD_HDR_CONFIGMASK] & SW_DATA_TOUCH ){
    // Valid touch
    uint16_t touch_action = data[SW_PAYLOAD_TOUCH+1] << 8 | data[SW_PAYLOAD_TOUCH];
    uint16_t comp = 1 << 14;
    uint8_t x;
    for(x = 0; x < 16; x++){
      if( touch_action & comp ){
        this->last_touch = x;
        if( this->handle_touch != NULL ) this->handle_touch(this->last_touch);
        return 0;
      }
    comp = comp >> 1;
    }
  }

  if( data[SW_PAYLOAD_HDR_CONFIGMASK] & SW_DATA_AIRWHEEL && data[SW_PAYLOAD_HDR_SYSINFO] & SW_SYS_AIRWHEEL ){

    double delta = (data[SW_PAYLOAD_AIRWHEEL] - lastrotation) / 32.0;

    if( delta != 0 && delta > -0.5 and delta < 0.5 ){
      //if( this->handle_airwheel != NULL ) this->handle_airwheel(delta * 360.0);  
      retVal = (int)(delta * 360.0);
    }

    this->rotation += delta * 360.0;
    this->rotation %= 360;

    this->lastrotation = data[SW_PAYLOAD_AIRWHEEL];
  }
  return retVal;
}

void DFRobot_Gesture::addEvent(GESTURE gesture)
{
  if(events == NULL){
    events = new gesturEvents;
    gesturEvents *event = new gesturEvents;
    event->time = 0;
    event->id = gesture;
    event->len = 0;
    event->next = NULL;
    events = event;
    //delete event;
    return;
  }

  gesturEvents *eventsHead = events;
  uint8_t i;
  for(i=1; i<7; i++){
    if(eventsHead->next == NULL)
      break;
    else if(eventsHead->id == gesture)
      return;
    eventsHead = eventsHead->next;
  }
  if(i>=6) return;
  gesturEvents *event = new gesturEvents;
  event->id = gesture;
  event->len = 0;
  event->next = NULL;
  eventsHead->next = event;
  //delete event;

  /*
  gesturEvents *sss = events;
  while(sss){
    Serial.println(sss->id);
    sss = sss->next;
  }Serial.println("=======");
  */
}

void DFRobot_Gesture::recordEvents(GESTURE gesture)
{
  if(gesture == GESTURE_NoN) return;
  gesturEvents *eventsHead = events;
  while(eventsHead)
  {
    if(eventsHead->id == gesture && eventsHead->len<3)
    {
      eventsHead->time = millis();
      eventsHead->len++;
      return;
    }
    eventsHead = eventsHead->next;
  }
}

bool DFRobot_Gesture::extractEvent(GESTURE gesture)
{
  if(gesture == GESTURE_NoN) return false;
  gesturEvents *eventsHead = events;
  while(eventsHead){
    if(eventsHead->id == gesture)
    {
      if(eventsHead->len > 0){
        eventsHead->len--;
        return millis() - eventsHead->time <= 1000;
      }
    }
    eventsHead = eventsHead->next;
  }
  return false;
}
