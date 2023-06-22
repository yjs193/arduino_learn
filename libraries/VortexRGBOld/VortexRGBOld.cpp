#include <Arduino.h>
#include "VortexRGBOld.h"
//#include "FastLED.h"
#include "DFRobot_NeoPixel.h"
#include <avr/eeprom.h>

#include "Wire.h"

VortexRGBOld LED_OLD;
DFRobot_NeoPixel stripOld;


VortexRGBOld::VortexRGBOld()
{
}

void VortexRGBOld::update()
{
    skirtAction();
    topAction();
}
void VortexRGBOld::Init()
{
    stripOld.begin(13, NUM_LEDS, 255);
    Wire.begin(); // join i2c bus (address optional for master)
}
static char switchCtr = 0xf0;
void VortexRGBOld::setPower(){
  Wire.beginTransmission(I2C_LED_ADDRESS << 1 | I2C_WRITE);
  Wire.write(0x55);
  Wire.write(0xaa);
  Wire.write(switchCtr);
  Wire.endTransmission();
}
void VortexRGBOld::setMotorPower(char isOpen){
  if(isOpen)
  switchCtr |= (1 << 5);
  else
  switchCtr &= ~(1<<5);
  setPower();
}
void VortexRGBOld::setGrayPower(char isOpen){
  if(isOpen)
  switchCtr |= 1<<6;
  else
  switchCtr &= ~(1<<6);
  setPower();
}
void VortexRGBOld::setMp3Power(char isOpen){
  if(isOpen)
  switchCtr |= 1<<4;
  else
  switchCtr &= ~(1<<4);
  setPower();
}
void VortexRGBOld::setSwitchLeftPower(char isOpen){
  if(isOpen)
  switchCtr |= 1<<3;
  else
  switchCtr &= ~(1<<3);
  setPower();
}
void VortexRGBOld::setSwitchRightPower(char isOpen){
  if(isOpen)
    switchCtr |= 1<<2;
  else
    switchCtr &= ~(1<<2);
  setPower();
}

int faceOld[10] = {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1};
int numOld = -1;
int colOld = -1;
int colUserOld = -1;
void VortexRGBOld::setUserFace(byte color,void *faceBuf)
{
  const char * buf=(const char *)faceBuf;
  int i=0;
  for(i=0; i<10; i++){
    if(buf[i] != faceOld[i] || colUserOld != color)  break;
  }
  if(i == 10) return;

  for(i=0; i<10; i++){
    faceOld[i] = buf[i];
  }
  colUserOld = color;
  
  numOld = -1;
  colOld = -1;
  
  Wire.beginTransmission(I2C_LED_ADDRESS << 1 | I2C_WRITE);
  Wire.write(0x55);
  Wire.write(0xAA);
  Wire.write(color);

  uint8_t bufn[10];
  for(uint8_t i=5; i<10; i++)
  {
    bufn[i-5] = buf[i];
    bufn[i] = buf[i-5];
  }

  uint8_t num = 0;
  num = bufn[0];
  bufn[0] = bufn[4];
  bufn[4] = num;
  num = bufn[1];
  bufn[1] = bufn[3];
  bufn[3] = num;

  for(int i=0;i<10; i++)
    Wire.write(bufn[i]);
  Wire.endTransmission();
  delay(10);
}

void VortexRGBOld::setFace(byte color,byte number)
{
    if(numOld == number && colOld == color) return;
    numOld = number;
    colOld = color;

    for(int i=0; i<10; i++){
        faceOld[i] = -1;
    }
    colUserOld = -1;

    if(number == 21)
        number = 0;
    currentFaceColor = color;
    currentFaceNumber = number%33;

    Wire.beginTransmission(I2C_LED_ADDRESS << 1 | I2C_WRITE); // transmit to device #4
    Wire.write(color);        //表情显示点阵颜色  R G B顺序  比如  0x05   0000  0101   低三位为颜色  ，红色和蓝色亮
    Wire.write(number);      //预置表情序列
    Wire.endTransmission();    // stop transmitting
    delay(10);
}

void VortexRGBOld::skirtCount(uint8_t date,uint8_t red,uint8_t green,uint8_t blue,uint8_t time)
{
    noInterrupts();
    uint8_t da = date;
    date &= 0xc9;
    date |= ((da>>4)&0x02);
    date |= ((da<<4)&0x20);
    date |= ((da>>2)&0x04);
    date |= ((da<<2)&0x10);
    for(int j=0;j<6;j++)
    {
        if(date&(1<<j))
        {

            skirtTarget[j][0] = red;
            skirtTarget[j][1] = green;
            skirtTarget[j][2] = blue;

            if(time==0)
            {
                skirtFastLed[j] = true;
                skirtCurrent[j][0] = skirtTarget[j][0];
                skirtCurrent[j][1] = skirtTarget[j][1];
                skirtCurrent[j][2] = skirtTarget[j][2];
            }
            else
            {
                skirtFastLed[j] = false;
                skirtLedAcc[j][0]=abs(float(red-skirtCurrent[j][0]))/float(time);
                skirtLedAcc[j][1]=abs(float(green-skirtCurrent[j][1]))/float(time);
                skirtLedAcc[j][2]=abs(float(blue-skirtCurrent[j][2]))/float(time);
            }

        }
    }
    interrupts();
}

