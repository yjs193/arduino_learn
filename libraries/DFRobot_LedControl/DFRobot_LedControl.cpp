#include "DFRobot_LedControl.h"

//the opcodes for the MAX7221 and MAX7219
#define OP_NOOP   0
#define OP_DIGIT0 1
#define OP_DIGIT1 2
#define OP_DIGIT2 3
#define OP_DIGIT3 4
#define OP_DIGIT4 5
#define OP_DIGIT5 6
#define OP_DIGIT6 7
#define OP_DIGIT7 8
#define OP_DECODEMODE  9
#define OP_INTENSITY   10
#define OP_SCANLIMIT   11
#define OP_SHUTDOWN    12
#define OP_DISPLAYTEST 15

DFRobot_LedControl::DFRobot_LedControl() {
    enable = false;
}

int DFRobot_LedControl::getDeviceCount() {
    return maxDevices;
}

void DFRobot_LedControl::shutdown(int addr, bool b) {
    checkEnable();
    if(addr<0 || addr>=maxDevices)
        return;
    if(b)
        spiTransfer(addr, OP_SHUTDOWN,0);
    else
        spiTransfer(addr, OP_SHUTDOWN,1);
}

void DFRobot_LedControl::setScanLimit(int addr, int limit) {
    checkEnable();
    if(addr<0 || addr>=maxDevices)
        return;
    if(limit>=0 && limit<8)
        spiTransfer(addr, OP_SCANLIMIT,limit);
}

void DFRobot_LedControl::setIntensity(int addr, int intensity) {
    checkEnable();
    if(addr<0 || addr>=maxDevices)
        return;
    if(intensity>=0 && intensity<16)    
        spiTransfer(addr, OP_INTENSITY,intensity);
}

void DFRobot_LedControl::clearDisplay(int addr) {
    checkEnable();
    int offset;

    if(addr<0 || addr>=maxDevices)
        return;
    offset=addr*8;
    for(int i=0;i<8;i++) {
        status[offset+i]=0;
        spiTransfer(addr, i+1,status[offset+i]);
    }
}

void DFRobot_LedControl::setLed(int addr, int row, int column, boolean state) {
    checkEnable();
    int offset;
    BYTE val=0x00;

    if(addr<0 || addr>=maxDevices)
        return;
    if(row<0 || row>7 || column<0 || column>7)
        return;
    offset=addr*8;
    val=B10000000 >> column;
    if(state)
        status[offset+row]=status[offset+row]|val;
    else {
        val=~val;
        status[offset+row]=status[offset+row]&val;
    }
    spiTransfer(addr, row+1,status[offset+row]);
}

void DFRobot_LedControl::setRow(int addr, int row, BYTE value) {
    checkEnable();
    int offset;
    if(addr<0 || addr>=maxDevices)
        return;
    if(row<0 || row>7)
        return;
    offset=addr*8;
    status[offset+row]=value;
    spiTransfer(addr, row+1,status[offset+row]);
}

void DFRobot_LedControl::setColumn(int addr, int col, BYTE value) {
    checkEnable();
    BYTE val;

    if(addr<0 || addr>=maxDevices)
        return;
    if(col<0 || col>7) 
        return;
    for(int row=0;row<8;row++) {
        val=value >> (7-row);
        val=val & 0x01;
        setLed(addr,row,col,val);
    }
}

void DFRobot_LedControl::setDigit(int addr, int digit, BYTE value, boolean dp) {
    checkEnable();
    int offset;
    BYTE v;

    if(addr<0 || addr>=maxDevices)
        return;
    if(digit<0 || digit>7 || value>15)
        return;
    offset=addr*8;
    v=pgm_read_byte_near(charTable + value); 
    if(dp)
        v|=B10000000;
    status[offset+digit]=v;
    spiTransfer(addr, digit+1,v);
}

void DFRobot_LedControl::setChar(int addr, int digit, char value, boolean dp) {
    checkEnable();
    int offset;
    BYTE index,v;

    if(addr<0 || addr>=maxDevices)
        return;
    if(digit<0 || digit>7)
        return;
    offset=addr*8;
    index=(BYTE)value;
    if(index >127) {
        //no defined beyond index 127, so we use the space char
        index=32;
    }
    v=pgm_read_byte_near(charTable + index); 
    if(dp)
        v|=B10000000;
    status[offset+digit]=v;
    spiTransfer(addr, digit+1,v);
}

void DFRobot_LedControl::spiTransfer(int addr, volatile BYTE opcode, volatile BYTE data) {
    checkEnable();
    //Create an array with the data to shift out
    int offset=addr*2;
    int maxbytes=maxDevices*2;

    for(int i=0;i<maxbytes;i++)
        spidata[i]=(BYTE)0;
    //put our device data into the array
    spidata[offset+1]=opcode;
    spidata[offset]=data;
    //enable the line 
    DIGITALWRITE(SPI_CS,LOW);
    //Now shift out the data 
    for(int i=maxbytes;i>0;i--)
        shiftOut(SPI_MOSI,SPI_CLK,MSBFIRST,spidata[i-1]);
    //latch the data onto the display
    DIGITALWRITE(SPI_CS,HIGH);
}

void DFRobot_LedControl::show(const BYTE mychar[], BYTE num) {
    checkEnable();
    for (int i = 0; i < 8; i++)
        setRow(num, i, mychar[i]);
}

void DFRobot_LedControl::begin(int dataPin, int clkPin, int csPin, int numDevices) {
    enable = true;
    SPI_MOSI=dataPin;
    SPI_CLK=clkPin;
    SPI_CS=csPin;
    if(numDevices<=0 || numDevices>8 )
        numDevices=8;
    maxDevices=numDevices;
    PINMODE(SPI_MOSI,OUTPUT);
    PINMODE(SPI_CLK,OUTPUT);
    PINMODE(SPI_CS,OUTPUT);
    DIGITALWRITE(SPI_CS,HIGH);
    SPI_MOSI=dataPin;
    for(int i=0;i<64;i++) 
        status[i]=0x00;
    for(int i=0;i<maxDevices;i++) {
        spiTransfer(i,OP_DISPLAYTEST,0);
        //scanlimit is set to max on startup
        setScanLimit(i,7);
        //decode is done in source
        spiTransfer(i,OP_DECODEMODE,0);
        clearDisplay(i);
        //we go into shutdown-mode on startup
        shutdown(i,true);
    }
    for(int i=0; i<maxDevices; i++) {
        shutdown(i,false);
        setIntensity(i,15);
        clearDisplay(i);
    }
}

void DFRobot_LedControl::checkEnable()
{
    if (!enable) {
        enable = true;
        begin(4, 6, 5);
    }
}