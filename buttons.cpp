/* ADL Includes */

#include "adl.h"

#include "binary-output.h"
#include "adl-debouncer.h"

#include "adl-oneshot-timer.h"
#include "adl-oneshot-task.h"
#include "adl-task.h"

/* Application Includes */
#include "buttons.h"

/* Defines and Typedefs */
static const uint8_t HC4067_PIN = 7;
static const uint8_t HC4067_POWER_PIN = 6;
static const uint8_t HC4067_GND_PIN = 12;

/* Local Classes */
class HC4067Reader : public DebounceReader
{
public:
    bool read() { return digitalRead(HC4067_PIN) == LOW;}
};

/* Local Objects and Variables */

static uint8_t s_current_button = 0;

static uint8_t s_levels[15] = {0};

static BinaryOutput * s_pButtonSelect;
static HC4067Reader s_HC4067Reader;

static ADLDebouncer s_debouncers[15] = {
    ADLDebouncer(s_HC4067Reader, 3),
    ADLDebouncer(s_HC4067Reader, 3),
    ADLDebouncer(s_HC4067Reader, 3),
    ADLDebouncer(s_HC4067Reader, 3),
    ADLDebouncer(s_HC4067Reader, 3),
    ADLDebouncer(s_HC4067Reader, 3),
    ADLDebouncer(s_HC4067Reader, 3),
    ADLDebouncer(s_HC4067Reader, 3),
    ADLDebouncer(s_HC4067Reader, 3),
    ADLDebouncer(s_HC4067Reader, 3),
    ADLDebouncer(s_HC4067Reader, 3),
    ADLDebouncer(s_HC4067Reader, 3),
    ADLDebouncer(s_HC4067Reader, 3),
    ADLDebouncer(s_HC4067Reader, 3),
    ADLDebouncer(s_HC4067Reader, 3)
};

static void debouncer_task_fn(ADLTask& pThisTask, void * pTaskData)
{
    (void)pThisTask;
    (void)pTaskData;

    s_debouncers[s_current_button].tick();

    s_current_button = s_current_button < 14 ? (s_current_button + 1) : 0;
    s_pButtonSelect->set(s_current_button);
}
static ADLTask debouncer_task(5, debouncer_task_fn, NULL);

static void debug_task_fn(ADLTask& pThisTask, void * pTaskData)
{
    (void)pThisTask;
    (void)pTaskData;

    adl_logln(LOG_BUT, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d",
        s_debouncers[0].count(),
        s_debouncers[1].count(),
        s_debouncers[2].count(),
        s_debouncers[3].count(),
        s_debouncers[4].count(),
        s_debouncers[5].count(),
        s_debouncers[6].count(),
        s_debouncers[7].count(),
        s_debouncers[8].count(),
        s_debouncers[9].count(),
        s_debouncers[10].count(),
        s_debouncers[11].count(),
        s_debouncers[12].count(),
        s_debouncers[13].count(),
        s_debouncers[14].count()
    );
}
static ADLTask debug_task(500, debug_task_fn, NULL);

/* Public Functions */

void buttons_setup(BinaryOutput * pButtonSelect)
{
    s_pButtonSelect = pButtonSelect;
    s_pButtonSelect->set(0);

    digitalWrite(HC4067_POWER_PIN, HIGH);
    digitalWrite(HC4067_GND_PIN, LOW);

    pinMode(HC4067_GND_PIN, OUTPUT);
    pinMode(HC4067_POWER_PIN, OUTPUT);

    pinMode(HC4067_PIN, INPUT_PULLUP);
}

void buttons_tick()
{
    debouncer_task.run();
    //debug_task.run();
    for(int i=0; i<15; i++)
    {
        if (s_debouncers[i].check_high_and_clear())
        {
            adl_logln(LOG_BUT, "Button %d pressed", i);
            incrementwithrollover(s_levels[i], 7);
        }
    }
}

uint8_t const * buttons_get_levels()
{
    return s_levels;
}