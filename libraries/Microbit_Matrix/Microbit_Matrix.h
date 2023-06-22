#ifndef _MICROBIT_MATRIX_H_
#define _MICROBIT_MATRIX_H_

#include <Arduino.h>
#include "Printable.h"
//include "Print.h"

#define ICON_OFFSET         6
#define WIDTH               5
#define HEIGHT              5

#define LED_ON              1
#define LED_OFF             0

#define INTERVAL_DEFAULT   150


class Microbit_Matrix// : public Print
{
    public:

    Microbit_Matrix();
    ~Microbit_Matrix(void);

    void      drawPixel(int16_t x, int16_t y, uint16_t color),
              clear(void),
              setBrightness( uint8_t brightness),
              show(const uint8_t bitmap[]),
              stopAnimation(),
              pixelBrightness(int x, int y, int brightness),
              enable(bool on);

    int       getBrightness();
    void      setInterval(int val);
    int       readLightLevel();
    // using     Print::write; // pull in write(str) and write(buf, size) from Print
    size_t    print(const __FlashStringHelper *);
    size_t    print(const String &);
    size_t    print(const char[]);
    size_t    print(char);
    size_t    print(unsigned char);
    size_t    print(int);
    size_t    print(unsigned int);
    size_t    print(long);
    size_t    print(unsigned long);
    size_t    print(double, uint8_t digits = 10);

const uint8_t EMPTYHEART[5] = 
  { B01010,
    B10101,
    B10001,
    B01010,
    B00100, };

const uint8_t HEART[5] = 
  { B01010,
    B11111,
    B11111,
    B01110,
    B00100, };

const uint8_t NO[5] = 
  { B10001,
    B01010,
    B00100,
    B01010,
    B10001, };

const uint8_t YES[5] = 
  { B00000,
    B00001,
    B00010,
    B10100,
    B01000, };

const uint8_t HAPPY[5] = 
  { B00000,
     B01010,
     B00000,
     B10001,
     B01110, };

const uint8_t ANGRY[5] = 
  { B10001,
     B01010,
     B00000,
     B11111,
     B10101,};
     
const uint8_t ARROW_E[5] = 
  { B00100,
     B00010,
     B11111,
     B00010,
     B00100, };
     
const uint8_t ARROW_N[5] = 
  { B00100,
     B01110,
     B10101,
     B00100,
     B00100, };
     
const uint8_t ARROW_NE[5] = 
  { B00111,
     B00011,
     B00101,
     B01000,
     B10000, };
     
     
     
const uint8_t ARROW_NW[5] = 
  { B11100,
     B11000,
     B10100,
     B00010,
     B00001, };

const uint8_t ARROW_S[5] = 
  { B00100,
     B00100,
     B10101,
     B01110,
     B00100,};
     
const uint8_t ARROW_SE[5] = 
  { B10000,
     B01000,
     B00101,
     B00011,
     B00111, };
     
const uint8_t ARROW_SW[5] = 
  { B00001,
     B00010,
     B10100,
     B11000,
     B11100, };
     
const uint8_t ARROW_W[5] = 
  { B00100,
     B01000,
     B11111,
     B01000,
     B00100, };
     
     
const uint8_t ASLEEP[5] = 
  { B00000,
     B11011,
     B00000,
     B01110,
     B00000, };

const uint8_t BUTTERFLY[5] = 
  { B11011,
     B11111,
     B00100,
     B11111,
     B11011,};
     
const uint8_t CHESSBOARD[5] = 
  { B01010,
     B10101,
     B01010,
     B10101,
     B01010, };
     
const uint8_t CLOCK1[5] = 
  { B00010,
     B00010,
     B00100,
     B00000,
     B00000, };
     
const uint8_t CLOCK10[5] = 
  { B00000,
     B11000,
     B00100,
     B00000,
     B00000 };



const uint8_t CLOCK11[5] = 
  { B01000,
     B01000,
     B00100,
     B00000,
     B00000, };

const uint8_t CLOCK12[5] = 
  { B00100,
     B00100,
     B00100,
     B00000,
     B00000,};
     
const uint8_t CLOCK2[5] = 
  { B00000,
     B00011,
     B00100,
     B00000,
     B00000, };
     
const uint8_t CLOCK3[5] = 
  { B00000,
     B00000,
     B00111,
     B00000,
     B00000, };
     
const uint8_t CLOCK4[5] = 
  { B00000,
     B00000,
     B00100,
     B00011,
     B00000};



const uint8_t CLOCK5[5] = 
  { B00000,
     B00000,
     B00100,
     B00010,
     B00010 };

const uint8_t CLOCK6[5] = 
  { B00000,
     B00000,
     B00100,
     B00100,
     B00100,};
     
const uint8_t CLOCK7[5] = 
  { B00000,
     B00000,
     B00100,
     B01000,
     B01000, };
     
const uint8_t CLOCK8[5] = 
  { B00000,
     B00000,
     B00100,
     B11000,
     B00000, };
     
const uint8_t CLOCK9[5] = 
  { B00000,
     B00000,
     B11100,
     B00000,
     B00000};


const uint8_t CONFUSED[5] = 
  { B00000,
     B01010,
     B00000,
     B01010,
     B10101};

const uint8_t COW[5] = 
  { B10001,
     B10001,
     B11111,
     B01110,
     B00100,};
     
const uint8_t DIAMOND[5] = 
  { B00100,
     B01010,
     B10001,
     B01010,
     B00100, };
     
const uint8_t DIAMOND_SMALL[5] = 
  { B00000,
     B00100,
     B01010,
     B00100,
     B00000, };
     
const uint8_t DUCK[5] = 
  { B01100,
     B11100,
     B01111,
     B01110,
     B00000};

const uint8_t FABULOUS[5] = 
  { B11111,
     B11011,
     B00000,
     B01010,
     B01110, };

const uint8_t GHOST[5] = 
  { B11111,
     B10101,
     B11111,
     B11111,
     B10101,};
     
const uint8_t GIRAFFE[5] = 
  { B11000,
     B01000,
     B01000,
     B01110,
     B01010, };
     
const uint8_t HEART_SMALL[5] = 
  { B00000,
     B01010,
     B01110,
     B00100,
     B00000, };
     
const uint8_t HOUSE[5] = 
  { B00100,
     B01110,
     B11111,
     B01110,
     B01010 };

const uint8_t MEH[5] = 
  { B01010,
     B00000,
     B00010,
     B00100,
     B01000, };

const uint8_t MUSIC_CROTCHET[5] = 
  { B00100,
     B00100,
     B00100,
     B11100,
     B11100,};
     
const uint8_t MUSIC_QUAVER[5] = 
  { B00100,
     B00110,
     B00101,
     B11100,
     B11100, };
     
const uint8_t MUSIC_QUAVERS[5] = 
  { B01111,
     B01001,
     B01001,
     B11011,
     B11011, };
     
const uint8_t PACMAN[5] = 
  { B01111,
     B11010,
     B11100,
     B11110,
     B01111};


const uint8_t PITCHFORK[5] = 
  { B10101,
     B10101,
     B11111,
     B00100,
     B00100, };
     
const uint8_t RABBIT[5] = 
  { B10100,
     B10100,
     B11110,
     B11010,
     B11110};

const uint8_t ROLLERSKATE[5] = 
  { B00011,
     B00011,
     B11111,
     B11111,
     B01010, };

const uint8_t SAD[5] = 
  { B00000,
     B01010,
     B00000,
     B01110,
     B10001,};
     
const uint8_t SILLY[5] = 
  { B10001,
     B00000,
     B11111,
     B00101,
     B00111 };
     
const uint8_t SKULL[5] = 
  { B01110,
     B10101,
     B11111,
     B01110,
     B01110 };
     
const uint8_t SMILE[5] = 
  { B00000,
     B00000,
     B00000,
     B10001,
     B01110};

const uint8_t SNAKE[5] = 
  { B11000,
     B11011,
     B01010,
     B01110,
     B00000};

const uint8_t SQUARE[5] = 
  { B11111,
     B10001,
     B10001,
     B10001,
     B11111 };

const uint8_t SQUARE_SMALL[5] = 
  { B00000,
     B01110,
     B01010,
     B01110,
     B00000};
     
const uint8_t STICKFIGURE[5] = 
  { B00100,
     B11111,
     B00100,
     B01010,
     B10001 };
     
const uint8_t SURPRISED[5] = 
  { B01010,
     B00000,
     B00100,
     B01010,
     B00100 };
     
const uint8_t SWORD[5] = 
  { B00100,
     B00100,
     B00100,
     B01110,
     B00100 };


const uint8_t TARGET[5] = 
  { B00100,
     B01110,
     B11011,
     B01110,
     B00100 };

const uint8_t TORTOISE[5] = 
  { B00000,
     B01110,
     B11111,
     B01010,
     B00000 };

const uint8_t TRIANGLE[5] = 
  { B00000,
     B00100,
     B01010,
     B11111,
     B00000 };

const uint8_t TRIANGLE_LEFT[5] = 
  { B10000,
     B11000,
     B10100,
     B10010,
     B11111 };
     
const uint8_t TSHIRT[5] = 
  { B11011,
     B11111,
     B01110,
     B01110,
     B01110 };
     
const uint8_t UMBRELLA[5] = 
  { B01110,
     B11111,
     B00100,
     B10100,
     B01100 };
     
const uint8_t XMAS[5] = 
  { B00100,
     B01110,
     B00100,
     B01110,
     B11111 };


const uint8_t ALL_CLOCKS[12][5] = 
{
  { B00010,
     B00010,
     B00100,
     B00000,
     B00000, },
  { B00000,
     B11000,
     B00100,
     B00000,
     B00000 },
  { B01000,
     B01000,
     B00100,
     B00000,
     B00000, },
  { B00100,
     B00100,
     B00100,
     B00000,
     B00000,},
  { B00000,
     B00011,
     B00100,
     B00000,
     B00000, },
  { B00000,
     B00000,
     B00111,
     B00000,
     B00000, },
  { B00000,
     B00000,
     B00100,
     B00011,
     B00000},
  { B00000,
     B00000,
     B00100,
     B00010,
     B00010 },

  { B00000,
     B00000,
     B00100,
     B00100,
     B00100,},
     

  { B00000,
     B00000,
     B00100,
     B01000,
     B01000, },
     

  { B00000,
     B00000,
     B00100,
     B11000,
     B00000, },

  { B00000,
     B00000,
     B11100,
     B00000,
     B00000},
};

    private:
    int       interval;
    bool      isEnable;
    bool      readLightLevelFirst;
    uint8_t heart[31] __attribute__ ((aligned (4))) = { 0xff, 0xff, 5, 0, 5, 0, 
    0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0,
    0, 0, 0, 0, 0,
    0, 0, 0, 0, 0,}; // a cute heart

    size_t    write(const char *, size_t);
};

extern Microbit_Matrix MMatrix;




#endif

