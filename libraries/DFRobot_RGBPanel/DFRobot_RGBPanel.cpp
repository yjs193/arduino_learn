#include <DFRobot_RGBPanel.h>
#include <Wire.h>
#include <Arduino.h>
DFRobot_RGBPanel::DFRobot_RGBPanel()
{
  beginFlag = false;
}

void DFRobot_RGBPanel::wireBegin(void)
{
  beginFlag = true;
  Wire.begin();
}

void DFRobot_RGBPanel::clear()
{
  if (!beginFlag)
  {
    this->wireBegin();
  }
  int i = 0;
  buf[0] = 0x1;
  for (i = 1; i < 50; i++)
  {
    buf[i] = 0;
  }
  setReg(0x02, buf, 50);
  delay(100);
}

void DFRobot_RGBPanel::scroll(unsigned char dir)
{
  if (!beginFlag)
  {
    this->wireBegin();
  }
  if ((dir != None) && (dir != Right) && (dir != Left))
  {
    return;
  }
  if (dir == None)
  {
    buf[0] &= (~(0x01 << 2));
  }
  else if (dir == Right)
  {
    buf[0] |= (0x01 << 2) | (0x01 << 1);
  }
  else if (dir == Left)
  {
    buf[0] |= (0x01 << 2);
    buf[0] &= (~(0x01 << 1));
  }
  else
    return;
}

void DFRobot_RGBPanel::display(unsigned char picIndex, unsigned char color)
{
  if (!beginFlag)
  {
    this->wireBegin();
  }
  if (picIndex >= 8 && picIndex <= 11)
    picIndex += 4;
  else if (picIndex >= 12 && picIndex <= 15)
    picIndex -= 4;
  else if (picIndex == 22)
    picIndex = 21;
  else if (picIndex == 21)
    picIndex = 22;
  else if (picIndex == 20)
    picIndex = 17;
  else if (picIndex == 18)
    picIndex = 20;
  else if (picIndex == 19)
    picIndex = 18;
  else if (picIndex == 17)
    picIndex = 20;

  buf[0] = (buf[0] & (0xe6)) | (0x02 << 3);
  buf[1] = color;
  buf[4] = picIndex;
  setReg(0x02, buf, 50);
  delay(100);
}

void DFRobot_RGBPanel::print(int val, unsigned char color)
{
  if (!beginFlag)
  {
    this->wireBegin();
  }
  String s = String(val);
  print(s, color);
}

void DFRobot_RGBPanel::print(double val, unsigned char color)
{
  if (!beginFlag)
  {
    this->wireBegin();
  }
  String s = String(val);
  print(s, color);
}

void DFRobot_RGBPanel::print(String s, unsigned char color)
{
  if (!beginFlag)
  {
    this->wireBegin();
  }
  unsigned char i = 0;
  int len = s.length();
  if (len > 21)
    len = 21;

  if (len > 30)
  {
    return;
  }
  buf[0] = (buf[0] & (0xe6)) | (0x03 << 3);
  buf[1] = color;
  for (i = 0; i <= len; i++)
  {
    buf[5 + i] = s[i];
  }
  setReg(0x02, buf, 50);
  delay(100);
}

void DFRobot_RGBPanel::pixel(unsigned char x, unsigned char y, unsigned char color)
{
  if (!beginFlag)
  {
    this->wireBegin();
  }
  buf[0] = (buf[0] & (0xe6)) | (0x01 << 3);
  buf[1] = color;
  buf[2] = x;
  buf[3] = y;
  setReg(0x02, buf, 50);
#ifdef ESP32
  delay(5);
#endif
}

void DFRobot_RGBPanel::fillScreen(unsigned char color)
{
  if (!beginFlag)
  {
    this->wireBegin();
  }
  buf[0] = 0x1;
  buf[0] = buf[0] = (buf[0] & (0xe7)) | (0x01 << 3);
  buf[1] = color;
  buf[2] = 0;
  buf[3] = 0;
  setReg(0x02, buf, 50);
  delay(100);
}

void DFRobot_RGBPanel::setReg(unsigned char Reg, unsigned char *pdata, unsigned char datalen)
{
  Wire.beginTransmission(_RGBAddr); // transmit to device #8
  Wire.write(Reg);                  // sends one byte
  for (uint8_t i = 0; i < datalen; i++)
  {
    Wire.write(*pdata);
    pdata++;
  }
  Wire.endTransmission(); // stop transmitting
}

int *DFRobot_RGBPanel::readReg(uint8_t addr, uint8_t num)
{
  static int result[64];
  int i = 0;
  Wire.beginTransmission(_RGBAddr); // Start transmission to device
  Wire.write(addr);                 // Sends register address to read rom
  Wire.endTransmission(false);      // End transmission

  Wire.requestFrom((uint8_t)_RGBAddr, num); // Send data n-bytes read
  while (Wire.available())                  // slave may send less than requested
  {
    result[i++] = Wire.read(); // print the character
  }
  //  Serial.println(result[0]);
  return result;
}

void DFRobot_RGBPanel::diy(const uint16_t *face, unsigned char color)
{
  if (!beginFlag)
  {
    this->wireBegin();
  }
  // Through the array
  int i, j;
  for (j = 0; j < 8; j++)
  {
    for (i = 0; i < 16; i++)
    {
      if ((face[j] << i) & 0x8000)
      {
        this->pixel(i, j, color);
      }
      else
      {
        this->pixel(i, j, QUENCH);
      }
    }
  }
}
