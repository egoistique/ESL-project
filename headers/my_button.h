#include <stdbool.h>
#include "states.h"
#include "nrf_gpio.h"
#include "nrfx_gpiote.h"
#include "nrfx_pwm.h"
#define BUTTON_PIN NRF_GPIO_PIN_MAP(1, 6)

extern volatile bool blink_enable;
extern volatile bool button_long_pressed;

void double_click_handler(void *context);
void debounce_handler(void *context);
void gpio_handler(nrfx_gpiote_pin_t pin, nrf_gpiote_polarity_t action);
void lfclk_request(void);
void timers_init(void);

void configure_button(int pin);
bool button_pressed(uint32_t pin);

