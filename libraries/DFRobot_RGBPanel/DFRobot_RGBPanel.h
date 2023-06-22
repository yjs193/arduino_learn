#ifndef __DFRobot_RGB_H__
#define __DFRobot_RGB_H__

#include <Wire.h>

class DFRobot_RGBPanel
{
public:

    enum Enum {
        _RGBAddr = 0x10,
        FUNC = 0x02,
        COLOR = 0x03,
        PIX_X = 0x04,
        PIX_Y = 0x05,
        BITMAP = 0x06,
        STR = 0x07,

        UNCLEAR = 0x0,
        CLEAR = 0x1,
        Left = (0x0 << 1),
        Right = (0x1 << 1),
        None = (0x11),
        UNSCROLL = (0x0 << 2),
        SCROLL = (0x1 <<2),
        PIX_ENABLE = (0x01 << 3),
        BITMAP_ENABLE = (0x10 << 3),
        STR_ENABLE = (0x11 << 3)
    };

    enum Color {
        QUENCH = 0,
        RED = 1,
        GREEN = 2,
        YELLOW = 3,
        BLUE = 4,
        PURPLE = 5,
        CYAN = 6,
        WHITE = 7
    };

    DFRobot_RGBPanel();
    void scroll(unsigned char dir);
    void display(unsigned char picIndex,unsigned char color);
    void print(String s,unsigned char color);
    void pixel(unsigned char x,unsigned char y,unsigned char color);
    void fillScreen(unsigned char color);
    void diy(const uint16_t*face, unsigned char color);
    void clear();
    unsigned char buf[50] = {0};
    int * readReg(uint8_t addr,uint8_t num);
    void print(int val,unsigned char color);
    void print(double val,unsigned char color);
private:
    void setReg(unsigned char Reg ,unsigned char *pdata, unsigned char datalen );
    void wireBegin(void);
    bool beginFlag;
};

#endif