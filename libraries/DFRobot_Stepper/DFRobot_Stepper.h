#ifndef _DF_STEPPER_H
#define _DF_STEPPER_H
#include <Arduino.h>

#define DFSTEPPER_MODE_IS_SINGLE 0
#define DFSTEPPER_MODE_IS_DOUBLE 1


class DFRobot_Stepper {

    public:

    enum Dir{
        CW = HIGH,
        CCW = LOW
    };

    void pinInit();
    /** 
     * @brief   构造函数
     * @param   serial:设备ID
     * @param   ssid:wifi账号
     * @param:  pwd:wifi密码
     * @param:  ip:服务器ip地址
     * @param:  port:服务器端口号
     * @return: 无
     */
    DFRobot_Stepper();

    void stepperConfig(int _pin_dir, int _pin_en, int _pin_drive, int _number_of_steps, int _speed);

    void stepperConfig(int _X_pin_dir, int _X_pin_en, int _X_pin_drive,
                       int _X_number_of_steps, int _X_speed,
                       int _Y_pin_dir, int _Y_pin_en, int _Y_pin_drive,
                       int _Y_number_of_steps, int _Y_speed);
                    
    ~DFRobot_Stepper();

    void step(int _steps_to_move, Dir dir);

    void step(int _X_steps_to_move, Dir _X_dir, int _Y_steps_to_move, Dir _Y_dir);

    void setSpeed(long _speed);

    void setSpeed(long _X_speed, long _Y_speed);

    private:

    int mode;
    unsigned long last_step_time;
    int pin_dir, pin_en, pin_drive, number_of_steps, speed;
    unsigned long X_last_step_time, Y_last_step_time;
    int X_pin_dir, X_pin_en, X_pin_drive, X_number_of_steps, X_speed;
    int Y_pin_dir, Y_pin_en, Y_pin_drive, Y_number_of_steps, Y_speed;

    unsigned long step_delay, X_step_delay, Y_step_delay;
    int getResolution(int nStep);

    //void pinInit();
};

#endif