void VortexRGBOld::skirtAction()
{
    for(int i=0;i<6;i++)
    {
        if(skirtFastLed[i])
            skirtCurrent[i][0]=skirtTarget[i][0];
        else//亮度平滑
        {
            if(skirtCurrent[i][0]<skirtTarget[i][0])
            {
                skirtCurrent[i][0]+=skirtLedAcc[i][0];
                if(skirtCurrent[i][0]>skirtTarget[i][0])
                    skirtCurrent[i][0]=skirtTarget[i][0];
            }
            if(skirtCurrent[i][0]>skirtTarget[i][0])
            {
                skirtCurrent[i][0]-=skirtLedAcc[i][0];
                if(skirtCurrent[i][0]<skirtTarget[i][0])
                    skirtCurrent[i][0]=skirtTarget[i][0];
            }

        }
        if(skirtFastLed[i])
            skirtCurrent[i][1]=skirtTarget[i][1];
        else//亮度平滑
        {
            if(skirtCurrent[i][1]<skirtTarget[i][1])
            {
                skirtCurrent[i][1]+=skirtLedAcc[i][1];
                if(skirtCurrent[i][1]>skirtTarget[i][1])
                    skirtCurrent[i][1]=skirtTarget[i][1];
            }
            if(skirtCurrent[i][1]>skirtTarget[i][1])
            {
                skirtCurrent[i][1]-=skirtLedAcc[i][1];
                if(skirtCurrent[i][1]<skirtTarget[i][1])
                    skirtCurrent[i][1]=skirtTarget[i][1];
            }

        }
        if(skirtFastLed[i])
            skirtCurrent[i][2]=skirtTarget[i][2];
        else//亮度平滑
        {
            if(skirtCurrent[i][2]<skirtTarget[i][2])
            {
                skirtCurrent[i][2]+=skirtLedAcc[i][2];
                if(skirtCurrent[i][2]>skirtTarget[i][2])
                    skirtCurrent[i][2]=skirtTarget[i][2];
            }
            if(skirtCurrent[i][2]>skirtTarget[i][2])
            {
                skirtCurrent[i][2]-=skirtLedAcc[i][2];
                if(skirtCurrent[i][2]<skirtTarget[i][2])
                    skirtCurrent[i][2]=skirtTarget[i][2];
            }

        }
        skirtFastLed[i] = false;

//        ledS[i]=CRGB(int(skirtCurrent[i][1]),int(skirtCurrent[i][0]),int(skirtCurrent[i][2]));

        stripOld.setPixelColor(i, stripOld.rgbToColor(int(skirtCurrent[i][0]), int(skirtCurrent[i][1]), int(skirtCurrent[i][2])), false);
     
    }
}
void VortexRGBOld::topCount(uint8_t date,uint8_t red,uint8_t green,uint8_t blue,uint8_t time)
{
    noInterrupts();
    for(int j=0;j<6;j++)
    {
        if(date&(1<<j))
        {
            topTarget[j][0] = red;
            topTarget[j][1] = green;
            topTarget[j][2] = blue;

            if(time==0)
            {
                TopFastLed[j] = true;
                topCurrent[j][0] = topTarget[j][0];
                topCurrent[j][1] = topTarget[j][1];
                topCurrent[j][2] = topTarget[j][2];
            }
            else
            {
                TopFastLed[j] = false;
                topLedAcc[j][0]=abs(float(red-topCurrent[j][0]))/float(time);
                topLedAcc[j][1]=abs(float(green-topCurrent[j][1]))/float(time);
                topLedAcc[j][2]=abs(float(blue-topCurrent[j][2]))/float(time);
            }
        }
    }
    interrupts();
}

void VortexRGBOld::topAction()
{
    for(int i=0;i<6;i++)

    {
        if(TopFastLed[i])
            topCurrent[i][0]=topTarget[i][0];
        else//亮度平滑
        {
            if(topCurrent[i][0]<topTarget[i][0])
            {
                topCurrent[i][0]+=topLedAcc[i][0];
                if(topCurrent[i][0]>topTarget[i][0])
                    topCurrent[i][0]=topTarget[i][0];
            }
            if(topCurrent[i][0]>topTarget[i][0])
            {
                topCurrent[i][0]-=topLedAcc[i][0];
                if(topCurrent[i][0]<topTarget[i][0])
                    topCurrent[i][0]=topTarget[i][0];
            }

        }
        if(TopFastLed[i])
            topCurrent[i][1]=topTarget[i][1];
        else//亮度平滑
        {
            if(topCurrent[i][1]<topTarget[i][1])
            {
                topCurrent[i][1]+=topLedAcc[i][1];
                if(topCurrent[i][1]>topTarget[i][1])
                    topCurrent[i][1]=topTarget[i][1];
            }
            if(topCurrent[i][1]>topTarget[i][1])
            {
                topCurrent[i][1]-=topLedAcc[i][1];
                if(topCurrent[i][1]<topTarget[i][1])
                    topCurrent[i][1]=topTarget[i][1];
            }

        }
        if(TopFastLed[i])
            topCurrent[i][2]=topTarget[i][2];
        else//亮度平滑
        {
            if(topCurrent[i][2]<topTarget[i][2])
            {
                topCurrent[i][2]+=topLedAcc[i][2];
                if(topCurrent[i][2]>topTarget[i][2])
                    topCurrent[i][2]=topTarget[i][2];
            }
            if(topCurrent[i][2]>topTarget[i][2])
            {
                topCurrent[i][2]-=topLedAcc[i][2];
                if(topCurrent[i][2]<topTarget[i][2])
                    topCurrent[i][2]=topTarget[i][2];
            }

        }
        TopFastLed[i] = false;
        stripOld.setPixelColor(i+6, stripOld.rgbToColor(int(topCurrent[i][0]), int(topCurrent[i][1]), int(topCurrent[i][2])), false);
//        ledS[i+6]=CRGB(int(topCurrent[i][1]),int(topCurrent[i][0]),int(topCurrent[i][2]));
    }
//    FastLED.show();

    stripOld.show();

}



