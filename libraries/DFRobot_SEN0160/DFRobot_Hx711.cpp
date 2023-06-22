/*
 * Hx711.cpp
 *
 *  Created on: Oct 31, 2012
 *      Author: agu
 */

#include "DFRobot_Hx711.h"

Hx711::Hx711()
{
}
void Hx711::begin(uint8_t pin_dout, uint8_t pin_slk)
{
    _pin_dout = pin_dout;
    _pin_slk = pin_slk;
    pinMode(_pin_slk, OUTPUT);
    pinMode(_pin_dout, INPUT);

    digitalWrite(_pin_slk, HIGH);
    delayMicroseconds(100);
    digitalWrite(_pin_slk, LOW);

    averageValue();
    this->setOffset(averageValue());
    this->setScale();
}
Hx711::~Hx711()
{

}
long Hx711::averageValue(BYTE times)
{
    long sum = 0;
    for (BYTE i = 0; i < times; i++)
    {
        sum += getValue();
    }

    return sum / times;
}

long Hx711::getValue()
{
    BYTE data[3];

    while (digitalRead(_pin_dout)==HIGH){delay(1);}
        

    for (BYTE j = 0; j < 3; j++)
    {
        for (BYTE i = 0; i < 8; i++)
        {
            digitalWrite(_pin_slk, HIGH);
            bitWrite(data[2 - j], 7 - i, digitalRead(_pin_dout));
            digitalWrite(_pin_slk, LOW);
        }
    }

    digitalWrite(_pin_slk, HIGH);
    digitalWrite(_pin_slk, LOW);

    return (((long) data[2] << 16) | ((long) data[1] << 8) | (long) data[0])^0x800000;
}

void Hx711::setOffset(long offset)
{
    _offset = offset;
}

void Hx711::setScale(float scale)
{
    _scale = scale;
}

float Hx711::getGram()
{
    long val = (averageValue() - _offset);
    return ((float) val / _scale);
}
