#ifndef DF_MICROBIT_PWM_H
#define DF_MICROBIT_PWM_H
#include "Arduino.h"

class Microbit_Pwm
{
  public:

  Microbit_Pwm();

  ~Microbit_Pwm();

  void pwmInit(uint8_t pin);
  void pwmFree(uint8_t pin);

  void pwmWrite(uint8_t pin, uint16_t value);
  uint16_t pwmRead(uint8_t pin);

  void pwmFreq(uint8_t pin, uint32_t hz);
  void pwmFreqKhz(uint8_t pin, uint16_t khz){pwmFreq(pin, khz*1000);}
  void pwmFreqMhz(uint8_t pin, uint16_t mhz){pwmFreqKhz(pin, mhz*1000);}

  void pwmPeriodUs(uint8_t pin, uint32_t us);
  void pwmPeriodMs(uint8_t pin, uint32_t ms){pwmPeriodUs(pin, 1000*ms);}
  void pwmPeriod(uint8_t pin, uint32_t s){pwmPeriodMs(pin, 1000*s);}
  
  void pwmoutPulsewidthUs(uint8_t pin, uint32_t us);
  void pwmoutPulsewidthMs(uint8_t pin, uint32_t ms){pwmoutPulsewidthUs(pin, 1000*ms);}
  void pwmoutPulsewidth(uint8_t pin, uint32_t s){pwmoutPulsewidthMs(pin, 1000*s);}

  private:

  void timerInit();
  int8_t pwmGetChannel(uint8_t pinNum);
  void pwmDisconnect(uint8_t pinNum);
  int8_t pwmAllocateChannel(uint8_t pinNum);
  void pinInit(uint8_t pinNum);
};



#endif

