#include "DFRobot_BME680_I2C.h"
#include "Wire.h"
DFRobot_BME680_I2C bme(0x77);  //0x77 I2C address
void setup()
{
  Serial.begin(9600);
  Serial.println();
  bme.begin();
  Serial.println("bme begin successful");
}
void loop()
{
  Serial.println();
  Serial.print("temperature(C) :");Serial.println(bme.readTemperature() / 100, 2);
  Serial.print("pressure(Pa) :");Serial.println(bme.readPressure());
  Serial.print("humidity(%rh) :");Serial.println(bme.readHumidity() / 1000, 2);
  Serial.print("gas resistance(ohm) :");Serial.println(bme.readGasResistance());
  Serial.print("altitude(m) :");Serial.println(bme.readAltitude());
  delay(1000);
}
