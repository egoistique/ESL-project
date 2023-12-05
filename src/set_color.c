#include "set_color.h"
#include "pwm.h"
#include "hsv.h"

 bool saturation_direction = true;
 bool brightness_direction = true;

void application_state_handler()
{
    if(DOUBLE_CLICK_RELEASED == button_state) {
        if(MODE_NUMBER <= ++app_state) {
            app_state = DEFAULT_MODE;
        }
    }

    switch (app_state) {
        case DEFAULT_MODE: //скорее всгео можно убрать
            status_indicator_step = 0; 
            pwm_values.channel_0 = PWM_DEFAULT_MODE_IND_INC; 
            break;
        case HUE_MODE: 
            status_indicator_step = PWM_HUE_MODE_IND_INC;
            break;
        case SATURATION_MODE: 
            status_indicator_step = PWM_SATURATION_MODE_IND_INC; 
            break;
        case VALUE_MODE: 
            status_indicator_step = 0; 
            pwm_values.channel_0 = PWM_VALUE_MODE_IND_INC; 
            break;
        default:
            break;
    }
}

void process_hsv_state(struct hsv *color)
{
    if(SINGLE_CLICK_PRESSED == button_state) {
        switch(app_state) {
            case HUE_MODE:
                set_hue(color);
                break;
            case SATURATION_MODE:
                if(saturation_direction) {
                    if(MAX_SATURATION <= ++color->saturation) {
                        saturation_direction = false;
                    }
                }
                else {
                    if(0 >= --color->saturation) {
                        saturation_direction = true;
                    }
                }
                
                break;
            case VALUE_MODE: 
                if(brightness_direction) {
                    if(MAX_SATURATION <= ++color->brightness) {
                        brightness_direction = false;
                    }
                }
                else {
                    if(0 >= --color->brightness) {
                        brightness_direction = true;
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
    process_hsv_state(&hsv_color);
}

