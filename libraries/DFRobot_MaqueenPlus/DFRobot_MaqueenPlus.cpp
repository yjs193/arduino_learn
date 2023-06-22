#include "DFRobot_MaqueenPlus.h"


DFRobot_MaqueenPlus::DFRobot_MaqueenPlus()
{
    first = true;
	_speedLeft = 0;
	_speedRight = 0;
    Wire.begin();
}

DFRobot_MaqueenPlus::~DFRobot_MaqueenPlus()
{
    first = true;
}

void DFRobot_MaqueenPlus::hardwareReset()
{
    if(!first) return;
    // motor
    uint8_t buf[] = {0x00, 0, 0, 0, 0};
    this->i2cWriteBuffer(0x10, buf, 5);
    // rgb
    buf[0] = 0x0B;
    buf[1] = this->PUT;
    buf[2] = this->PUT;
    i2cWriteBuffer(0x10, buf, 3);

    first = false;
}

void DFRobot_MaqueenPlus::writePid(PID pid)
{
    hardwareReset();
    uint8_t buf[2];
    buf[0] = 0x0A;
    buf[1] = pid;
    this->i2cWriteBuffer(0x10, buf, 2);
}

void DFRobot_MaqueenPlus::motorRun(Dir index, SpeedDir direction, int speed)
{
    hardwareReset();
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

void DFRobot_MaqueenPlus::motorStop(Dir index)
{
    hardwareReset();
    motorRun(index, CW, 0);
}

void DFRobot_MaqueenPlus::mostotCompensation(Dir motor, int speed)
{
    hardwareReset();
    uint8_t buf[2];
    buf[1] = speed;
    if (motor == 1) 
        buf[0] = 0x08;
    else if (motor == 2) 
        buf[0] = 0x09;
    else {
        buf[0] = 0x08;
        this->i2cWriteBuffer(0x10, buf, 2);
        buf[0] = 0x09;
    }
    this->i2cWriteBuffer(0x10, buf, 2);
}

uint32_t DFRobot_MaqueenPlus::readSpeed(Dir index)
{
    hardwareReset();
    uint8_t x[4];
    this->i2cReadBuffer(0x00, x, 4);
    if (index == 1) {
		if(x[1]<20 && x[1]!=0){
			return x[1]+255;
		}else{
			return x[1];
		}
		//return _speedLeft>120?(x[1]<20?x[1]+255:x[1]):x[1];
        //return x[1];
    } else if (index == 2) {
		if(x[3]<20 && x[3]!=0){
			return x[3]+255;
		}else{
			return x[3];
		}
		//return _speedRight>120?(x[3]<20?x[3]+255:x[3]):x[3];
        //return x[3];
    }
}

uint32_t DFRobot_MaqueenPlus::readDirection(Dir index)
{
    hardwareReset();
    uint8_t x[4];
    this->i2cReadBuffer(0x00, x, 4);
    if (index == 1) {
        return x[0];
    } else if (index == 2) {
        return x[2];
    }
    return 0;
}

void DFRobot_MaqueenPlus::servoRun(Servos index, uint8_t angle)
{
    hardwareReset();
    uint8_t buf[2];
    buf[1] = constrain(angle, 0, 180);
    if (index == 1) 
        buf[0] = 0x14;
    else if (index == 2)
        buf[0] = 0x15;
    else if (index == 3)
        buf[0] = 0x16;
    else if (index == 4) {
        buf[0] = 0x14;
        i2cWriteBuffer(0x10, buf, 2);
        buf[0] = 0x15;
        i2cWriteBuffer(0x10, buf, 2);
        buf[0] = 0x16;
    } else {
        return;
    }
    i2cWriteBuffer(0x10, buf, 2);
}

void DFRobot_MaqueenPlus::setRGB(Dir rgb, Color color)
{
    hardwareReset();
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

uint32_t DFRobot_MaqueenPlus::readPatrol(Patrol patrol)
{
    hardwareReset();
    uint8_t y[1];
    i2cReadBuffer(0x1D, y, 1);
    uint8_t mark;
    switch(patrol){
        case 1: mark = (y[0] & 0x04) == 0x04 ? 1 : 0; break;
        case 2: mark = (y[0] & 0x02) == 0x02 ? 1 : 0; break;
        case 3: mark = (y[0] & 0x08) == 0x08 ? 1 : 0; break;
        case 4: mark = (y[0] & 0x10) == 0x10 ? 1 : 0; break;
        case 5: mark = (y[0] & 0x01) == 0x01 ? 1 : 0; break;
        case 6: mark = (y[0] & 0x20) == 0x20 ? 1 : 0; break;
    }
    return mark;
}

uint32_t DFRobot_MaqueenPlus::readPatrolVoltage(Patrol patrol)
{
    hardwareReset();
    uint8_t y[12];
    i2cReadBuffer(0x1E, y, 12);
    if (patrol == 1) {
        return y[5] | y[4] << 8;
    } else if (patrol == 2) {
        return y[3] | y[2] << 8;
    } else if (patrol == 3) {
        return y[7] | y[6] << 8;
    } else if (patrol == 4) {
        return y[9] | y[8] << 8;
    } else if (patrol == 5) {
        return y[1] | y[0] << 8;
    } else {
        return y[11] | y[10] << 8;
    }
    return 0;
}

String DFRobot_MaqueenPlus::readVersion()
{
    hardwareReset();
    uint8_t len;
    i2cReadBuffer(0x32, &len, 1);
    uint8_t versionName[len+1];
    versionName[len] = '\0';
    i2cReadBuffer(0x33, versionName, len);
    return String((char *)versionName);
}

float DFRobot_MaqueenPlus::readeDistance(Dir index)
{
	hardwareReset();
	uint8_t y[4];
    i2cReadBuffer(0x04, y, 4);
	if(index == 1){
		return (((y[0]<<8 | y[1])*10)/900.00);
		//int b = String(a).indexOf(".");
		//return String(a).substring(0, b+3);
	}else{
		return (((y[2]<<8 | y[3])*10)/900.00);
		//int b = String(a).indexOf(".");
		//return String(a).substring(0, b+3);
	}
}

void DFRobot_MaqueenPlus::clearDistance(Dir index)
{
	hardwareReset();
    if (index == 1){
        uint8_t buf[2];
        buf[0] = 0x04;
        buf[1] = 0;
        i2cWriteBuffer(0x10, buf, 2);
    } else if (index == 2){
        uint8_t buf[2];
        buf[0] = 0x06;
        buf[1] = 0;
        i2cWriteBuffer(0x10, buf, 2);
    } else if (index == 3){
        uint8_t buf[4];
        buf[0] = 0x04;
        buf[1] = 0;
        buf[2] = 0;
		buf[3] = 0;
        i2cWriteBuffer(0x10, buf, 4);
    }
}

void DFRobot_MaqueenPlus::i2cWriteBuffer(int addr, unsigned char *p, int len)
{
    Wire.setClock(100000);
    Wire.beginTransmission(addr);
    for(int i=0; i<len; i++)
        Wire.write((uint8_t)p[i]);
    Wire.endTransmission();
}

void DFRobot_MaqueenPlus::i2cReadBuffer(uint8_t reg, unsigned char* p, int len)
{
    Wire.setClock(100000);
    Wire.beginTransmission(0x10);
    Wire.write(reg);
    Wire.endTransmission();
    Wire.requestFrom(0x10, len);
    for(int i=0; i<len; i++)
        p[i] = Wire.read();
}
