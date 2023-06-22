/*
*名称：IIC EEPROM数据管理模块
*芯片：24LC256
*SKU：DFR0117
*功能：采用Key—Value的形式存放数据，支持String和float两种数据类型
*
*数据包格式规定如下（如格式错乱可用VS Code查看）
*┏━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┳━━━━━━━━━━━━━┓
*┃                       Key                       ┃    Value    ┃
*┣━━━━━━┳━━━━┳━━━━━━━━━━━┳━━━━━━━━━━━┳━━━━━━┳━━━━━━╋━━━━━━┳━━━━━━┫
*┃ next ┃type┃name length┃data length┃ name ┃check1┃ data ┃check2┃
*┣━━━━━━╋━━━━╋━━━━━━━━━━━╋━━━━━━━━━━━╋━━━━━━╋━━━━━━╋━━━━━━╋━━━━━━┫
*┃2bytes┃1bit┃   7bits   ┃  2bytes   ┃nbytes┃1byte ┃nbytes┃1byte ┃
*┣━━━━━━┻━━━━┻━━━━━━━━━━━┻━━━━━━━━━━━┻━━━━━━┻━━━━━━╋━━━━━━┻━━━━━━┫
*┃addr1                                            ┃addr2        ┃
*┗━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┻━━━━━━━━━━━━━┛
*
*一个数据包为链表中的一个结点，各部分含义如下
*
*       next：下一个结点的起始地址
*       type：数据类型，0 = String，1 = float
*name length：name的长度，0 ~ 127
*data length：数据体的长度
*       name：数据名称
*     check1：Key和addr1的异或校验
*       data：数据体
*     check2：Value和addr2的异或校验
*      addr1：Key的起始地址
*      addr2：Value的起始地址
*
*一个数据包至少要占用7个字节
*芯片的前两个字节保存第一个结点的地址
*/
#ifndef DFROBOT_24LC256_H
#define DFROBOT_24LC256_H

#include <Arduino.h>
#include <Wire.h>

// #define ENABLE_DBG

#ifdef ENABLE_DBG
#define DBG(...) {Serial.print("["); Serial.print(__FUNCTION__); Serial.print("(): "); Serial.print(__LINE__); Serial.print(" ] "); Serial.println(__VA_ARGS__);}
#else
#define DBG(...)
#endif

class DFRobot_24LC256
{
public:
    DFRobot_24LC256();

    bool begin(uint8_t device);//初始化EEPROM
    bool exists(const String &name);//判断结点是否存在

    void write(const String &name, const String &value);//写文本
    void write(const String &name, float value);//写数字

    void format();//伪格式化EEPROM，数据没有真正删除
    void remove(const String &name);//移除结点，数据没有真正删除
    void printNameList(uint32_t baud = 9600);//串口打印所有结点名称和数据

    bool isString(const String &name);//判断结点是否是文本
    bool isNumber(const String &name);//判断结点是否是数字

    String readString(const String &name);//读取文本
    float readNumber(const String &name);//读取数字

    uint32_t getCount();//获取结点个数
    uint32_t getFreeStorage();//获取剩余空间，最大为器件容量-2
    uint32_t getCapacity();//获取器件容量大小
private:
    struct EEPROM//Key的结构体
    {      /*名称               含义         占用空间*/
        uint16_t start;     //起始地址(addr1)   0
        uint16_t end;       //结束地址          0
        uint16_t value;     //数据地址(addr2)   0
        uint16_t rest;      //剩余空间          0
        uint16_t next;      //下个结点          2
        uint8_t type;       //数据类型          1
        uint16_t length;    //数据长度          2
        String name;        //结点名称          0 ~ 127
        EEPROM(uint16_t data = 0) : start(data) {}
    };

    uint16_t readFirst();//读第一个结点的地址
    uint8_t getType(const String &name);//获取结点数据类型
    bool requestStorage(EEPROM *key, uint16_t size);//申请一块空间
    bool search(EEPROM *key, const String &name);//根据名称查找结点

    String readString(uint16_t addr, uint16_t len);//读取文本
    float readNumber(uint16_t addr);//读取数字

    void writeBytes(uint16_t addr, const uint16_t &data);//写两个字节
    void writeBytes(uint16_t addr, const uint8_t *data, uint16_t len);//写任意字节

    uint16_t readBytes(uint16_t addr);//读两个字节
    void readBytes(uint16_t addr, uint8_t *data, uint16_t len);//读任意字节

    void writeKey(EEPROM *key);//写Key
    bool readKey(EEPROM *key);//读Key，返回值代表校验值是否正确

    void writeValue(uint16_t addr, const uint8_t *data, uint16_t len);//写Value
    bool readValue(uint16_t addr, uint8_t *data, uint16_t len);//读Value，返回值代表校验值是否正确

    String floatToString(float value);//浮点数转字符串
    void adjust(EEPROM *key, uint16_t len);//计算Key的数据地址、结束地址和剩余空间
    bool deleteError(EEPROM *key, uint16_t prior);//删除损坏的结点

    uint8_t _device; //器件IIC地址
    bool _begin;     //初始化是否成功
    bool _lastStatus;//通信是否正常

    volatile uint16_t WRITE_SIZE;//一次写入的缓冲区大小，只能为8或16
    volatile uint16_t READ_SIZE; //一次读取的缓冲区大小，不大于30
    volatile uint16_t TOTAL_SIZE;//器件的容量(Bytes)
};

#endif
