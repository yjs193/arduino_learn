/*!
 * @file backwardMAXDemo.ino
 * @brief DFRobot's MAX retreat
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

#define SPEED_L	200 //0~255
#define SPEED_R	200 //0~255

DFRobot_MAX myMax;

void setup() {
	myMax.begin();
}

void loop() {
   myMax.backward(SPEED_L, SPEED_R);
}