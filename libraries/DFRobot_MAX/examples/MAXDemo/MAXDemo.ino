/*!
 * @file MAXDemo.ino
 * @brief DFRobot's MAX demo
 * @n [Get the module here](等上架后添加商品购买链接)
 * @n [Connection and Diagram](等上架后添加wiki链接)
 *
 * @copyright	[DFRobot](http://www.dfrobot.com), 2016
 * @copyright	GNU Lesser General Public License
 *
 * @author [yangyang](971326313@qq.com)
 * @version  V1.0
 * @date  2017-6-29
 */
#include <DFRobot_MAX.h>
#include <Metro.h>
#include <GoBLE.h>

#define DATA_L 			2
#define DATA_M			1
#define DATA_R			0
#define DEFAULT_SPEED 	120


static uint32_t musicTime0, musicTime1, emoticonsTime, RGBTime;

DFRobot_MAX myMax;

void setup()
{
	// put your setup code here, to run once:
	delay(500);
	Serial.begin(115200);
	myMax.begin();

	myMax.showFace(14, 5);
	for(uint8_t i=0; i<4; i++)
		myMax.rgbLed(i, 255, 0, 0);
	myMax.playSound(12);
	delay(500);

	for(uint8_t i=0; i<4; i++)
		myMax.rgbLed(i, 0, 255, 0);
	delay(500);

	for(uint8_t i=0; i<4; i++)
		myMax.rgbLed(i, 0, 0, 255);
	delay(500);

	for(uint8_t i=0; i<4; i++)
		myMax.rgbLed(i, 0, 0, 0);
	myMax.playSound(16);
	delay(500);

	Serial.println("---------MAX demo----------");
}

void loop()
{
	static uint32_t modeTime;
	static uint8_t selectMode=1;

	if (millis()-150 > modeTime) { //150ms检测一次是否按键按下改变模式
		if (!digitalRead(KEY)) {
			myMax.swerve(ADVANCE_L, 0, ADVANCE_R, 0); //MAX stop
			myMax.lineLED(0); //close led

			selectMode++;
			if (selectMode == 5) selectMode = 1;
			//Serial.println(selectMode);

			while (!digitalRead(KEY)) delay(5);
		}

		modeTime = millis();
	}

	switch (selectMode) {
	case 1:
		avoidObstacle();
		break;
	case 2:
		linePatrol();
		break;
	case 3:
		rayTracing();
		break;
	case 4:
		bluetoothControl();
		break;

	default:
		break;
	}
}


void ledRotation(uint8_t r, uint8_t g, uint8_t b)
{
	static uint8_t rotation[4]= {0, 1, 2, 3};

	myMax.rgbLed(rotation[0], r, g, b);
	myMax.rgbLed(rotation[1], r, g, b);
	myMax.rgbLed(rotation[2], 0, 0, 0);
	myMax.rgbLed(rotation[3], 0, 0, 0);

	if(rotation[0] == 3) rotation[0] = 0;
	else rotation[0]++;

	if(rotation[1] == 3) rotation[1] = 0;
	else rotation[1]++;

	if(rotation[2] == 3) rotation[2] = 0;
	else rotation[2]++;

	if(rotation[3] == 3) rotation[3] = 0;
	else rotation[3]++;
}

///< avoid obstacle mode
void avoidObstacle()
{
	static uint8_t randomNum=1;
	int16_t getValue = myMax.distanceValue();//read the value of the ultrasound

	if (millis()-150 > RGBTime) { //150ms cycle once, the lamplight counterclockwise rotation
		ledRotation(255, 0, 0);
		RGBTime = millis();
	}

	if(getValue > 105) {
		myMax.forward(120, 120);
	} else if(getValue > 95) {
		myMax.playSound(1); //Play music 1
	} else if(getValue > 85) {
		myMax.playSound(2); //Play music 2
	} else if(getValue > 75) {
		myMax.playSound(3); //Play music 3
	} else if(getValue > 65) {
		myMax.playSound(4); //Play music 4
	} else if(getValue > 55) {
		myMax.playSound(5); //Play music 5
	} else if(getValue > 45) {
		myMax.playSound(6); //Play music 6
	} else if(getValue > 35) {
		myMax.playSound(7); //Play music 7
	} else if(getValue > 15) {
		myMax.playSound(9); //Play music 9
		if(randomNum == 1) {
			myMax.swerve(ADVANCE_L, 150, ADVANCE_R, 50); //turn right
		} else if(randomNum == 2) {
			myMax.swerve(ADVANCE_L, 50, ADVANCE_R, 150); //turn left
		} else if(randomNum == 3) {
			myMax.swerve(RETREAT_L, 150, RETREAT_R, 50); //retreat left deflection
		} else {
			myMax.swerve(RETREAT_L, 50, RETREAT_R, 150); //retreat right deflection
		}
	} else {
		myMax.backward(150, 150);
	}

	if (millis()-1000 > emoticonsTime) {
		myMax.showFace(random(16), random(1, 8)); //show emoticons and color

		if (randomNum++%4 == 0)	randomNum = 1;
		emoticonsTime = millis();
	}
}

