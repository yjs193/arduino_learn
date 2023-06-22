#include <DFRobot_24LC256.h>

DFRobot_24LC256 eeprom;

void setup()
{
    Serial.begin(9600);
    eeprom.begin(true ,true, true);//A0 = ON, A1 = ON, A2 = ON
    eeprom.write(0/*Starting address*/, "hello"/*5 bytes*/);
    delay(1000);
}

void loop()
{
    Serial.println(eeprom.read(0, 5/*Length of bytes to read*/));
    delay(1000);
}
