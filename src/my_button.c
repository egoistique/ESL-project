#include "my_button.h"
#include "app_timer.h"
#include "nrf_drv_clock.h"
#include "nrfx_glue.h"
#include "nrfx_systick.h"
#include "states.h"
#include "pwm.h"

custom_button_context_t main_button = {
    .pin = BUTTON_PIN
};

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


