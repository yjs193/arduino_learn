#ifndef DFROBOT_EEPROM_h
#define DFROBOT_EEPROM_h
#if defined(NRF5) || defined(NRF52833)
#include <Arduino.h>
#include <nrf.h>
#define EEPROMSTART 0x3FC00
#define EEPROMSIZE 1024
#elif ESP_PLATFORM
#include <Arduino.h>
#include <EEPROM.h>
#elif defined(ARDUINO_ARCH_RP2040)
#define EEPROMSIZE 512
#include <Arduino.h>
#include <EEPROM.h>
#else
#include <Arduino.h>
#include <EEPROM.h>
#endif
class DFRobot_Eeprom
{
  public:
    DFRobot_Eeprom();
    ~DFRobot_Eeprom();
#if defined(NRF5) || defined(NRF52833)
    void clear();
    uint8_t read(uint32_t addr);
    void write(uint32_t addr, uint8_t data);
    uint32_t eeprom_crc(uint32_t addrA, uint32_t addrB); 
    uint32_t length();
#elif ESP_PLATFORM
    bool begin();
    void clear();
    uint8_t read(uint32_t addr);
    void write(uint32_t addr, uint8_t data);
    uint32_t eeprom_crc(uint32_t addrA, uint32_t addrB);
    uint32_t length();
    void commit();
#elif defined(ARDUINO_ARCH_RP2040)
    void begin();
    void clear();
    uint8_t read(uint32_t addr);
    uint32_t eeprom_crc(uint32_t addrA, uint32_t addrB);
    void write(uint32_t addr, uint8_t data);
    uint32_t length();
    void commit();
#else
    void clear();
    uint8_t read(uint32_t addr);
    void write(uint32_t addr, uint8_t data);
    uint32_t eeprom_crc(uint32_t addrA, uint32_t addrB);
    uint32_t length();
#endif
  private:
#if defined(NRF5) || defined(NRF52833)

#elif ESP_PLATFORM
    uint32_t size = 1024;
    uint32_t len = 0;
#elif defined(ARDUINO_ARCH_RP2040)
    uint32_t size = 512;
    uint32_t len = 0;
#else

#endif
};
#endif
