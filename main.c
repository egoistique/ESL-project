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

#define PERIOD 1000
#define DOUBLE_CLICK_TIME_MS 5

static const unsigned int device_id[] = {6, 5, 8, 1};
static const int32_t leds[] = LEDS;
static volatile bool blink_enable = true;
static volatile bool button_clicked = false;
static app_timer_id_t double_click_timer;

void wait_microseconds(int32_t us) {
    nrf_delay_us(us);
}

void smooth_blink(int32_t led, int32_t duration_ms, volatile bool * enable)
{
    int32_t num_steps = (duration_ms * 1000) / (2 * PERIOD); 
    int32_t step = PERIOD / num_steps; 
    int32_t current_duty_cycle = 0; 

    bool direction = true;

    nrfx_systick_state_t state;

    while (true) {
        if (*enable) {
            if (direction && (current_duty_cycle >= PERIOD)) {
                direction = false;
            }

            current_duty_cycle += direction ? step : -step;

            if (current_duty_cycle <= 0) {
                break;
            }
        }

        if (current_duty_cycle > 0) {
            led_on(led);
            nrfx_systick_get(&state);
            wait_microseconds(current_duty_cycle);
            led_off(led);
            nrfx_systick_get(&state);
            wait_microseconds(PERIOD - current_duty_cycle);
        }
    }
}

void smooth_blink_any_times(int32_t led , int32_t num, int32_t period_ms, volatile bool *enable)
{
    for(unsigned int i = 0; i < num; i++){
        smooth_blink(led, period_ms, enable);
        nrf_delay_ms(period_ms);
    }
}

void double_click_timeout_handler(void* p_context)
{
    button_clicked = false;
}

void button_handler(nrfx_gpiote_pin_t pin, nrf_gpiote_polarity_t action)
{
    if (action == NRF_GPIOTE_POLARITY_HITOLO)
    {
        if (!button_clicked)
        {
            button_clicked = true;
            ret_code_t err_code = app_timer_start(double_click_timer, APP_TIMER_TICKS(DOUBLE_CLICK_TIME_MS), NULL);
            APP_ERROR_CHECK(err_code);
        }
        else
        {
            blink_enable = !blink_enable; 
            app_timer_stop(double_click_timer); 
            button_clicked = false; 
        }
    }
}

int main(void)
{
    configure_leds(sizeof(leds)/sizeof(*leds), leds);
    all_leds_off(sizeof(leds)/sizeof(*leds), leds);
    configure_button(BUTTON_PIN);

    nrfx_systick_init();
    nrfx_gpiote_init();

    ret_code_t err_code = app_timer_create(&double_click_timer, APP_TIMER_MODE_SINGLE_SHOT, double_click_timeout_handler);
    APP_ERROR_CHECK(err_code);

    nrfx_gpiote_in_config_t button_config = NRFX_GPIOTE_CONFIG_IN_SENSE_HITOLO(true);
    nrfx_gpiote_in_init(BUTTON_PIN, &button_config, button_handler);
    nrfx_gpiote_in_event_enable(BUTTON_PIN, true);

    while(true) {
        for (int i = 0; i < sizeof(device_id)/sizeof(*device_id); i++)
        {
            smooth_blink_any_times(leds[i], device_id[i], PERIOD, &blink_enable);
        }
    }
}
