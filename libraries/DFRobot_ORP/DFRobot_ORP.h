#ifndef DFROBOT_ORP_H
#define DFROBOT_ORP_H

#if ARDUINO < 100
  #include <WProgram.h>
#else
  #include <Arduino.h>
#endif

#define VOLTAGE 5.00    //system voltage
#define OFFSET 22       //zero drift voltage
#define ArrayLenth  5    //times of collection

class DFRobot_ORP
{
  public:

  DFRobot_ORP();
  int readORP(int pin);
};

#endif 
