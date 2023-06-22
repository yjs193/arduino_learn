#ifndef DF_MPYTHON_TASK_H
#define DF_MPYTHON_TASK_H
#include "Arduino.h"
#ifdef __AVR_ATmega2560__
#include <Arduino_FreeRTOS.h>
#include <queue.h>
#endif

#define TASK_START true
#define TASK_FREE false
//最多允许8个task
#define MPYTHON_TASK_USER_DEFINE_NUM_MAX  8

typedef void (*taskCallbackPt)
#if defined(NRF5) || defined(NRF52833)
(void);
#elif defined(ESP32)
(void *param);
#else
(void);
#endif

// 启动task
extern  void mpythonFiberStart(String name);
// 释放task
extern  void mpythonFiberFree(String name);
// 清除标志
extern  void mpythonFiberFlagClean(String name);


class DFRobot_Task
{
    public:
    DFRobot_Task(taskCallbackPt callback, const char *name);
    ~DFRobot_Task();

    void start();

    private:
};

#define combination(sta,dynamic)  sta##dynamic
#define toStr(a) #a

//定义一个task class
#define newClassBegin(mytask)\
        class mytask{ public: mytask(){}; void setup(); void loop();
#define newClassEnd(mytask) }; mytask mytask;
//定义task执行函数
#if defined(NRF5) || defined(NRF52833)
#define newStaicCallback(mytask)\
        static void combination(mytask,callback)(void) { mytask.setup();while (1)\
        { mytask.loop(); delay(20); } }
#elif defined(ESP32)
#define newStaicCallback(mytask)\
        static void combination(mytask,callback)(void *param) { mytask.setup();while (1)\
        { mytask.loop(); yield(); } }
#elif defined(__AVR_ATmega2560__)
#define newStaicCallback(mytask)\
        static void combination(mytask,callback)(void *param) { mytask.setup();while (1)\
        { mytask.loop(); yield(); } }
#endif
//添加到task队列
#define newAddToqueue(mytask) DFRobot_Task combination(bbc_,mytask)\
        (&combination(mytask,callback),(const char *)toStr(mytask));
#define newSetupLoop(begin,end,callback,queue) begin end callback queue
//用户创建一个新的task  例: newTask(task1)  void task1::setup(){ }  void task1::loop(){ }
#define newTask(...) newSetupLoop( newClassBegin(__VA_ARGS__),\
        newClassEnd(__VA_ARGS__),\
        newStaicCallback(__VA_ARGS__),\
        newAddToqueue(__VA_ARGS__));
//释放task  例: taskFree()//释放所有task    taskFree(task1)//释放task1
#define taskFree(...) mpythonFiberFree(toStr(__VA_ARGS__));
//启动task  例: taskStart()//启动所有task    taskStart(task1)//启动task1
//#define taskStart(...) mpythonFiberFree(toStr(__VA_ARGS__)); mpythonFiberStart(toStr(__VA_ARGS__));
#define taskStart(...) mpythonFiberStart(toStr(__VA_ARGS__));


#endif

