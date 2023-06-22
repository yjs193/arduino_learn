#include "DFRobot_eeprom.h"


DFRobot_Eeprom::DFRobot_Eeprom(){}

DFRobot_Eeprom::~DFRobot_Eeprom(){}

#if defined(NRF5) || defined(NRF52833)
void DFRobot_Eeprom::clear()
{
  __disable_irq();
  // Enable erase.
  NRF_NVMC->CONFIG = NVMC_CONFIG_WEN_Een;
  while (NRF_NVMC->READY == NVMC_READY_READY_Busy){}

  // Erase the page
  NRF_NVMC->ERASEPAGE = EEPROMSTART;
  while (NRF_NVMC->READY == NVMC_READY_READY_Busy){}
  
  NRF_NVMC->CONFIG = NVMC_CONFIG_WEN_Ren;
  while (NRF_NVMC->READY == NVMC_READY_READY_Busy){}
}

uint8_t DFRobot_Eeprom::read(uint32_t addr)
{
  uint32_t flashAddr = 0;
  int flag = addr%4;
  uint8_t back = 0;
  if(flag == 0){
    flashAddr = EEPROMSTART + addr;
  }else if(flag == 1){
    flashAddr = EEPROMSTART + addr-1;
  }else if(flag == 2){
    flashAddr = EEPROMSTART + addr-2;
  }else{
    flashAddr = EEPROMSTART + addr-3;
  }
  
  uint32_t data = *(__IO uint32_t*)(flashAddr);

  if(flag == 0){
    back = (data>>24) & 0xff;
  }else if(flag == 1){
    back = (data>>16) & 0xff;
  }else if(flag == 2){
    back = (data>>8) & 0xff;
  }else{
    back = (data) & 0xff;
  }
  
  return back;
}

void DFRobot_Eeprom::write(uint32_t addr, uint8_t data)
{
  uint8_t all[1024];
  uint32_t allOne;
  uint32_t startAddr = EEPROMSTART;
  for(int i = 0; i < 1024; )
  {
      allOne = *(__IO uint32_t*)(startAddr);
      all[i] = (allOne>>24) & 0xff;
      all[i+1] = (allOne>>16) & 0xff;
      all[i+2] = (allOne>>8) & 0xff;
      all[i+3] = (allOne>>0) & 0xff;
      i += 4;
      startAddr += 4;
  }
  all[addr] = data;
  clear();
  startAddr = EEPROMSTART;
  
  NRF_NVMC->CONFIG = (NVMC_CONFIG_WEN_Wen << NVMC_CONFIG_WEN_Pos);
  while (NRF_NVMC->READY == NVMC_READY_READY_Busy) {}
  for(int i=0;i<1024;){
    *(__IO uint32_t*)startAddr = (uint32_t)all[i]<<24 | ((uint32_t)(all[i+1]))<<16 | 
                                   ((uint32_t)(all[i+2]))<<8 | ((uint32_t)(all[i+3]));
    while (NRF_NVMC->READY == NVMC_READY_READY_Busy){}
    i+=4;
    startAddr+=4;
  }
  NRF_NVMC->CONFIG = (NVMC_CONFIG_WEN_Ren << NVMC_CONFIG_WEN_Pos);
}

uint32_t DFRobot_Eeprom::eeprom_crc(uint32_t addrA, uint32_t addrB)
{
  if((addrA > EEPROMSIZE) || (addrB > EEPROMSIZE) || (addrA > addrB)){
    return 0;  
  }
  const unsigned long crc_table[16] = {
    0x00000000, 0x1db71064, 0x3b6e20c8, 0x26d930ac,
    0x76dc4190, 0x6b6b51f4, 0x4db26158, 0x5005713c,
    0xedb88320, 0xf00f9344, 0xd6d6a3e8, 0xcb61b38c,
    0x9b64c2b0, 0x86d3d2d4, 0xa00ae278, 0xbdbdf21c
  };
  
  uint8_t data[addrB-addrA+1];
  memset(data,0,addrB-addrA+1);
  for(int index = 0; index <= (addrB-addrA); index++){
    data[index] = read(index+addrA);
  }

  unsigned long crc = ~0L;
  for(int index = 0; index <= (addrB-addrA);index++){  
    crc = crc_table[(crc ^ data[index]) & 0x0f] ^ (crc >> 4);
    crc = crc_table[(crc ^ (data[index] >> 4)) & 0x0f] ^ (crc >> 4);
    crc = ~crc;
  }
  
  return crc;
}

uint32_t DFRobot_Eeprom::length()
{
  return EEPROMSIZE;
}

#elif ESP_PLATFORM
bool DFRobot_Eeprom::begin()
{
  if(EEPROM.begin(size)){
    len = size;
    return true;  
  }else{
    len = 0;
    return false;  
  }
}

void DFRobot_Eeprom::clear()
{
  for (int i = 0 ; i < len ; i++) {
    EEPROM.write(i, 0);
  }
  EEPROM.commit();
}

uint8_t DFRobot_Eeprom::read(uint32_t addr)
{
  return EEPROM.read(addr);
}

