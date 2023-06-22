//h
#ifndef __DFROBOT_ULTRASONIC_H__
#define __DFROBOT_ULTRASONIC_H__
#define  MAX_RANG      (520)//ģ���༫ֵΪ520cm(����Ч�������ֵ�Դ�)
#define  ADC_SOLUTION      (1023.0)//Arduino UNO ��ADC����Ϊ10bit
#include <Arduino.h>
class DFRobot_Ultrasonic
{
public:
	DFRobot_Ultrasonic();
	void begin(int analogPin);
	int getDistanceCm();
private:
	int _analogPin;
};
#endif	