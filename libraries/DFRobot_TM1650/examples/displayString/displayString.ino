#include <DFRobot_TM1650.h>

DFRobot_TM1650 tm;

void setup()
{
  tm.init();
}

void loop()
{
  tm.displayString("1256");
}