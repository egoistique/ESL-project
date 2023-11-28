#include "my_button.h"

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