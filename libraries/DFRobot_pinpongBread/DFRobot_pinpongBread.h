#ifndef _DFRobot_XiaMi_H
#define _DFRobot_XiaMi_H

#include <Arduino.h>
#include <Wire.h>
#include <DFRobot_URM10.h>
#include <math.h>
#include <DFRobot_SSD1306.h>


#define pBaord_I2CBEGIN()        Wire.setClock(100000);


class DFRobot_pinpongBread: public DFRobot_SSD1306
{
	public:  // 公有
	enum MOTOR {
    	M1=0X02,M2=0X00, ALL=0X04
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

	uint8_t recvpin;
	bool enable;
	uint32_t irData;

    uint32_t irEventData;

	void setRefreshRange(int address, uint8_t reg, int columnStart, int columnEnd, int rowStart, int rowEnd);
	int writeBuffer(int address, uint8_t reg, unsigned char *pdata, int length);
	int writeByte(uint8_t address, uint8_t reg, uint8_t pdata);

	DFRobot_pinpongBread(); // 构造函数
	~DFRobot_pinpongBread(); // 析构函数

	void setCursorString(int32_t index, String str);


	void motorRun(MOTOR index, DIRECTION dir, int speed);  //控制电机运行
	void motorStop(MOTOR index);  //控制电机停止
	
	void LED(int state);  //控制LED
	uint8_t readButton(uint8_t pin);

	int readAngle();  //读取旋钮编码器数据
	int readLight();  //获取光线传感器数据

	void AHT20Init();  //初始化AHT20传感器
	double readAHT20(AHT20 state);  //获取温湿度数据

	void enableIRIn();
	void ir_begin(int recvpin1);
	void decode(uint32_t code);
	uint32_t getData(void);
	String getIrCommand();

	void mpu6050Init();
	void mpu6050GetData();

	float getYaw(void);
	float getRoll(void);
	float getPitch(void);

	String calData(String text, uint8_t bit);
	String calData(uint16_t text, uint8_t bit);
	String calData(uint8_t value, uint8_t bit);
	String calData(uint32_t value, uint8_t bit);
	String calData(float value, uint8_t bit);
	String calData(int value, uint8_t bit);


private:
	int16_t ax, ay, az, gx, gy, gz;
	uint8_t devAddr;
	uint8_t buffer[14];

	float pitch,yaw,roll;
	
};









#endif // DFRobot_pinpongBread