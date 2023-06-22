#include <DFRobot_DS18B20.h>

DFRobot_DS18B20 ds18b20_3(3);//引脚3

void setup()
{
  ds18b20_3.begin();
}

void loop()
{
  ds18b20_3.getTempC();//获取温度（摄氏度）
}