void DFRobot_Eeprom::write(uint32_t addr, uint8_t data)
{
  EEPROM.write(addr,data);
  EEPROM.commit();
}

void DFRobot_Eeprom::commit()
{
  EEPROM.commit();  
}

uint32_t DFRobot_Eeprom::eeprom_crc(uint32_t addrA, uint32_t addrB)
{
  if((addrA > length()) || (addrB > length()) || (addrA > addrB)){
    return 0;  
  }
  const unsigned long crc_table[16] = {
    0x00000000, 0x1db71064, 0x3b6e20c8, 0x26d930ac,
    0x76dc4190, 0x6b6b51f4, 0x4db26158, 0x5005713c,
    0xedb88320, 0xf00f9344, 0xd6d6a3e8, 0xcb61b38c,
    0x9b64c2b0, 0x86d3d2d4, 0xa00ae278, 0xbdbdf21c
  };
  
  uint8_t data[addrB-addrA+1];
  memset(data,0,addrB-addrA+1);
  for(int index = 0; index <= (addrB-addrA); index++){
    data[index] = EEPROM.read(index+addrA);
  }

  unsigned long crc = ~0L;
  for(int index = 0; index <= (addrB-addrA); index++){  
    crc = crc_table[(crc ^ data[index]) & 0x0f] ^ (crc >> 4);
    crc = crc_table[(crc ^ (data[index] >> 4)) & 0x0f] ^ (crc >> 4);
    crc = ~crc;
  }
  
  return crc;
}

uint32_t DFRobot_Eeprom::length()
{
  return len;  
}

#elif defined(ARDUINO_ARCH_RP2040)
void DFRobot_Eeprom::begin()
{
  EEPROM.begin(size);
  len = size;
}

void DFRobot_Eeprom::clear()
{
  for (int i = 0 ; i < len ; i++) {
    EEPROM.write(i, 0);
  }
  EEPROM.commit();
}

uint8_t DFRobot_Eeprom::read(uint32_t addr)
{
  return EEPROM.read(addr);
}

void DFRobot_Eeprom::write(uint32_t addr, uint8_t data)
{
  EEPROM.write(addr,data);
  EEPROM.commit();
}

void DFRobot_Eeprom::commit()
{
  EEPROM.commit();  
}

uint32_t DFRobot_Eeprom::eeprom_crc(uint32_t addrA, uint32_t addrB)
{
  if((addrA > length()) || (addrB > length()) || (addrA > addrB)){
    return 0;  
  }
  const unsigned long crc_table[16] = {
    0x00000000, 0x1db71064, 0x3b6e20c8, 0x26d930ac,
    0x76dc4190, 0x6b6b51f4, 0x4db26158, 0x5005713c,
    0xedb88320, 0xf00f9344, 0xd6d6a3e8, 0xcb61b38c,
    0x9b64c2b0, 0x86d3d2d4, 0xa00ae278, 0xbdbdf21c
  };
  
  uint8_t data[addrB-addrA+1];
  memset(data,0,addrB-addrA+1);
  for(int index = 0; index <= (addrB-addrA); index++){
    data[index] = EEPROM.read(index+addrA);
  }

  unsigned long crc = ~0L;
  for(int index = 0; index <= (addrB-addrA); index++){  
    crc = crc_table[(crc ^ data[index]) & 0x0f] ^ (crc >> 4);
    crc = crc_table[(crc ^ (data[index] >> 4)) & 0x0f] ^ (crc >> 4);
    crc = ~crc;
  }
  
  return crc;
}

uint32_t DFRobot_Eeprom::length()
{
  return len;  
}
#else
void DFRobot_Eeprom::clear()
{
  for (int i = 0 ; i < EEPROM.length() ; i++) {
    EEPROM.write(i, 0);
  }
}

uint8_t DFRobot_Eeprom::read(uint32_t addr)
{
  return EEPROM.read(addr);
}

void DFRobot_Eeprom::write(uint32_t addr, uint8_t data)
{
  EEPROM.write(addr,data);
}

uint32_t DFRobot_Eeprom::eeprom_crc(uint32_t addrA, uint32_t addrB)
{
  if((addrA > EEPROM.length()) || (addrB > EEPROM.length()) || (addrA > addrB)){
    return 0;  
  }
  const unsigned long crc_table[16] = {
    0x00000000, 0x1db71064, 0x3b6e20c8, 0x26d930ac,
    0x76dc4190, 0x6b6b51f4, 0x4db26158, 0x5005713c,
    0xedb88320, 0xf00f9344, 0xd6d6a3e8, 0xcb61b38c,
    0x9b64c2b0, 0x86d3d2d4, 0xa00ae278, 0xbdbdf21c
  };

  unsigned long crc = ~0L;

  for(int index = addrA; index <= addrB; index++){
    crc = crc_table[(crc ^ EEPROM[index]) & 0x0f] ^ (crc >> 4);
    crc = crc_table[(crc ^ (EEPROM[index] >> 4)) & 0x0f] ^ (crc >> 4);
    crc = ~crc;
  }
  return crc;
}

uint32_t DFRobot_Eeprom::length()
{
  return EEPROM.length();
}

#endif
