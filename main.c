#include <stdbool.h>
#include <stdint.h>
#include "boards.h"
#include "nrf_delay.h"
#include "nrf_gpio.h"
#include "nrfx_systick.h"
#include "nrfx_gpiote.h"
#include "app_timer.h"
#include "my_leds.h"
#include "my_button.h"
#include "my_blink.h"

#define DEBOUNCE_TIME_MS 10
#define DOUBLE_CLICK_TIME_MS 2000

static const unsigned int device_id[] = {1, 5, 8, 1};
static const int32_t leds[] = LEDS;
static volatile bool blink_enable = true;
static volatile bool button_clicked = false;
static volatile unsigned int click_num = 0;
static volatile bool debounce_in_progress = false;

static app_timer_id_t debounce_timer;
static app_timer_id_t double_click_timer;


void double_click_timeout_handler(void* p_context)
{
    if (click_num == 2) {
        blink_enable = !blink_enable;
        click_num = 0;
    }
}


void debounce_timer_handler(void* p_context)
{
    debounce_in_progress = false;
}


void debounce_handler(nrfx_gpiote_pin_t pin, nrf_gpiote_polarity_t action)
{
    if (action == NRF_GPIOTE_POLARITY_HITOLO)
    {
        if (debounce_in_progress)
        {
            ret_code_t err_code = app_timer_start(debounce_timer, APP_TIMER_TICKS(DEBOUNCE_TIME_MS), NULL);
            APP_ERROR_CHECK(err_code);
        }
        else
        {
            click_num++;
            debounce_in_progress = true;
            ret_code_t err_code = app_timer_start(debounce_timer, APP_TIMER_TICKS(DEBOUNCE_TIME_MS), NULL);
            err_code = app_timer_start(double_click_timer, APP_TIMER_TICKS(DOUBLE_CLICK_TIME_MS), NULL);
            APP_ERROR_CHECK(err_code);

        }

    }
}


int main(void)
{
    configure_leds(sizeof(leds)/sizeof(*leds), leds);
   
    configure_button(BUTTON_PIN);

    nrfx_systick_init();
    nrfx_gpiote_init();


    ret_code_t err_code = app_timer_create(&debounce_timer, APP_TIMER_MODE_SINGLE_SHOT, debounce_timer_handler);
    APP_ERROR_CHECK(err_code);

    err_code = app_timer_create(&double_click_timer, APP_TIMER_MODE_SINGLE_SHOT, double_click_timeout_handler);
    APP_ERROR_CHECK(err_code);


    nrfx_gpiote_in_config_t button_config = NRFX_GPIOTE_CONFIG_IN_SENSE_HITOLO(true);
    nrfx_gpiote_in_init(BUTTON_PIN, &button_config, debounce_handler);
    nrfx_gpiote_in_event_enable(BUTTON_PIN, true);


    while(true) {
        for (int i = 0; i < sizeof(device_id)/sizeof(*device_id); i++)
        {
            smooth_blink_any_times(leds[i], device_id[i], PERIOD, &blink_enable);
        }
    }
}