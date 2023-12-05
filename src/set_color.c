#include "set_color.h"
#include "pwm.h"
#include "hsv.h"

bool saturation_increases = true;
bool value_increases = true;
int high_value_yellow_led = 1200;
volatile uint16_t yellow_led_step = 0;
    static bool is_increasing = true;

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

void mode_state_handler()
{
    if(DOUBLE_CLICK_RELEASED == button_state) {
        state = (state != VALUE_MODE) ? (state + 1) : DEFAULT_MODE;
    }

    switch (state) {
        case DEFAULT_MODE: 
            yellow_led_step = 0; 
            pwm_values.channel_0 = PWM_DEFAULT_MODE; 
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


void set_saturation(struct hsv *color) {
    if (saturation_increases) {
        if (MAX_SATURATION <= ++color->saturation) {
            saturation_increases = false;
        }
    } else {
        if (0 >= --color->saturation) {
            saturation_increases = true;
        }
    }
}

void set_hue(struct hsv *color){
    if(360 <= ++color->hue) {
        color->hue = 0;
     }
}

void set_value(struct hsv *color) {
    if (value_increases) {
        if (MAX_SATURATION <= ++color->value) {
            value_increases = false;
        }
    } else {
        if (0 >= --color->value) {
            value_increases = true;
        }
    }
}

void rgb_led_set_state(struct hsv *color) {
    if (LONG_CLICK_PRESSED == button_state) {
        switch (state) {
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

    union rgb the_color = { .components = {0} };
    hsv_to_rgb(hsv_color, &the_color);

    pwm_values.channel_1 = the_color.red;
    pwm_values.channel_2 = the_color.green;
    pwm_values.channel_3 = the_color.blue;
}





