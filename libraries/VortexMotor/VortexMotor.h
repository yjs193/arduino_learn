//
//  VortexMotor
//
//
//  Created by kelvin on 14-10-30.
//
//

#ifndef _VortexMotor_H_
#define _VortexMotor_H_
#include <Arduino.h>
#include "VortexMotor.h"
#include "PID_v1.h"
#define ACCMODE 0
#define ARCMODE 1
static double OutputLeft;
static double OutputRight;
static double stepDistanceLeft,stepDistanceRight;
static double leftCurrentDistance,rightCurrentDistance;
static double leftSetDistance,rightSetDistance;
static double G_Kp_Max = 6, G_Ki_Max = 0.4, G_Kd_Max = 0.6;
static double G_Kp_Min = 0, G_Ki_Min = 0, G_Kd_Min = 0;
static double G_Kp_Current, G_Ki_Current, G_Kd_Current;

class VortexMotor
{
public:
    float accLeft,accRight,currentLeftSpeed,currentRightSpeed;
    float transformLeftSpeed,transformRightSpeed;
    float leftDistance_20ms;
    float rightDistance_20ms;
public:
    int pinENL,pinENR,pinDirL,pinDirR;//电机引脚

    int pinInputLeft;
    int pinInputRight;
    int pinInputLeftDir;
    int pinInputRightDir;

    void Init();
    static void PWMDrive(int leftSpeed,int rightSpeed);//普通驱动方式
    void AccCount(byte leftSpeed,byte rightSpeed,byte accTime);//20metro
    void CurrentSpeed();//20metro


    static void leftCallBack();
    static void rightCallBack();

    static void update();//20ms
    void startPID();
    void stopPID();

};
extern VortexMotor motor;
#endif
