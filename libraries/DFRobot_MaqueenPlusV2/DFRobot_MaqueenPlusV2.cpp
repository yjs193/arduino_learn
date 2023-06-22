#include "DFRobot_MaqueenPlusV2.h"

#if defined(NRF5) || defined(NRF52833)
#include <avr/interrupt.h>
#include <basic.h>
#endif


volatile uint32_t tmpV2=0;
volatile int counterV2=0;
volatile unsigned long tsV2;
volatile uint32_t dataV2;
volatile uint32_t eventDataV2;
volatile bool iravailableV2;
long micrV2 = 0;
uint32_t rec_dataV2;

#if defined(NRF52833)
int ir_codeV2 = 0x00;
int ir_addrV2 = 0x00;
#endif

DFRobot_MaqueenPlusV2 *IRremote_dal_ptV2 = NULL;
bool task_initV2;

int8_t transMindV2(uint8_t dataV2){
	switch(dataV2){
		case 255: dataV2 = 0; break;
		case 127: dataV2 = 1; break;
		case 191: dataV2 = 2; break;
		case 223: dataV2 = 4; break;
		case 95: dataV2 = 5; break;
		case 159: dataV2 = 6; break;
		case 239: dataV2 = 8; break;
		case 111: dataV2 = 9; break;
		case 175: dataV2 = 10; break;
		case 207: dataV2 = 12; break;
		case 79: dataV2 = 13; break;
		case 143: dataV2 = 14; break;
		case 247: dataV2 = 16; break;
		case 119: dataV2 = 17; break;
		case 183: dataV2 = 18; break;
		case 215: dataV2 = 20; break;
		case 87: dataV2 = 21; break;
		case 151: dataV2 = 22; break;
		case 231: dataV2 = 24; break;
		case 103: dataV2 = 25; break;
		case 167: dataV2 = 26; break;
		default: return -1;
	}
	return dataV2;
}


void loopEventV2(){
    while(true){
        uint32_t val = eventDataV2;
        eventDataV2 = 0;
        if(val != 0){
            //if(((val&0xff)^((val>>8)&0xff)) == 0xff){
                if(IRremote_dal_ptV2->IRcb != NULL){
                    IRremote_dal_ptV2->IRcb(transMindV2(val));
                }
            //}
        }

    fiber_sleep(50);

    }
}



#if defined(NRF52833)
void Ir_init(int e) {
	if(e == 0) return;
    iravailableV2 = false;

    unsigned long long x = micros();
    unsigned long interval = x-tsV2;

    tsV2 = x;
    if(interval > 3000){
        counterV2 = 0;
        tmpV2 = 0;
        return;
    }

    if((interval > 2000) && (interval<2500)){
        tmpV2 <<= 1;
        tmpV2 |=  1;
    }else{
        tmpV2 <<= 1;
    }

    counterV2++;

    if(counterV2 == 32){
        IRremote_dal_ptV2->decode(tmpV2); 
        counterV2 = 0;
    }

}
#else
void Ir_init() {
    iravailableV2 = false;

    unsigned long long x = micros();
    unsigned long interval = x-tsV2;

    tsV2 = x;
    if(interval > 3000){
        counterV2 = 0;
        tmpV2 = 0;
        return;
    }

    if((interval > 2000) && (interval<2500)){
        tmpV2 <<= 1;
        tmpV2 |=  1;
    }else{
        tmpV2 <<= 1;
    }

    counterV2++;

    if(counterV2 == 32){
        //Serial.print("tmpV2= ");Serial.println(tmpV2);
        IRremote_dal_ptV2->decode(tmpV2); 
        counterV2 = 0;
    }

}
#endif

DFRobot_MaqueenPlusV2::DFRobot_MaqueenPlusV2()
{
#if defined(NRF52833)      //解决麦昆V2 huskylens 同时上电启动后初始化不成功 wsq
    delay(500);
#elif defined(NRF5)         //microbit v1.3 也有上电后初始化不成功 wsq
    delay(200);
#endif
    IRremote_dal_ptV2 = this;
    first = true;
	_speedLeft = 0;
	_speedRight = 0;
    Wire.begin();
}

DFRobot_MaqueenPlusV2::~DFRobot_MaqueenPlusV2()
{
    first = true;
}

int8_t DFRobot_MaqueenPlusV2::getData(void) {
    uint32_t _data = irData;
    irData = 0;
    return transMindV2(_data);
}

void DFRobot_MaqueenPlusV2::decode(uint32_t code) {  
    //byte add = (code>>24)&0xff;
    dataV2 = (code)&0xffffffff;
    eventDataV2 = dataV2;
    irData = dataV2;
	irEventData = irData;
    iravailableV2 = true;
}

void DFRobot_MaqueenPlusV2::enableIRIn() {
    pinMode(recvpin,INPUT);
    attachInterrupt(recvpin,Ir_init,RISING);
}


