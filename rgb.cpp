/* Arduino Includes */

#include <Adafruit_NeoPixel.h>

/* ADL Includes */

#include "adl.h"

#include "binary-output.h"
#include "adl-debouncer.h"

#include "adl-oneshot-timer.h"
#include "adl-oneshot-task.h"
#include "adl-task.h"

#include "rgb-param.h"
#include "adafruit-neopixel-adl.h"

/* Application Includes */

#include "rgb.h"

/* Defines and Typedefs */

#define NEOPIXELS 16
#define TLC5973 1

#define PIXEL_TYPE NEOPIXELS

/* Local Classes */

/* Local Objects and Variables */
static AdafruitNeoPixelADL * s_pFixed;
static AdafruitNeoPixelADL * s_pVariable;

static void update_fixed(RGBParam * pRGBFixed[5])
{
    for (uint8_t i=0; i<5; i++)
    {
        s_pFixed->pixels().setPixelColor(
            i,
            pRGBFixed[i]->get(eR) / PIXEL_TYPE,
            pRGBFixed[i]->get(eG) / PIXEL_TYPE,
            pRGBFixed[i]->get(eB) / PIXEL_TYPE
        );
    }
}

/* Public Functions */

void rgb_setup(AdafruitNeoPixelADL * pFixed, AdafruitNeoPixelADL * pVariable)
{
    s_pFixed = pFixed;
    s_pVariable = pVariable;
}

void rgb_tick(RGBParam * pRGBFixed[5])
{
    update_fixed(pRGBFixed);
}
