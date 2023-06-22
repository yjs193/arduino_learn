#include "DFRobot_pinpongBread.h"
#include "string.h"
// #include "MPU6050.h"
#include "PB_MPU6050_6Axis_MotionApps20.h"
#include "PB_I2Cdev.h"

volatile bool piravailable;
volatile uint32_t ptmp = 0;
volatile int pcounter = 0;
volatile unsigned long pts;
volatile uint32_t pdata;
volatile uint32_t peventData;

DFRobot_pinpongBread *pIRremote_dal_pt = NULL;

MPU6050 mpu;

DFRobot_pinpongBread::DFRobot_pinpongBread()
{
    devAddr = 0x68;
    pIRremote_dal_pt = this;
    Wire.begin();
    
}

DFRobot_pinpongBread::~DFRobot_pinpongBread()
{
}

uint8_t mpuIntStatus;   // holds actual interrupt status byte from MPU
uint8_t devStatus;      // return status after each device operation (0 = success, !0 = error)
uint16_t packetSize;    // expected DMP packet size (default is 42 bytes)
uint16_t fifoCount;     // count of all bytes currently in FIFO
uint8_t fifoBuffer[64]; // FIFO storage buffer

Quaternion q; 
float euler[3];

void DFRobot_pinpongBread::mpu6050Init()
{
    
    mpu.initialize();

    Serial.println(mpu.testConnection() ? F("MPU6050 connection successful") : F("MPU6050 connection failed"));
    devStatus = mpu.dmpInitialize();
    if (devStatus == 0) {

        mpu.setDMPEnabled(true);

        mpuIntStatus = mpu.getIntStatus();
        packetSize = mpu.dmpGetFIFOPacketSize();
    } 
}

void DFRobot_pinpongBread::mpu6050GetData()
{
    mpuIntStatus = mpu.getIntStatus();
    // get current FIFO count
    fifoCount = mpu.getFIFOCount();
    if ((mpuIntStatus & 0x10) || fifoCount == 1024) {
        mpu.resetFIFO();
    } else if (mpuIntStatus & 0x02) {
        while (fifoCount < packetSize) fifoCount = mpu.getFIFOCount();
        mpu.getFIFOBytes(fifoBuffer, packetSize);
        fifoCount -= packetSize;
        mpu.dmpGetQuaternion(&q, fifoBuffer);
        mpu.dmpGetEuler(euler, &q);
        yaw = euler[0] * 180/M_PI;
        roll = euler[1] * 180/M_PI;
        pitch = euler[2] * 180/M_PI;
    }
}

void DFRobot_pinpongBread::setCursorString(int32_t index, String str){
    if(index <= 0 || index > 4){
        return;
    }else{
        this->setCursorLine(index);
        this->printLine(str);
    }
}

String DFRobot_pinpongBread::calData(String text, uint8_t bit){
    String temp = "";
    if (text.length() > bit){
        for (uint8_t i = 0; i < bit; i ++){
            temp += text[i];
        }
    }else if (text.length() < bit){
        for (uint8_t i = 0; i < bit - text.length(); i ++){
            temp += " ";
        }
        temp += text;
    }else{
        temp += text;
    }
    return temp;
}

String DFRobot_pinpongBread::calData(uint16_t value, uint8_t bit){
    String text = String(value);
    String temp = "";
    if (text.length() > bit){
        for (uint8_t i = 0; i < bit; i ++){
            temp += text[i];
        }
    }else if (text.length() < bit){
        for (uint8_t i = 0; i < bit - text.length(); i ++){
            temp += " ";
        }
        temp += text;
    }else{
        temp += text;
    }
    return temp;
}

String DFRobot_pinpongBread::calData(uint32_t value, uint8_t bit){
    String text = String(value);
    String temp = "";
    if (text.length() > bit){
        for (uint8_t i = 0; i < bit; i ++){
            temp += text[i];
        }
    }else if (text.length() < bit){
        for (uint8_t i = 0; i < bit - text.length(); i ++){
            temp += " ";
        }
        temp += text;
    }else{
        temp += text;
    }
    return temp;
}

