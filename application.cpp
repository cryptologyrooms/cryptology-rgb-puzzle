/* Arduino Includes */

#include <Arduino.h>
#include "adl-defs.h"

#if (PIXEL_TYPE == PIXEL_TYPE_NEOPIXELS)
#include <Adafruit_NeoPixel.h>
#endif

/* ADL Includes */

#include "adl.h"

#if (PIXEL_TYPE == PIXEL_TYPE_NEOPIXELS)
#include "adafruit-neopixel-adl.h"
typedef AdafruitNeoPixelADL PixelType;
#elif  (PIXEL_TYPE == PIXEL_TYPE_TLC5973)
#include "TLC5973.h"
typedef TLC5973 PixelType;
#endif

#include "binary-output.h"

#include "adl-oneshot-timer.h"
#include "adl-oneshot-task.h"
#include "adl-task.h"

#include "boolean-param.h"
#include "integer-param.h"
#include "rgb-param.h"

/* Application Includes */

#include "buttons.h"
#include "rgb.h"

/* Defines, typedefs, constants */

static const uint8_t RELAY_PIN = 2;

/* Local Variables */
static bool s_game_running = true;
static bool s_on_reset_zero_all;
static bool s_matched [5] = {false};
static IntegerParam* s_pFakeButtonParam = NULL;

/* Private Functions */

static void my_task_fn(ADLTask& pThisTask, void * pTaskData)
{
    (void)pThisTask;
    (void)pTaskData;
}
static ADLTask my_task(5000, my_task_fn, NULL);

/* Local Functions */

static bool match_lights(RGBParam * pRGBFixed[5], uint8_t const * const pVariableLevels)
{
    bool all_matched = true;
    for(uint8_t i=0; i<5; i++)
    {
        s_matched[i] = true;
        for(uint8_t j=0; j<3; j++)
        {
            if (pRGBFixed[i]->get((eRGB)j) != pVariableLevels[(i*3)+j])
            {
                s_matched[i] = false;
                all_matched = false;
            }
        }
    }

    return all_matched;
}

void app_reset_rgb()
{
    for (uint8_t i=0; i<5;i++)
    {
        if (s_on_reset_zero_all || !s_matched[i])
        {
            buttons_reset_level(i);
        }
    }
}

bool app_get_rgb_matched(uint8_t i)
{
    return s_matched[i];
}

/* ADL Functions */

void adl_custom_setup(DeviceBase * pdevices[], int ndevices, ParameterBase * pparams[], int nparams)
{
    (void)ndevices; (void)pparams; (void)nparams;
    
    int32_t maximum = (int32_t)((IntegerParam*)pparams[5])->get();
    RGBParam * pRGBFinish = (RGBParam*)pparams[6];
    bool nonlinear_brightness = ((BooleanParam*)pparams[8])->get();
    uint8_t nsteps = (uint8_t)((IntegerParam*)pparams[9])->get();

    if (nsteps < 1)
    {
        nsteps = 5;
        ((IntegerParam*)pparams[9])->set(nsteps);
    }

    s_pFakeButtonParam = (IntegerParam*)pparams[10];

    rgb_setup((PixelType*)(pdevices[0]), (PixelType*)(pdevices[1]), pRGBFinish, maximum, nonlinear_brightness, nsteps);
    buttons_setup((BinaryOutput*)(pdevices[2]));

    pinMode(RELAY_PIN, OUTPUT);

    my_task.start();

    s_pFakeButtonParam->set(0xFFFF);
}

void adl_custom_loop(DeviceBase * pdevices[], int ndevices, ParameterBase * pparams[], int nparams)
{
    (void)pdevices; (void)ndevices; (void)nparams;
    my_task.run();

    int32_t maximum = (int32_t)((IntegerParam*)pparams[5])->get();

    s_on_reset_zero_all = ((BooleanParam*)pparams[7])->get();
    
    bool nonlinear_brightness = ((BooleanParam*)pparams[8])->get();

    uint8_t nsteps = (uint8_t)((IntegerParam*)pparams[9])->get();
    
    int32_t fake_button = s_pFakeButtonParam->get();

    if (s_game_running)
    {
        buttons_tick(fake_button, nsteps-1);
    }

    rgb_tick(
        (RGBParam**)pparams,
        buttons_get_levels(),
        maximum,
        nonlinear_brightness,
        nsteps
    );

    s_game_running = !match_lights((RGBParam**)pparams, buttons_get_levels());
    digitalWrite(RELAY_PIN, s_game_running ? LOW : HIGH);

    s_pFakeButtonParam->set(0xFFFF);
}
