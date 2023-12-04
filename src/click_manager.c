#include "click_manager.h"
#include "app_timer.h"
#include "nrf_drv_clock.h"
#include "my_button.h"
#include "states.h"
#include "set_color.h"

APP_TIMER_DEF(debounce_timer);
APP_TIMER_DEF(double_click_timer);


void double_click_handler(void *context)
{
    custom_button_context_t * button = (custom_button_context_t *) context;

    if(button_te_is_pressed(BUTTON_PIN)) {
        switch (current_click_num) {
            case 1: button->button_state = SINGLE_CLICK_PRESSED; break;
            case 2: button->button_state = DOUBLE_CLICK_PRESSED; break;
        }
    }

    if(button_te_is_released(BUTTON_PIN)) {
        switch(current_click_num) {
            case 0:
                if(SINGLE_CLICK_PRESSED == button->button_state) {
                    button->button_state = SINGLE_CLICK_RELEASED;
                }
                else {
                    button->button_state = DOUBLE_CLICK_RELEASED;
                }
                break;
            case 1: button->button_state = SINGLE_CLICK_RELEASED; break;
            case 2: button->button_state = DOUBLE_CLICK_RELEASED; break;
        }
    }

    current_click_num = 0;

    application_state_handler(button);
}

void debounce_handler(void *context)
{
    if(button_te_is_pressed(BUTTON_PIN)) { 
        if(2 > current_click_num) {
            current_click_num++;
        }
    }
}

void gpio_handler(nrfx_gpiote_pin_t pin, nrf_gpiote_polarity_t action)
{
    APP_ERROR_CHECK(app_timer_stop(debounce_timer));
    APP_ERROR_CHECK(app_timer_start(debounce_timer, APP_TIMER_TICKS(DEBOUNCE_TIME_MS), (void *)&main_button));
    APP_ERROR_CHECK(app_timer_start(double_click_timer, APP_TIMER_TICKS(DOUBLE_CLICK_TIME_MS), (void *)&main_button));
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