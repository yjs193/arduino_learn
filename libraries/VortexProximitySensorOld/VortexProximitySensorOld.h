#include "Wire.h"

#define IR_IN  7//IR receiver pin
#define L_IR 8  //left ir transmitter pin
#define R_IR 12 //right ir transmitter pin

int count;
bool irCount;



void proximitySensorInit()
{
  pinMode(L_IR,OUTPUT);//init the left transmitter pin
  pinMode(R_IR,OUTPUT);//init the right transmitter pin
  pinMode(IR_IN,INPUT);//init the ir receiver pin
  PCICR |= 1 << PCIE2;
  PCMSK2 |= 1 << PCINT23;
  sei();
}

void rightSend38KHZ(void){//right ir transmitter sends 38kHZ pulse
  int i;
  for(i=0;i<24;i++){
    digitalWrite(R_IR,LOW);
    delayMicroseconds(8);
    digitalWrite(R_IR,HIGH);
    delayMicroseconds(8);
  }
}

void leftSend38KHZ(void){//left ir transmitter sends 38kHZ pulse
  int i;
  for(i=0;i<24;i++){
    digitalWrite(L_IR,LOW);
    delayMicroseconds(8);
    digitalWrite(L_IR,HIGH);
    delayMicroseconds(8);
  }
}

ISR(PCINT2_vect){//motor encoder interrupt
  count++;
}

bool proximitySensorRead(void){
  char i;
  static long time = millis();
  if(millis()-time < 50) return irCount;
  for(i=0;i<20;i++){  //left transmitter sends 20 pulses
    leftSend38KHZ();
    delayMicroseconds(200);
  } 
  for(i=0;i<20;i++){//right transmitter sends 20 pulses
    rightSend38KHZ(); 
    delayMicroseconds(200);
  }
  if(count>0)
  {
    count = 0;
    irCount = true;
  }else
  {
    count = 0;
    irCount = false;
  }
  time = millis();
  return irCount;
}
