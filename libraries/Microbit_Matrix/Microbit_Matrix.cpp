#include <Microbit_Matrix.h>
#include <basic.h>


Microbit_Matrix MMatrix;


Microbit_Matrix::~Microbit_Matrix(void) {
  this->interval = INTERVAL_DEFAULT;
  this->isEnable = true;
  this->readLightLevelFirst = true;
}

Microbit_Matrix::Microbit_Matrix() {
  this->interval = INTERVAL_DEFAULT;
  this->isEnable = true;
  this->readLightLevelFirst = true;
}

int Microbit_Matrix::readLightLevel() {
  if(!isEnable) return 0xff;
  int lightLevel = uBit.display.readLightLevel();
  if(this->readLightLevelFirst){
    lightLevel = lightLevel == 255 ? 0 : lightLevel;
    this->readLightLevelFirst = false;
  }
  return lightLevel;
}

void Microbit_Matrix::setBrightness(uint8_t b) {
#ifdef NRF52833   //debugjh 与实时不同。实时直接0-255
  b = constrain(b, 0, 9);
  b = map(b, 0, 9, 0, 255);
  uBit.display.setBrightness(b);
#else
  b = constrain(b, 0, 9);
  b = map(b, 0, 9, 0, 255);
  uBit.display.setBrightness(b);
#endif
}

void Microbit_Matrix::drawPixel(int16_t x, int16_t y, uint16_t color) {
  if(!isEnable) return;
  uBit.display.image.setPixelValue(x, y, color ? 0xff : 0);
}

void Microbit_Matrix::clear(void) {
  if(!isEnable) return;
  uBit.display.image.clear();
}

void Microbit_Matrix::show(const uint8_t bitmap[]) {
  if(!isEnable) return;
  for(int i = 0; i < HEIGHT; i++){
    for(int j=0; j < WIDTH; j++){
      if(bitmap[i]<<(3+j) & 0x80)
#ifdef NRF52833   //debugjh
        heart[i*WIDTH+j + ICON_OFFSET] = uBit.display.getBrightness();
#else
        heart[i*WIDTH+j + ICON_OFFSET] = 1;
#endif
      else
        heart[i*WIDTH+j + ICON_OFFSET] = 0;
      }
  }

  MicroBitImage icon((ImageData*)(void*)heart);

  uBit.display.print(icon, 0, 0, 0, 200);
}

void Microbit_Matrix::setInterval(int val) {
  if(val < 1) val = 1;
  this->interval = val;
}

size_t Microbit_Matrix::write(const char *str, size_t size) {
  if(!isEnable) return size;
  if(this->interval < 1) this->interval = 1;
  ManagedString s(str);
  int l = s.length();
  if (l == 0) {
    uBit.display.clear();
    fiber_sleep(this->interval * 5);
  } else if (l > 1) {
    uBit.display.scroll(s, this->interval);
  } else {
    uBit.display.print(s.charAt(0), this->interval * 5);
  }
  return size;
}

void Microbit_Matrix::stopAnimation() {
  if(!isEnable) return;
  uBit.display.stopAnimation();
}

void Microbit_Matrix::pixelBrightness(int x, int y, int brightness) {
  brightness = constrain(brightness, 0, 9);
  brightness = map(brightness, 0, 9, 0, 255);
  if (brightness != 0 && brightness != 0xff && uBit.display.getDisplayMode() != DISPLAY_MODE_GREYSCALE)
    uBit.display.setDisplayMode(DISPLAY_MODE_GREYSCALE);
  uBit.display.image.setPixelValue(x, y, brightness);
}

void Microbit_Matrix::enable(bool on){
  if (on && !isEnable) {
    uBit.display.enable();
    isEnable = true;
  }
  if (!on && isEnable) {
    uBit.display.disable();
    isEnable = false;
  }
}

int Microbit_Matrix::getBrightness() {
  return uBit.display.getBrightness();
}

size_t Microbit_Matrix::print(const __FlashStringHelper *ifsh)
{
  return print(reinterpret_cast<const char *>(ifsh));
}

size_t Microbit_Matrix::print(const String &s)
{
  return write(s.c_str(), s.length());
}

size_t Microbit_Matrix::print(const char str[])
{
  if (str == NULL) return 0;
  return write((const char *)str, strlen(str));
}

size_t Microbit_Matrix::print(char c)
{
  const char str[] = {c, '\0'};
  return write((const char *)str, strlen(str));
}

size_t Microbit_Matrix::print(unsigned char b)
{
  return print((unsigned long) b);
}

size_t Microbit_Matrix::print(int n)
{
  return print((long) n);
}

size_t Microbit_Matrix::print(unsigned int n)
{
  return print((unsigned long) n);
}

size_t Microbit_Matrix::print(long n)
{
  return print(String(n));
}

size_t Microbit_Matrix::print(unsigned long n)
{
  return print(String(n));
}

size_t Microbit_Matrix::print(double number, uint8_t digits)
{
  size_t n = 0;
  String str = "";

  if (isnan(number)) return print("nan");
  if (isinf(number)) return print("inf");
  if (number > 4294967040.0) return print ("ovf");  // constant determined empirically
  if (number <-4294967040.0) return print ("ovf");  // constant determined empirically

  // Handle negative numbers
  if (number < 0.0)
  {
     //n += print('-');
     str += "-";
     number = -number;
  }

  // Round correctly so that print(1.999, 2) prints as "2.00"
  double rounding = 0.5;
  for (uint8_t i=0; i<digits; ++i)
    rounding /= 10.0;

  number += rounding;

  // Extract the integer part of the number and print it
  unsigned long int_part = (unsigned long)number;
  double remainder = number - (double)int_part;
  //n += print(int_part);
  str += int_part;

  // Print the decimal point, but only if there are digits beyond
  if (digits > 0) {
    //n += print(".");
    str += ".";
  }

  // Extract digits from the remainder one at a time
  while (digits-- > 0)
  {
    remainder *= 10.0;
    int toPrint = int(remainder);
    //n += print(toPrint);
    str += toPrint;
    remainder -= toPrint;
  }
  int len = str.length();
  while(len-- > 0)
  {
    if(str[len-1] == '0') str[len-1] = '\0';
    else if(str[len-1] == '.')
    {
      str[len-1] = '\0';
      break;
    }
    else break;
  }

  return print(str);
}

