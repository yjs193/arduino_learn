#include "DFRobot_Libraries.h"

// 套件Romeo
void DFRobot_Romeo::motorBegin(){
    digitalWrite(6, LOW);
    digitalWrite(7, LOW);
    digitalWrite(4, LOW);
    digitalWrite(5, LOW);
}

void DFRobot_Romeo::motorRun(MotorDir dir, uint32_t speed){
    switch(dir){
        case FORWARD :
            setMotor(1, speed);
            setMotor(2, speed);
            break;
        case BACKWARDS :
            setMotor(1, -speed);
            setMotor(2, -speed);
        break;
        case TURNLEFT :
            setMotor(1, -speed);
            setMotor(2, speed);
        break;
        case TURNRIGHT :
            setMotor(1, speed);
            setMotor(2, -speed);
        break;
        default :
            motorStop();
        break;
    }
}

void DFRobot_Romeo::motorRun(MotorIndex index, MotorDir dir, uint32_t speed){
    if(dir == FORWARD) {
        setMotor(index, speed);
    }else if(dir == BACKWARDS) {
        setMotor(index, -speed);
    }else{
        motorStop();
    }
}

void DFRobot_Romeo::motorStop(){
    setMotor(1, 0);
    setMotor(2, 0);
}

void DFRobot_Romeo::setMotor(int motorId, int speed)
{
    int speedPin, directionPin;
    if (motorId == 2){
        speedPin = 6;
        directionPin = 7;
    }else if (motorId == 1){
        speedPin = 5;
        directionPin = 4;
    }else{
        return;
    }
    if (speed == 0){
        digitalWrite(speedPin, LOW);
    }else{
        if (speed > 0){
            digitalWrite(directionPin, HIGH);
        }else{
            digitalWrite(directionPin, LOW);
        }
        analogWrite(speedPin, speed>0?speed:-speed);
    }
}

void DFRobot_Romeo::digitalTubeDisplay(uint32_t val){
    int segment[]={0xFC,0x60,0xDA,0xF2,0x66,0xB6,0xBE,0xE0,0xFE,0xF6,0xEE,0x3E,0x9C,0x7A,0x9E,0x8E,0x00};
    for(int i=6;i<14;i++){
        digitalWrite(i,LOW);
    }
    if(val > 0x10 || val < 0){
        return;
    }
    for(int i=0; i<8; i++){
        digitalWrite(13-i, (segment[val]>>(7-i))&1);
    }
}



// 套件R3
void DFRobot_UNOR3::motorBegin(){
    digitalWrite(8, LOW);
    digitalWrite(7, LOW);
    digitalWrite(6, LOW);
    digitalWrite(5, LOW);
}

void DFRobot_UNOR3::motorRun(MotorDir dir, uint32_t speed){
    speed = speed*255/100;
    switch(dir){
        case FORWARD :
            setMotor(1, speed);
            setMotor(2, speed);
            break;
        case BACKWARDS :
            setMotor(1, -speed);
            setMotor(2, -speed);
        break;
        case TURNRIGHT :
            setMotor(1, -speed);
            setMotor(2, speed);
        break;
        case TURNLEFT :
            setMotor(1, speed);
            setMotor(2, -speed);
        break;
        default :
            motorStop();
        break;
    }
}

void DFRobot_UNOR3::motorRun(MotorIndex index, MotorDir dir, uint32_t speed){
    speed = speed*255/100;
    if(dir == FORWARD) {
        setMotor(index, speed);
    }else if(dir == BACKWARDS) {
        setMotor(index, -speed);
    }else{
        motorStop();
    }
}

void DFRobot_UNOR3::motorStop(){
    setMotor(1, 0);
    setMotor(2, 0);
}

void DFRobot_UNOR3::setMotor(int motorId, int speed){
    int speedPin, directionPin;
    if (motorId == 2){
        speedPin = 6;
        directionPin = 8;
    }else if (motorId == 1){
        speedPin = 5;
        directionPin = 7;
    }else{
        return;
    }
    if (speed == 0){
        digitalWrite(speedPin, LOW);
    }else{
        if (speed > 0){
            digitalWrite(directionPin, LOW);
        }else{
            digitalWrite(directionPin, HIGH);
        }
        analogWrite(speedPin, speed>0?speed:-speed);
    }
}



// 套件motorbit
void DFRobot_MaxBot::motorRun(MotorIndex index, MotorDir dir, uint32_t speed){
    static bool first = false;
    if(!first){
          ppmMotor1.attach(12);
          ppmMotor1.angle(90);
          ppmMotor2.attach(8);
          ppmMotor2.angle(90);
          first = true;
    }
    int _speed = speed;
    if(dir == BACKWARDS) _speed = -_speed;
    _speed = 90 + (int)((_speed/255.0)*90);
    if(index == LEFT)
          ppmMotor1.angle(_speed);
    else
          ppmMotor2.angle(_speed);
}



// 套件Vortex
void DFRobot_Vortex::motorRun(MotorDir dir, uint32_t speed){
    speed = map(speed, 0, 127, 0, 255);
    switch (dir) {
        case FORWARD:
            analogWrite(5, speed);
            digitalWrite(9, HIGH);
            analogWrite(6, speed);
            digitalWrite(10, HIGH);
            break;
        case BACKWARDS:
            analogWrite(5, speed);
            digitalWrite(9, LOW);
            analogWrite(6, speed);
            digitalWrite(10, LOW);
            break;
        case TURNLEFT:
            analogWrite(5, speed);
            digitalWrite(9, LOW);
            analogWrite(6, speed);
            digitalWrite(10, HIGH);
            break;
        case TURNRIGHT:
            analogWrite(5, speed);
            digitalWrite(9, HIGH);
            analogWrite(6, speed);
            digitalWrite(10, LOW);
            break;
        default :
            motorStop();
            break;
    }
}

