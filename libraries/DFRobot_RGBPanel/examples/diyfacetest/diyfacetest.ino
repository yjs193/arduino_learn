#include<Wire.h>
#include <DFRobot_RGBPanel.h>
DFRobot_RGBPanel panel;

uint16_t face[8]={0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFF};

void setup(){
  Serial.begin(9600);
}

//第一列除了顶部第一个不显示，第一列剩余的7个分别显示红、绿、黄、蓝、紫、青、白七种颜色
void loop(){
   panel.diy(face,RED);
    //panel.clear();
}
