#include "DFRobot_Task.h"

#if defined(NRF5) || defined(NRF52833)
#include "basic.h"
#elif defined(ESP32)
#if CONFIG_FREERTOS_UNICORE
#define ARDUINO_RUNNING_CORE 0
#else
#define ARDUINO_RUNNING_CORE 1
#endif
#endif

//task回调函数
taskCallbackPt taskCallback[MPYTHON_TASK_USER_DEFINE_NUM_MAX] = {NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL};
//task 状态标志
bool taskState[MPYTHON_TASK_USER_DEFINE_NUM_MAX] = {TASK_FREE,TASK_FREE,TASK_FREE,TASK_FREE,TASK_FREE,TASK_FREE,TASK_FREE,TASK_FREE};
//用户设置的子线程名称
#if defined(NRF5) || defined(NRF52833)
char taskName[MPYTHON_TASK_USER_DEFINE_NUM_MAX][20];
#else
String taskName[MPYTHON_TASK_USER_DEFINE_NUM_MAX];
#endif
//task队列
#if defined(NRF5) || defined(NRF52833)
Fiber *
#elif defined(ESP32)
TaskHandle_t
#elif defined(__AVR_ATmega2560__)
TaskHandle_t
#endif


registeredFiber[MPYTHON_TASK_USER_DEFINE_NUM_MAX];


void mpythonFiberStart(String name)
{
    if(name != "")//启动一个task
    {
        for(int i=0; i<MPYTHON_TASK_USER_DEFINE_NUM_MAX; i++)
        {
            String str = taskName[i];
            if(str == name && taskCallback[i] && !taskState[i])
            {
                #if defined(NRF5) || defined(NRF52833)
                Fiber *f = create_fiber(taskCallback[i]);//注册回调函数
                if(!f){
                    taskCallback[i] = NULL;
                    registeredFiber[i] = NULL;
                    taskState[i] = TASK_FREE;
                    break;
                }
                registeredFiber[i] = f;
                #elif defined(ESP32)
                BaseType_t f = xTaskCreatePinnedToCore(taskCallback[i], name.c_str(), 2048, NULL, 1, &registeredFiber[i], ARDUINO_RUNNING_CORE);//注册回调函数
                if(f != pdPASS){
                    taskCallback[i] = NULL;
                    taskState[i] = TASK_FREE;
                    break;
                }
				#elif defined(__AVR_ATmega2560__)
			    BaseType_t f =  xTaskCreate(taskCallback[i],name.c_str(),
				2048,NULL, 0,&registeredFiber[i]);
				if(f != pdPASS){
					taskCallback[i] = NULL;
                    taskState[i] = TASK_FREE;
                    break;
				}
				
                #endif
                taskState[i] = TASK_START;//设置状态标志
                break;
            }
        }
    }else{//启动所有task
        for(int i=0; i<MPYTHON_TASK_USER_DEFINE_NUM_MAX; i++)
        {//uBit.serial.printf("0x%X   %d    0x%X\r\n",taskCallback[i], taskState[i], registeredFiber[i]);
            if(taskCallback[i] && !taskState[i])
            {//uBit.serial.printf("11\r\n");
                #if defined(NRF5) || defined(NRF52833)
                Fiber *f = create_fiber(taskCallback[i]);//注册回调函数
                if(!f){//uBit.serial.printf("22\r\n");
                    taskCallback[i] = NULL;
                    registeredFiber[i] = NULL;
                    taskState[i] = TASK_FREE;
                    continue;
                }
                registeredFiber[i] = f;
                #elif defined(ESP32)
                BaseType_t f = xTaskCreatePinnedToCore(taskCallback[i], name.c_str(), 2048, NULL, 1, &registeredFiber[i], ARDUINO_RUNNING_CORE);//注册回调函数
                if(!f){//uBit.serial.printf("22\r\n");
                    taskCallback[i] = NULL;
                    taskState[i] = TASK_FREE;
                    continue;
                }
				#elif defined(__AVR_ATmega2560__)
				BaseType_t f =  xTaskCreate(taskCallback[i],name.c_str(),
				2048,NULL, 0,&registeredFiber[i]);
				if(f != pdPASS){
					taskCallback[i] = NULL;
                    taskState[i] = TASK_FREE;
                    break;
				}
                #endif
                taskState[i] = TASK_START;//设置状态标志
            }
        }
    }
}

DFRobot_Task::DFRobot_Task(taskCallbackPt callback, const char *name)
{
    for(int i=0; i<MPYTHON_TASK_USER_DEFINE_NUM_MAX; i++)
    {
        if(taskCallback[i] == NULL && !taskState[i]){
            //注册task名称
            #if defined(NRF5) || defined(NRF52833)
            memcpy(taskName[i],name,strlen(name));
            taskName[i][strlen(name)]='\0';
            #elif defined(ESP32)
            taskName[i]=name;
			#elif defined(__AVR_ATmega2560__)
			taskName[i]=name;
            #endif
            //注册task所有回调函数
            taskCallback[i] = callback;
            break;
        }
    }
}

void mpythonFiberFlagClean(String name)
{
    if(name != "")
    {
        for(int i=0; i<MPYTHON_TASK_USER_DEFINE_NUM_MAX; i++)
        {
            String str = taskName[i];
            if(str == name && taskState[i])
            {
                taskState[i] = TASK_FREE;//设置状态标志为free
                break;
            }
        }
        return;
    }
}

void mpythonFiberFree(String name)
{
    if(name != "") // 退出一个task
    {
        for(int i=0; i<MPYTHON_TASK_USER_DEFINE_NUM_MAX; i++)
        {
            String str = taskName[i];
            if(str == name  && taskState[i])
            {
                mpythonFiberFlagClean(name);
                #if defined(NRF5) || defined(NRF52833)
                delay(10);
                release_fiber(registeredFiber[i]);
				delay(50);
                #elif defined(ESP32)
                vTaskDelete(registeredFiber[i]);
				delay(50);
				#elif defined(__AVR_ATmega2560__)
				vTaskDelete(registeredFiber[i]);
                #elif defined(ARDUINO_ARCH_RP2040)
				vTaskDelete(registeredFiber[i]);
                #endif
                break;
            }
        }
    }else{//所有task退出
        for(int i=0; i<MPYTHON_TASK_USER_DEFINE_NUM_MAX; i++)
        {
            if(taskState[i])
            {
                mpythonFiberFlagClean(taskName[i]);
                #if defined(NRF5) || defined(NRF52833)
                release_fiber(registeredFiber[i]);
				delay(50);
                #elif defined(ESP32)
                vTaskDelete(registeredFiber[i]);
				delay(50);
				#elif defined(__AVR_ATmega2560__)
				vTaskDelete(registeredFiber[i]);
                #elif defined(ARDUINO_ARCH_RP2040)
				vTaskDelete(registeredFiber[i]);
                #endif
            }
        }
    }
}

DFRobot_Task::~DFRobot_Task()
{
}

void DFRobot_Task::start()
{
}


