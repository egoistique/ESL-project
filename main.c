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

static volatile int click_number = 0;
static app_timer_id_t debouncing_timer;
static app_timer_id_t double_click_timer;

void double_click_timeout_handler(void* p_context) {
    click_number = 0;
}

void debounce_timer_handler(void* p_context) {
    click_number++;
    if (click_number >= 1)
    {
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

        err_code = app_timer_start(double_click_timer, APP_TIMER_TICKS(DOUBLE_CLICK_TIME_MS), 0);
        APP_ERROR_CHECK(err_code);
    }
}

int main(void) {
    configure_leds(sizeof(leds) / sizeof(*leds), leds);
    configure_button(BUTTON_PIN);

    nrfx_systick_init();
    nrfx_gpiote_init();

        
    ret_code_t err_code = app_timer_create(&debouncing_timer, APP_TIMER_MODE_SINGLE_SHOT, debounce_timer_handler);
    APP_ERROR_CHECK(err_code);
    err_code = app_timer_create(&double_click_timer, APP_TIMER_MODE_SINGLE_SHOT, double_click_timeout_handler);
    APP_ERROR_CHECK(err_code);

    nrfx_gpiote_in_config_t button_config = NRFX_GPIOTE_CONFIG_IN_SENSE_HITOLO(true);
    nrfx_gpiote_in_init(BUTTON_PIN, &button_config, gpio_handler);
    nrfx_gpiote_in_event_enable(BUTTON_PIN, true);

    while (true) {
        for (int i = 0; i < sizeof(device_id) / sizeof(*device_id); i++) {
            smooth_blink_any_times(leds[i], device_id[i], PERIOD, &blink_enable);
        }
        printf("Main loop\n");
    }
}