void DFRobot_MaqueenPlusV2::IRbegin(int recvpin1) {
	recvpin = recvpin1;
#if defined(NRF52833)
    if(recvpin1 == 20){//maqueen maqueenPlus  //锟斤拷锟斤拷p16锟叫断诧拷锟斤拷锟斤拷 bug wsq
		;
	}else{
		this->enableIRIn();
		this->enable = true;
	}
#endif
    this->enableIRIn();
    this->enable = true;

}

void DFRobot_MaqueenPlusV2::setCallback(IRCall cb){
    IRcb = cb;
    if(!task_initV2){
#if defined(NRF5)
    create_fiber(loopEventV2);
#elif defined(NRF52833)
    if(recvpin == 20){  // bug wsq 16引脚可以进中断
		//this->resume();
		//this->enable = false;
		// create_fiber(loopEventMaqueen);
	}else{
		create_fiber(loopEventV2);
	}
#endif
        task_initV2 = true;
    }
}


void DFRobot_MaqueenPlusV2::motorRun(Dir index, SpeedDir direction, int speed)
{
    // hardwareReset();
    speed = abs(speed);
    if(speed >= 255) {
        speed = 255;
    }
    if(index > 3 || index < 1)
        return;
    if(index == 1){
        uint8_t buf[] = {0x00, direction, speed};
        this->i2cWriteBuffer(0x10, buf, 3);
		_speedLeft = speed;
    }
    if(index == 2){
        uint8_t buf[] = {0x02, direction, speed};
        this->i2cWriteBuffer(0x10, buf, 3);
		_speedRight = speed;
    }
    if(index == 3){
        uint8_t buf[] = {0x00, direction, speed, direction, speed};
        this->i2cWriteBuffer(0x10, buf, 5);
		_speedLeft = speed;
		_speedRight = speed;
    }
}

void DFRobot_MaqueenPlusV2::motorStop(Dir index)
{
    // hardwareReset();
    motorRun(index, CW, 0);
}

void DFRobot_MaqueenPlusV2::setRGB(Dir rgb, Color color)
{
    // hardwareReset();
    if (rgb == 1){
        uint8_t buf[2];
        buf[0] = 0x0B;
        buf[1] = color;
        i2cWriteBuffer(0x10, buf, 2);
    } else if (rgb == 2){
        uint8_t buf[2];
        buf[0] = 0x0C;
        buf[1] = color;
        i2cWriteBuffer(0x10, buf, 2);
    } else if (rgb == 3){
        uint8_t buf[3];
        buf[0] = 0x0B;
        buf[1] = color;
        buf[2] = color;
        i2cWriteBuffer(0x10, buf, 3);
    }
}

uint32_t DFRobot_MaqueenPlusV2::readPatrol(Patrol patrol)
{
    // hardwareReset();
    uint8_t y[1];
    i2cReadBuffer(0x1D, y, 1);
    uint8_t mark;
    switch(patrol){
        case 1: mark = (y[0] & 0x08) == 0x08 ? 1 : 0; break;
        case 2: mark = (y[0] & 0x04) == 0x04 ? 1 : 0; break;
        case 3: mark = (y[0] & 0x02) == 0x02 ? 1 : 0; break;
        case 4: mark = (y[0] & 0x10) == 0x10 ? 1 : 0; break;
        case 5: mark = (y[0] & 0x01) == 0x01 ? 1 : 0; break;
    }
    return mark;
}

uint32_t DFRobot_MaqueenPlusV2::readPatrolVoltage(Patrol patrol)
{
    // hardwareReset();
    uint8_t y[2];
    // i2cReadBuffer(0x1E, y, 12);
    if (patrol == 1) {
        i2cReadBuffer(0x24, y, 2);
        return y[0] | y[1] << 8;
    } else if (patrol == 2) {
        i2cReadBuffer(0X22, y, 2);
        return y[0] | y[1] << 8;
    } else if (patrol == 3) {
        i2cReadBuffer(0X20, y, 2);
        return y[0] | y[1] << 8;
    } else if (patrol == 4) {
        i2cReadBuffer(0X26, y, 2);
        return y[0] | y[1] << 8;
    } else if (patrol == 5) {
        i2cReadBuffer(0X1E, y, 2);
        return y[0] | y[1] << 8;
    }
    return 0;
}

String DFRobot_MaqueenPlusV2::readVersion()
{
    // hardwareReset();
    uint8_t len;
    i2cReadBuffer(0x32, &len, 1);
    uint8_t versionName[len+1];
    versionName[len] = '\0';
    i2cReadBuffer(0x33, versionName, len);
    return String((char *)versionName);
}

void DFRobot_MaqueenPlusV2::i2cWriteBuffer(int addr, unsigned char *p, int len)
{
    Wire.setClock(100000);
    Wire.beginTransmission(addr);
    for(int i=0; i<len; i++)
        Wire.write((uint8_t)p[i]);
    Wire.endTransmission();
}

void DFRobot_MaqueenPlusV2::i2cReadBuffer(uint8_t reg, unsigned char* p, int len)
{
    Wire.setClock(100000);
    Wire.beginTransmission(0x10);
    Wire.write(reg);
    Wire.endTransmission();
    Wire.requestFrom(0x10, len);
    for(int i=0; i<len; i++)
        p[i] = Wire.read();
}
