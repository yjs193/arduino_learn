/*!
 * MindPlus
 * microbit
 *
 */
#include <DFRobot_eeprom.h>

// 动态变量
String mind_n_a;
// 创建对象
DFRobot_Eeprom eeprom;


// 主程序开始
void setup() {
  Serial.begin(9600);
  //eeprom.begin(); //esp32
  eeprom.clear();
  eeprom.write(1023,56);
  mind_n_a = "";
}
void loop() {
  Serial.println(eeprom.read(1023));
  Serial.println(eeprom.eeprom_crc(1023,1023));
  mind_n_a = eeprom.eeprom_crc(1023,1023);
  Serial.println(mind_n_a);
  delay(1000);
}
