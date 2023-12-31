#include "click_manager.h"
#include "app_timer.h"
#include "nrf_drv_clock.h"
#include "my_button.h"
#include "states.h"
#include "set_color.h"

APP_TIMER_DEF(debounce_timer);
APP_TIMER_DEF(double_click_timer);

bool double_click_after_deounce = false;
volatile unsigned int click_number = 0;

void double_click_handler(void *context)
{
    if(button_pressed(BUTTON_PIN)) {
        if(click_number == 1) {
            button_state = LONG_CLICK;
        }
    } else {
         if(LONG_CLICK == button_state) {
            button_state = DEFAULT;
         }else if (double_click_after_deounce) {
            button_state = DOUBLE_CLICK;
        }
    }

    click_number = 0;
    double_click_after_deounce = false;
    set_mode();
}

void debounce_handler(void *context)
{
    if(button_pressed(BUTTON_PIN)) { 
        click_number++;
    }

    if(click_number == 2) {
        double_click_after_deounce = true;
        click_number = 0;
    }
}

void gpio_handler(nrfx_gpiote_pin_t pin, nrf_gpiote_polarity_t action)
{
    APP_ERROR_CHECK(app_timer_stop(debounce_timer));
    APP_ERROR_CHECK(app_timer_start(debounce_timer, APP_TIMER_TICKS(DEBOUNCE_TIME_MS),NULL));
    APP_ERROR_CHECK(app_timer_start(double_click_timer, APP_TIMER_TICKS(DOUBLE_CLICK_TIME_MS), NULL));
}


void timers_init(void)
{
   nrfx_gpiote_init();

    APP_ERROR_CHECK(app_timer_init());
    APP_ERROR_CHECK(app_timer_create(&debounce_timer, APP_TIMER_MODE_SINGLE_SHOT, debounce_handler));
    APP_ERROR_CHECK(app_timer_create(&double_click_timer, APP_TIMER_MODE_SINGLE_SHOT, double_click_handler));

    nrfx_gpiote_in_config_t user_gpiote_cfg = NRFX_GPIOTE_CONFIG_IN_SENSE_TOGGLE(false);
    user_gpiote_cfg.pull = NRF_GPIO_PIN_PULLUP;
    APP_ERROR_CHECK(nrfx_gpiote_in_init(BUTTON_PIN, &user_gpiote_cfg, gpio_handler));
    nrfx_gpiote_in_event_enable(BUTTON_PIN, true);

}