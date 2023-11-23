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
#include "nrfx_pwm.h"
#include "nrf_drv_clock.h"
#include "nrfx_glue.h"

#define DEBOUNCE_TIME_MS 10
#define DOUBLE_CLICK_TIME_MS 1000

static const unsigned int device_id[] = {1, 1, 8, 1};
static const int32_t leds[] = LEDS;
static volatile bool blink_enable = true;

static volatile int click_number = 0;

APP_TIMER_DEF(debouncing_timer);
APP_TIMER_DEF(double_click_timer);

void double_click_timeout_handler(void* p_context) {
    click_number = 0;
//    blink(0);
}

void debounce_timer_handler(void* p_context) {
    ret_code_t err_code = app_timer_start(double_click_timer, APP_TIMER_TICKS(DOUBLE_CLICK_TIME_MS), 0);
    APP_ERROR_CHECK(err_code);
    click_number++;
    
 //   blink(1);

    if (click_number == 2)
    {
       // blink(2);
        blink_enable = !blink_enable; 
        click_number = 0;
    }
}

void gpio_handler(nrfx_gpiote_pin_t pin, nrf_gpiote_polarity_t action) {   
    if (action == NRF_GPIOTE_POLARITY_HITOLO) 
    {
        app_timer_stop(debouncing_timer);
        
        ret_code_t err_code = app_timer_start(debouncing_timer, APP_TIMER_TICKS(DEBOUNCE_TIME_MS), 0);
        APP_ERROR_CHECK(err_code);

        // err_code = app_timer_start(double_click_timer, APP_TIMER_TICKS(DOUBLE_CLICK_TIME_MS), 0);
        // APP_ERROR_CHECK(err_code);

        //blink(3);
    }
}

void lfclk_request(void)
{
    ret_code_t err_code = nrf_drv_clock_init();
    APP_ERROR_CHECK(err_code);
    nrf_drv_clock_lfclk_request(NULL);

    while (!nrf_drv_clock_lfclk_is_running())
    {
        /* Just waiting until the lf clock starts up */
    }
}

void pwm_init(void) {
    nrfx_pwm_config_t pwm_config = NRFX_PWM_DEFAULT_CONFIG;
    nrfx_pwm_t pwm_instance = NRFX_PWM_INSTANCE(0);
    nrfx_pwm_init(&pwm_instance, &pwm_config, NULL);
}


int main(void) {
    configure_leds(sizeof(leds) / sizeof(*leds), leds);
    configure_button(BUTTON_PIN);

    lfclk_request();

    nrfx_systick_init();
    nrfx_gpiote_init();
    app_timer_init();
        
    ret_code_t err_code = app_timer_create(&debouncing_timer, APP_TIMER_MODE_SINGLE_SHOT, debounce_timer_handler);
    APP_ERROR_CHECK(err_code);
    err_code = app_timer_create(&double_click_timer, APP_TIMER_MODE_SINGLE_SHOT, double_click_timeout_handler);
    APP_ERROR_CHECK(err_code);

    nrfx_gpiote_in_config_t button_config = NRFX_GPIOTE_CONFIG_IN_SENSE_HITOLO(true);
    nrfx_gpiote_in_init(BUTTON_PIN, &button_config, gpio_handler);
    nrfx_gpiote_in_event_enable(BUTTON_PIN, true);

    pwm_init();

    while (true) {
        for (int i = 0; i < sizeof(device_id) / sizeof(*device_id); i++) {
           smooth_blink_any_times(leds[i], device_id[i], PERIOD, &blink_enable);
        }
    }
}