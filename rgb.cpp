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

static RGBParam ** s_pRGBFixed;

static void update_fixed(RGBParam * pRGBFixed[5], uint32_t multiplier)
{
    for (uint8_t i=0; i<5; i++)
    {
        s_pFixed->pixels().setPixelColor(
            i,
            (pRGBFixed[i]->get(eR) * multiplier) / PIXEL_TYPE,
            (pRGBFixed[i]->get(eG) * multiplier) / PIXEL_TYPE,
            (pRGBFixed[i]->get(eB) * multiplier) / PIXEL_TYPE
        );
    }
    s_pFixed->pixels().show();
}

static void update_variable(uint8_t const * const pVariableLevels, uint32_t multiplier)
{
    for (uint8_t i=0; i<5; i++)
    {
        s_pVariable->pixels().setPixelColor(
            i,
            (multiplier * pVariableLevels[i*3]) / PIXEL_TYPE,
            (multiplier * pVariableLevels[(i*3)+1]) / PIXEL_TYPE,
            (multiplier * pVariableLevels[(i*3)+2]) / PIXEL_TYPE
        );
    }
    s_pVariable->pixels().show();
}

static void debug_task_fn(ADLTask& pThisTask, void * pTaskData)
{
    (void)pThisTask;
    (void)pTaskData;
    for (uint8_t i=0; i<5; i++)
    {
        adl_logln(LOG_RGB, "%d,%d,%d", s_pRGBFixed[i]->get(eR), s_pRGBFixed[i]->get(eG), s_pRGBFixed[i]->get(eB));
    }
}
static ADLTask debug_task(2000, debug_task_fn, NULL);

/* Public Functions */

void rgb_setup(AdafruitNeoPixelADL * pFixed, AdafruitNeoPixelADL * pVariable)
{
    s_pFixed = pFixed;
    s_pVariable = pVariable;
}

void rgb_tick(RGBParam * pRGBFixed[5], uint8_t const * const pVariableLevels, uint32_t multiplier)
{
    s_pRGBFixed = pRGBFixed;
    debug_task.run();
    update_fixed(pRGBFixed, multiplier);
    update_variable(pVariableLevels, multiplier);
}
