#include "DFMicrobit_Radio.h"
#include "basic.h"

//onDataNumPacketReceivedCb onDataNumPacketReceived = NULL;
//onDataValPacketReceivedCb onDataValPacketReceived = NULL;
onDataStrPacketReceivedCb onDataStrPacketReceived = NULL;
bool radioEnabled;
DFMicrobit_Radio *radio_pt = NULL;


////////////////////////////////static////////////////////////////////
void dispatchEventRadio(MicroBitEvent e) {
    // ManagedString sss("77");
    // uBit.display.scroll(sss, 150);
    String val = "";
#ifdef NRF52833   //debugjh
    PacketBuffer s = uBit.radio.datagram.recv();
    radio_pt->staticRadioMessage = String((char*)s.getBytes());
    if(!onDataStrPacketReceived) return;
	String back = String((char*)s.getBytes()).substring(0,s.length());
    onDataStrPacketReceived(back);
#else
    ManagedString s = uBit.radio.datagram.recv();
    radio_pt->staticRadioMessage = s.toCharArray();
    // for(uint8_t i = 0; i < radio_pt->staticRadioMessage.length(); i++){
    //     Serial.print((uint8_t)radio_pt->staticRadioMessage[i]);
    //     Serial.print(" ");
    // }
    if(!onDataStrPacketReceived) return;
    onDataStrPacketReceived(s.toCharArray());
#endif
}

////////////////////////////////class///////////////////////////////

DFMicrobit_Radio::DFMicrobit_Radio()
{
  radio_pt = this;
  staticRadioMessage = "";
  radioEnabled = false;
  this->group = 0;
}

DFMicrobit_Radio::~DFMicrobit_Radio()
{
}

int DFMicrobit_Radio::turnOn()
{
  if (radioEnabled) return MICROBIT_OK;

  int r = uBit.radio.enable();
  if (r != MICROBIT_OK) {
    uBit.panic(43);
    uBit.serial.printf("Radio turn on failure! error %d\r\n", r);
    return r;
  }

  r = uBit.radio.setTransmitPower(7);
  if (r != MICROBIT_OK) {
    this->turnOff();
    uBit.panic(43);
    uBit.serial.printf("Radio setting power failure! error %d\r\n", r);
    return r;
  }

  radioEnabled = true;
  return r;
}

int DFMicrobit_Radio::turnOff()
{
  if (!radioEnabled) return MICROBIT_OK;

  int r = uBit.radio.disable();
  if (r != MICROBIT_OK) {
    uBit.panic(43);
    uBit.serial.printf("Radio turn off failure! error %d\r\n", r);
    return r;
  }
  
  radioEnabled = false;
  this->group = -1;
  return r;
}

void DFMicrobit_Radio::setGroup(uint8_t group)
{
  if(!radioEnabled)
  {
    int r = turnOn();
    if (r != MICROBIT_OK) return;
  }

  uBit.radio.setGroup(group);
  this->group = group;
}

/*
void DFMicrobit_Radio::send(String msg)
{
  if(!radioEnabled) return;

  uBit.radio.datagram.send(msg.c_str());
}
*/

void DFMicrobit_Radio::send(String msg)
{
  if(!radioEnabled) return;
  uint8_t len = msg.length();
  if(len > 18){
    len = 18;
  }
  uint8_t value[32] = {0};
  uint8_t *p = value;
  p[0] = 2;
  p[9] = len;
  for(uint8_t i = 0; i < len; i++){
    p[10+i] = msg[i];
  }
  uBit.radio.datagram.send(p, 32);
}


void DFMicrobit_Radio::send(int32_t i)
{
  char buffer[32];
  memset(buffer, 0, 32);

  itoa(i, buffer, 10);
  send((String)buffer);
}

void DFMicrobit_Radio::send(uint32_t i)
{
  char buffer[32];
  memset(buffer, 0, 32);

  itoa(i, buffer, 10);
  send((String)buffer);
}

#define MAX_PRECISION	(10)
static const double rounders[MAX_PRECISION + 1] = {
    0.5,                // 0
    0.05,               // 1
    0.005,              // 2
    0.0005,             // 3
    0.00005,            // 4
    0.000005,           // 5
    0.0000005,          // 6
    0.00000005,         // 7
    0.000000005,        // 8
    0.0000000005,       // 9
    0.00000000005       // 10
};

void DFMicrobit_Radio::send(double f, int precision)
{
  char buf[80];

  char * ptr = buf;
  char * p = ptr;
  char * p1;
  char c;
  long intPart;

  // check precision bounds
  if (precision > MAX_PRECISION)
    precision = MAX_PRECISION;

  // sign stuff
  if (f < 0) {
    f = -f;
    *ptr++ = '-';
  }

  if (precision > 0) {  // negative precision == automatic precision guess
    if (f < 1.0) precision = 6;
    else if (f < 10.0) precision = 5;
    else if (f < 100.0) precision = 4;
    else if (f < 1000.0) precision = 3;
    else if (f < 10000.0) precision = 2;
    else if (f < 100000.0) precision = 1;
    else precision = 0;
  }

  // round value according the precision
  if (precision)
    f += rounders[precision];

  // integer part...
  intPart = f;
  f -= intPart;

  if (!intPart)
    *ptr++ = '0';
  else {
    // save start pointer
    p = ptr;

    // convert (reverse order)
    while (intPart) {
      *p++ = '0' + intPart % 10;
      intPart /= 10;
    }

    // save end pos
    p1 = p;

    // reverse result
    while (p > ptr) {
      c = *--p;
      *p = *ptr;
      *ptr++ = c;
    }
    // restore end pos
    ptr = p1;
  }

  // decimal part
  if (precision) {
    // place decimal point
    *ptr++ = '.';

    // convert
    while (precision--) {
      f *= 10.0;
      c = f;
      *ptr++ = '0' + c;
      f -= c;
    }
  }

  // terminating zero
  *ptr = 0;

  for(int i = strlen(buf)-1; i>=0; i--){
    if(buf[i] == '.') break;
    if(buf[i] == '0') {
      buf[i] = '\0';
      if(buf[i-1] == '.'){
        buf[i-1] = '\0';
        break;
      }
    }else break;
  }
  send((String)buf);
}

void DFMicrobit_Radio::setCallback(onDataStrPacketReceivedCb cb) {
  if (!radioEnabled) {
    this->setGroup(this->group>=0? this->group : 0);
  }
  onDataStrPacketReceived = cb;
  uBit.messageBus.listen(MICROBIT_ID_RADIO, MICROBIT_RADIO_EVT_DATAGRAM, dispatchEventRadio);
  // make sure the receive buffer has a free spot
  //receiveNumber();
}

String DFMicrobit_Radio::getMessage()
{
  String message = this->staticRadioMessage;
  this->staticRadioMessage = "";
  return message;
}

DFMicrobit_Radio Radio;
