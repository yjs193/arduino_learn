/*!
 * @file DFRobot_IICScan.cpp
 * @brief Define the basic structure of class DFRobot_IICScan
 * @copyright   Copyright (c) 2010 DFRobot Co.Ltd (http://www.dfrobot.com)
 * @licence     The MIT License (MIT)
 * @author [lixin](xin.li@dfrobot.com)
 * @version  V1.6.2
 * @date  2019-11-14
 */


#include <DFRobot_IICScan.h>


/**
 * Constructor
 * @brief Create an instance of DFRobot_IICScan
 */
DFRobot_IICScan::DFRobot_IICScan(){
#if !defined(ARDUINO_ARCH_RP2040)
    Wire.begin();
#endif
}

/**
  * @brief 扫描I2C地址api
  *
  * @return Return 以字符串形式返回十六进制格式的地址，例如：'0xAA'
  */
String DFRobot_IICScan::scan()
{
#if defined(ARDUINO_ARCH_RP2040)
    Wire.begin();
#endif
    String str;
    int nDevices = 0;
    for(uint8_t address = 1; address < 127; address++ ) {
        Wire.beginTransmission(address);
        if (Wire.endTransmission() == 0){
#if defined(NRF5) || defined(NRF52833)
            if((address==0x0D)||(address==0x0E)||(address==0x1D)){
                continue;
            }
#endif
#if defined(ESP32)
            if((address==0x6B)||(address==0x7E)){
                continue;
            }
#endif
            nDevices++;
            if(nDevices > 1) {
                str += ",";
            }
            str += format(address);
        }
    }
    return str;
}

/**
  * @brief 将数字格式化成字符串
  *
  * @param data: 一个字节的数据
  *
  * @return Return 返回字符串，例如：'0xAA'
  */
String DFRobot_IICScan::format(uint8_t data)
{
    char buf[10];
    sprintf(buf, "0x%02X", data);
    return String(buf);
}



