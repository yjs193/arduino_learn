#include <DFRobot_MicroTouch.h>

DFRobot_MicroTouch *mitcoTouchActiveObject = NULL;

static void microTouchEvent();
static void touchKeyCallback0();
static void touchKeyCallback1();
static void touchKeyCallback2();
static void touchKeyCallback3();
static void touchKeyCallback4();
static void touchKeyCallback5();
static void touchKeyCallback6();
static void touchKeyCallback7();
static void touchKeyCallback8();
static void touchKeyCallback9();
static void touchKeyCallback10();
static void touchKeyCallback11();
static void touchKeyCallback12();
static void touchKeyCallback13();
static void touchKeyCallback14();
static void touchKeyCallback15();
static void touchKeyCallback16();
static const TouchKeyAction touchKeyAction[17] = {
    touchKeyCallback0, touchKeyCallback1, touchKeyCallback2, touchKeyCallback3, touchKeyCallback4,
    touchKeyCallback5, touchKeyCallback6, touchKeyCallback7, touchKeyCallback8, touchKeyCallback9,
    touchKeyCallback10, touchKeyCallback11, touchKeyCallback12, touchKeyCallback13, touchKeyCallback14,
    touchKeyCallback15, touchKeyCallback16
};


DFRobot_MicroTouch::DFRobot_MicroTouch()
{
    mitcoTouchActiveObject = this;
    i2cEnable = false;
    eventCount = 0;
}

DFRobot_MicroTouch::~DFRobot_MicroTouch()
{
    
}

void DFRobot_MicroTouch::begin()
{
    if(!i2cEnable){
        Wire.begin();
        i2cEnable = true;
    }
    create_fiber(microTouchEvent);
}


void DFRobot_MicroTouch::servo(Servo index, uint32_t angle)
{
    angle = constrain(angle, 0, 180);
    uint8_t buf[2];
    switch(index){
        case DFRobot_MicroTouch::S1:
            buf[0] = 0x14;
        break;
        case DFRobot_MicroTouch::S2:
            buf[0] = 0x15;
        break;
        default:
            buf[0] = 0x14;
        break;
    }
    buf[1] = angle;
    i2cWrite(0x10, buf, 2);
}

void DFRobot_MicroTouch::motorRun(Motor index, uint32_t speed, Direction dir)
{
    uint8_t buf[3];
    buf[2] = speed;
    switch(dir){
        case DFRobot_MicroTouch::Forward:
            buf[1] = 0x01;
        break;
        case DFRobot_MicroTouch::Backward:
            buf[1] = 0x00;
        break;
        default:
            buf[1] = 0x01;
        break;
    }
    switch(index){
        case DFRobot_MicroTouch::M1:
            buf[0] = 0x00;
        break;
        case DFRobot_MicroTouch::M2:
            buf[0] = 0x02;
        break;
        case DFRobot_MicroTouch::ALL:
            buf[0] = 0x00;
            i2cWrite(0x10, buf, 3);
            buf[0] = 0x02;
        break;
        default:
            buf[0] = 0x00;
        break;
    }
    i2cWrite(0x10, buf, 3);
}

void DFRobot_MicroTouch::motorStop(Motor index)
{
    motorRun(index, 0);
}

void DFRobot_MicroTouch::vibrationMotor(bool sw)
{
    uint8_t buf[2];
    buf[0] = 0x0A;
    if(sw){
        buf[1] = 0x01;
    }else{
        buf[1] = 0x00;
    }
    i2cWrite(0x10, buf, 2);
}

void DFRobot_MicroTouch::touchKeyRegister(KeyValue kv, const TouchKeyAction action)
{
    if(action == NULL) return;
    KV item = {kv, action, false, NULL, 0};
    kbCallback.push_back(item);
}

bool DFRobot_MicroTouch::keyPressed(KeyValue kv)
{
    if (keyRow & kv) {
        return true;
    } else {
        return false;
    }
}

uint32_t DFRobot_MicroTouch::coorToPixel(uint32_t x, uint32_t y)
{
    uint8_t coord[][4] = {{1, 2, 3, 10},
                         {4, 5, 6, 11},
                         {7, 8, 9, 12},
                         {15, 0, 14, 13}};
    x = constrain(x, 0, 3);
    y = constrain(y, 0, 3);
    return coord[y][x];
}

