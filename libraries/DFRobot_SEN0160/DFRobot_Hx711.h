/* Arduino library for digital weight scale of hx711
 *
 * hardware design: syyyd
 * available at http://syyyd.taobao.com
 *
 * library design: Weihong Guan (@aguegu)
 * http://aguegu.net
 *
 * library host on
 * https://github.com/aguegu/Arduino
 *
 *  Created on: Oct 31, 2012
 */

#ifndef HX711_H_
#define HX711_H_

#include "Arduino.h"
typedef unsigned char BYTE;
class Hx711
{
public:
    Hx711();
    virtual ~Hx711();
    long getValue();
    long averageValue(BYTE times = 25);
    void setOffset(long offset);
    void setScale(float scale = 1992.f);
    float getGram();
    void begin(uint8_t pin_dout, uint8_t pin_slk);
private:
    uint8_t _pin_dout;
    uint8_t _pin_slk;
    long _offset;
    float _scale;
};

#endif /* HX711_H_ */
