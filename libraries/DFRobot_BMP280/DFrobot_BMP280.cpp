/*!
 * @file DFRobot_BMP280.cpp
 * @brief DFRobot's DFRobot_BMP280
 * @n DFRobot's Temperature、Pressure and Approx altitude
 *
 * @copyright	[DFRobot](http://www.dfrobot.com), 2016
 * @copyright	GNU Lesser General Public License
 *
 * @author [yuxiang](1137717512@qq.com)
 * @version  V1.0
 * @date  2016-12-6
 */
#include "DFRobot_BMP280.h"


/***************************************************************************
 PRIVATE FUNCTIONS
 ***************************************************************************/


DFRobot_BMP280::DFRobot_BMP280()
{ }

bool first = true;
bool DFRobot_BMP280::begin(uint8_t a, uint8_t chipid) {
  enable = false; 
  uint8_t num = 0;
  this->chipid = chipid;
  _i2caddr = a; 
    // i2c
  Wire.begin();
  
  if(first){

      while (!this->config()) {
          delay(1000);
          num++;
          if(num > 5) return false;//timeout
      }

      first = false;
  }
  enable = true;
  this->seaLevel = readSeaLevel(525.0);
  return true;
}

bool DFRobot_BMP280::config()
{
  if (read8(BMP280_REGISTER_CHIPID) != this->chipid){
      _i2caddr = BMP280_ADDRESS1;
      if(read8(BMP280_REGISTER_CHIPID) != this->chipid){
         return false;  
      }
  }
  getCoefficients();
  write8(BMP280_REGISTER_CONTROL, 0x3F);

  return true;
}

/**************************************************************************/
/*!
    @brief  Writes an 8 bit value over I2C
*/
/**************************************************************************/
void DFRobot_BMP280::write8(BYTE reg, BYTE value)
{
  Wire.beginTransmission((uint8_t)_i2caddr);
  Wire.write((uint8_t)reg);
  Wire.write((uint8_t)value);
  Wire.endTransmission();
}

/**************************************************************************/
/*!
    @brief  Reads an 8 bit value over I2C
*/
/**************************************************************************/
uint8_t DFRobot_BMP280::read8(BYTE reg)
{
  uint8_t value;

  Wire.beginTransmission((uint8_t)_i2caddr);
  Wire.write((uint8_t)reg);
  Wire.endTransmission();
  Wire.requestFrom((uint8_t)_i2caddr, (BYTE)1);
  value = Wire.read();
	
  return value;
}

/**************************************************************************/
/*!
    @brief  Reads a 16 bit value over I2C
*/
/**************************************************************************/
uint16_t DFRobot_BMP280::read16(BYTE reg)
{
  uint16_t value;

  Wire.beginTransmission((uint8_t)_i2caddr);
  Wire.write((uint8_t)reg);
  Wire.endTransmission();
  Wire.requestFrom((uint8_t)_i2caddr, (BYTE)2);
  value = (Wire.read() << 8) | Wire.read();

  return value;
}

uint16_t DFRobot_BMP280::read16_LE(BYTE reg) {
  uint16_t temp = read16(reg);
  return (temp >> 8) | (temp << 8);
}

/**************************************************************************/
/*!
    @brief  Reads a signed 16 bit value over I2C
*/
/**************************************************************************/
int16_t DFRobot_BMP280::readS16(BYTE reg)
{
  return (int16_t)read16(reg);
}

int16_t DFRobot_BMP280::readS16_LE(BYTE reg)
{
  return (int16_t)read16_LE(reg);
}


/**************************************************************************/
/*!
    @brief  Reads a signed 16 bit value over I2C
*/
/**************************************************************************/

uint32_t DFRobot_BMP280::read24(BYTE reg)
{
  uint32_t value;

  Wire.beginTransmission((uint8_t)_i2caddr);
  Wire.write((uint8_t)reg);
  Wire.endTransmission();
  Wire.requestFrom((uint8_t)_i2caddr, (BYTE)3);
    
  value = Wire.read();
  value <<= 8;
  value |= Wire.read();
  value <<= 8;
  value |= Wire.read();

  return value;
}

