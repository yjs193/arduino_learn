/*!
 * @file bmp280test.ino
 * @brief DFRobot's Temperature、Pressure and Approx altitude
 * @n [Get the module here](等上架后添加商品购买链接)
 * @n This example read the Temperature、Pressure and Altitude from BMP280, and then print them
 * @n [Connection and Diagram](等上架后添加wiki链接)
 *
 * @copyright	[DFRobot](http://www.dfrobot.com), 2016
 * @copyright	GNU Lesser General Public License
 *
 * @author [yuxiang](1137717512@qq.com)
 * @version  V1.0
 * @date  2016-12-06
 */

#include <Wire.h>
#include "DFRobot_BMP280.h"

DFRobot_BMP280 bmp280;

void setup() {
  Serial.begin(115200);
  if (!bmp280.begin()) {  
    Serial.println("Error initializing BMP280!");
  }
}

void loop() {
  Serial.print("Temperature: ");
  Serial.print(bmp280.getTemperature());
  Serial.print(" Pressure: ");
  Serial.print(bmp280.getPressure());
  Serial.print(" Altitude:");
  Serial.println(bmp280.getAltitude());
  /* use a sampling time of at least 38ms */
  delay(50);
}