#include <stdbool.h>
#include "nrf_gpio.h"

#define BUTTON_PIN NRF_GPIO_PIN_MAP(1, 6)

void configure_button(int pin);
bool button_pressed(int pin);
