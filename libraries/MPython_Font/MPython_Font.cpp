#include "MPython_Font.h"


MPython_Font::MPython_Font(unsigned int addr):
    fontAddress(addr),
    firstCharInfoAddress(0),
    enable(false)
{
}

MPython_Font::~MPython_Font()
{
}

void MPython_Font::init(){
    if(enable) return;
    spi_flash_init();
    spi_flash_read(fontAddress,(uint8_t*)&character,sizeof(character_t));
    firstCharInfoAddress = fontAddress + 18;
    if(character.header[0] != 'G' ||
        character.header[1] != 'U' ||
        character.header[2] != 'I' ||
        character.header[3] != 'X' ||
        character.height != 16 ||
        character.width != 16)
    {
        enable = false;
        return;
    }
    enable = true;
}

mfontOne MPython_Font::getCharacterData(const char *str){
    mfontOne font;
    memset(font.data, 0, 32);
    font.height = 0;
    font.data_len = 0;
    font.width = 0;
    font.bytes_per_line = 0;
    if(!enable) {
        return font;
    };
    unsigned long uni = str[0];
    if((*str & 0xf0) == 0xe0 && (*(str + 1) & 0xc0) == 0x80 && (*(str + 2) & 0xc0) == 0x80 || (*str == 0xc2 && *(str + 1) == 0xb7)){
        utf8ToUnicode(str, &uni);
    }
    else if((*str & 0x80) == 0x80){
#if MPYTHON_FONT_GB22312
        gb2312ToUnicode(str, &uni);
#else
        return font;
#endif
    }
    if (uni < character.first_char || uni > character.last_char){
            return font;
    }
    uint32_t charInfoAddress = firstCharInfoAddress + (uni - character.first_char) * 6;
    spi_flash_read(charInfoAddress, (uint8_t*)&chardata, sizeof(chardata_t));
    if ((chardata.ptr_char_data) == 0 || (chardata.len == 0))
        return font;
    uint8_t buffer[chardata.len];
    spi_flash_read(chardata.ptr_char_data + fontAddress, buffer, chardata.len);
    memcpy(&font, buffer, 4);
    font.height = character.height;
    font.data_len = chardata.len-4;
    memcpy(font.data, buffer+4, chardata.len-4);
    return font;
}

#if MPYTHON_FONT_GB22312
int MPython_Font::gb2312ToUnicode(const char* pInput, unsigned long *Unic)
{
    uint16_t gb2312 = (uint16_t)pInput[0]<<8 | (uint16_t)pInput[1];
    for(int i=0; i<21243; i++) {
        if(giGB2312[i][0] == gb2312) 
        {
            *Unic = giGB2312[i][1];
            return 0;
        }
    }
    return -1;
}
#endif

