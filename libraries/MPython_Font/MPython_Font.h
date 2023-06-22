#ifndef _MPYTHON_FONT_H
#define _MPYTHON_FONT_H

#define MPYTHON_FONT_GB22312 1


#if ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif
#include "esp_spi_flash.h"

#if MPYTHON_FONT_GB22312
#include "Unicode.h"
#endif

#include <stdio.h>
#include <string.h>
#include <math.h>
#include <wchar.h>



typedef struct mfontOne_t {
    unsigned short width;          // 文字宽度
    unsigned short bytes_per_line; // 每行字节数
    unsigned short height;         // 文字高度
    unsigned char  data_len;       //点阵字节数
    unsigned char  data[32];          // 点阵数据
} mfontOne; // mfontOne_t

typedef struct mfontInfo_t {
    unsigned short height;
    unsigned short width;
    unsigned short first_char;
    unsigned short last_char;
    unsigned short address;
    unsigned short type;
} mfontInfo; // mfontInfo_t


class MPython_Font
{
public:

    MPython_Font(unsigned int addr=0x400000);
    ~MPython_Font();

    void      init();
    mfontOne  getCharacterData(const char *str);
    mfontInfo getFontInfo();
    bool      chekingFont();
    void      printText(mfontOne font);

private:

    struct chardata_t{
        unsigned int ptr_char_data;
        unsigned short len;
    }chardata;

    struct character_t{
        char header[4];
        unsigned short height;
        unsigned short width;
        unsigned short baseline;
        unsigned short X_height;
        unsigned short Y_height;
        unsigned short first_char;
        unsigned short last_char;
    }character;

#if MPYTHON_FONT_GB22312
    int gb2312ToUnicode(const char* pInput, unsigned long *Unic);
#endif
    int utf8ToUnicode(const char* pInput, unsigned long *Unic);
    int getUtf8Size(const char *pInput);
    unsigned int firstCharInfoAddress;
    unsigned int fontAddress;
    bool         enable;
};

extern MPython_Font mFont;

#endif // _MPYTHON_FONT_H

