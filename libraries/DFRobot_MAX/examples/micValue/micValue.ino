/*!
 * @file micValue.ino
 * @brief DFRobot's read mic value
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

DFRobot_MAX myMax;

void setup() {
	Serial.begin(115200);
	myMax.begin();
}

void loop() {
	int16_t data = myMax.micValue();
  
	Serial.println(data);
	
	delay(100); 
} 