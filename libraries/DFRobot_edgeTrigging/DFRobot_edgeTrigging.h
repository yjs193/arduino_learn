/*
 * @Author: LiXin 
 * @Date: 2018-09-30 09:54:51 
 * @Last Modified by: LiXin
 * @Last Modified time: 2018-09-30
 */
#ifndef _DF_MICROBIT_THREADS_H
#define _DF_MICROBIT_THREADS_H
#include <Arduino.h>



//最多同时支持22个通道
#define EDGE_HANDLE_NUM_MAX_NRF 22
#define EDGE_HANDLE_NUM_MAX_ESP 40

//边沿模式
enum Edge{
    EDGE_RISING,
    EDGE_FALLING,
    EDGE_CHANGE,
    EDGE_NONE
};

//边沿触发回调函数
typedef void (*taskHandleCb) (void);

class DFRobot_edgeTrigging {
    public:

    /** 
     * @brief   构造函数,初始化内部参数
     * @return: 无
     */
    DFRobot_edgeTrigging();

    /** 
     * @brief 析构函数
     * @return: 无
     */
    ~DFRobot_edgeTrigging();

    /** 
     * @brief   设置引脚、设置模式以及注册回调函数
     * @param   pin:引脚
     * @param   e:事件触发模式
     * @param:  task:回掉函数
     * @return: 无
     */
    void registerEvent(uint8_t pin, Edge e, taskHandleCb task);

    /** 
     * @brief   释放引脚
     * @param   pin:引脚
     * @return: 无
     */
    void releaseEvent(uint8_t pin);
#if defined(NRF5) || defined(NRF52833)    //ESP32
    /** 
     * @brief   判断是否触发边沿模式
     * @param   pin:引脚
     * @return: 模式
     */
    Edge edgeJudgment(uint8_t pin);



    //是否是第一次注册，如果是将开启后台子线程
    bool firstRegister = true;
    //上升沿回调函数指针
    taskHandleCb CbsRising[22];
    //下降沿回调函数指针
    taskHandleCb CbsFalling[22];
    //上升沿注册标志
    bool EdgesRising[22];
    //下降沿注册标志
    bool EdgesFalling[22];
    //引脚电平状态
    uint8_t pinState[22];
    //上次引脚电平状态
    uint8_t pinStateOld[22];
    //引脚注册标志
    int pins[22];
    //通道占用个数
    int channelNum = 0;
#endif

#ifdef ESP32
    /** 
     * @brief   判断是否触发边沿模式
     * @param   pin:引脚
     * @return: 模式
     */
    Edge edgeJudgment(uint8_t pin);



    //是否是第一次注册，如果是将开启后台子线程
    bool firstRegister = true;
    //上升沿回调函数指针
    taskHandleCb CbsRising[40];
    //下降沿回调函数指针
    taskHandleCb CbsFalling[40];
    //上升沿注册标志
    bool EdgesRising[40];
    //下降沿注册标志
    bool EdgesFalling[40];
    //引脚电平状态
    uint8_t pinState[40];
    //上次引脚电平状态
    uint8_t pinStateOld[40];
    //引脚注册标志
    int pins[40];
    //通道占用个数
    int channelNum = 0;
#endif

};

#endif

