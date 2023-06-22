#include <Arduino.h>
#include <Wire.h>

typedef uint8_t   BYTE;

class DFRobot_BH1750
{
public:
  DFRobot_BH1750();
  uint16_t BH1750_Read(void);
  void begin(void);
private:
  void BH1750_Init(int address);
  uint8_t rRead(int address, BYTE *p);
};
