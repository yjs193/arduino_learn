/*
 * @Author: Jason 
 * @Date: 2018-02-09 09:54:28 
 * @Last Modified by: Jason
 * @Last Modified time: 2018-02-09 17:56:19
 */
#include "DFRobot_edgeTrigging.h"
//#ifdef NRF5
//#include "basic.h"#ifdef NRF5
//#endif

//#if defined(NRF5)
#if defined(NRF5) || defined(NRF52833) 
#include "basic.h"
#elif defined(ESP32)
#if CONFIG_FREERTOS_UNICORE
#define ARDUINO_RUNNING_CORE 0
#else
#define ARDUINO_RUNNING_CORE 1
#endif
#endif


DFRobot_edgeTrigging *privateEdgeTrigging = NULL;

DFRobot_edgeTrigging::DFRobot_edgeTrigging()
{
//#ifdef NRF5
#if defined(NRF5) || defined(NRF52833) 
    privateEdgeTrigging = this;

    //初始化内部参数
    for(int i = 0; i < EDGE_HANDLE_NUM_MAX_NRF; i++)
    {
        CbsRising[i] = NULL;
        CbsFalling[i] = NULL;
        EdgesRising[i] = false;
        EdgesFalling[i] = false;
        pins[i] = -1;
        pinState[i] = 2;
        pinStateOld[i] = 2;
    }
#endif

#ifdef ESP32
    privateEdgeTrigging = this;

    //初始化内部参数
    for(int i = 0; i < EDGE_HANDLE_NUM_MAX_ESP; i++)
    {
        CbsRising[i] = NULL;
        CbsFalling[i] = NULL;
        EdgesRising[i] = false;
        EdgesFalling[i] = false;
        pins[i] = -1;
        pinState[i] = 2;
        pinStateOld[i] = 2;
    }
#endif

}

DFRobot_edgeTrigging::~DFRobot_edgeTrigging(){}
#ifdef ESP32
static void edgehandle(void *param)
{
    bool pstate;
    while(privateEdgeTrigging->channelNum){//如果通道被打开，开始循环查询, 否则子线程退出
        for(int i = 0; i < EDGE_HANDLE_NUM_MAX_ESP; i++){//查询所有通道是否有事件触发
            if(privateEdgeTrigging->pins[i] == 1){//判断这个通道是否被打开
                pstate = digitalRead(i);//获取引脚状态
                delay(20);
                if (pstate == digitalRead(i)){
                    privateEdgeTrigging->pinState[i] = pstate;
                    Edge edgeHandle = privateEdgeTrigging->edgeJudgment(i);//获取边沿模式
                    if(edgeHandle == EDGE_RISING) {//判断上升沿事件是否被触发
                        if(privateEdgeTrigging->CbsRising[i]) privateEdgeTrigging->CbsRising[i]();//调用回调函数
                    }else if(edgeHandle == EDGE_FALLING){//判断下降沿事件是否被触发
                        if(privateEdgeTrigging->CbsFalling[i]) privateEdgeTrigging->CbsFalling[i]();//调用回调函数
                    }
                }  
            }
        }
        delay(1);//边沿触发分辨率>1ms
    }
    vTaskDelete(NULL);
}
Edge DFRobot_edgeTrigging::edgeJudgment(uint8_t pin)
{
    if(pinState[pin] == pinStateOld[pin]) return EDGE_NONE; //如果电平状态没发生改变，返回空模式
    if(EdgesRising[pin] == true && !pinStateOld[pin] && pinStateOld[pin] != 2){
        pinStateOld[pin] = pinState[pin];
        return EDGE_RISING;//如果电平状态由低跳转到高，则返回上升沿模式
    }
    if(EdgesFalling[pin] == true && pinStateOld[pin] && pinStateOld[pin] != 2){
        pinStateOld[pin] = pinState[pin];
        return EDGE_FALLING;//如果电平状态由高跳转到低，则返回上升沿模式
    }
    pinStateOld[pin] = pinState[pin];//记录本次电平状态
    return EDGE_NONE;//其他情况返回空模式
}
#endif
//#ifdef NRF5
#if defined(NRF5) || defined(NRF52833) 
static void edgehandle()
{
    bool pstate;
    while(privateEdgeTrigging->channelNum){//如果通道被打开，开始循环查询, 否则子线程退出
        for(int i = 0; i < EDGE_HANDLE_NUM_MAX_NRF; i++){//查询所有通道是否有事件触发
            if(privateEdgeTrigging->pins[i] == 1){//判断这个通道是否被打开
                pstate = digitalRead(i);//获取引脚状态
                delay(20);
                if (pstate == digitalRead(i)){
                    privateEdgeTrigging->pinState[i] = pstate;
                    Edge edgeHandle = privateEdgeTrigging->edgeJudgment(i);//获取边沿模式
                    if(edgeHandle == EDGE_RISING) {//判断上升沿事件是否被触发
                        if(privateEdgeTrigging->CbsRising[i]) privateEdgeTrigging->CbsRising[i]();//调用回调函数
                    }else if(edgeHandle == EDGE_FALLING){//判断下降沿事件是否被触发
                        if(privateEdgeTrigging->CbsFalling[i]) privateEdgeTrigging->CbsFalling[i]();//调用回调函数
                    }
                }           
            }
        }
#if defined(NRF5)
        fiber_sleep(1);//边沿触发分辨率>1ms
#elif defined(NRF52833)
        fiber_sleep(1);//边沿触发分辨率>1ms
		// yield();    wsq   
#endif
    }
}

