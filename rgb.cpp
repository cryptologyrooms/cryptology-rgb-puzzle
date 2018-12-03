/* Arduino Includes */

#if (PIXEL_TYPE == NEOPIXELS)
#include <Adafruit_NeoPixel.h>
#endif

/* ADL Includes */

#include "adl.h"

#include "binary-output.h"
#include "adl-debouncer.h"

#include "adl-oneshot-timer.h"
#include "adl-oneshot-task.h"
#include "adl-task.h"

#include "rgb-param.h"

#if (PIXEL_TYPE == PIXEL_TYPE_NEOPIXELS)

#include "adafruit-neopixel-adl.h"
typedef AdafruitNeoPixelADL PixelType;
#define PIXEL_DIVIDER 16

#elif  (PIXEL_TYPE == PIXEL_TYPE_TLC5973)

#include "TLC5973.h"
typedef TLC5973 PixelType;
#define PIXEL_DIVIDER 1

#endif

/* Application Includes */

#include "application.h"
#include "rgb.h"
#include "nonlinear.h"

/* Defines, typedefs, constants */

static const uint32_t FADER_MAX = 250;
static const uint32_t FADER_MIN = 20;

/* Local Objects and Variables */

static PixelType * s_pFixed;
static PixelType * s_pVariable;
static uint8_t const * s_pVariableLevels;
static bool s_nonlinear = false;

static RGBParam ** s_pRGBFixed;
static RGBParam *s_pRGBFinish;

static uint32_t s_multiplier = 0;
static uint8_t s_nsteps = 0;

static uint16_t s_brightness_table[8] = {0};

static uint32_t s_fader = FADER_MIN;
static bool s_fade_up = true;

static void update_fader()
{
    if (s_fade_up)
    {
        incrementwithlimit(s_fader, FADER_MAX);
        incrementwithlimit(s_fader, FADER_MAX);
        s_fade_up = (s_fader < FADER_MAX);
    }
    else
    {
        decrementwithlimit(s_fader, FADER_MIN);
        decrementwithlimit(s_fader, FADER_MIN);
        s_fade_up = (s_fader == FADER_MIN);
    }
}

static void update_fixed(RGBParam * pRGBFixed[5], uint16_t * p_brightness_table)
{
    for (uint8_t i=0; i<5; i++)
    {
        if (!app_get_rgb_matched(i))
        {
            s_pFixed->setPixelColor(
                i,
                p_brightness_table[pRGBFixed[i]->get(eR)] / PIXEL_DIVIDER,
                p_brightness_table[pRGBFixed[i]->get(eG)] / PIXEL_DIVIDER,
                p_brightness_table[pRGBFixed[i]->get(eB)] / PIXEL_DIVIDER
            );
        }
        else
        {
            s_pFixed->setPixelColor(
                i,
                (s_pRGBFinish->get(eR) * s_fader) / (FADER_MAX * PIXEL_DIVIDER),
                (s_pRGBFinish->get(eG) * s_fader) / (FADER_MAX * PIXEL_DIVIDER),
                (s_pRGBFinish->get(eB) * s_fader) / (FADER_MAX * PIXEL_DIVIDER)
            );   
        }
    }
    s_pFixed->show();
}

static void update_variable(uint8_t const * const pVariableLevels, uint16_t * p_brightness_table)
{
    for (uint8_t i=0; i<5; i++)
    {
        if (!app_get_rgb_matched(i))
        {
            s_pVariable->setPixelColor(
                i,
                p_brightness_table[pVariableLevels[(i*3)+0]] / PIXEL_DIVIDER,
                p_brightness_table[pVariableLevels[(i*3)+1]] / PIXEL_DIVIDER,
                p_brightness_table[pVariableLevels[(i*3)+2]] / PIXEL_DIVIDER
            );
        }
        else
        {
            s_pVariable->setPixelColor(
                i,
                (s_pRGBFinish->get(eR) * s_fader) / (FADER_MAX * PIXEL_DIVIDER),
                (s_pRGBFinish->get(eG) * s_fader) / (FADER_MAX * PIXEL_DIVIDER),
                (s_pRGBFinish->get(eB) * s_fader) / (FADER_MAX * PIXEL_DIVIDER)
            );   
        }
    }
    s_pVariable->show();
}

static void debug_task_fn(ADLTask& pThisTask, void * pTaskData)
{
    (void)pThisTask;
    (void)pTaskData;
    adl_logln(LOG_RGB, "Levels:");
    for (uint8_t i=0; i<5; i++)
    {
        adl_logln(LOG_RGB, "F: %d,%d,%d, V: %d,%d,%d",
            s_pRGBFixed[i]->get(eR), s_pRGBFixed[i]->get(eG), s_pRGBFixed[i]->get(eB),
            s_pVariableLevels[(i*3)+0], s_pVariableLevels[(i*3)+1], s_pVariableLevels[(i*3)+2]
        );
    }
}
static ADLTask debug_task(2000, debug_task_fn, NULL);

static void update_task_fn(ADLTask& pThisTask, void * pTaskData)
{
    (void)pThisTask;
    (void)pTaskData;
    update_fixed(s_pRGBFixed, s_brightness_table);
    update_variable(s_pVariableLevels, s_brightness_table);
    update_fader();
}
static ADLTask update_task(10, update_task_fn, NULL);

static void update_brightness_table(uint16_t * p_table, uint32_t multiplier, bool nonlinear_brightness, uint8_t nsteps)
{
    if (nonlinear_brightness)
    {
        update_nonlinear_table(p_table, multiplier, nsteps);
    }
    else
    {
        for (uint8_t i = 0; i<nsteps; i++)
        {
            p_table[i] = multiplier*i;
        }
    }
}

static bool brightness_table_requires_update(uint32_t multiplier, bool nonlinear_brightness, uint8_t nsteps)
{
    return (s_multiplier != multiplier) || (s_nonlinear != nonlinear_brightness) || (s_nsteps != nsteps);
}

/* Public Functions */

void rgb_setup(PixelType * pFixed, PixelType * pVariable, RGBParam * pRGBFinish, uint32_t multiplier, bool nonlinear_brightness, uint8_t nsteps)
{
    s_pFixed = pFixed;
    s_pVariable = pVariable;

    s_pRGBFinish = pRGBFinish;

    s_multiplier = multiplier;
    s_nonlinear = nonlinear_brightness;
    update_brightness_table(s_brightness_table, s_multiplier, nonlinear_brightness, nsteps);
}

void rgb_tick(RGBParam * pRGBFixed[5], uint8_t const * const pVariableLevels, uint32_t multiplier, bool nonlinear_brightness, uint8_t nsteps)
{
    s_pRGBFixed = pRGBFixed;
    s_pVariableLevels = pVariableLevels;

    if (brightness_table_requires_update(multiplier, nonlinear_brightness, nsteps))
    {
        s_multiplier = multiplier;
        s_nonlinear = nonlinear_brightness;
        s_nsteps = nsteps;
        update_brightness_table(s_brightness_table, s_multiplier, nonlinear_brightness, nsteps);
        Serial.println("New table:");
        for (uint8_t i; i < nsteps; i++)
        {
            Serial.print(s_brightness_table[i]);
            Serial.print(", ");
        }
    }
    
    debug_task.run();
    update_task.run();
}
