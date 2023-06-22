/**************************************************************************/
/*
    @file     SHT30_TempRH_Read.ino
    @author   Henry (DFRobot)
    @version  V0.1
    @date     2019-07-05
    @license  GNU Lesser General Public License (see license.txt)
    @breif    This example read and print the temperature and relative humidity.

    This demo and related libraries are for DFRobot Gravity: Analog SHT30 Temperature & Humidity Sensor
    Check out the links below for tutorials and connection diagrams
    Product(CH): http://www.dfrobot.com.cn/
    Product(EN): https://www.dfrobot.com/

    Copyright   [DFRobot](http://www.dfrobot.com), 2019
    Copyright   GNU Lesser General Public License
*/
/**************************************************************************/

// VREF: Analog reference
// For Arduino UNO, Leonardo and mega2560, etc. change VREF to 5
// For Arduino Zero, Due, MKR Family, ESP32, etc. 3V3 controllers, change VREF to 3.3
#include "DFRobot_SHT30.h"
DFRobot_SHT30 dfr;
void setup() {
#ifdef ARDUINO_ARCH_AVR
  dfr.begin(A0,A1);
#elif ESP_PLATFORM
  dfr.begin(P0,P1);
#elif NRF5
  dfr.begin(0,1);
#endif
  Serial.begin(115200);
  Serial.println("SHT30 Starts up.");
}

void loop() {
  Serial.print("HUMIDITY=" );
//  dfr.get_TempCentigrade(dfr.CENTIGRADE);
  Serial.println(dfr.get_HUMIDITY());
  Serial.print("Temp=" );
  Serial.println(dfr.get_TempCentigrade(dfr.CENTIGRADE));
  delay(2000);
}
