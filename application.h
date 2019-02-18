#ifndef _APPLICATION_H_
#define _APPLICATION_H_

#if (PIXEL_TYPE == PIXEL_TYPE_NEOPIXELS)

#include "adafruit-neopixel-raat.h"
typedef AdafruitNeoPixelADL PixelType;
#define PIXEL_DIVIDER 16

#elif  (PIXEL_TYPE == PIXEL_TYPE_TLC5973)

#include "TLC5973.h"
typedef TLC5973 PixelType;
#define PIXEL_DIVIDER 1

#endif

void app_reset_rgb();
bool app_get_rgb_matched(uint8_t i);

#endif
