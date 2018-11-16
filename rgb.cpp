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

#include "rgb.h"

/* Local Objects and Variables */

static PixelType * s_pFixed;
static PixelType * s_pVariable;
static uint8_t const * s_pVariableLevels;
static bool s_log_mode = false;
static RGBParam ** s_pRGBFixed;

static uint32_t s_multiplier = 0;
static uint32_t s_brightness_table[8] = {0};

static void update_fixed(RGBParam * pRGBFixed[5], uint32_t * p_brightness_table)
{
    for (uint8_t i=0; i<5; i++)
    {
        s_pFixed->setPixelColor(
            i,
            p_brightness_table[pRGBFixed[i]->get(eR)] / PIXEL_DIVIDER,
            p_brightness_table[pRGBFixed[i]->get(eG)] / PIXEL_DIVIDER,
            p_brightness_table[pRGBFixed[i]->get(eB)] / PIXEL_DIVIDER
        );
    }
    s_pFixed->show();
}

static void update_variable(uint8_t const * const pVariableLevels, uint32_t * p_brightness_table)
{
    for (uint8_t i=0; i<5; i++)
    {
        s_pVariable->setPixelColor(
            i,
            p_brightness_table[pVariableLevels[(i*3)+0]] / PIXEL_DIVIDER,
            p_brightness_table[pVariableLevels[(i*3)+1]] / PIXEL_DIVIDER,
            p_brightness_table[pVariableLevels[(i*3)+2]] / PIXEL_DIVIDER
        );
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
}
static ADLTask update_task(50, update_task_fn, NULL);

static void update_brightness_table(uint32_t * p_table, bool log_mode, uint32_t multiplier)
{
    if (log_mode)
    {
        uint32_t maximum = 7*multiplier;
        p_table[0] = (maximum * 0) / 1000;
        p_table[1] = (maximum * 20) / 1000;
        p_table[2] = (maximum * 82) / 1000;
        p_table[3] = (maximum * 184) / 1000;
        p_table[4] = (maximum * 327) / 1000;
        p_table[5] = (maximum * 510) / 1000;
        p_table[6] = (maximum * 735) / 1000;
        p_table[7] = maximum;
    }
    else
    {
        for (uint8_t i = 0; i<8; i++)
        {
            p_table[i] = multiplier*i;
        }
    }
}

/* Public Functions */

void rgb_setup(PixelType * pFixed, PixelType * pVariable)
{
    s_pFixed = pFixed;
    s_pVariable = pVariable;
}

void rgb_tick(RGBParam * pRGBFixed[5], uint8_t const * const pVariableLevels, uint32_t multiplier, bool log_mode)
{
    s_pRGBFixed = pRGBFixed;
    s_pVariableLevels = pVariableLevels;

    if (s_multiplier != multiplier)
    {
        s_multiplier = multiplier;
        update_brightness_table(s_brightness_table, log_mode, s_multiplier);
    }
    
    debug_task.run();
    update_task.run();
}
