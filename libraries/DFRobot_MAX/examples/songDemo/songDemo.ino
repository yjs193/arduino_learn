/*!
 * @file songDemo.ino
 * @brief DFRobot's song mode setting
 * @n [Get the module here](等上架后添加商品购买链接)
 * @n This example Set the volume size and song music snippet.
 * @n [Connection and Diagram](等上架后添加wiki链接)
 *
 * @copyright	[DFRobot](http://www.dfrobot.com), 2016
 * @copyright	GNU Lesser General Public License
 *
 * @author [yangyang](971326313@qq.com)
 * @version  V1.0
 * @date  2016-09-20
 */
#include <DFRobot_MAX.h>
//#define DISCOVERY_SINGLE_BUS_PIN 2

//SoftwareSinglebus  mySingleBus(DISCOVERY_SINGLE_BUS_PIN);
//DFRobot_MAX myMax(&mySingleBus);
DFRobot_MAX myMax;

void setup() {
  // put your setup code here, to run once:
  myMax.begin();
}

void loop() {
  // put your main code here, to run repeatedly:
  for(uint8_t i = 1; i < 17; i++){
    myMax.playSound(i);
    delay(1000);
  }
}