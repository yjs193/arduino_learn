#ifndef _DFRobot_XiaMi_H
#define _DFRobot_XiaMi_H

#include <Arduino.h>
#include <Wire.h>
#include <DFRobot_URM10.h>
#include <math.h>
// #include <DFRobot_IRremote.h>


class DFRobot_XiaMi //: public IRremote_Receive
{


	public:  // 公有
	enum MOTOR {
    	M1=0X00,M2=0X02,M3=0X04,M4=0X06, ALL=0X08
	};

	enum DIRECTION {
    	CW=0X00,CCW=0X01
	};

	enum STATE{
    	SPEED, DIR
	};

	enum LIGHT {
    	RD=0X8,YW=0X09,GN=0X0A
	};

	enum RELAY{
    	CLOSE,DISCON
	};
	
	enum AHT20{
    	TEMP, HUM
	};
	
	DFRobot_XiaMi(); // 构造函数
	~DFRobot_XiaMi(); // 析构函数
	
	//void begin(); // api
	uint8_t sensorAddress; //IIC address
	void motorRun(MOTOR index, DIRECTION dir, int speed);  //控制电机运行
	void motorStop(MOTOR index);  //控制电机停止
	void LED(int state1,int state2,int state3);  //控制交通灯
	int readAngle();  //读取旋钮编码器数据
	int readFlre();  //获取火焰传感器数据
	int motorState(MOTOR motor,STATE state);  //获取电机方向和速度
	int LEDState(LIGHT color);  //获取交通灯状态
	void setRelay(RELAY state);  //控制继电器
	int Ultrasonic();  //获取超声波数据
	void AHT20Init(uint8_t AHT20address);  //初始化AHT20传感器
	double readAHT20(AHT20 state);  //获取温湿度数据




	// int RGBColor(uint8_t r,uint8_t g,uint8_t b); //rgb颜色
	// uint8_t ledRange(uint8_t from,uint8_t to);  //设置rgb点亮从a~b
	// void setIndexColor(uint8_t index,uint8_t rgb);  //指定灯号显示颜色
	// void showColor(uint8_t rgb);  //设置所有rgb灯显示颜色
	// void setBrightness(uint8_t brightness);  //设置rgb灯亮度
	// void ledBlank();  //关闭所有rgb灯





	// private: // 私有
	
	//void i2cInit();
	
	//uint8_t addr;
	
};









#endif // _DFRobot_XiaMi_H