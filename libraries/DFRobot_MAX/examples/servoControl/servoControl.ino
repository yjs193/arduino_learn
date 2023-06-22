/*!
 * @file servoControl.ino
 * @brief DFRobot's servo control
 * @n [Get the module here](���ϼܺ������Ʒ��������)
 * @n [Connection and Diagram](���ϼܺ����wiki����)
 *
 * @copyright	[DFRobot](http://www.dfrobot.com), 2016
 * @copyright	GNU Lesser General Public License
 *
 * @author [yangyang](971326313@qq.com)
 * @version  V1.0
 * @date  2017-6-29
 */
#include <DFRobot_MAX.h>

DFRobot_MAX myMax;

void setup() {
	myMax.begin();
}

void loop() {
	myMax.servoControl(SP2, 120);
	delay(1000);
	myMax.servoControl(SP2, 30);
	delay(1000);
}