/**************************************************************************/
/*!
    @brief  Reads the factory-set coefficients
*/
/**************************************************************************/
void DFRobot_BMP280::getCoefficients(void)
{
  _bmp280Calib.digT1 = read16_LE(BMP280_REGISTER_DIG_T1);
  _bmp280Calib.digT2 = readS16_LE(BMP280_REGISTER_DIG_T2);
  _bmp280Calib.digT3 = readS16_LE(BMP280_REGISTER_DIG_T3);

  _bmp280Calib.digP1 = read16_LE(BMP280_REGISTER_DIG_P1);
  _bmp280Calib.digP2 = readS16_LE(BMP280_REGISTER_DIG_P2);
  _bmp280Calib.digP3 = readS16_LE(BMP280_REGISTER_DIG_P3);
  _bmp280Calib.digP4 = readS16_LE(BMP280_REGISTER_DIG_P4);
  _bmp280Calib.digP5 = readS16_LE(BMP280_REGISTER_DIG_P5);
  _bmp280Calib.digP6 = readS16_LE(BMP280_REGISTER_DIG_P6);
  _bmp280Calib.digP7 = readS16_LE(BMP280_REGISTER_DIG_P7);
  _bmp280Calib.digP8 = readS16_LE(BMP280_REGISTER_DIG_P8);
  _bmp280Calib.digP9 = readS16_LE(BMP280_REGISTER_DIG_P9);
}

/**************************************************************************/
/*!

*/
/**************************************************************************/
float DFRobot_BMP280::getTemperature(void)
{
  if (!enable) {
    return 0;
  }
  int32_t var1, var2;

  int32_t adc_T = read24(BMP280_REGISTER_TEMPDATA);
  adc_T >>= 4;

  var1  = ((((adc_T>>3) - ((int32_t)_bmp280Calib.digT1 <<1))) *
	   ((int32_t)_bmp280Calib.digT2)) >> 11;

  var2  = (((((adc_T>>4) - ((int32_t)_bmp280Calib.digT1)) *
	     ((adc_T>>4) - ((int32_t)_bmp280Calib.digT1))) >> 12) *
	   ((int32_t)_bmp280Calib.digT3)) >> 14;

  t_fine = var1 + var2;

  float T  = (t_fine * 5 + 128) >> 8;
  return T/100;
}

/**************************************************************************/
/*!

*/
/**************************************************************************/
float DFRobot_BMP280::getPressure(void) {
  if (!enable) {
    return 0;
  }
  int64_t var1, var2, p;

  // Must be done first to get the t_fine variable set up
  getTemperature();

  int32_t adc_P = read24(BMP280_REGISTER_PRESSUREDATA);
  adc_P >>= 4;

  var1 = ((int64_t)t_fine) - 128000;
  var2 = var1 * var1 * (int64_t)_bmp280Calib.digP6;
  var2 = var2 + ((var1*(int64_t)_bmp280Calib.digP5)<<17);
  var2 = var2 + (((int64_t)_bmp280Calib.digP4)<<35);
  var1 = ((var1 * var1 * (int64_t)_bmp280Calib.digP3)>>8) +
    ((var1 * (int64_t)_bmp280Calib.digP2)<<12);
  var1 = (((((int64_t)1)<<47)+var1))*((int64_t)_bmp280Calib.digP1)>>33;

  if (var1 == 0) {
    return 0;  // avoid exception caused by division by zero
  }
  p = 1048576 - adc_P;
  p = (((p<<31) - var2)*3125) / var1;
  var1 = (((int64_t)_bmp280Calib.digP9) * (p>>13) * (p>>13)) >> 25;
  var2 = (((int64_t)_bmp280Calib.digP8) * p) >> 19;

  p = ((p + var1 + var2) >> 8) + (((int64_t)_bmp280Calib.digP7)<<4);
  return (float)p/256;
}

float DFRobot_BMP280::readSeaLevel(float altitude)
{
  float pressure = getPressure();
  return (pressure / pow(1.0 - (altitude / 44330.0), 5.255));
}

float DFRobot_BMP280::getAltitude() {
  if (!enable) {
    return 0;
  }
  float pressure = getPressure();
  return (1.0 - pow(pressure / /*101325 ?????525.0*/this->seaLevel, 0.190284)) * 287.15 / 0.0065;
}
