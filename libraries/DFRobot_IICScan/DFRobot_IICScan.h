/*!
 * @file DFRobot_IICScan.h
 * @brief Define the basic structure of class DFRobot_IICScan
 * @copyright   Copyright (c) 2010 DFRobot Co.Ltd (http://www.dfrobot.com)
 * @licence     The MIT License (MIT)
 * @author [lixin](xin.li@dfrobot.com)
 * @version  V1.6.2
 * @date  2019-11-14
 */


#ifndef DFROBOT_IICSCAN_H
#define DFROBOT_IICSCAN_H

#include <Arduino.h>
#include <Wire.h>



/**
  * Class definition for DFRobot_IICScan.
  *
  * 这个库用于扫描I2C地址，适用于所有arduino主控
  */
class DFRobot_IICScan
{
public:

    /**
     * Constructor
     * @brief Create an instance of DFRobot_IICScan
     */
    DFRobot_IICScan();

    /**
      * @brief 扫描I2C地址api
      *
      * @return Return 以字符串形式返回十六进制格式的地址，例如：'0xAA'
      */
    String scan();

private:

    /**
      * @brief 将数字格式化成字符串
      *
      * @param data: 一个字节的数据
      *
      * @return Return 返回字符串，例如：'0xAA'
      */
    String format(uint8_t data);
};

#endif // DFROBOT_IICSCAN_H