String DFRobot_pinpongBread::calData(uint8_t value, uint8_t bit){
    String text = String(value);
    String temp = "";
    if (text.length() > bit){
        for (uint8_t i = 0; i < bit; i ++){
            temp += text[i];
        }
    }else if (text.length() < bit){
        for (uint8_t i = 0; i < bit - text.length(); i ++){
            temp += " ";
        }
        temp += text;
    }else{
        temp += text;
    }
    return temp;
}

String DFRobot_pinpongBread::calData(float value, uint8_t bit){
    String text = String(value);
    String temp = "";
    if (text.length() > bit){
        for (uint8_t i = 0; i < bit; i ++){
            temp += text[i];
        }
    }else if (text.length() < bit){
        for (uint8_t i = 0; i < bit - text.length(); i ++){
            temp += " ";
        }
        temp += text;
    }else{
        temp += text;
    }
    return temp;
}

String DFRobot_pinpongBread::calData(int value, uint8_t bit){
    String text = String(value);
    String temp = "";
    if (text.length() > bit){
        for (uint8_t i = 0; i < bit; i ++){
            temp += text[i];
        }
    }else if (text.length() < bit){
        for (uint8_t i = 0; i < bit - text.length(); i ++){
            temp += " ";
        }
        temp += text;
    }else{
        temp += text;
    }
    return temp;
}

float DFRobot_pinpongBread::getYaw(void){
    mpu6050GetData();
    return yaw;
}
float DFRobot_pinpongBread::getRoll(void){
    mpu6050GetData();
    return roll;
}
float DFRobot_pinpongBread::getPitch(void){
    mpu6050GetData();
    return pitch;
}

inline void DFRobot_pinpongBread::setRefreshRange(int address, uint8_t reg, int columnStart, int columnEnd, int rowStart, int rowEnd)
{
    // send a bunch of pdata in one xmission
    writeByte(address, reg, SSD1306_COLUMNADDR);
    writeByte(address, reg, columnStart);
    writeByte(address, reg, columnEnd);
    writeByte(address, reg, SSD1306_PAGEADDR);
    writeByte(address, reg, rowStart);
    writeByte(address, reg, rowEnd);
}

inline int DFRobot_pinpongBread::writeByte(uint8_t address, uint8_t reg, uint8_t pdata)
{
    pBaord_I2CBEGIN();
    Wire.beginTransmission(address);
    Wire.write(reg);
    Wire.write(pdata);
    Wire.endTransmission();
    return 1;
}

inline int DFRobot_pinpongBread::writeBuffer(int address, uint8_t reg, unsigned char *pdata, int length)
{
    int count = 0;
    pBaord_I2CBEGIN();
    Wire.beginTransmission(address);
    Wire.write(reg);
    while (count < length)
    {
        Wire.write(*pdata);
        pdata++;
        count++;
    }
    Wire.endTransmission();
    return count;
}

//控制电机运行
void DFRobot_pinpongBread::motorRun(MOTOR index, DIRECTION dir, int speed)
{
    uint8_t buf[3];
    if (index != 0x4)
    {
        buf[0] = index;
        buf[1] = dir;
        buf[2] = speed;
        Wire.beginTransmission(0x10);
        Wire.write(buf, 3);
        Wire.endTransmission();
    }
    else
    {
        buf[0] = 0x00;
        buf[1] = dir;
        buf[2] = speed;
        Wire.beginTransmission(0x10);
        Wire.write(buf, 3);
        Wire.endTransmission();

        buf[0] = 0x02;
        Wire.beginTransmission(0x10);
        Wire.write(buf, 3);
        Wire.endTransmission();
    }
}

//电机停止
void DFRobot_pinpongBread::motorStop(MOTOR index)
{
    motorRun(index, 0, 0);
}

