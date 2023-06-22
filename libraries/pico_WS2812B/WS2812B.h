#pragma once
//GPIO Pin to which data line of WS2812B leds is connected
#define WS2812B_pin 0
//amount of WS2812B leds 
#define WS2812B_leds 8
//macro to get 24 bit combined colour from 3 separate r g b values
#define WS2812B_24bit(r,g,b) (uint32_t)((g&0x00FF)|((r&0x00FF)<<8)|((b&0x00FF)<<16))

//send colour for 1 led
void WS2812B_sendColour(uint32_t colour);
//fill all leds with the same colour
void WS2812B_fillColour(uint32_t colour);
//fill leds with colours in array
void WS2812B_sendArray(uint32_t* arr, uint8_t cnt);
//reset sequence for leds
void WS2812B_reset();