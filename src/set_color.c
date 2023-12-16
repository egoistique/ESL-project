#include "set_color.h"
#include "pwm.h"
#include "hsv.h"
#include "nvmc.h"

bool saturation_increases = true;
bool value_increases = true;
int high_value_yellow_led = 1000;
volatile uint16_t yellow_led_step = 0;
static bool is_increasing = true;

struct hsv hsv_color = {
    .hue = 292,
    .saturation = 100,
    .value = 100
};

void pwm_handler(nrfx_pwm_evt_type_t event_type)
{
    yellow_led_set_state();    
    rgb_led_set_state(&hsv_color);
}

void yellow_led_set_state(void)
{
    if (yellow_led_step > 0) {
        if (is_increasing) {
            increase_yellow_led();
        } else {
            decrease_yellow_led();
        }
    }
}

void increase_yellow_led(void)
{
    if (pwm_values.channel_0 + yellow_led_step <= high_value_yellow_led) {
        pwm_values.channel_0 += yellow_led_step;
    } else {
        pwm_values.channel_0 = high_value_yellow_led;
        is_increasing = false;
    }
}

void decrease_yellow_led(void)
{
    if (pwm_values.channel_0 - yellow_led_step >= 0) {
        pwm_values.channel_0 -= yellow_led_step;
    } else {
        pwm_values.channel_0 = 0;
        is_increasing = true;
    }
}

void yellow_led_sync(){
    switch (mode) {
        case DEFAULT_MODE: 
            yellow_led_step = 0; 
            pwm_values.channel_0 = 0; 
            break;
        case HUE_MODE: 
            yellow_led_step = PWM_HUE_MODE;
            break;
        case SATURATION_MODE: 
            yellow_led_step = PWM_SATURATION_MODE; 
            break;
        case VALUE_MODE: 
            yellow_led_step = 0; 
            pwm_values.channel_0 = PWM_VALUE_MODE; 
            break;
        default:
            break;
    }
}

void set_mode()
{
    if(DOUBLE_CLICK == button_state) {
        NRF_LOG_INFO("DOUBLE CLICKS");
        mode = (mode != VALUE_MODE) ? (mode + 1) : DEFAULT_MODE;
        if(mode == DEFAULT_MODE) {
            save_data_to_nvm(&hsv_color);
        }
    }
    
    yellow_led_sync();
}

void set_hue(struct hsv *color){
    color->hue++;
    if(color->hue >= 360) {
        color->hue = 0;
     }
}

void set_saturation(struct hsv *color) {
    if (saturation_increases) {
        color->saturation++;
        if (color->saturation >= MAX_SATURATION) {
            saturation_increases = false;
        }
    } else {
        color->saturation--;
        if (color->saturation <= 0) {
            saturation_increases = true;
        }
    }
}


void set_value(struct hsv *color) {
    if (value_increases) {
        color->value++;
        if (color->value >= MAX_VALUE) {
            value_increases = false;
        }
    } else {
        color->value--;
        if (color->value <= 0) {
            value_increases = true;
        }
    }
}

void rgb_led_set_state(struct hsv *color) {
    if (LONG_CLICK == button_state) {
        switch (mode) {
            case HUE_MODE:
                set_hue(color);
                break;
            case SATURATION_MODE:
                set_saturation(color);
                break;
            case VALUE_MODE:
                set_value(color);
                break;
            default:
                break;
        }
    }

    struct RGB the_color;
    hsv2rgb(hsv_color, &the_color);
   
    pwm_values.channel_1 = the_color.red;
    pwm_values.channel_2 = the_color.green;
    pwm_values.channel_3 = the_color.blue;
}





