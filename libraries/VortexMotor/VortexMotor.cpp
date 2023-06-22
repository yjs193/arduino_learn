//
//  VortexMotor
//
//
//  Created by kelvin on 15/1/5.
//
//

#include "VortexMotor.h"
#include "PID_v1.h"

VortexMotor motor;
static PID leftPID(&leftCurrentDistance, &OutputLeft, &leftSetDistance, G_Kp_Max,G_Ki_Max,G_Kd_Max, DIRECT);
static PID rightPID(&rightCurrentDistance, &OutputRight, &rightSetDistance, G_Kp_Max,G_Ki_Max,G_Kd_Max, DIRECT);
void VortexMotor::Init()
{
    pinInputLeft = 0;
    pinInputRight = 1;
    pinInputLeftDir = 8;
    pinInputRightDir = 12;
    pinMode(pinENL,OUTPUT);
    pinMode(pinENR,OUTPUT);
    pinMode(pinDirL,OUTPUT);
    pinMode(pinDirR,OUTPUT);

    pinMode(pinInputLeftDir,INPUT);
    pinMode(pinInputRightDir,INPUT);
    attachInterrupt(pinInputLeft,leftCallBack,RISING);//开编码器中断
    attachInterrupt(pinInputRight,rightCallBack,RISING);//开编码器中断


    leftPID.SetOutputLimits(-255,255);
    rightPID.SetOutputLimits(-255,255);
    stopPID();
}

void VortexMotor::startPID()
{
  stepDistanceLeft = 0;
  stepDistanceRight = 0;
  leftCurrentDistance = 0;
  rightCurrentDistance = 0;
  leftSetDistance = 0;
  rightSetDistance = 0;
  OutputLeft = 0;
  OutputRight = 0;

  float proportion = abs(transformLeftSpeed/127);
  G_Kp_Current = proportion * (G_Kp_Max-G_Kp_Min)+G_Kp_Min;
  G_Ki_Current = proportion * (G_Ki_Max-G_Ki_Min)+G_Ki_Min;
  G_Kd_Current = proportion * (G_Kd_Max-G_Kd_Min)+G_Kd_Min;
  leftPID.SetTunings(G_Kp_Current,G_Ki_Current,G_Kd_Current);
  proportion = abs(transformRightSpeed/127);
  G_Kp_Current = proportion * (G_Kp_Max-G_Kp_Min)+G_Kp_Min;
  G_Ki_Current = proportion * (G_Ki_Max-G_Ki_Min)+G_Ki_Min;
  G_Kd_Current = proportion * (G_Kd_Max-G_Kd_Min)+G_Kd_Min;

  rightPID.SetTunings(G_Kp_Current,G_Ki_Current,G_Kd_Current);


  leftPID.SetMode(AUTOMATIC);
  rightPID.SetMode(AUTOMATIC);
  motor.PWMDrive(-motor.leftDistance_20ms*40,-motor.rightDistance_20ms*40);
  delay(20);
}
void VortexMotor::stopPID()
{
  leftPID.SetMode(MANUAL);
  rightPID.SetMode(MANUAL);
}
void VortexMotor::leftCallBack()
{
      if(digitalRead(motor.pinInputLeftDir))
      {
        leftCurrentDistance--;
      }
      else
      {
        leftCurrentDistance++;
      }
}
void VortexMotor::rightCallBack()
{
      if(digitalRead(motor.pinInputRightDir))
      {
        rightCurrentDistance--;
      }
      else
      {
        rightCurrentDistance++;
      }
}
void VortexMotor::update()
{
    motor.leftDistance_20ms = leftCurrentDistance;
    motor.rightDistance_20ms = rightCurrentDistance;
    motor.CurrentSpeed();
    if(leftPID.GetMode())
    {
      stepDistanceLeft = motor.currentLeftSpeed/40;
      stepDistanceRight = motor.currentRightSpeed/40;
      leftSetDistance += stepDistanceLeft;
      rightSetDistance += stepDistanceRight;
      leftPID.Compute();
      rightPID.Compute();
      motor.PWMDrive(OutputLeft,OutputRight);
    }
    else{
      leftCurrentDistance = 0;
      rightCurrentDistance = 0;
      motor.PWMDrive(motor.currentLeftSpeed*1.5,motor.currentRightSpeed*1.5);//临时
    }
}

void VortexMotor::PWMDrive(int leftSpeed,int rightSpeed)//普通驱动方式
{
    analogWrite(motor.pinENL,constrain(abs(leftSpeed),0,255));

    if(leftSpeed>0)
    {
        digitalWrite(motor.pinDirL,HIGH);
    }
    else
    {
        digitalWrite(motor.pinDirL,LOW);
    }
    analogWrite(motor.pinENR,constrain(abs(rightSpeed),0,255));
    if(rightSpeed>0)
    {
        digitalWrite(motor.pinDirR,HIGH);
    }
    else
    {
        digitalWrite(motor.pinDirR,LOW);
    }
}
void VortexMotor::CurrentSpeed()
{
    if(transformLeftSpeed>currentLeftSpeed)
    {
        currentLeftSpeed += accLeft;
        if(transformLeftSpeed<currentLeftSpeed)
            currentLeftSpeed=transformLeftSpeed;
    }
    if(transformLeftSpeed<currentLeftSpeed)
    {
        currentLeftSpeed-=accLeft;
        if(transformLeftSpeed>currentLeftSpeed)
            currentLeftSpeed=transformLeftSpeed;
    }
    if(transformRightSpeed>currentRightSpeed)
    {
        currentRightSpeed += accRight;
        if(transformRightSpeed<currentRightSpeed)
            currentRightSpeed=transformRightSpeed;
    }
    if(transformRightSpeed<currentRightSpeed)
    {
        currentRightSpeed-=accRight;
        if(transformRightSpeed>currentRightSpeed)
            currentRightSpeed=transformRightSpeed;
    }
}
void VortexMotor::AccCount(byte leftSpeed,byte rightSpeed,byte accTime)//20metro
{
    transformLeftSpeed = (leftSpeed&0x7f)<<1;
    transformRightSpeed = (rightSpeed&0x7f)<<1;
    // transformLeftSpeed*=1.3;
    // transformRightSpeed*=1.3;
    if (!(leftSpeed>>7)) {
        transformLeftSpeed *= -1;
    }
    if (!(rightSpeed>>7)) {
        transformRightSpeed *= -1;
    }
    if(accTime==0)
    {
        accLeft=0;
        currentLeftSpeed = transformLeftSpeed;
        accRight=0;
        currentRightSpeed = transformRightSpeed;
    }
    else
    {
        accLeft=abs(float((transformLeftSpeed-currentLeftSpeed))/(float(accTime)));
        accRight=abs(float((transformRightSpeed-currentRightSpeed))/(float(accTime)));
    }
}