int32_t DFRobot_MicroTouch::readKeyBasc()
{
    uint16_t tab[] = {0x02, 0x80, 0x40, 0x20, 0x200, 0x100, 0x400, 0x4000,
    0x1000, 0x2000, 0x10, 0x800, 0x8000, 0x08, 0x04, 0x01};
    
    uint16_t TPval = i2cRead16(0x57);
    prevKey = key;
    key = -1;
    keyRow = TPval;
    for (int i = 0; i < 16; i++) {
        if (TPval & tab[i]) {
            key = i;
            break;
        }
    }
    if (key > 9) {
        if (key != -1) {
            String tmp[] = {"+", "-", "*", "/", "=", "DF"};
            mathKeyNumber = -1;
            mathKeyFunction = tmp[key - 10];
        }
    } else {
        if ((prevKey != key) && (key != -1)) {
            newKeyFlag = true;
        } else {
            newKeyFlag = false;
        }
        if (newKeyFlag) {
            if (mathKeyNumber == -1) {
                mathKeyNumber = 0;
            }
            mathKeyNumber = mathKeyNumber * 10 + key;
            newKeyFlag = false;
        }
    }
    return key;
}

int32_t DFRobot_MicroTouch::readNumKeyMath()
{
    return mathKeyNumber;
}

String  DFRobot_MicroTouch::readFunKeyMath()
{
    return mathKeyFunction;
}


void DFRobot_MicroTouch::i2cWrite(uint8_t addr, uint8_t* buf, uint16_t length)
{
    if(!i2cEnable){
        Wire.begin();
        i2cEnable = true;
    }
    Wire.setClock(100000);
    Wire.beginTransmission(addr);
    while(length--) {
        Wire.write(*buf);
        buf++;
    }
    Wire.endTransmission();
    Wire.setClock(100000);
}

void DFRobot_MicroTouch::i2cRead(uint8_t addr, uint8_t* buf, uint16_t length)
{
    if(!i2cEnable){
        Wire.begin();
        i2cEnable = true;
    }
    Wire.setClock(100000);
    Wire.requestFrom(addr, length);
    for(int i=0; i<length; i++){
        buf[i] = Wire.read();
    }
    Wire.setClock(100000);
}

uint16_t DFRobot_MicroTouch::i2cRead16(uint8_t addr)
{
    if(!i2cEnable){
        Wire.begin();
        i2cEnable = true;
    }
    uint8_t buf[2];
    Wire.setClock(100000);
    Wire.requestFrom(addr, 2);
    buf[0] = Wire.read();
    buf[1] = Wire.read();
    Wire.setClock(100000);
    return (((uint16_t)buf[0])<<8)| buf[1];
}

void DFRobot_MicroTouch::runCallback(uint8_t index)
{
    if(!kbCallback[index].action) return;
    if(kbCallback[index].fiber){
        if(millis()-kbCallback[index].time > 1000) {
            release_fiber(kbCallback[index].fiber);
            kbCallback[index].fiber = NULL;
            eventCount--;
        }else{
            return;
        }
    }
    Fiber *f = create_fiber(touchKeyAction[index]);
    if(!f) return;
    kbCallback[index].fiber = f;
    kbCallback[index].time = millis();
}

static void touchKeyCallback0(){
    mitcoTouchActiveObject->eventCount++;
    mitcoTouchActiveObject->kbCallback[0].action();
    mitcoTouchActiveObject->kbCallback[0].fiber = NULL;
    mitcoTouchActiveObject->eventCount--;
}

static void touchKeyCallback1(){
    mitcoTouchActiveObject->eventCount++;
    mitcoTouchActiveObject->kbCallback[1].action();
    mitcoTouchActiveObject->kbCallback[1].fiber = NULL;
    mitcoTouchActiveObject->eventCount--;
}

static void touchKeyCallback2(){
    mitcoTouchActiveObject->eventCount++;
    mitcoTouchActiveObject->kbCallback[2].action();
    mitcoTouchActiveObject->kbCallback[2].fiber = NULL;
    mitcoTouchActiveObject->eventCount--;
}

static void touchKeyCallback3(){
    mitcoTouchActiveObject->eventCount++;
    mitcoTouchActiveObject->kbCallback[3].action();
    mitcoTouchActiveObject->kbCallback[3].fiber = NULL;
    mitcoTouchActiveObject->eventCount--;
}

static void touchKeyCallback4(){
    mitcoTouchActiveObject->eventCount++;
    mitcoTouchActiveObject->kbCallback[4].action();
    mitcoTouchActiveObject->kbCallback[4].fiber = NULL;
    mitcoTouchActiveObject->eventCount--;
}

static void touchKeyCallback5(){
    mitcoTouchActiveObject->eventCount++;
    mitcoTouchActiveObject->kbCallback[5].action();
    mitcoTouchActiveObject->kbCallback[5].fiber = NULL;
    mitcoTouchActiveObject->eventCount--;
}

