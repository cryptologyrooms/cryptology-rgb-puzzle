/* Arduino Includes */

#include <Arduino.h>
#include "raat-defs.h"

#if (PIXEL_TYPE == PIXEL_TYPE_NEOPIXELS)
#include <Adafruit_NeoPixel.h>
#endif

/* ADL Includes */

#include "raat.h"

#include "binary-output.h"

#include "raat-oneshot-timer.h"
#include "raat-oneshot-task.h"
#include "raat-task.h"

#include "boolean-param.h"
#include "integer-param.h"
#include "rgb-param.h"

/* Application Includes */

#include "application.h"
#include "buttons.h"
#include "rgb.h"

/* Defines, typedefs, constants */

static const uint8_t RELAY_PIN = 2;

/* Local Variables */
static bool s_game_running = true;
static bool s_on_reset_zero_all;
static bool s_matched [5] = {false};

/* Private Functions */

static void my_task_fn(RAATTask& pThisTask, void * pTaskData)
{
    (void)pThisTask;
    (void)pTaskData;
}
static RAATTask my_task(5000, my_task_fn, NULL);

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

static void play_intro(PixelType * pFixed, PixelType * pVariable)
{           
    const uint16_t intens = 2048 / PIXEL_DIVIDER;
     
    for (uint8_t light = 0; light < 5; light++)
    {
        for (uint8_t rgb = 0; rgb < 3; rgb++)
        {
            pFixed->setPixelColor(light, rgb==0?intens:0, rgb==1?intens:0, rgb==2?intens:0);
            pFixed->show();
            pVariable->setPixelColor(light, rgb==0?intens:0, rgb==1?intens:0, rgb==2?intens:0);
            pVariable->show();
            delay(500);
            pFixed->setPixelColor(light, 0,0,0);
            pFixed->show();
            pVariable->setPixelColor(light, 0,0,0);
            pVariable->show();
        }
    }
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

/* RAAT Functions */

void raat_custom_setup(const raat_devices_struct& devices, const raat_params_struct& params)
{

    int32_t maximum = (int32_t)params.pMaximum->get();
    bool nonlinear_brightness = params.pBrightnessBehaviour->get();
    uint8_t nsteps = params.pNumSteps->get();

    if (nsteps < 1)
    {
        nsteps = 5;
        params.pNumSteps->set(nsteps);
    }

    play_intro(devices.pFixed_LEDs, devices.pVariable_LEDs);

    rgb_setup(devices.pFixed_LEDs, devices.pVariable_LEDs, params.pFinishRGB, maximum, nonlinear_brightness, nsteps);
    buttons_setup(devices.pBinary_Output);

    pinMode(RELAY_PIN, OUTPUT);

    my_task.start();

    params.pFakeButtonPress->set(0xFFFF);

    /*Serial.println("Settings:");
    Serial.print("Reset ");
    if (params.pResetBehaviour->get())
    {
        Serial.println("all");
    }
    else
    {
        Serial.println("matched");
    }

    if (params.pBrightnessBehaviour->get())
    {
        Serial.print("Non");
    }
    Serial.println("linear");*/
}

void raat_custom_loop(const raat_devices_struct& devices, const raat_params_struct& params)
{
    (void)devices;
    my_task.run();

    int32_t maximum = params.pMaximum->get();

    s_on_reset_zero_all = params.pResetBehaviour->get();
    
    bool nonlinear_brightness = params.pBrightnessBehaviour->get();

    uint8_t nsteps = params.pNumSteps->get();
    
    int32_t fake_button = params.pFakeButtonPress->get();

    if (s_game_running)
    {
        buttons_tick(fake_button, nsteps-1);
    }

    RGBParam* pRGBParams[5] = 
    {
        params.pRGB0,
        params.pRGB1,
        params.pRGB2,
        params.pRGB3,
        params.pRGB4
    };

    rgb_tick(
        pRGBParams,
        buttons_get_levels(),
        maximum,
        nonlinear_brightness,
        nsteps
    );

    s_game_running = !match_lights(pRGBParams, buttons_get_levels());
    digitalWrite(RELAY_PIN, s_game_running ? LOW : HIGH);

    params.pFakeButtonPress->set(0xFFFF);
}
