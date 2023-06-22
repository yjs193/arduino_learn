/*!
 * @file MPython_Display_I2C.cpp
 * @brief Define the basic structure of class MPython_Display_I2C
 * @copyright   Copyright (c) 2010 DFRobot Co.Ltd (http://www.dfrobot.com)
 * @licence     The MIT License (MIT)
 * @author [lixin](xin.li@dfrobot.com)
 * @version  V1.6.2
 * @date  2019-11-14
 */


#include "MPython_Display_I2C.h"


/**
  * Class definition for MPython_Display_I2C.
  *
  * Represents an instance of DFRobot_SSD1306 that accepts the I2C version of SSD1306
  */
MPython_Display_I2C::MPython_Display_I2C()
{

}

/**
  * @brief Set screen cache refresh range
  *
  * @param address: 8-bit I2C slave address [ addr | 0 ]
  *
  * @param reg: The address of the register in the device to write
  *
  * @param columnStart: Column start position
  *
  * @param columnEnd: End of column
  *
  * @param rowStart: Line start position
  *
  * @param rowEnd: End of line
  */
inline void MPython_Display_I2C::setRefreshRange(int address, uint8_t reg, int columnStart, int columnEnd, int rowStart, int rowEnd)
{
    // send a bunch of data in one xmission
    writeByte(address, reg, 0xB0 + rowStart);//set page address
    writeByte(address, reg, (2+columnStart) & 0xf);//set lower column address
    writeByte(address, reg, 0x10 | ((2+columnStart) >> 4));//set higher column address
}


/**
  * @brief Write data to the register address in batches
  *
  * @param address: 8-bit I2C slave address [ addr | 0 ]
  *
  * @param reg: The address of the register in the device to write
  *
  * @param data: A pointer to a byte buffer containing the data to write
  *
  * @param length: The number of bytes to read
  *
  * @return Return the number of data
  */
inline int MPython_Display_I2C::writeBuffer(int address, uint8_t reg, unsigned char *data, int length)
{
    int count=0;
    SSD1306_I2CBEGIN();
    Wire.beginTransmission(address);
    Wire.write(reg);
    while(count<length) {
        Wire.write(*data);
        data++;
        count++;
    }
    Wire.endTransmission();
    return count;
}

/**
  * @brief Write a data to the register address
  *
  * @param address: 8-bit I2C slave address [ addr | 0 ]
  *
  * @param reg: The address of the register in the device to write.
  *
  * @param Data: to be written
  *
  * @return Return the number of data
  */
inline int MPython_Display_I2C::writeByte(uint8_t address, uint8_t reg, uint8_t data)
{
    SSD1306_I2CBEGIN();
    Wire.beginTransmission(address);
    Wire.write(reg);
    Wire.write(data);
    Wire.endTransmission();
    return 1;
}
