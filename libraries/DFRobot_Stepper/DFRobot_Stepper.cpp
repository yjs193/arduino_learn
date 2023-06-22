#include "DFRobot_Stepper.h"


DFRobot_Stepper::DFRobot_Stepper(){}

void DFRobot_Stepper::stepperConfig(int _pin_dir, int _pin_en, int _pin_drive, int _number_of_steps, int _speed)
{
    this->pin_dir = _pin_dir;
    this->pin_en = _pin_en;
    this->pin_drive = _pin_drive;
    this->number_of_steps = this->getResolution(_number_of_steps);
    this->speed = _speed;
    this->speed = max(this->speed,0);
    this->speed = min(this->speed,450);
    this->last_step_time = 0;
    this->mode = DFSTEPPER_MODE_IS_SINGLE;
    
    this->pinInit();
    this->setSpeed(this->speed);

}

void DFRobot_Stepper::stepperConfig(int _X_pin_dir, int _X_pin_en, int _X_pin_drive,
                                 int _X_number_of_steps, int _X_speed,
                                 int _Y_pin_dir, int _Y_pin_en, int _Y_pin_drive,
                                 int _Y_number_of_steps, int _Y_speed)
{
    this->X_pin_dir = _X_pin_dir;
    this->X_pin_en = _X_pin_en;
    this->X_pin_drive = _X_pin_drive;
    this->X_number_of_steps = this->getResolution(_X_number_of_steps);
    this->X_speed = _X_speed;
    this->X_speed = max(this->X_speed,0);
    this->X_speed = min(this->X_speed,450);
    this->X_last_step_time = 0;
    
    this->Y_pin_dir = _Y_pin_dir;
    this->Y_pin_en = _Y_pin_en;
    this->Y_pin_drive = _Y_pin_drive;
    this->Y_number_of_steps = this->getResolution(_Y_number_of_steps);
    this->Y_speed = _Y_speed;
    this->Y_speed = max(this->Y_speed,0);
    this->Y_speed = min(this->Y_speed,450);
    this->Y_last_step_time = 0;
    
    this->mode = DFSTEPPER_MODE_IS_DOUBLE;
    this->pinInit();
    this->setSpeed(this->X_speed, this->Y_speed);
}

DFRobot_Stepper::~DFRobot_Stepper()
{}

int DFRobot_Stepper::getResolution(int nStep){
    if(nStep > 0 && nStep <= 200) return 200;
    if(nStep > 200 && nStep <= 400) return 400;
    if(nStep > 400 && nStep <= 800) return 800;
    if(nStep > 800 && nStep <= 1600) return 1600;
    if(nStep > 1600 && nStep <= 3200) return 3200;
    if(nStep > 3200 && nStep <= 6400) return 6400;
    return 200;
}

void DFRobot_Stepper::setSpeed(long _speed)
{
    if(_speed > 0) this->step_delay = 60L * 1000L * 1000L / this->number_of_steps / _speed;
}

void DFRobot_Stepper::setSpeed(long _X_speed, long _Y_speed)
{
    if(_X_speed > 0) this->X_step_delay = 60L * 1000L * 1000L / this->X_number_of_steps / _X_speed;
    if(_Y_speed > 0) this->Y_step_delay = 60L * 1000L * 1000L / this->Y_number_of_steps / _Y_speed;
}

void DFRobot_Stepper::pinInit()
{
    if(this->mode == DFSTEPPER_MODE_IS_SINGLE)
    {
        pinMode(this->pin_dir, OUTPUT);
        pinMode(this->pin_en, OUTPUT);
        pinMode(this->pin_drive, OUTPUT);
        digitalWrite(this->pin_en,LOW);
    }else{
        pinMode(this->X_pin_dir, OUTPUT);
        pinMode(this->X_pin_en, OUTPUT);
        pinMode(this->X_pin_drive, OUTPUT);
        pinMode(this->Y_pin_dir, OUTPUT);
        pinMode(this->Y_pin_en, OUTPUT);
        pinMode(this->Y_pin_drive, OUTPUT);
        digitalWrite(this->X_pin_en,LOW);
        digitalWrite(this->Y_pin_en,LOW);
    }
}

void DFRobot_Stepper::step(int _steps_to_move, Dir dir)
{
    if(this->speed <= 0) return;
    digitalWrite(this->pin_dir,dir);
    while(_steps_to_move){
        unsigned long now = micros();
        if (now - this->last_step_time >= this->step_delay) {
            this->last_step_time = now;
            digitalWrite(this->pin_drive,LOW);
            delayMicroseconds(2);
            digitalWrite(this->pin_drive,HIGH);
            _steps_to_move--;
        }
    }
    if(this->speed > 300) delay(20);
}

void DFRobot_Stepper::step(int _X_steps_to_move, Dir _X_dir, int _Y_steps_to_move, Dir _Y_dir)
{
    if(X_speed <= 0 && X_speed <= 0) return;
    else if(X_speed <= 0 && Y_speed > 0){
        digitalWrite(this->Y_pin_dir, _Y_dir);
        while(_Y_steps_to_move){
            unsigned long now = micros();
            if (now - this->Y_last_step_time >= this->Y_step_delay) {
                this->Y_last_step_time = now;
                digitalWrite(this->Y_pin_drive,LOW);
                delayMicroseconds(2);
                digitalWrite(this->Y_pin_drive,HIGH);
                _Y_steps_to_move--;
            }
        }
    }else if(Y_speed <= 0 && X_speed > 0){
        digitalWrite(this->X_pin_dir, _X_dir);
        while(_X_steps_to_move){
            unsigned long now = micros();
            if (now - this->X_last_step_time >= this->X_step_delay) {
                this->X_last_step_time = now;
                digitalWrite(this->X_pin_drive,LOW);
                delayMicroseconds(2);
                digitalWrite(this->X_pin_drive,HIGH);
                _X_steps_to_move--;
            }
        }
    }else{
        digitalWrite(this->X_pin_dir,_X_dir);
        digitalWrite(this->Y_pin_dir,_Y_dir);
        int X_steps_to_move = _X_steps_to_move;
        int Y_steps_to_move = _Y_steps_to_move;
        int max_steps_to_move = max(_X_steps_to_move, _Y_steps_to_move);
        int max_steps = max_steps_to_move;
        while(X_steps_to_move || Y_steps_to_move)
        {
            unsigned long X_now = micros();
            unsigned long Y_now = micros();
            if (X_now - this->X_last_step_time >= this->X_step_delay && X_steps_to_move > 0) {
                this->X_last_step_time = X_now;
                digitalWrite(this->X_pin_drive,LOW);
                delayMicroseconds(2);
                digitalWrite(this->X_pin_drive,HIGH);
                X_steps_to_move--;
            }
            if (Y_now - this->Y_last_step_time >= this->Y_step_delay && Y_steps_to_move > 0) {
                this->Y_last_step_time = Y_now;
                digitalWrite(this->Y_pin_drive,LOW);
                delayMicroseconds(2);
                digitalWrite(this->Y_pin_drive,HIGH);
                Y_steps_to_move--;
            }
        }
    }
    if(this->X_speed > 300 || Y_speed > 300) delay(20);
}


