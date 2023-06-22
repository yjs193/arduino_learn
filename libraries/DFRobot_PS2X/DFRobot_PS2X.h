#ifndef PS2X_h
  #define PS2X_h

#if ARDUINO > 22
  #include "Arduino.h"
#else
  #include "WProgram.h"
#endif

#include <math.h>
#include <stdio.h>
#include <stdint.h>
#ifdef __AVR__
  // AVR
  #include <avr/io.h>
  #define CTRL_CLK        4
  #define CTRL_BYTE_DELAY 3
#else
  // Pic32...
  #include <pins_arduino.h>
  #define CTRL_CLK        5
  #define CTRL_CLK_HIGH   5
  #define CTRL_BYTE_DELAY 4
#endif 

//These are our button constants
#define PSB_SELECT      0x0001
#define PSB_L3          0x0002
#define PSB_R3          0x0004
#define PSB_START       0x0008
#define PSB_PAD_UP      0x0010
#define PSB_PAD_RIGHT   0x0020
#define PSB_PAD_DOWN    0x0040
#define PSB_PAD_LEFT    0x0080
#define PSB_L2          0x0100
#define PSB_R2          0x0200
#define PSB_L1          0x0400
#define PSB_R1          0x0800
#define PSB_GREEN       0x1000
#define PSB_RED         0x2000
#define PSB_BLUE        0x4000
#define PSB_PINK        0x8000
#define PSB_TRIANGLE    0x1000
#define PSB_CIRCLE      0x2000
#define PSB_CROSS       0x4000
#define PSB_SQUARE      0x8000

//Guitar  button constants
#define UP_STRUM		0x0010
#define DOWN_STRUM		0x0040
#define STAR_POWER		0x0100
#define GREEN_FRET		0x0200
#define YELLOW_FRET		0x1000
#define RED_FRET		0x2000
#define BLUE_FRET		0x4000
#define ORANGE_FRET		0x8000
#define WHAMMY_BAR		8

//These are stick values
#define PSS_RX 5
#define PSS_RY 6
#define PSS_LX 7
#define PSS_LY 8

//These are analog buttons
#define PSAB_PAD_RIGHT    9
#define PSAB_PAD_UP      11
#define PSAB_PAD_DOWN    12
#define PSAB_PAD_LEFT    10
#define PSAB_L2          19
#define PSAB_R2          20
#define PSAB_L1          17
#define PSAB_R1          18
#define PSAB_GREEN       13
#define PSAB_RED         14
#define PSAB_BLUE        15
#define PSAB_PINK        16
#define PSAB_TRIANGLE    13
#define PSAB_CIRCLE      14
#define PSAB_CROSS       15
#define PSAB_SQUARE      16

#define SET(x,y) (x|=(1<<y))
#define CLR(x,y) (x&=(~(1<<y)))
#define CHK(x,y) (x & (1<<y))
#define TOG(x,y) (x^=(1<<y))

typedef uint8_t   BYTE;

class DFRobot_PS2X {
  public:
    boolean Button(uint16_t);                //will be TRUE if button is being pressed
    unsigned int ButtonDataByte();
    boolean NewButtonState();
    boolean NewButtonState(unsigned int);    //will be TRUE if button was JUST pressed OR released
    boolean ButtonPressed(unsigned int);     //will be TRUE if button was JUST pressed
    boolean ButtonReleased(unsigned int);    //will be TRUE if button was JUST released
    void read_gamepad();
    boolean  read_gamepad(boolean, BYTE);
    void  set_rumble(boolean, BYTE);
    BYTE readType();
    BYTE config_gamepad(uint8_t, uint8_t, uint8_t, uint8_t);
    BYTE config_gamepad(uint8_t, uint8_t, uint8_t, uint8_t, bool, bool);
    void enableRumble();
    bool enablePressures();
    BYTE Analog(BYTE);
    void reconfig_gamepad();

  private:
    inline void CLK_SET(void);
    inline void CLK_CLR(void);
    inline void CMD_SET(void);
    inline void CMD_CLR(void);
    inline void ATT_SET(void);
    inline void ATT_CLR(void);
    inline bool DAT_CHK(void);
    
    unsigned char _gamepad_shiftinout (unsigned char);
    unsigned char PS2data[21];
    void sendCommandString(BYTE*, BYTE);
    unsigned char i;
    unsigned int last_buttons;
    unsigned int buttons;
	uint8_t rumble_f = 0;
    uint8_t rumble_v = 0;
	
    #ifdef __AVR__
      uint8_t maskToBitNum(uint8_t);
      uint8_t _clk_mask; 
      volatile uint8_t *_clk_oreg;
      uint8_t _cmd_mask; 
      volatile uint8_t *_cmd_oreg;
      uint8_t _att_mask; 
      volatile uint8_t *_att_oreg;
      uint8_t _dat_mask; 
      volatile uint8_t *_dat_ireg;
    #else
      uint8_t _clk;
      uint8_t _cmd;
      uint8_t _att;
      uint8_t _dat;
      
      #endif

    unsigned long last_read;
    BYTE read_delay;
    BYTE controller_type;
    boolean en_Rumble;
    boolean en_Pressures;
};

#endif
