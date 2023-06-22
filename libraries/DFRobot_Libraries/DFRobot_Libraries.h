#ifndef DFROBOT_LIBRARIES_H
#define DFROBOT_LIBRARIES_H
#include <Arduino.h>
#include <DFRobot_Servo.h>

class DFRobot_Romeo {
    public:
    enum MotorDir {
        FORWARD = 0,
        BACKWARDS,
        TURNLEFT,
        TURNRIGHT
    };
    enum MotorIndex {
        M1 = 1,
        M2 = 2
    };
    
    DFRobot_Romeo(){};
    ~DFRobot_Romeo(){};
    // motor
    void motorBegin();
    void motorRun(MotorDir dir, uint32_t speed);
    void motorRun(MotorIndex index, MotorDir dir, uint32_t speed);
    void motorStop();
    // 八段数码管
    void digitalTubeDisplay(uint32_t val);
    private :
    void setMotor(int motorId, int speed);
};

class DFRobot_UNOR3 {
    public:
    enum MotorDir {
        FORWARD = 0,
        BACKWARDS,
        TURNLEFT,
        TURNRIGHT
    };
    enum MotorIndex {
        M1 = 1,
        M2 = 2
    };
    
    DFRobot_UNOR3(){};
    ~DFRobot_UNOR3(){};
    // motor
    void motorBegin();
    void motorRun(MotorDir dir, uint32_t speed);
    void motorRun(MotorIndex index, MotorDir dir, uint32_t speed);
    void motorStop();
    private :
    void setMotor(int motorId, int speed);
};

class DFRobot_MaxBot {
    public:
    enum MotorDir {
        FORWARD = 0,
        BACKWARDS,
    };
    enum MotorIndex {
        LEFT = 1,
        RIGHT = 2
    };
    
    DFRobot_MaxBot(){};
    ~DFRobot_MaxBot(){};
    // motor
    void motorRun(MotorIndex index, MotorDir dir, uint32_t speed);
    private :
    Servo ppmMotor1;
    Servo ppmMotor2;
};

class DFRobot_Vortex {
    public:
    enum MotorDir {
        FORWARD = 0,
        BACKWARDS,
        TURNLEFT,
        TURNRIGHT
    };
    enum MotorIndex {
        LEFT = 1,
        RIGHT = 2
    };
    
    DFRobot_Vortex(){};
    ~DFRobot_Vortex(){};
    // motor
    void motorRun(MotorDir dir, uint32_t speed);
    void motorRun(MotorIndex index, MotorDir dir, uint32_t speed);
    void motorStop();
    private :
};


class DFRobot_Shake {
    public:
    DFRobot_Shake(){};
    ~DFRobot_Shake(){};
    void onShakeEvent(uint32_t pin, void (*func)(void));
    private :
};


class DFRobot_LM35 {
    public:
    DFRobot_LM35(){};
    ~DFRobot_LM35(){};
    float getTemperature(uint32_t pin);
    private :
};


class DFRobot_Servo360
{
  public:
    DFRobot_Servo360();
    ~DFRobot_Servo360();
    void speed(int32_t speed);
    void attach(uint32_t pin);
    void detach();

  private:
    Servo* servo;
};

class DFRobot_AnalogVoltage
{
  public:
    DFRobot_AnalogVoltage();
    ~DFRobot_AnalogVoltage();
    float getVoltage(uint32_t pin);

  private:
};

class DFRobot_Sharp
{
  public:
    DFRobot_Sharp();
    ~DFRobot_Sharp();
    float distanceMm(uint32_t pin);

  private:
};

#if defined(ARDUINO_ARCH_AVR)
class DFRobot_Tone
{
  public:
    DFRobot_Tone();
    ~DFRobot_Tone();
    void play(uint32_t pin, unsigned int frequency, unsigned long duration);
    void stop(uint32_t pin);
  private:
};
#endif


extern DFRobot_Romeo romeo;
extern DFRobot_UNOR3 unor3;
extern DFRobot_MaxBot maxbot;
extern DFRobot_Vortex vortex;

#endif    //DFROBOT_LIBRARIES_H

