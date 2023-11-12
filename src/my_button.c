#include "my_button.h"

void configure_button(int pin)
{
    nrf_gpio_cfg_input(pin, NRF_GPIO_PIN_PULLUP);
}

bool button_pressed(int pin)
{
    return nrf_gpio_pin_read(pin) == 0;
}
