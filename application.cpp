/* Arduino Includes */

#if (PIXEL_TYPE == NEOPIXELS)
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

#include "integer-param.h"
#include "rgb-param.h"

/* Application Includes */

#include "buttons.h"
#include "rgb.h"

/* Defines, typedefs, constants */

static const uint8_t RELAY_PIN = 2;
static bool s_game_running = true;

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
    for(uint8_t i=0; i<5; i++)
    {
        for(uint8_t j=0; j<3; j++)
        {
            if (pRGBFixed[i]->get((eRGB)j) != pVariableLevels[(i*3)+j])
            {
                return false;
            }
        }
    }

    return true;
}

/* ADL Functions */

void adl_custom_setup(DeviceBase * pdevices[], int ndevices, ParameterBase * pparams[], int nparams)
{
    (void)ndevices; (void)pparams; (void)nparams;

    rgb_setup((PixelType*)(pdevices[0]), (PixelType*)(pdevices[1]));
    buttons_setup((BinaryOutput*)(pdevices[2]));

    pinMode(RELAY_PIN, OUTPUT);

    my_task.start();
}

void adl_custom_loop(DeviceBase * pdevices[], int ndevices, ParameterBase * pparams[], int nparams)
{
    (void)pdevices; (void)ndevices; (void)nparams;
    my_task.run();

    if (s_game_running)
    {
        buttons_tick();
    }
        rgb_tick((RGBParam**)pparams, buttons_get_levels(), ((IntegerParam*)pparams[5])->get());

    s_game_running = !match_lights((RGBParam**)pparams, buttons_get_levels());
    digitalWrite(RELAY_PIN, s_game_running ? LOW : HIGH);
}
