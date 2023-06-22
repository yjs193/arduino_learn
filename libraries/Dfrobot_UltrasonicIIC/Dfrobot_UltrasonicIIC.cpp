//Dfrobot_UltrasonicIIC.cpp
#include <Dfrobot_UltrasonicIIC.h>
#include <string.h>

Dfrobot_UltrasonicIIC::Dfrobot_UltrasonicIIC()
{
  memset(txbuf, 0, sizeof(txbuf));
  memset(rxbuf, 0, sizeof(rxbuf));
}
bool Dfrobot_UltrasonicIIC::checkProductID()
{
  i2cReadBytes(SLAVEADDR_INDEX);
  delay(10);
  return rxbuf[0] == addr0;
}
uint8_t Dfrobot_UltrasonicIIC::scan()
{
  for (uint8_t address = 1; address < 127; address++)
  {
    Wire.beginTransmission(address);
    if (Wire.endTransmission() == 0)
    {
      unsigned char old_addr = addr0;
      addr0 = address;
      bool ret = checkProductID();
      addr0 = old_addr;
      if (ret)
        return address;
    }
  }
  return 0xff;
}
uint8_t Dfrobot_UltrasonicIIC::begin(uint8_t addr)
{
  Wire.begin();                                         //debug pico wsq
  addr0 = addr;                                         // join i2c bus (address optional for master)
  txbuf[0] = (MEASURE_MODE_PASSIVE | MEASURE_RANG_500); //模锟斤拷锟斤拷锟斤拷为锟斤拷锟斤拷模式锟斤拷锟斤拷喾段э拷锟斤拷锟轿�500CM
  uint8_t ret = i2cWriteBytes(CFG_INDEX, &txbuf[0]);    //
  delay(100);
  return ret;
}
void Dfrobot_UltrasonicIIC::changeAddr(unsigned char addr)
{
  Wire.begin();
  // 鎵弿璁惧鍦板潃锛岀涓€涓澶�
  uint8_t scan_addr = scan();
  if (scan_addr == 0xff)
    return;
  // 鍒濆鍖栬澶�
  begin(scan_addr);

  // 淇敼鍦板潃
  txbuf[0] = addr;
  i2cReadBytes(SLAVEADDR_INDEX); //锟斤拷取锟接伙拷锟斤拷址锟侥达拷锟斤拷
  addr = rxbuf[0];
  delay(10);

  i2cWriteBytes(SLAVEADDR_INDEX, &txbuf[0]); // 锟斤拷锟铰碉拷址0x12写锟斤拷锟街凤拷拇锟斤拷锟�
  delay(100);
  i2cReadBytes(SLAVEADDR_INDEX); //锟斤拷取锟接伙拷锟斤拷址锟侥达拷锟斤拷
  addr = rxbuf[0];
  delay(10);
}

uint8_t Dfrobot_UltrasonicIIC::i2cWriteBytes(unsigned char Reg, unsigned char *pdata)
{
  _Reg = Reg;
  Wire.beginTransmission(addr0); // transmit to device #8
  Wire.write(_Reg);              // sends one byte
  for (uint8_t i = 0; i < 1; i++)
  {
    Wire.write(*pdata);
    pdata++;
  }
  return Wire.endTransmission(); // stop transmitting
}

void Dfrobot_UltrasonicIIC::i2cReadBytes(unsigned char Reg)
{
  _Reg = Reg;
  // unsigned char i = 0;
  Wire.beginTransmission(addr0); // transmit to device #8
  Wire.write(_Reg);              // sends one byte
  Wire.endTransmission();        // stop transmitting
  Wire.requestFrom((int)addr0, 2);
  // while (Wire.available()) // slave may send less than requested
  // {
  //   rxbuf[i] = Wire.read();
  //   i++;
  // }
  rxbuf[0] = Wire.read();
  rxbuf[1] = Wire.read();
}

float Dfrobot_UltrasonicIIC::getDistanceCm()
{
  txbuf[0] = CMD_DISTANCE_MEASURE; 
  i2cWriteBytes(CMD_INDEX, &txbuf[0]); //写锟斤拷锟斤拷拇锟斤拷锟�,锟斤拷锟酵诧拷锟斤拷锟斤拷锟�
  delay(200);
  i2cReadBytes(DIST_H_INDEX);
  float dist = ((uint16_t)rxbuf[0] << 8) + rxbuf[1];
  return (dist);
}
float Dfrobot_UltrasonicIIC::getTemperature()
{
  txbuf[0] = CMD_DISTANCE_MEASURE;
  i2cWriteBytes(CMD_INDEX, &txbuf[0]); //写锟斤拷锟斤拷拇锟斤拷锟�,锟斤拷锟酵诧拷锟斤拷锟斤拷锟�
  delay(200);
  i2cReadBytes(TEMP_H_INDEX);
  float temp = ((uint16_t)rxbuf[0] << 8) + rxbuf[1];
  return (temp / 10);
}