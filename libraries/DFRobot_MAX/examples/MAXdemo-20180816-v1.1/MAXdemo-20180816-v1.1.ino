/*!
   @file MAX.ino
   @brief DFRobot's Explorer MAX
   @n [Get the module here]()
   @n This example is a factory procedure, obstacle avoidance, patrol, search light, bluetooth remote control functions.
   @n [Connection and Diagram](http://wiki.dfrobot.com.cn/index.php?title=(SKU:ROB0137)_%E6%8E%A2%E7%B4%A2%E8%80%85MAX%E6%9C%BA%E5%99%A8%E4%BA%BA))_探索者MAX机器人)
   @copyright  [DFRobot](http://www.dfrobot.com), 2017
   @copyright GNU Lesser General Public License
   @author [lijun](ju.li@dfrobot.com)
   @debuggers[shichao](sc.mingyang@dfrobot.com)
   @version  V1.1
   @date  2018-08-16
*/

#include <DFRobot_MAX.h>
#include "GoBLE.h"
DFRobot_MAX myMax;

#define DEFAULTSPEED 120

unsigned char findLineflag; //巡线标志
unsigned int  Distance;
unsigned int  Mode = 1, Mode1 = 0; //小车模式
unsigned int  Mark = 1, Mark_1 = 0; //当前状态
unsigned long lasttime = 0, Modetime = 0, Musictime = 0, RGBtime = 0,Smiliestime = 0,Watchtime = 0;
uint8_t  ExpressionCnt = 0x00;
uint8_t  ExpressionColor = 0x01;
uint8_t  Music = 1,Watch = 1;
uint16_t findLinecnt=0;
uint32_t a = 0, b = 1, c = 2, d = 3, e, f;
int buttonState[6];


///////////////////****巡线函数****////////////////////////
void findLineControl(unsigned char Direction) 
{
  static uint8_t speedL = DEFAULTSPEED, speedR = DEFAULTSPEED; //定义左右轮的速度
  static uint8_t findLineFlag;
  uint8_t maxSpeed = 70, minSpeed =50; //定义左右两个轮子的最大值和最小值
  uint8_t fast = minSpeed;

  if (Direction) findLinecnt = 0;
  else
  {
    findLinecnt++;
    if (findLinecnt > 100)
    {
      findLinecnt = 101;
      myMax.swerve(ADVANCE_L, 0, ADVANCE_R, 0);
      return;
    }
  }

  switch (Direction) //判断max方向与所巡路径的相对值，从而实行相应的方向调整
  {
    case 0x06:     //->left
      speedR = DEFAULTSPEED + 50 ;
      speedL = DEFAULTSPEED;
      findLineFlag = Direction;
      break;
    case 0x04:
      speedR = DEFAULTSPEED + 50 ;
      speedL = DEFAULTSPEED - 30 ;
      findLineFlag = Direction;
      break;
    case 0x03:     //->right
      speedR = DEFAULTSPEED;
      speedL = DEFAULTSPEED + 50;
      findLineFlag = Direction;
      break;
    case 0x01:
      speedR = DEFAULTSPEED - 30 ;
      speedL = DEFAULTSPEED + 50 ;
      findLineFlag = Direction;
      break;
    case 0x02:
      speedR = DEFAULTSPEED;
      speedL = DEFAULTSPEED;
      findLineFlag = Direction;
      break;
    case 0x00:   //no return
      if (Mark == 1)
      {
        myMax.playSound(8);
        Mark == 2;
        Musictime = millis();
      }

      if (0x01 & findLineFlag) //调节MAX动作时运动速度，可以使运行更平稳
      {
        if (speedR > DEFAULTSPEED) speedR -= fast;
        else if (speedL < maxSpeed) speedL += fast;
        else if (speedR > minSpeed) speedR -= fast;
      }
      else
      {
        if (speedL > DEFAULTSPEED) speedL -= fast;
        else if (speedR < maxSpeed) speedR += fast;
        else if (speedL > minSpeed) speedL -= fast;
      }

      if (millis() - 500 > Musictime)
      {
        myMax.playSound(14);
      }
      break;
    default: break;
  }
  myMax.swerve(ADVANCE_L, speedL, ADVANCE_R, speedR); 
}


