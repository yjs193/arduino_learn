#include "DFRobot_XiaMi.h"
#include "string.h"

DFRobot_XiaMi::DFRobot_XiaMi():sensorAddress(0x10){
	//this->addr;
    Wire.begin();
}

DFRobot_XiaMi::~DFRobot_XiaMi(){
	
}


// void DFRobot_XiaMi::begin(){
// 	Serial.println("DFRobot_XiaMi::begin");
// }
// void DFRobot_XiaMi::  (){
// 	Serial.println("DFRobot_XiaMi::i2cInit");
// }

//控制电机运行
void DFRobot_XiaMi::motorRun(MOTOR index, DIRECTION dir, int speed){
    
    uint8_t buf[3];
    if  (index !=0x08){ 
        buf[0]=index;
        buf[1]=dir;
        buf[2]=speed;
        Wire.beginTransmission(sensorAddress);
        Wire.write(buf,3);
        Wire.endTransmission();
    }else{
        buf[0]=0x00;
        buf[1]=dir;
        buf[2]=speed;
        Wire.beginTransmission(sensorAddress);
        Wire.write(buf,3);
        Wire.endTransmission();
        //delay(50);
        buf[0]=0x02;
        Wire.beginTransmission(sensorAddress);
        Wire.write(buf,3);
        Wire.endTransmission();
        //delay(50);
        buf[0]=0x04;
        Wire.beginTransmission(sensorAddress);
        Wire.write(buf,3);
        Wire.endTransmission();
        //delay(50);
        buf[0]=0x06;
        Wire.beginTransmission(sensorAddress);
        Wire.write(buf,3);
        Wire.endTransmission();
        //delay(50);
    }
}


//电机停止
void DFRobot_XiaMi::motorStop(MOTOR index){
    uint8_t buf[3];
    if(index != 0x08){
        //Serial.println(22);
        buf[0]=index;
        buf[1]=0;
        buf[2]=0;
        Wire.beginTransmission(sensorAddress);
        Wire.write(buf,3);
        Wire.endTransmission();

    }else{
        buf[0]=0x00;
        buf[1]=0;
        buf[2]=0;
        Wire.beginTransmission(sensorAddress);
        Wire.write(buf,3);
        Wire.endTransmission();
        //delay(50);
        buf[0]=0x02;
        Wire.beginTransmission(sensorAddress);
        Wire.write(buf,3);
        Wire.endTransmission();
        buf[0]=0x04;
        Wire.beginTransmission(sensorAddress);
        Wire.write(buf,3);
        Wire.endTransmission();
        buf[0]=0x06;
        Wire.beginTransmission(sensorAddress);
        Wire.write(buf,3);
        Wire.endTransmission();
    }

}

//控制交通灯
//% state1.min=0 state1.max=1
//% state2.min=0 state2.max=1
//% state3.min=0 state3.max=1
void DFRobot_XiaMi::LED(int state1,int state2,int state3){
    uint8_t buf[2];
    buf[0]=0x8;
    buf[1]=state1;
    Wire.beginTransmission(sensorAddress);
    Wire.write(buf,2);
    Wire.endTransmission();
    buf[0]=0x9;
    buf[1]=state2;
    Wire.beginTransmission(sensorAddress);
    Wire.write(buf,2);
    Wire.endTransmission();
    buf[0]=0x0A;
    buf[1]=state3;
    Wire.beginTransmission(sensorAddress);
    Wire.write(buf,2);
    Wire.endTransmission();
}

//获取旋转编码器数据
int DFRobot_XiaMi::readAngle(){
    uint8_t buf[2];
    Wire.beginTransmission(sensorAddress);
    Wire.write(0X0B);
    Wire.endTransmission();
    Wire.requestFrom(sensorAddress,(uint8_t)2);
    buf[0]=Wire.read();
    buf[1]=Wire.read(); 
    return buf[0]<<8|buf[1];
}

//获取火焰传感器数据
int DFRobot_XiaMi::readFlre(){
    uint8_t buf[2];
    Wire.beginTransmission(sensorAddress);
    Wire.write(0x0D);
    Wire.endTransmission();
    Wire.requestFrom(sensorAddress,(uint8_t)2);
    buf[0]=Wire.read();
    buf[1]=Wire.read(); 
    return buf[0]<<8|buf[1];

} 

//获取电机方向和速度
int DFRobot_XiaMi::motorState(MOTOR motor,STATE state){
    uint8_t buf[2];
    int data;
    Wire.beginTransmission(sensorAddress);
    Wire.write(motor);
    Wire.endTransmission();
    Wire.requestFrom(sensorAddress,(uint8_t)2);
    buf[0] = Wire.read();
    buf[1] = Wire.read();
    switch(state){
        case SPEED: data = buf[1];break;
        case DIR: data = buf[0];break;
    }
    return data;
}

