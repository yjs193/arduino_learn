/*!
 * @file EmoticonsDemo.ino
 * @brief DFRobot's RGB Emoticons
 * @n [Get the module here](等上架后添加商品购买链接)
 * @n This example Set the volume size and play music snippet.
 * @n [Connection and Diagram](等上架后添加wiki链接)
 *
 * @copyright	[DFRobot](http://www.dfrobot.com), 2017
 * @copyright	GNU Lesser General Public License
 
 * @author [YangYang](Yang.Yang@dfrobot.com)
 * @version  V1.0
 * @date  2017-6-29
 */
#include <DFRobot_MAX.h>

#define Emoticons_NUM 	23

#define RED				1
#define GREEN			2
#define YELLOW			3
#define BLUE			4
#define PURPLE			5
#define CYAN			6
#define WHITE			7

uint8_t EmoticonsCnt=0;

DFRobot_MAX myMax;

void setup() {
  myMax.begin();
}

void loop() {
  myMax.showFace(EmoticonsCnt, RED);//Emoticons,numerical in 0~22
  EmoticonsCnt++;
  if(Emoticons_NUM == EmoticonsCnt) EmoticonsCnt = 0;
  delay(1000);
}
