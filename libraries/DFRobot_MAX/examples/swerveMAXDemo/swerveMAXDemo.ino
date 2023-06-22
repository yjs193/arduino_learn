/*!
 * @file swerveMAXDemo.ino
 * @brief DFRobot's MAX swerve
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

#define SPEED_L		200 //0~255
#define SPEED_R		200 //0~255
#define ADVANCE_L	HIGH
#define RETREAT_L	LOW
#define ADVANCE_R	HIGH
#define RETREAT_R	LOW

DFRobot_MAX myMax;

void setup() {
	myMax.begin();
}

void loop() {
   myMax.swerve(ADVANCE_L, SPEED_L, RETREAT_R, SPEED_R);
}