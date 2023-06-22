/*!
 * @file customEmoticonsDemo.ino
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


#define RED			1
#define GREEN		2
#define YELLOW		3
#define BLUE		4
#define PURPLE		5
#define CYAN		6
#define WHITE		7

static uint8_t Emoticons[129] = {
		1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0, //0
		1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0, //1
		1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0, //2
		1,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0, //3
		1,1,0,1,1,0,0,0,0,0,0,0,0,0,0,0, //4
		1,1,0,0,1,1,0,0,0,0,0,0,0,0,0,0, //5
		1,1,0,0,0,1,0,0,0,0,0,0,0,0,0,0, //6
		1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0, //7
	};


DFRobot_MAX myMax;

void setup() {
  myMax.begin();
  myMax.clearScreen();//Clear the screen
}

void loop() {
  /*
   *像素点地址寄存器，高4位代表Y坐标，低4位代表X坐标（0<=y<8,0<=x<16）
   *向该地址写入数据将选择相应坐标位置像素点
   *将自定义的表情的数据放到一个数组中 然后循环要点亮LED的次数
   *以下是展示“卜”这个表情
   */
  myMax.customFace(Emoticons, GREEN);//parameter1:pixel; parameter2:color
  delay(3000);
  myMax.clearScreen();//Clear the screen
  delay(1000);
}
