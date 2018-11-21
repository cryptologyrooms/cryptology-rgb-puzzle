#ifndef _RGB_H_
#define _RGB_H_

void rgb_setup(PixelType * pFixed, PixelType * pControlled, RGBParam * pRGBFinish, uint32_t multiplier, bool nonlinear_brightness);
void rgb_tick(RGBParam * pRGBFixed[5], uint8_t const * const pVariableLevels, uint32_t multiplier, bool nonlinear_brightness);

#endif
