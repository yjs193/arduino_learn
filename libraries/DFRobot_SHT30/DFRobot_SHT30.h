#include "Arduino.h"


class DFRobot_SHT30
{
public:
    DFRobot_SHT30();
    ~DFRobot_SHT30();
    void    begin(uint8_t tp=-1,uint8_t hp=-1);
    float   getTemperatureC(),
            getTemperatureF(),
            getHumidity();

private:
    uint8_t tempPin, humyPin;
    float
#ifdef ARDUINO_ARCH_AVR
    map = 1024,
    vref = 5.0;
#elif defined(ESP_PLATFORM)
    map = 4096,
    vref = 3.3;
#elif defined(NRF5) || defined(NRF52833)
    map = 1024,
    vref = 3.3;
#elif defined(ARDUINO_ARCH_RP2040)
    map = 1024,
    vref = 3.3;
#endif
};