///////////////////****初始化****////////////////////////
void setup()
{
  myMax.begin();
  Goble.begin();
  Serial.begin(115200); // 打开串口，设置速率为115200 bps
  delay(500);

  myMax.displayEmoticons(0x14);//显示表情符号
  myMax.setColor(0x05); //设置颜色
  myMax.rgbLed(-1,255, 0, 0);//所有led显示红色
  myMax.playSound(13); //播放编号12音乐
  delay(500);
  myMax.rgbLed(-1,0, 255, 0);//所有led显示绿色
  delay(500);
  myMax.rgbLed(-1,0, 0, 255);//所有led显示蓝色
  delay(1000);
  myMax.rgbLed(-1,0, 0, 0);//关灯
  myMax.playSound(13);
  delay(1000);
  lasttime = millis();
}

void loop()
{
  int a1 = 1,a2 = 1,a3 = 1,a4 = 1,a5 = 1,a6 = 1,a7 = 1;
  if (millis() - 150 > Modetime)//150ms检测一次是否按键按下改变模式
  {
    Modetime = millis();
    if (myMax.buttonValue())
      if (myMax.buttonValue())
        Mode += 1;
    if (Mode > 4)
      Mode = 1;

    if (Mode1 != Mode) //检测到按键按下
    {
      myMax.lineLED(0);//关巡线传感器LED灯
      myMax.stopMotor();//停止
      Mode1 = Mode;
    }

    while (myMax.buttonValue());
  }

  //1.避障程序模式
  if (Mode == 1)
  {
    myMax.rgbLed(a,255,0,0); myMax.rgbLed(b,255,0,0); //红色
    myMax.rgbLed(c,0, 0, 0); myMax.rgbLed(d,0, 0, 0);
    if (millis() - 150 > RGBtime)//150ms循环一次，灯光逆时针旋转
    {
      a = a == 3 ? 0 : a + 1;
      b = b == 3 ? 0 : b + 1;
      c = c == 3 ? 0 : c + 1;
      d = d == 3 ? 0 : d + 1;
      RGBtime = millis();
    }

    if(millis() - 1000 > Watchtime)
    {
      Watch = Watch == 4 ? 1 : Watch + 1;
      Watchtime = millis();
    }

    Distance = myMax.distanceValue();//读取超声波的值 值赋给Distance
    if (Distance <= 105 && Distance > 95 && a1 == 1)
    {
      myMax.playSound(0);//播放编号1音乐
      a1=0; a2=1; a3=1; a4=1; a5=1; a6==1; a7==1;
    }

    else if (Distance <= 95 && Distance > 85 && a2 == 1)
    {
     myMax.playSound(2);//播放编号2音乐
     a1=1; a2=0; a3=1; a4=1; a5=1; a6==1; a7==1;
    }

    else if (Distance <= 85 && Distance > 75 && a3 == 1)
    {
      myMax.playSound(2);//播放编号3音乐
      a1=1; a2=1; a3=0; a4=1; a5=1; a6==1; a7==1;
    }

    else if (Distance <= 75 && Distance > 65 && a4 == 1)
    {
      myMax.playSound(3);//播放编号4音乐
      a1=1; a2=1; a3=1; a4=0; a5=1; a6==1; a7==1;
    }

    else if (Distance <= 65 && Distance > 55 && a5 == 1)
    {
      myMax.playSound(4);//播放编号5音乐
      a1=1; a2=1; a3=1; a4=1; a5=0; a6==1; a7==1;
    }

    else if (Distance <= 55 && Distance > 45 && a6 == 1)
    {
      myMax.playSound(5);//播放编号6音乐
      a1=1; a2=1; a3=1; a4=1; a5=1; a6==0; a7==1;
    }

    else if (Distance <= 45 && Distance > 35 && a7 == 1)
    {
      myMax.playSound(6);//播放编号7音乐
      a1=1; a2=1; a3=1; a4=1; a5=1; a6==1; a7==0;
    }

    else if (Distance <= 35 && Distance > 15)
    {
      myMax.playSound(10);
      if(Watch == 1)      { myMax.swerve(ADVANCE_L, 140, RETREAT_R, 140); delay(300); }
      else if(Watch == 2) { myMax.swerve(RETREAT_L, 140, ADVANCE_R, 140); delay(300); }
      else if(Watch == 3) { myMax.swerve(RETREAT_L, 60,  RETREAT_R, 140); delay(600); }
      else if(Watch == 4) { myMax.swerve(RETREAT_L, 140, RETREAT_R, 60 ); delay(600); }
    }

    else if(Distance <= 15)
    {
      myMax.swerve(ADVANCE_L, 100, RETREAT_R, 100); delay(1200);
    }

    else
    {
        for(int j = 50;j < 120;j++) myMax.swerve(ADVANCE_L, j, ADVANCE_R, j);
    }

    if (millis() - 1000 > Smiliestime)//1s改变一次表情和色彩
    {
      myMax.displayEmoticons(ExpressionCnt);
      myMax.setColor(ExpressionColor);

      ExpressionCnt = ExpressionCnt == 0x16 ? 0x00 : ExpressionCnt + 1;
      ExpressionColor = ExpressionColor == 0x07 ? 0x01 : ExpressionColor + 1;

      Smiliestime = millis();
    }
  }

  //2.巡线程序模式
  else if (Mode == 2)
  {
    myMax.rgbLed(a,0,255,0); myMax.rgbLed(b,0,255,0); //绿色
    myMax.rgbLed(c,0, 0, 0); myMax.rgbLed(d,0, 0, 0);
    if (millis() - 150 > RGBtime)//150ms循环一次，灯光逆时针旋转
    {
      a = a == 3 ? 0 : a + 1;
      b = b == 3 ? 0 : b + 1;
      c = c == 3 ? 0 : c + 1;
      d = d == 3 ? 0 : d + 1;
      RGBtime = millis();
    }
    myMax.lineLED(1); //开巡线传感器LED灯

    if (millis() - 1000 > Smiliestime)//循环表情和表情显示彩色
    {
      myMax.displayEmoticons(ExpressionCnt);
      myMax.setColor(ExpressionColor);

      ExpressionCnt = ExpressionCnt == 0x16 ? 0x00 : ExpressionCnt + 1;
      ExpressionColor = ExpressionColor == 0x07 ? 0x01 : ExpressionColor + 1;

      Smiliestime = millis();
    }

    int16_t data = myMax.patrolValue();
    if (data != -1)//读取巡线传感器的值
    {
      if (findLineflag == data)
      {
        findLineControl(findLineflag);
        if (millis() - 500 > Musictime)//循环播放1号到8号音乐，500ms改变一次
        {
          myMax.playSound(Music-1);
          Music = Music == 8 ? 1 : Music + 1;
          Musictime = millis();
          Mark = 1;
        }
      }
      findLineflag = data;//将巡线值赋给findLineflag
    }
  }

  //3.追光程序模式
  else if (Mode == 3)
  {
    myMax.rgbLed(a,30,144,255); myMax.rgbLed(b,30,144,255); //道奇蓝色
    myMax.rgbLed(c,0, 0,  0  ); myMax.rgbLed(d,0, 0,  0  );
    if (millis() - 150 > RGBtime)//150ms循环一次，灯光逆时针旋转
    {
      a = a == 3 ? 0 : a + 1;
      b = b == 3 ? 0 : b + 1;
      c = c == 3 ? 0 : c + 1;
      d = d == 3 ? 0 : d + 1;
      RGBtime = millis();
    }

    int rightLed = myMax.lightValue(0);//右光敏传感器数据
    int leftLed =  myMax.lightValue(1);//左光敏传感器数据
    if (rightLed - leftLed > 130)//光线在右，向右移动
    {
      myMax.swerve(ADVANCE_L, 150, ADVANCE_R, 50);
      myMax.displayEmoticons(0x01);
      myMax.setColor(0x02);;
      myMax.playSound(12);
    }

    else if (leftLed - rightLed > 130)//光线在左，向左移动
    {
      myMax.swerve(ADVANCE_L, 50, ADVANCE_R, 150);
      myMax.displayEmoticons(0x02);
      myMax.setColor(0x02);
      myMax.playSound(12);
    }

    else if (abs(rightLed - leftLed) <= 130)//光线在前方，向前移动
    {
      myMax.swerve(ADVANCE_L, 100, ADVANCE_R, 100);
      myMax.displayEmoticons(0x14);
      myMax.setColor(0x05);
    }

    else//未找到光线
    {
      myMax.swerve(ADVANCE_L, 0, ADVANCE_R, 0);
      myMax.displayEmoticons(0x00);
      myMax.setColor(0x01);
    }
  }

  //4.蓝牙程序模式
  else if (Mode == 4)
  {
    if (Goble.available())//读取蓝牙数据
    {
      buttonState[SWITCH_UP]     = !Goble.readSwitchUp();
      buttonState[SWITCH_DOWN]   = !Goble.readSwitchDown();
      buttonState[SWITCH_LEFT]   = !Goble.readSwitchLeft();
      buttonState[SWITCH_RIGHT]  = !Goble.readSwitchRight();
      buttonState[SWITCH_SELECT] = !Goble.readSwitchSelect();
      buttonState[SWITCH_START]  = !Goble.readSwitchStart();

      for(int j = 1; j < 7; j++)
        if (buttonState[j] == PRESSED) { Mark_1 = j; break; }//向前
    }

    switch (Mark_1)
    {
      case 1:
      {
         myMax.rgbLed(a,255,245,245); myMax.rgbLed(b,0,  255,0  ); //RGB模式为正面两白灯，尾部两绿灯
         myMax.rgbLed(c,0,  255,0  ); myMax.rgbLed(d,250,245,245);
         for(int j = 60;j < 150;j++)  myMax.swerve(ADVANCE_L, j, ADVANCE_R, j);
         a=2;b=3;c=0;d=1; //RGB灯分配的位置
      } break; //前进
      case 2:
      {
        myMax.rgbLed(a,255,128, 0 ); myMax.rgbLed(b,0,  255,255); //RGB模式为右前方为橙色灯，其余为兰色
        myMax.rgbLed(c, 0, 255,255); myMax.rgbLed(d,0,  255,255);
        for(int j = 100;j < 150;j++) myMax.swerve(ADVANCE_L, j, ADVANCE_R, 40);
        a=1;b=2;c=0;d=3;
      } break; //右转
      case 3:
      {
        myMax.rgbLed(a,250,245,245); myMax.rgbLed(b,255, 0,  0 ); //RGB模式为正面两白灯，尾部两红灯灯
        myMax.rgbLed(c,255, 0,  0 ); myMax.rgbLed(d,250,245,245);
        myMax.rgbLed(e,30, 144,255); myMax.rgbLed(f,30, 144,255);
        a=1;b=0;c=3;d=2;
        if (millis() - 200 > RGBtime)//150ms循环一次，灯光逆时针旋转
        {
          b = b == 3 ? 0 : b + 3;
          c = c == 3 ? 0 : c + 3;
          e = e == 3 ? 0 : e + 3;
          f = f == 3 ? 0 : f + 3;
          RGBtime = millis();
        }
        for(int j = 90;j < 130; j++) myMax.swerve(RETREAT_L, j, RETREAT_R, j);//后退
      } break;
      case 4:
      {
        myMax.rgbLed(a,255,128, 0 ); myMax.rgbLed(b,0,255,255); //RGB模式为左前方为橙色灯，其余为兰色
        myMax.rgbLed(c, 0, 255,255); myMax.rgbLed(d,0,255,255);
        for(int j = 100;j < 150;j++) myMax.swerve(ADVANCE_L, 40, ADVANCE_R, j);
        a=2;b=0;c=1;d=3;
      } break; //左转
      case 5:
      {
        myMax.swerve(ADVANCE_L, 0, ADVANCE_R, 0); //停止
        myMax.rgbLed(a,255, 0, 0 ); myMax.rgbLed(b, 0, 255, 0 );
        myMax.rgbLed(c, 0,  0,255); myMax.rgbLed(d,250,245,245);
        if (millis() - 300 > RGBtime)//150ms循环一次，灯光逆时针旋转
        {
          a = a == 3 ? 0 : a + 1;
          b = b == 3 ? 0 : b + 1;
          c = c == 3 ? 0 : c + 1;
          d = d == 3 ? 0 : d + 1;
          RGBtime = millis();
        }
      } break;
      case 6:
      {
        myMax.rgbLed(a,200,73, 0 ); myMax.rgbLed(b,30,144,255);
        myMax.rgbLed(c,153,51,250); myMax.rgbLed(d,51,161,201);
        if (millis() - 200> RGBtime)//150ms循环一次，灯光逆时针旋转
        {
          a = a == 0 ? 3 : a - 1;
          b = b == 0 ? 3 : b - 1;
          c = c == 0 ? 3 : c - 1;
          d = d == 0 ? 3 : d - 1;
          RGBtime = millis();
        }
        myMax.swerve(ADVANCE_L, 100, RETREAT_R, 100);
      } break;//原地旋转
      default:
      {
        myMax.rgbLed(a,200,73, 0 ); myMax.rgbLed(b,30,144,255);
        myMax.rgbLed(c,153,51,250); myMax.rgbLed(d,0, 201,201);
        a=0;b=1;c=2;d=3;
      } break;
    }
  }
}