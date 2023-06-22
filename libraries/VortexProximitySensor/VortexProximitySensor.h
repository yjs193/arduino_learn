#include "Wire.h"

#define I2C_LED_ADDRESS 0b1100000
#define I2C_WRITE   0x00
#define CONTROLPOWER  0x80
#define ENABLE_MOTO 0x20
#define ENABLE_GRAY 0x40
#define ENABLE_MP3  0x10
int proximitySensorCount;
long time;


void proximitySensorInit()
{    
    PCICR |= 1 << PCIE2;
    PCMSK2 |= 1 << PCINT23;
}

bool proximitySensorRead()
{
  static int ret;
  if(micros()-time<100000) return ret;
  ret = proximitySensorCount;
  proximitySensorCount=0;
  time = micros();
  return ret;
}
ISR(PCINT2_vect)
{
    proximitySensorCount++;
}
static char proximitySensorSwitchCtr = 0xf0;

void setPower(){
  Wire.beginTransmission(I2C_LED_ADDRESS << 1 | I2C_WRITE);
  Wire.write(0x55);
  Wire.write(0xaa);
  Wire.write(proximitySensorSwitchCtr);
  Wire.endTransmission();
}
void setSwitchLeftPower(char isOpen){
  if(isOpen)
    proximitySensorSwitchCtr |= 1<<3;
  else
    proximitySensorSwitchCtr &= ~(1<<3);
  setPower();
}
void setSwitchRightPower(char isOpen){
  if(isOpen)
    proximitySensorSwitchCtr |= 1<<2;
  else
    proximitySensorSwitchCtr &= ~(1<<2);
  setPower();
}

void proximitySensorSetup()
{
  proximitySensorInit();
  Wire.begin(); // join i2c bus (address optional for master)
  setSwitchLeftPower(1);
  setSwitchRightPower(1);
}