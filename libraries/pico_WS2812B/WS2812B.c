#include "pico/stdlib.h"
#include "WS2812B.h"
//timings for high sequence, set up at 125Mhz
void WS2812B_send1()
{
    int on = 21;
    int off = 17;
    gpio_put_masked(1<<WS2812B_pin,1);
    while(on--) 
    {
        asm("mov r0,r0");
    }
    gpio_put_masked(1<<WS2812B_pin,0);
    while(off--) 
    {
        asm("mov r0,r0");
        asm("mov r0,r0");
        asm("mov r0,r0");
    }
}
//timings for low sequence, set up at 125Mhz
void WS2812B_send0()
{
    int on = 17;
    int off = 21;
    gpio_put_masked(1<<WS2812B_pin,1);
    while(on--) 
    {
        asm("mov r0,r0");
        asm("mov r0,r0");
        asm("mov r0,r0");
    }
    gpio_put_masked(1<<WS2812B_pin,0);
    while(off--) 
    {
        asm("mov r0,r0");
    }
}

void WS2812B_sendColour(uint32_t colour)
{
    uint8_t bit=0;
    while(bit!=24)
    {
        if(colour&(1<<bit))
            WS2812B_send1();
        else
            WS2812B_send0();
        bit++;
    }
}

void WS2812B_fillColour(uint32_t colour)
{
    for(uint8_t led=0;led<WS2812B_leds;led++)
    {
        WS2812B_sendColour(colour);
    }
}

void WS2812B_sendArray(uint32_t* arr, uint8_t cnt)
{
    if(cnt>WS2812B_leds)
        cnt = WS2812B_leds;
    uint32_t curr_colour = 0;
    while(cnt--)
    {
        curr_colour = *arr++;
        WS2812B_sendColour(curr_colour);
    }
}
void WS2812B_reset()
{
    gpio_clr_mask(1<<WS2812B_pin);
    sleep_us(100);
}