static void touchKeyCallback6(){
    mitcoTouchActiveObject->eventCount++;
    mitcoTouchActiveObject->kbCallback[6].action();
    mitcoTouchActiveObject->kbCallback[6].fiber = NULL;
    mitcoTouchActiveObject->eventCount--;
}

static void touchKeyCallback7(){
    mitcoTouchActiveObject->eventCount++;
    mitcoTouchActiveObject->kbCallback[7].action();
    mitcoTouchActiveObject->kbCallback[7].fiber = NULL;
    mitcoTouchActiveObject->eventCount--;
}

static void touchKeyCallback8(){
    mitcoTouchActiveObject->eventCount++;
    mitcoTouchActiveObject->kbCallback[8].action();
    mitcoTouchActiveObject->kbCallback[8].fiber = NULL;
    mitcoTouchActiveObject->eventCount--;
}

static void touchKeyCallback9(){
    mitcoTouchActiveObject->eventCount++;
    mitcoTouchActiveObject->kbCallback[9].action();
    mitcoTouchActiveObject->kbCallback[9].fiber = NULL;
    mitcoTouchActiveObject->eventCount--;
}

static void touchKeyCallback10(){
    mitcoTouchActiveObject->eventCount++;
    mitcoTouchActiveObject->kbCallback[10].action();
    mitcoTouchActiveObject->kbCallback[10].fiber = NULL;
    mitcoTouchActiveObject->eventCount--;
}

static void touchKeyCallback11(){
    mitcoTouchActiveObject->eventCount++;
    mitcoTouchActiveObject->kbCallback[11].action();
    mitcoTouchActiveObject->kbCallback[11].fiber = NULL;
    mitcoTouchActiveObject->eventCount--;
}

static void touchKeyCallback12(){
    mitcoTouchActiveObject->eventCount++;
    mitcoTouchActiveObject->kbCallback[12].action();
    mitcoTouchActiveObject->kbCallback[12].fiber = NULL;
    mitcoTouchActiveObject->eventCount--;
}

static void touchKeyCallback13(){
    mitcoTouchActiveObject->eventCount++;
    mitcoTouchActiveObject->kbCallback[13].action();
    mitcoTouchActiveObject->kbCallback[13].fiber = NULL;
    mitcoTouchActiveObject->eventCount--;
}

static void touchKeyCallback14(){
    mitcoTouchActiveObject->eventCount++;
    mitcoTouchActiveObject->kbCallback[14].action();
    mitcoTouchActiveObject->kbCallback[14].fiber = NULL;
    mitcoTouchActiveObject->eventCount--;
}

static void touchKeyCallback15(){
    mitcoTouchActiveObject->eventCount++;
    mitcoTouchActiveObject->kbCallback[15].action();
    mitcoTouchActiveObject->kbCallback[15].fiber = NULL;
    mitcoTouchActiveObject->eventCount--;
}

static void touchKeyCallback16(){
    mitcoTouchActiveObject->eventCount++;
    mitcoTouchActiveObject->kbCallback[16].action();
    mitcoTouchActiveObject->kbCallback[16].fiber = NULL;
    mitcoTouchActiveObject->eventCount--;
}

static void microTouchEvent(){
    while(true){
        uint16_t TPval = mitcoTouchActiveObject->i2cRead16(0x57);
        mitcoTouchActiveObject->readKeyBasc();
        // Serial.println(TPval, HEX);
        // Serial.println(mitcoTouchActiveObject->eventCount);
        bool busy = false;
        for(int i=0; i< mitcoTouchActiveObject->kbCallback.size(); i++){
            if(TPval != 0 && mitcoTouchActiveObject->kbCallback[i].key & TPval && mitcoTouchActiveObject->eventCount < 4) {
                if(!mitcoTouchActiveObject->kbCallback[i].isTouch){
                    if(mitcoTouchActiveObject->kbCallback[i].action) mitcoTouchActiveObject->runCallback(i);
                    mitcoTouchActiveObject->kbCallback[i].isTouch = true;
                }
            }else{
                mitcoTouchActiveObject->kbCallback[i].isTouch = false;
            }
            if(mitcoTouchActiveObject->kbCallback[i].fiber != NULL) busy = true;
            yield();
        }
        if(!busy || mitcoTouchActiveObject->eventCount<0) mitcoTouchActiveObject->eventCount = 0;
        delay(50);
    }
}


DFRobot_MicroTouch microTouch;