///< line patrol mode
void linePatrol()
{
	static uint8_t minSpeed = 50, speed_R, speed_L, musicNum=1;
	int16_t data_L = myMax.lineValue(DATA_L);
	int16_t data_M = myMax.lineValue(DATA_M);
	int16_t data_R = myMax.lineValue(DATA_R);

	if (millis()-150 > RGBTime) { //150ms循环一次，灯光逆时针旋转
		ledRotation(0, 255, 0);
		RGBTime = millis();
	}

	myMax.lineLED(1); //open led

	if (data_L && data_M && !data_R) {
		speed_R = DEFAULT_SPEED + 50;
		speed_L = DEFAULT_SPEED;
	} else if(data_L && !data_M && !data_R) {
		speed_R = DEFAULT_SPEED + 50;
		speed_L = DEFAULT_SPEED - 30;
	} else if(!data_L && data_M && data_R) {
		speed_R = DEFAULT_SPEED;
		speed_L = DEFAULT_SPEED + 50;
	} else if(!data_L && !data_M && data_R) {
		speed_R = DEFAULT_SPEED - 30;
		speed_L = DEFAULT_SPEED + 50;
	} else if(!data_L && data_M && !data_R) {
		speed_R = DEFAULT_SPEED;
		speed_L = DEFAULT_SPEED;
	} else if(!data_L && !data_M && !data_R) {
		myMax.playSound(10);
		if (speed_L > speed_R) {
			speed_R = minSpeed;
		} else if (speed_L < speed_R) {
			speed_L = minSpeed;
		} else {
			speed_L = 0;
			speed_R = 0;
		}

		if (millis()-500 > musicTime0) {
			myMax.playSound(16);
			musicTime0 = millis();
		}
	}

	if (millis()-500 > musicTime1) {
		myMax.playSound(musicNum);
		musicTime1 = millis();
		if (musicNum == 8) musicNum = 1;
		else musicNum++;

	}

	if (millis()-1000 > emoticonsTime) {
		myMax.showFace(random(16), random(1, 8));
		emoticonsTime = millis();
	}

	myMax.swerve(ADVANCE_L, speed_L, ADVANCE_R, speed_R);
}

///< ray tracing mode
void rayTracing()
{
	int16_t data_L = myMax.lightValue(1);//左光敏传感器数据
	int16_t data_R = myMax.lightValue(0);//右光敏传感器数据

	if (millis()-150 > RGBTime) { //150ms循环一次，灯光逆时针旋转
		ledRotation(0, 0, 255);
		RGBTime = millis();
	}

	if (data_L - data_R > 150) { //光线在右，向右移动
		myMax.swerve(ADVANCE_L, 150, ADVANCE_R, 50);
		myMax.showFace(1, 2);
		myMax.playSound(13);
	} else if (data_R - data_L > 150) { //光线在左，向左移动
		myMax.swerve(ADVANCE_L, 50, ADVANCE_R, 150);
		myMax.showFace(2, 2);
		myMax.playSound(13);
	} else if (data_R + data_L < 1000) { //光线在前方，向前移动
		myMax.swerve(ADVANCE_L, 100, ADVANCE_R, 100);
		myMax.showFace(20, 5);
		//myMax.playSound(16);
	} else { //未找到光线
		myMax.swerve(ADVANCE_L, 0, ADVANCE_R, 0);
		myMax.showFace(0, 1);
		myMax.playSound(16);
	}
}

///< bluetooth control mode
void bluetoothControl()
{
	static int bluetoothStatus[6];
	static uint8_t rotation[4]= {0, 1, 2, 3}, command;

	if (millis()-150 > RGBTime) { //150ms循环一次，灯光逆时针旋转
		myMax.rgbLed(rotation[0], 255, 0, 0);
		myMax.rgbLed(rotation[1], 0, 255, 0);
		myMax.rgbLed(rotation[2], 0, 0, 255);
		myMax.rgbLed(rotation[3], 255, 255, 255);

		if(rotation[0] == 3) rotation[0] = 0;
		else rotation[0]++;

		if(rotation[1] == 3) rotation[1] = 0;
		else rotation[1]++;

		if(rotation[2] == 3) rotation[2] = 0;
		else rotation[2]++;

		if(rotation[3] == 3) rotation[3] = 0;
		else rotation[3]++;

		RGBTime = millis();
	}
	delay(1);

	if (Goble.available()) { //读取蓝牙数据
		bluetoothStatus[SWITCH_UP]     = !Goble.readSwitchUp();
		bluetoothStatus[SWITCH_DOWN]   = !Goble.readSwitchDown();
		bluetoothStatus[SWITCH_LEFT]   = !Goble.readSwitchLeft();
		bluetoothStatus[SWITCH_RIGHT]  = !Goble.readSwitchRight();
		bluetoothStatus[SWITCH_SELECT] = !Goble.readSwitchSelect();
		bluetoothStatus[SWITCH_START]  = !Goble.readSwitchStart();

		//向前
		if (bluetoothStatus[1] == PRESSED) command = 1;
		//向右
		else if (bluetoothStatus[2] == PRESSED) command = 2;
		//向后
		else if (bluetoothStatus[3] == PRESSED) command = 3;
		//向左
		else if (bluetoothStatus[4] == PRESSED) command = 4;
		//停止
		else if (bluetoothStatus[5] == PRESSED) command = 5;
	}

	switch (command) {
	case 1:
		myMax.swerve(ADVANCE_L, 100, ADVANCE_R, 100);
		break;
	case 2:
		myMax.swerve(ADVANCE_L, 150, ADVANCE_R, 40);
		break;
	case 3:
		myMax.swerve(RETREAT_L, 100, RETREAT_R, 100);
		break;
	case 4:
		myMax.swerve(ADVANCE_L, 40, ADVANCE_R, 150);
		break;
	case 5:
		myMax.swerve(ADVANCE_L, 0, ADVANCE_R, 0);
		break;
	default:
		//myMax.swerve(ADVANCE_L, 0, ADVANCE_R, 0);
		break;
	}
}