//获取交通灯状态
int DFRobot_XiaMi::LEDState(LIGHT color){
    Wire.beginTransmission(sensorAddress);
    Wire.write(color);
    Wire.endTransmission();
    Wire.requestFrom(sensorAddress,(uint8_t)1);
    return Wire.read();
}

//控制继电器
void DFRobot_XiaMi::setRelay(RELAY state){
    digitalWrite(9, state == CLOSE ? HIGH : LOW);
}

//获取超声波数据
int DFRobot_XiaMi::Ultrasonic(){
    DFRobot_URM10 urm10;
    return urm10.getDistanceCM(0,1);
}

/**************************RGB**************************/
// int _brightness = 255;
// uint neopixel_buf[16*3];
// int i;
// for(i=0;i<48;i++){neopixel_buf[i]=0;}

// //设置R、G、B颜色
// int DFRobot_XiaMi::RGBColor(uint8_t r,uint8_t g,uint8_t b){
//     return (r<<16)+(g<<8)+b;
// }

// //设置rgb点亮从a~b
// uint8_t DFRobot_XiaMi::ledRange(uint8_t from,uint8_t to){
//     return (from-1<<16)+(2<<8)+to;
// }

// //指定灯号显示颜色
// void DFRobot_XiaMi::setIndexColor(uint8_t index,uint8_t rgb){
//     uint8_t f = index-1;
//     uint8_t t = index-1;
//     int j;
//     uint8_t r = ((rgb>>16)) * (_brightness / 255);
//     uint8_t g = ((rgb>>8) & 0xFF) * (_brightness / 255);
//     uint8_t b = ((rgb) & 0xFF) * (_brightness / 255);
//     if((index-1) > 15){
//         if((((index-1) >> 8) & 0xFF) == 0x02){
//             f = (index-1) >> 16;
//             t = (index-1) >> 8;
//         }else{
//             f = 0;
//             t = -1;
//         }
//     }
//     for(j=f;j<=t;j++){
//         neopixel_buf[j*3+0] = round(g) ;
//         neopixel_buf[j*3+1] = round(r);
//         neopixel_buf[j*3+2] = round(b);
//     }
//     digitalWrite(15,neopixel_buf);

// }

// //设置所有rgb灯显示颜色
// void DFRobot_XiaMi::showColor(uint8_t rgb){
//     uint8_t r = (rgb >> 16) * (_brightness/255);
//     uint8_t g = ((rgb >> 8) & 0xFF) * (_brightness/255);
//     uint8_t b = ((rgb) & 0xFF) * (_brightness/255);
//     int j;
//     for(j=0;j<48;j++){
//         if((j%3)==0){neopixel_buf[j] = round(g);}
//         if((j%3)==1){neopixel_buf[j] = round(r);}
//         if((j%3)==2){neopixel_buf[j] = round(b);}
//     }
//     //digitalWrite(15,neopixel_buf);
// }

// //设置rgb灯亮度
// void DFRobot_XiaMi::setBrightness(uint8_t brightness){
//     _brightness = brightness;
// }

// //关闭所有rgb灯
// void DFRobot_XiaMi::ledBlank(){
//     showColor(0);
// }

/**************************RGB**************************/

/**************************获取温湿度数据**************************/
//初始化AHT20传感器
void DFRobot_XiaMi::AHT20Init(uint8_t AHT20address){
    uint8_t data;
    Wire.beginTransmission(AHT20address);
    Wire.write(0xBA);
    Wire.endTransmission();
    Wire.requestFrom(sensorAddress,(uint8_t)1);
    data=Wire.read();
    if((data & 0x08) != 1){
        uint8_t buf[3];
        buf[0]=0xBE;
        buf[1]=0X08;
        buf[2]=0x00;
        Wire.beginTransmission(AHT20address);
        Wire.write(buf,3);
        Wire.endTransmission();
    }
}

//获取温湿度数据
double DFRobot_XiaMi::readAHT20(AHT20 state){
    uint8_t buf[3];
    double data;
    uint8_t buf1[7];
    buf[0]=0xAC;
    buf[1]=0X33;
    buf[2]=0x00;
    Wire.beginTransmission(0x38);
    Wire.write(buf,3);
    Wire.endTransmission();
    Wire.requestFrom(0x38,(int)7);
    for(int i=0;i<7;i++){
        buf1[i]=Wire.read();
    }
    switch(state){
        case HUM: data=(double)((buf1[1] << 12) + (buf1[2] << 4) + (buf1[3] >> 4)) / 1048576 * 100;break;
        case TEMP: data=(double)(((buf1[3] & 0x0f) << 16) + (buf1[4] << 8) + (buf1[5])) / 1048576 * 200 - 50,2;break;
        default: break;
    }
    return data;
}