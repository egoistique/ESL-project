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
#include "hsv.h"
#include "pwm.h"
#include "states.h"
//#include "set_color.h"

static const int32_t leds[] = LEDS;

APP_TIMER_DEF(debounce_timer);
APP_TIMER_DEF(double_click_timer);

struct hsv_control_state hsv_ctrl_state_ctx = { .button = &main_button };

void process_hsv_state(struct hsv *color, struct hsv_control_state *state)
{
    if(SINGLE_CLICK_PRESSED == state->button->button_state) {
        switch(app_state) {
            case HUE_MODE:
                set_hue(color);
                break;
            case SATURATION_MODE:
                if(state->saturation_direction) {
                    if(MAX_SATURATION <= ++color->saturation) {
                        state->saturation_direction = false;
                    }
                }
                else {
                    if(0 >= --color->saturation) {
                        state->saturation_direction = true;
                    }
                }
                
                break;
            case VALUE_MODE: 
                if(state->brightness_direction) {
                    if(MAX_SATURATION <= ++color->brightness) {
                        state->brightness_direction = false;
                    }
                }
                else {
                    if(0 >= --color->brightness) {
                        state->brightness_direction = true;
                    }
                }
                break;
            default:
                break;
        }
    }

    union rgb the_color = { .components = {0} };

    hsv_to_rgb(hsv_color, &the_color);

    pwm_values.channel_1 = the_color.red;
    pwm_values.channel_2 = the_color.green;
    pwm_values.channel_3 = the_color.blue;
}


void custom_pwm_handler(nrfx_pwm_evt_type_t event_type)
{
    static bool status_indicator_direction = true;

    if(status_indicator_step) {
        if(status_indicator_direction) {
            if(1200 >= (pwm_values.channel_0 + status_indicator_step)) {
                pwm_values.channel_0 += status_indicator_step;
            }
            else {
                pwm_values.channel_0 = 1200; 
                status_indicator_direction = false;
            }
        }
        else {
            if(0 <= (pwm_values.channel_0 - status_indicator_step)) {
                pwm_values.channel_0 -= status_indicator_step;
            }
            else {
                pwm_values.channel_0 = 0;  
                status_indicator_direction = true;
            }
        }
    }
    process_hsv_state(&hsv_color, &hsv_ctrl_state_ctx);
}

void double_click_handler(void *context)
{
    custom_button_context_t * button = (custom_button_context_t *) context;

    if(button_te_is_pressed(button->pin)) {
        switch (button->current_click_num) {
            case 1: button->button_state = SINGLE_CLICK_PRESSED; break;
            case 2: button->button_state = DOUBLE_CLICK_PRESSED; break;
        }
    }

    if(button_te_is_released(button->pin)) {
        switch(button->current_click_num) {
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

    button->current_click_num = 0;

    application_state_handler(button);
}

void debounce_handler(void *context)
{
    custom_button_context_t * button = (custom_button_context_t *) context;
    if(button_te_is_pressed(button->pin)) { 
        if(2 > button->current_click_num) {
            button->current_click_num++;
        }
    }
}

void gpio_handler(nrfx_gpiote_pin_t pin, nrf_gpiote_polarity_t action)
{
    uint32_t ret = 0;

    ret = app_timer_stop(debounce_timer);
    APP_ERROR_CHECK(ret);
    ret = app_timer_start(debounce_timer, APP_TIMER_TICKS(DEBOUNCE_TIME_MS), (void *)&main_button);
    APP_ERROR_CHECK(ret);
    ret = app_timer_start(double_click_timer, APP_TIMER_TICKS(DOUBLE_CLICK_TIME_MS), (void *)&main_button);
    APP_ERROR_CHECK(ret);
}


void timers_init(void){
   nrfx_gpiote_init();

    APP_ERROR_CHECK(app_timer_init());
    APP_ERROR_CHECK(app_timer_create(&debounce_timer, APP_TIMER_MODE_SINGLE_SHOT, debounce_handler));
    APP_ERROR_CHECK(app_timer_create(&double_click_timer, APP_TIMER_MODE_SINGLE_SHOT, double_click_handler));

    nrfx_gpiote_in_config_t user_gpiote_cfg = NRFX_GPIOTE_CONFIG_IN_SENSE_TOGGLE(false);
    user_gpiote_cfg.pull = NRF_GPIO_PIN_PULLUP;
    APP_ERROR_CHECK(nrfx_gpiote_in_init(BUTTON_PIN, &user_gpiote_cfg, gpio_handler));
    nrfx_gpiote_in_event_enable(BUTTON_PIN, true);

}

hsv_control_state_t settings_state = DEFAULT_MODE;

static volatile bool pwm_is_finished = true;

int main(void)
{
    configure_leds(sizeof(leds) / sizeof(*leds), leds);
    configure_button(BUTTON_PIN);
    lfclk_request();
    timers_init();

    nrfx_pwm_init(&pwm_inst, &pwm_config, custom_pwm_handler);
    nrfx_pwm_simple_playback(&pwm_inst, &pwm_sequence, PWM_PLAYBACK_COUNT, NRFX_PWM_FLAG_LOOP);

    while (true)
    {
 
     }
}