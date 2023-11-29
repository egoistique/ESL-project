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
//#include "click.h"
#include "hsv.h"
#include "pwm.h"
#include "states.h"
//#include "set_color.h"

static const int32_t leds[] = LEDS;





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