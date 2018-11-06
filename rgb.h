#ifndef _RGB_H_
#define _RGB_H_

void rgb_setup(AdafruitNeoPixelADL * pFixed, AdafruitNeoPixelADL * pControlled);
void rgb_tick(RGBParam * pRGBFixed[5], uint8_t const * const pVariableLevels, uint32_t multiplier);

#endif