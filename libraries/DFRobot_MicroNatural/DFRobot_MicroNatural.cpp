#include <DFRobot_MicroNatural.h>


// TDS
float MicroNatural_TDS::readK()
{
    unsigned char integer = readReg(REG_STM32_K_INTEGER);
    unsigned char decimal = readReg(REG_SEM32_K_DECIMAL);
    return (float)integer + (float)decimal / 100.0;
}

void  MicroNatural_TDS::setK(float k)
{
    unsigned char integer = floor(k);
    unsigned char decimal = floor(k*100 - integer*100);
    writeReg(REG_STM32_K_INTEGER, integer);
    writeReg(REG_SEM32_K_DECIMAL, decimal);
}

uint32_t MicroNatural_TDS::read()
{
    unsigned short int data_h = readReg(REG_STM32_TDS_H);
    unsigned short int data_l = readReg(REG_SEM32_TDS_L);
    unsigned int value = (data_h << 8) | data_l;
    return value > 2000 ? 0 : value;
}

unsigned long MicroNatural_TDS::getClock()
{
    return iicFrequency;
}

unsigned char MicroNatural_TDS::getAddress()
{
    return iicAddress;
}


// sound
uint32_t MicroNatural_Sound::read()
{
    unsigned short int data_h = readReg(REG_SEM32_NOISE_H);
    unsigned short int data_l = readReg(REG_STM32_NOISE_L);
    return round((((data_h << 8) | data_l) / double(4096.0)) * double(1024.0));
}

unsigned long MicroNatural_Sound::getClock()
{
    return iicFrequency;
}

unsigned char MicroNatural_Sound::getAddress()
{
    return iicAddress;
}


// LED
void MicroNatural_LED::set(bool sw)
{
    writeReg(REG_SEM32_LED_CONTROL, sw ? REG_SEM32_LED_ON : REG_SEM32_LED_OFF);
}

unsigned long MicroNatural_LED::getClock()
{
    return iicFrequency;
}

unsigned char MicroNatural_LED::getAddress()
{
    return iicAddress;
}


// UV
float MicroNatural_UV::read()
{
    unsigned char integer = readReg(REG_STM32_UV_INTEGER);
    unsigned char decimal = readReg(REG_SEM32_UV_DECIMAL);
    return (float)integer + (float)decimal / 100.0;
}

unsigned long MicroNatural_UV::getClock()
{
    return iicFrequency;
}

unsigned char MicroNatural_UV::getAddress()
{
    return iicAddress;
}


// TCS34725
MicroNatural_RGBC::MicroNatural_RGBC() : 
    init(false),
    iicAddress(0x29),
    iicFrequency(100000)
{
    
}

MicroNatural_RGBC::~MicroNatural_RGBC()
{

}

uint32_t MicroNatural_RGBC::red()
{
    flush();
    return round((double(rgbcRaw.r)/double(rgbcRaw.c))*double(255.0));
}

uint32_t MicroNatural_RGBC::green()
{
    flush();
    return round((double(rgbcRaw.g)/double(rgbcRaw.c))*double(255.0));
}

uint32_t MicroNatural_RGBC::blue()
{
    flush();
    return round((double(rgbcRaw.b)/double(rgbcRaw.c))*double(255.0));
}

uint32_t MicroNatural_RGBC::naturalLight()
{
    flush();
    return round(rgbcRaw.c);
}

bool MicroNatural_RGBC::config()
{
    if(init)
        return true;
    unsigned char id = readReg(REG_TCS34725_ID | REG_TCS34725_COMMAND_BIT);
    if((id != 0x44) && (id != 0x10))
        return false;
    init = true;
    writeReg(REG_TCS34725_ATIME | REG_TCS34725_COMMAND_BIT, 0xEB);
    writeReg(REG_TCS34725_GAIN | REG_TCS34725_COMMAND_BIT, 0x01);
    writeReg(REG_TCS34725_ENABLE | REG_TCS34725_COMMAND_BIT, 0x01);
    delay(3);
    writeReg(REG_TCS34725_ENABLE | REG_TCS34725_COMMAND_BIT, 0x01 | 0x02);
    return true;
}

void MicroNatural_RGBC::flush()
{
    if(!config())
        return;
    rgbcRaw.c = readRegUint16(REG_CLEAR_CHANNEL_L | REG_TCS34725_COMMAND_BIT);
    rgbcRaw.r = readRegUint16(REG_RED_CHANNEL_L | REG_TCS34725_COMMAND_BIT);
    rgbcRaw.g = readRegUint16(REG_GREEN_CHANNEL_L | REG_TCS34725_COMMAND_BIT);
    rgbcRaw.b = readRegUint16(REG_BLUE_CHANNEL_L | REG_TCS34725_COMMAND_BIT);
    delay(50);
    unsigned char ret = readReg(REG_TCS34725_ENABLE | REG_TCS34725_COMMAND_BIT);
    ret |= TCS34725_ENABLE_AIEN;
    writeReg(REG_TCS34725_ENABLE | REG_TCS34725_COMMAND_BIT, ret);
}

unsigned long MicroNatural_RGBC::getClock()
{
    return iicFrequency;
}

unsigned char MicroNatural_RGBC::getAddress()
{
    return iicAddress;
}


// i2c
void DFRobot_MicroNatural::writeReg(unsigned char reg, unsigned char data)
{
    if(!init) {
        Wire.begin();
        init = true;
    }
    Wire.setClock(getClock());
    Wire.beginTransmission(getAddress());
    Wire.write(reg);
    Wire.write(data);
    Wire.endTransmission();
}

unsigned char DFRobot_MicroNatural::readReg(unsigned char reg)
{
    if(!init) {
        Wire.begin();
        init = true;
    }
    Wire.setClock(getClock());
    Wire.beginTransmission(getAddress());
    Wire.write(reg);
    Wire.endTransmission();
    Wire.requestFrom(getAddress(), 1);
    if (Wire.available()) {
        return (unsigned char)Wire.read();
    }
    return 0;
}

unsigned short int DFRobot_MicroNatural::readRegUint16(unsigned char reg)
{
    if(!init) {
        Wire.begin();
        init = true;
    }
    Wire.setClock(getClock());
    Wire.beginTransmission(getAddress());
    Wire.write(reg);
    Wire.endTransmission();
    Wire.requestFrom(getAddress(), 2);
    if (Wire.available()) {
        unsigned short int data_l = Wire.read();
        unsigned short int data_h = Wire.read();
        return (data_h<<8) | data_l;
    }
    return 0;
}