int MPython_Font::utf8ToUnicode(const char* pInput, unsigned long *Unic) 
{
    assert(pInput != NULL && Unic != NULL); 
    char b1, b2, b3, b4, b5, b6; 

    *Unic = 0x0;  
    int utfbytes = getUtf8Size(pInput); 
    unsigned char *pOutput = (unsigned char *) Unic; 
    switch ( utfbytes ) 
    { 
        case 0: 
            *pOutput     = *pInput; 
            utfbytes    += 1; 
            break;
        case 2: 
            b1 = *pInput; 
            b2 = *(pInput + 1); 
            if ( (b2 & 0xE0) != 0x80 ) 
                return 0; 
            *pOutput     = (b1 << 6) + (b2 & 0x3F); 
            *(pOutput+1) = (b1 >> 2) & 0x07; 
            break;
             
        case 3: 
            b1 = *pInput; 
            b2 = *(pInput + 1); 
            b3 = *(pInput + 2); 
            if ( ((b2 & 0xC0) != 0x80) || ((b3 & 0xC0) != 0x80) ) 
                return 0; 
            *pOutput     = (b2 << 6) + (b3 & 0x3F); 
            *(pOutput+1) = (b1 << 4) + ((b2 >> 2) & 0x0F); 
            break;
        case 4: 
            b1 = *pInput; 
            b2 = *(pInput + 1); 
            b3 = *(pInput + 2); 
            b4 = *(pInput + 3); 
            if ( ((b2 & 0xC0) != 0x80) || ((b3 & 0xC0) != 0x80) 
                    || ((b4 & 0xC0) != 0x80) ) 
                return 0; 
            *pOutput     = (b3 << 6) + (b4 & 0x3F); 
            *(pOutput+1) = (b2 << 4) + ((b3 >> 2) & 0x0F); 
            *(pOutput+2) = ((b1 << 2) & 0x1C)  + ((b2 >> 4) & 0x03); 
            break;
        case 5: 
            b1 = *pInput; 
            b2 = *(pInput + 1); 
            b3 = *(pInput + 2); 
            b4 = *(pInput + 3); 
            b5 = *(pInput + 4); 
            if ( ((b2 & 0xC0) != 0x80) || ((b3 & 0xC0) != 0x80) 
                    || ((b4 & 0xC0) != 0x80) || ((b5 & 0xC0) != 0x80) ) 
                return 0; 
            *pOutput     = (b4 << 6) + (b5 & 0x3F); 
            *(pOutput+1) = (b3 << 4) + ((b4 >> 2) & 0x0F); 
            *(pOutput+2) = (b2 << 2) + ((b3 >> 4) & 0x03); 
            *(pOutput+3) = (b1 << 6); 
            break;
        case 6: 
            b1 = *pInput; 
            b2 = *(pInput + 1); 
            b3 = *(pInput + 2); 
            b4 = *(pInput + 3); 
            b5 = *(pInput + 4); 
            b6 = *(pInput + 5); 
            if ( ((b2 & 0xC0) != 0x80) || ((b3 & 0xC0) != 0x80) 
                    || ((b4 & 0xC0) != 0x80) || ((b5 & 0xC0) != 0x80) 
                    || ((b6 & 0xC0) != 0x80) ) 
                return 0; 
            *pOutput     = (b5 << 6) + (b6 & 0x3F); 
            *(pOutput+1) = (b5 << 4) + ((b6 >> 2) & 0x0F); 
            *(pOutput+2) = (b3 << 2) + ((b4 >> 4) & 0x03); 
            *(pOutput+3) = ((b1 << 6) & 0x40) + (b2 & 0x3F); 
            break;
        default: 
            return 0; 
            break; 
    }
    return utfbytes; 
}

int MPython_Font::getUtf8Size(const char *pInput)
{
    unsigned char c = *((unsigned char *)pInput);
    if(c< 0x80) return 0;                // 0xxxxxxx 返回0
    if(c>=0x80 && c<0xC0) return -1;     // 10xxxxxx 返回-1
    if(c>=0xC0 && c<0xE0) return 2;      // 110xxxxx 返回2
    if(c>=0xE0 && c<0xF0) return 3;      // 1110xxxx 返回3
    if(c>=0xF0 && c<0xF8) return 4;      // 11110xxx 返回4
    if(c>=0xF8 && c<0xFC) return 5;      // 111110xx 返回5
    if(c>=0xFC) return 6;                // 1111110x 返回6
}

void MPython_Font::printText(mfontOne font){
    for(int h=0; h<font.height; h++){
        for(int i=0; i<font.bytes_per_line; i++){
            uint8_t data = *(font.data + h*font.bytes_per_line + i);
            for(int k=0; k<8; k++){
                if(data & 0x80) {
                    Serial.print("*");
                }else{
                    Serial.print(" ");
                }
                data = data << 1;
            }
        }
        Serial.println();
    }
    Serial.println();
}

bool MPython_Font::chekingFont()
{
    init();
    return enable;
}

MPython_Font mFont(0x400000);