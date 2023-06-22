#include "DFRobot_ShiftOut.h"

DFRobot_ShiftOut::DFRobot_ShiftOut()
{
  BYTE tmp[]={0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,0xff};
  for(int i=0;i<sizeof(tmp);i++){
    Tab[i] = tmp[i];
  }
}
void DFRobot_ShiftOut::begin(uint8_t data, uint8_t clk, uint8_t latch, uint8_t bitorder)
{
  _data = data;
  _clk = clk;
  _latch = latch;
  _bitorder = bitorder;
  pinMode(_latch, OUTPUT);
  pinMode(_data, OUTPUT);  
  pinMode(_clk, OUTPUT);
}

void DFRobot_ShiftOut::write(uint8_t segment)
{
  _segment = segment;
  digitalWrite(_latch, LOW);
#if defined(ARDUINO_ARCH_RP2040)
  shiftOut(_data, _clk, (BitOrder)_bitorder, _segment);
#else
  shiftOut(_data, _clk, _bitorder, _segment);
#endif
  digitalWrite(_latch, HIGH);
}

void DFRobot_ShiftOut::write(String segment)
{
  segment.toLowerCase();
  _segment = 0;
  if(segment.length() == 1){
    _segment = shiftParseOne(segment[0]);
  }else{
    _segment = shiftParseDouble(segment[segment.length()-2],segment[segment.length()-1]);
  }
  write(_segment);
}

void DFRobot_ShiftOut::led(int value)
{
  if((value >= 0) && (value <=9)){
    write(Tab[value]);
  }else if(value == -1){
	write(Tab[10]);
  }
}

void DFRobot_ShiftOut::dot(bool on)
{
  if(on){
    _segment &= 0x7F;
  }else{
    _segment |= 0x80;
  }
  write(_segment);
}

uint8_t DFRobot_ShiftOut::shiftParseOne(uint8_t a)
{
  if(a<0x30){
    return 0;  
  }else if((a>=0x30) && (a<=0x39)){
    return a-0x30;  
  }else if((a>0x39)&&(a<0x61)){
    return 0;  
  }else if((a>=0x61)&&(a<=0x66)){
    return a-0x57;  
  }else{
    return 15;  
  }
}

uint8_t DFRobot_ShiftOut::shiftParseDouble(uint8_t a, uint8_t b)
{
  uint8_t c,d;
  if(a<0x30){
    c = 0;  
  }else if((a>=0x30) && (a<=0x39)){
    c = a-0x30;  
  }else if((a>0x39)&&(a<0x61)){
    c = 0;  
  }else if((a>=0x61)&&(a<=0x66)){
    c = a-0x57;  
  }else{
    c = 15;  
  }

  if(b<0x30){
    d = 0;  
  }else if((b>=0x30) && (b<=0x39)){
    d = b-0x30;  
  }else if((b>0x39)&&(b<0x61)){
    d = 0;  
  }else if((b>=0x61)&&(b<=0x66)){
    d = b-0x57;  
  }else{
    d = 15;  
  }

  return c*16+d;
}