Edge DFRobot_edgeTrigging::edgeJudgment(uint8_t pin)
{
    if(pinState[pin] == pinStateOld[pin]) return EDGE_NONE; //如果电平状态没发生改变，返回空模式
    if(EdgesRising[pin] == true && !pinStateOld[pin] && pinStateOld[pin] != 2){
        pinStateOld[pin] = pinState[pin];
        return EDGE_RISING;//如果电平状态由低跳转到高，则返回上升沿模式
    }
    if(EdgesFalling[pin] == true && pinStateOld[pin] && pinStateOld[pin] != 2){
        pinStateOld[pin] = pinState[pin];
        return EDGE_FALLING;//如果电平状态由高跳转到低，则返回上升沿模式
    }
    pinStateOld[pin] = pinState[pin];//记录本次电平状态
    return EDGE_NONE;//其他情况返回空模式
}
#endif

void DFRobot_edgeTrigging::registerEvent(uint8_t pin, Edge e, taskHandleCb task)
{
//#if defined(NRF5)
#if defined(NRF5) || defined(NRF52833) 
    if(pin>21) return;

    //注册边沿模式和回调函数
    if(e == EDGE_RISING){
        this->CbsRising[pin] = task;
        this->EdgesRising[pin] = true;
    }else if(e == EDGE_FALLING){
        this->CbsFalling[pin] = task;
        this->EdgesFalling[pin] = true;
    }else{
        this->CbsRising[pin] = NULL;
        this->CbsFalling[pin] = NULL;
    }
    //判断这个引脚是否被第一次注册，如果是，通道数量+1
    if(this->pins[pin] == -1) channelNum++;
    //打开通道
    this->pins[pin] = 1;

    if(firstRegister) {
        //如果是第一次注册，需要打开子线程任务
        create_fiber(edgehandle);
        firstRegister = false;
    }
#elif defined(ESP32)
    if(pin>40) return;
    //注册边沿模式和回调函数
    if(e == EDGE_RISING){
        this->CbsRising[pin] = task;
        this->EdgesRising[pin] = true;
    }else if(e == EDGE_FALLING){
        this->CbsFalling[pin] = task;
        this->EdgesFalling[pin] = true;
    }else{
        this->CbsRising[pin] = NULL;
        this->CbsFalling[pin] = NULL;
    }
    //判断这个引脚是否被第一次注册，如果是，通道数量+1
    if(this->pins[pin] == -1) channelNum++;
    //打开通道
    this->pins[pin] = 1;

    if(firstRegister) {
        //如果是第一次注册，需要打开子线程任务
        xTaskCreatePinnedToCore(edgehandle, "edgeTrigging", 2048, this, 1, NULL, ARDUINO_RUNNING_CORE);//注册回调函数
        firstRegister = false;
    }
#else
    pinMode(pin,INPUT);
    if(e == EDGE_RISING)
        attachInterrupt(digitalPinToInterrupt(pin),task,RISING);
    else if(e == EDGE_FALLING)
        attachInterrupt(digitalPinToInterrupt(pin),task,FALLING);
    else if(e == EDGE_CHANGE)
        attachInterrupt(digitalPinToInterrupt(pin),task,CHANGE);
#endif
}


void DFRobot_edgeTrigging::releaseEvent(uint8_t pin)
{
//#if defined(NRF5)
#if defined(NRF5) || defined(NRF52833) 
    //减小一个通道占用数量
    if(this->CbsRising[pin] != NULL || this->CbsFalling[pin] != NULL)
        channelNum--;
    //通道参数复位
    this->CbsRising[pin] = NULL;
    this->CbsFalling[pin] = NULL;
    this->EdgesRising[pin] = false;
    this->EdgesFalling[pin] = false;
    this->pinState[pin] = 2;
    this->pinStateOld[pin] = 2;
    this->pins[pin] = -1;
    if(channelNum == 0)
        //如果通道全部被释放，则需要关闭子线程，
        //第一次记录标志为真，下次注册需要重新打开子线程
		release_fiber();
        firstRegister = true;
#elif defined(ESP32)
    //减小一个通道占用数量
    if(this->CbsRising[pin] != NULL || this->CbsFalling[pin] != NULL)
        channelNum--;
    //通道参数复位
    this->CbsRising[pin] = NULL;
    this->CbsFalling[pin] = NULL;
    this->EdgesRising[pin] = false;
    this->EdgesFalling[pin] = false;
    this->pinState[pin] = 2;
    this->pinStateOld[pin] = 2;
    this->pins[pin] = -1;
    if(channelNum == 0)
        //如果通道全部被释放，则需要关闭子线程，
        //第一次记录标志为真，下次注册需要重新打开子线程
        firstRegister = true;
#else
//#if defined(ESP32)
    //pin = pins_remap_esp32[pin];
//#endif
    detachInterrupt(digitalPinToInterrupt(pin));
#endif
}