void DFRobot_Vortex::motorRun(MotorIndex index, MotorDir dir, uint32_t speed) {
    speed = map(speed, 0, 127, 0, 255);
    if (index == LEFT) {
        switch (dir) {
            case FORWARD:
                analogWrite(5 ,speed);
                digitalWrite(9 ,HIGH);
                break;
            case BACKWARDS:
                analogWrite(5 ,speed);
                digitalWrite(9 ,LOW);
                break;
            default :
                motorStop();
                break;
        }
    } else if (index == RIGHT) {
        switch (dir) {
            case FORWARD:
                analogWrite(6 ,speed);
                digitalWrite(10 ,HIGH);
                break;
            case BACKWARDS:
                analogWrite(6 ,speed);
                digitalWrite(10 ,LOW);
                break;
            default :
                motorStop();
                break;
        }
    } else {
        motorStop();
    }
}

void DFRobot_Vortex::motorStop(){
    analogWrite(5, 0);
    analogWrite(6, 0);
}



// 数字震动传感器
void DFRobot_Shake::onShakeEvent(uint32_t pin, void (*func)(void)){
#if defined(NRF5)
    attachInterrupt(pin,func,FALLING);
#elif defined(NRF52833)//NRF52833走的event 不会走到这里
    ;
#else
    attachInterrupt(digitalPinToInterrupt(pin),func,FALLING);
#endif
}



// LM35温度传感器
float DFRobot_LM35::getTemperature(uint32_t pin){
#if defined(NRF5)
    return (float)(analogRead(pin)) * (3.1 / 10.24);
#elif defined NRF52833
    return (float)(analogRead(pin)) * (3.1 / 10.24);
#elif defined ARDUINO_ARCH_RP2040
    return (float)(analogRead(pin)) * (3.1 / 10.24);
#else
    return (float)(analogRead(pin)) * (5.5 / 10.24);
#endif
}



//360°舵机
DFRobot_Servo360::DFRobot_Servo360(){
    servo = NULL;
}

DFRobot_Servo360::~DFRobot_Servo360(){
    if(servo) delete servo;
    servo = NULL;
}

void DFRobot_Servo360::speed(int32_t speed)
{
    int _speed = 90 - (int)((speed/100.0)*90);
#if defined(NRF5)
    _speed = constrain(_speed-4,0,180);
#elif defined NRF52833
    _speed = constrain(_speed-4,0,180);
#else
    _speed = constrain(_speed,0,180);
#endif
    Serial.println(_speed);
    servo->angle(_speed);
    // Serial.println(_speed);
}

void DFRobot_Servo360::attach(uint32_t pin)
{
    if(servo) return;
    servo = new Servo();
    servo->attach(pin);
}

void DFRobot_Servo360::detach()
{
    servo->detach();
    if(servo) delete servo;
    servo = NULL;
}

DFRobot_AnalogVoltage::DFRobot_AnalogVoltage()
{
    
}

DFRobot_AnalogVoltage::~DFRobot_AnalogVoltage()
{
    
}

float DFRobot_AnalogVoltage::getVoltage(uint32_t pin)
{
#if defined(NRF5)
    return (double)analogRead(pin) / 1024.0 * 3.3 * 5.0;
#elif defined NRF52833
    return (double)analogRead(pin) / 1024.0 * 3.3 * 5.0;
#elif defined ARDUINO_ARCH_RP2040
    return (double)analogRead(pin) / 1024.0 * 3.3 * 5.0;
#else
    return (double)analogRead(pin) / 1024.0 * 5.0 * 5.0;
#endif
}

DFRobot_Sharp::DFRobot_Sharp()
{
    
}

DFRobot_Sharp::~DFRobot_Sharp()
{
    
}

float DFRobot_Sharp::distanceMm(uint32_t pin)
{
    uint16_t value = analogRead(pin);
#if defined(NRF5)
    value = value * 3.3 / 5.0;
#elif defined NRF52833
    value = value * 3.3 / 5.0;
#endif
    value = value < 30? 30:value;
    return ((67870.0 / (value - 3.0)) - 40.0);
}

#if defined(ARDUINO_ARCH_AVR)

DFRobot_Tone::DFRobot_Tone()
{
    
}

DFRobot_Tone::~DFRobot_Tone()
{
    
}

void DFRobot_Tone::play(uint32_t pin, unsigned int frequency, unsigned long duration)
{
  if(frequency == 0){
    digitalWrite(pin,LOW);
    return;
  }
  int period = 1000000.0 / frequency;
  int pulse = period / 2.0;
  pinMode(pin,OUTPUT);
  for (int i = 1; i <= ((duration * 1000.0) / period); i++ ) {
    digitalWrite(pin,HIGH);
    delayMicroseconds(pulse);
    digitalWrite(pin,LOW);
    delayMicroseconds(pulse);
  }
}

void DFRobot_Tone::stop(uint32_t pin)
{
    digitalWrite(pin,LOW);
}

#endif



DFRobot_Romeo romeo;
DFRobot_UNOR3 unor3;
DFRobot_MaxBot maxbot;
DFRobot_Vortex vortex;