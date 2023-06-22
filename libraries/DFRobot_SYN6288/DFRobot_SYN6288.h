/*
*名称：Speech Synthesizer Bee语音合成模块
*芯片：SYN6288
*SKU：SEN0117
*功能：实现文字到语音的转换，支持GB2312、GBK、BIG5、UNICODE、UTF-8编码。
*/
#ifndef DFROBOT_SYN6288_H
#define DFROBOT_SYN6288_H

#include <Arduino.h>
#include <HardwareSerial.h>
#include <SoftwareSerial.h>

class DFRobot_SYN6288
{
public:
    DFRobot_SYN6288();

    /*
    *芯片支持的文本编码
    */
    enum encoding {eGB2312, eGBK, eBIG5, eUNICODE};

    /*
    *brief:使用硬串口初始化主机和芯片
    *parms:pware 硬串口对象
    *parms:rx 硬串口的RX引脚
    *parms:tx 硬串口的TX引脚
    *parms:busy 芯片的状态引脚
    *parms:baud 初始化使用的波特率
    */
    void begin(HardwareSerial *pware, int rx, int tx, int busy, uint32_t baud = 9600);

    /*
    *brief:使用软串口初始化主机和芯片
    *parms:pware 软串口对象
    *parms:busy 芯片的状态引脚
    *parms:baud 初始化使用的波特率
    */
    void begin(SoftwareSerial *pware, int busy, uint32_t baud = 9600);

    /*
    *brief:播放文字和背景音乐
    *param:text uint16_t *待播放的文字编码数组
    *param:len 数组的字节数
    *param:text String& 待播放的原始文字，仅支持UTF-8与GBK编码
    *param:text float 待播放的数字
    *param:music 背景音乐 0 ~ 15，0 = 不播放
    */
    void playText(const uint16_t *text, uint32_t len, uint32_t music = 0);
    void playText(const String &text, uint32_t music = 0);
    void playText(float text, uint32_t music = 0);

    /*
    *brief:播放声音提示音
    *param:index 1 ~ 25
    *param:wait true = 等待播放完成，false = 不等待，继续向下执行
    */
    void playPrompt(uint32_t index, bool wait = true);

    /*
    *brief:播放和弦提示音
    *param:index 1 ~ 8
    *param:wait true = 等待播放完成，false = 不等待，继续向下执行
    */
    void playChord(uint32_t index, bool wait = true);

    /*
    *brief:播放和弦铃声
    *param:index 1 ~ 15
    *param:wait true = 等待播放完成，false = 不等待，继续向下执行
    */
    void playMusic(uint32_t index, bool wait = true);

    /*
    *brief:设置前景音量或背景音量
    *param:fore true = 前景音量，false = 背景音量
    *param:vol 0 ~ 16，0 = 静音
    */
    void setVolume(bool fore, uint32_t vol);

    /*
    *brief:获取当前音量值
    *param:fore true = 前景音量，false = 背景音量
    *return:音量值 0 ~ 16
    */
    int32_t getVolume(bool fore);

    /*
    *brief:设置朗读语速
    *param:speed 0 ~ 5
    */
    void setSpeed(uint32_t speed);

    /*
    *brief:获取当前语速值
    *return:语速值 0 ~ 5
    */
    int32_t getSpeed();

    /*
    *brief:设置通信的波特率
    *param:baud 9600(default),19200,38400(not recommended)
    */
    // void setRate(uint32_t baud = 9600);

    // #if defined (ESP_PLATFORM)
    // void setRate(int rx, int tx, uint32_t baud = 9600);
    // #endif

    /*
    *brief:设置文字编码
    *param:code eGB2312,eGBK,eBIG5,eUNICODE
    */
    void setCode(encoding code);

    /*
    *brief:停止播放
    */
    void stop();

    /*
    *brief:暂停播放
    */
    void pause();

    /*
    *brief:继续播放
    */
    void resume();

    /*
    *brief:判断芯片是否空闲
    *return:true = 芯片空闲，false = 正在播放
    */
    bool isIdle();
private:
    /*
    *通信波特率代号
    */
    // const uint8_t RATE_9600; //9600，芯片上电后的默认波特率
    // const uint8_t RATE_19200;//19200
    // const uint8_t RATE_38400;//38400，不稳定，不建议使用

    /*
    *命令字
    */
    const uint8_t CMD_HEAD;      //数据帧头
    const uint8_t CMD_SYNTHESIS; //语音合成播放命令
    const uint8_t CMD_STOPSYN;   //停止合成命令
    const uint8_t CMD_PAUSESYN;  //暂停合成命令
    const uint8_t CMD_RESTORESYN;//恢复合成命令
    const uint8_t CMD_CHIPSTATUS;//芯片状态查询命令
    const uint8_t CMD_BAUDRATE;  //设置芯片波特率命令
    const uint8_t CMD_POWERDOWN; //芯片进入低功耗命令

    /*
    *芯片回传消息
    */
    // const uint8_t INIT_OK;  //初始化成功
    // const uint8_t RECV_OK;  //接收成功
    // const uint8_t RECV_ERR; //接收失败
    // const uint8_t CHIP_BUSY;//芯片正忙
    // const uint8_t CHIP_IDLE;//芯片空闲

    /*
    *brief:根据标点符号提取最多100个字
    */
    void slice();

    /*
    *brief:向芯片发送数据
    */
    void writeData(uint8_t *head, uint8_t headlen, uint8_t *body, uint8_t bodylen);

    /*
    *brief:查找标点符号所在的位置索引
    */
    uint32_t indexOf();

    /*
    *brief:等待本次播放完成
    */
    void wait();

    /*
    *brief:停止播放，可能需要多次停止
    */
    void _stop();

    /*
    *brief:UTF-8转UNICODE
    *param:dat 指向UTF-8的第一个字节
    *param:len UTF-8的长度，2或3
    *return:对应的UNICODE
    */
    uint16_t utf8_unicode(uint8_t *dat, uint8_t len);

    /*
    *brief:encoding转uint8_t
    *param:code 需要转换的encoding成员
    *return:对应的uint8_t
    */
    uint8_t code2uint8_t(encoding code);

    /*
    *Some global variables
    */
    const uint16_t *_text;//待播放的文字编码数组
    uint32_t _len;        //待播放的文字数
    uint8_t text_[200];   //提取的最多100个字
    uint8_t len_;         //待播放的字节数
    uint32_t _start;      //播放的起始位置
    encoding _code;       //文字编码
    // uint8_t _rate;     //芯片波特率代号，非波特率数值
    int _busypin;         //状态引脚
    bool _begin;          //是否初始化
    uint8_t _fvolume;     //前景音量
    uint8_t _bvolume;     //背景音量
    uint8_t _speed;       //语速
    HardwareSerial *_hware;
    SoftwareSerial *_sware;
};

#endif
