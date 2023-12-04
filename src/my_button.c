#include "my_button.h"
#include "app_timer.h"
#include "nrf_drv_clock.h"
#include "nrfx_glue.h"
#include "nrfx_systick.h"
// #include "states.h"
#include "pwm.h"
#include "hsv.h"

custom_button_context_t main_button = {
    .pin = BUTTON_PIN
};

struct hsv_control_state hsv_ctrl_state_ctx = { .button = &main_button };

void configure_button(int pin)
{
    nrf_gpio_cfg_input(pin, NRF_GPIO_PIN_PULLUP);
}

bool button_pressed(int pin)
{
    return nrf_gpio_pin_read(pin) == 0;
}
bool button_te_is_pressed(uint32_t pin)
{
    return !nrfx_gpiote_in_is_set(pin);
}

bool button_te_is_released(uint32_t pin)
{
    return nrfx_gpiote_in_is_set(pin);
}

void lfclk_request(void)
{
    APP_ERROR_CHECK(nrf_drv_clock_init());
    nrf_drv_clock_lfclk_request(NULL);

    while (!nrf_drv_clock_lfclk_is_running())
    {
        /* Just waiting until the lf clock starts up */
    }
}


void application_state_handler(const custom_button_context_t *button)
{
    if(DOUBLE_CLICK_RELEASED == button->button_state) {
        if(MODE_NUMBER <= ++app_state) {
            app_state = DEFAULT_MODE;
        }
    }

    switch (app_state) {
        case DEFAULT_MODE: 
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