void DFRobot_pinpongBread::LED(int state)
{
    digitalWrite(2, state == 1 ? HIGH : LOW);
}

//获取按键值
uint8_t DFRobot_pinpongBread::readButton(uint8_t pin)
{
    return digitalRead(pin);
}

//获取角度传感器
int DFRobot_pinpongBread::readAngle()
{
    return analogRead(A3);
}

//获取光线感器
int DFRobot_pinpongBread::readLight()
{
    return analogRead(A2);
}

/**************************获取温湿度数据**************************/
//初始化AHT20传感器
void DFRobot_pinpongBread::AHT20Init()
{
    uint8_t pdata;
    Wire.beginTransmission(0x38);
    Wire.write(0xBA);
    Wire.endTransmission();
    Wire.requestFrom(0x38, (uint8_t)1);
    pdata = Wire.read();
    Serial.println("-----------------");
    Serial.println(pdata);
    if ((pdata & 0x08) != 1)
    {
        uint8_t buf[3];
        buf[0] = 0xBE;
        buf[1] = 0X08;
        buf[2] = 0x00;
        Wire.beginTransmission(0x38);
        Wire.write(buf, 3);
        Wire.endTransmission();
    }
}

//获取温湿度数据
double DFRobot_pinpongBread::readAHT20(AHT20 state)
{
    uint8_t buf[3];
    double pdata;
    uint8_t buf1[7];
    buf[0] = 0xAC;
    buf[1] = 0X33;
    buf[2] = 0x00;
    Wire.beginTransmission(0x38);
    Wire.write(buf, 3);
    Wire.endTransmission();
    Wire.requestFrom(0x38, (int)7);
    for (int i = 0; i < 7; i++)
    {
        buf1[i] = Wire.read();
    }
    uint32_t hum = ((uint32_t)buf1[1] << 12) + ((uint32_t)buf1[2] << 4) + ((uint32_t)buf1[3] >> 4);
    uint32_t temp = ((uint32_t)(buf1[3] & 0x0f) << 16) + ((uint32_t)buf1[4] << 8) + ((uint32_t)buf1[5]);
    switch (state)
    {
    case HUM:
        pdata = (double)(hum) / 1048576.0 * 100;
        break;
    case TEMP:
        pdata = (double)(temp) / 1048576.0 * 200 - 50;
        break;
    default:
        break;
    }
    return pdata;
}

void PIR_INT()
{
    piravailable = false;

    unsigned long long x = micros();
    unsigned long interval = x - pts;

    pts = x;
    if (interval > 3000)
    {
        pcounter = 0;
        ptmp = 0;
        return;
    }

    if ((interval > 2000) && (interval < 2500))
    {
        ptmp <<= 1;
        ptmp |= 1;
    }
    else
    {
        ptmp <<= 1;
    }

    pcounter++;

    if (pcounter == 32)
    {
        //Serial.print("ptmp= ");Serial.println(ptmp);
        pIRremote_dal_pt->decode(ptmp);
        pcounter = 0;
    }
}

void DFRobot_pinpongBread::decode(uint32_t code)
{
    //byte add = (code>>24)&0xff;
    pdata = (code)&0xffffffff;
    peventData = pdata;
    irData = pdata;
    irEventData = irData;
    piravailable = true;
}

// initialization
void DFRobot_pinpongBread::enableIRIn()
{
    pinMode(recvpin, INPUT);
    attachInterrupt(digitalPinToInterrupt(recvpin), PIR_INT, RISING);
}

void DFRobot_pinpongBread::ir_begin(int recvpin1)
{
    recvpin = recvpin1;
    this->enableIRIn();
    this->enable = true;
}

uint32_t DFRobot_pinpongBread::getData(void)
{
    uint32_t _data = irData;
    irData = 0;
    return _data;
}

String DFRobot_pinpongBread::getIrCommand()
{
    uint32_t command = this->getData();
    char string[20];

    memset(string, '\0', 20);
    sprintf(string, "%lX", command);
    return (String)string;
}
