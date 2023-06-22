/*!
 * @file advanceMAXDemo.ino
 * @brief DFRobot's MAX advance
 * @n [Get the module here](���ϼܺ������Ʒ��������)
 * @n [Connection and Diagram](���ϼܺ����wiki����)
 *
 * @copyright	[DFRobot](http://www.dfrobot.com), 2016
 * @copyright	GNU Lesser General Public License
 *
 * @author [yangyang](971326313@qq.com)
 * @version  V1.0
 * @date  2016-12-07
 */
#include <DFRobot_MAX.h>

#define SPEED_L	200 //0~255
#define SPEED_R	200 //0~255
//#define SINGLE_BUS_PIN 	2

//SoftwareSinglebus  mySingleBus(SINGLE_BUS_PIN);
//DFRobot_MAX myMax(&mySingleBus);
DFRobot_MAX myMax;

void setup() {
	myMax.begin();
}

void loop() {
   myMax.advance(SPEED_L, SPEED_R);
}