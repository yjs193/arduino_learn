#ifndef DFROBOT_DO_H
#define DFROBOT_DO_H

#if ARDUINO < 100
  #include <WProgram.h>
#else
  #include <Arduino.h>
#endif

#define VREF 5000
#define OFFSET 0        //zero drift voltage
#define ArrayLenth  5    //times of collection




class DFRobot_DO
{
  public:

  DFRobot_DO();
  float readDO(int pin,float temperature);
};

#endif 
