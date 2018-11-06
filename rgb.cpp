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
    s_pFixed->pixels().show();
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

void rgb_tick(RGBParam * pRGBFixed[5])
{
    s_pRGBFixed = pRGBFixed;
    debug_task.run();
    update_fixed(pRGBFixed);
}
