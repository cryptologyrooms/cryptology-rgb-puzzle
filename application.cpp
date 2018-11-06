/* Arduino Includes */

#include <Adafruit_NeoPixel.h>

/* ADL Includes */

#include "adl.h"

#include "adafruit-neopixel-adl.h"
#include "binary-output.h"

#include "adl-oneshot-timer.h"
#include "adl-oneshot-task.h"
#include "adl-task.h"

#include "integer-param.h"
#include "rgb-param.h"

/* Application Includes */

#include "buttons.h"
#include "rgb.h"

/* Private Functions */

static void my_task_fn(ADLTask& pThisTask, void * pTaskData)
{
    (void)pThisTask;
    (void)pTaskData;
}
static ADLTask my_task(5000, my_task_fn, NULL);

/* ADL Functions */

void adl_custom_setup(DeviceBase * pdevices[], int ndevices, ParameterBase * pparams[], int nparams)
{
    (void)ndevices; (void)pparams; (void)nparams;

    rgb_setup((AdafruitNeoPixelADL*)(pdevices[0]), (AdafruitNeoPixelADL*)(pdevices[1]));
    buttons_setup((BinaryOutput*)(pdevices[2]));

    my_task.start();
}

void adl_custom_loop(DeviceBase * pdevices[], int ndevices, ParameterBase * pparams[], int nparams)
{
    (void)pdevices; (void)ndevices; (void)nparams;
    my_task.run();

    buttons_tick();
    rgb_tick((RGBParam**)pparams, buttons_get_levels(), ((IntegerParam*)pparams[5])->get());
}
