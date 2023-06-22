#include "DFRobot_LedControl.h"

DFRobot_LedControl led(0,2,1);

byte LedArray[]={ B11011010,
                  B11011010,
                  B11011010,
                  B11011010,
                  B11011010,
                  B11011010,
                  B11011010,
                  B11011010, };

void setup()
{
  led.begin();
}

void loop()
{
  led.show(LedArray);
}