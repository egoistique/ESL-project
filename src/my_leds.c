#include "my_leds.h"

void led_on(int32_t pin)
{
    nrf_gpio_pin_clear(pin);
}

void led_off(int32_t pin)
{
    nrf_gpio_pin_set(pin);
}

void all_leds_off(unsigned int num, const int32_t leds[num])
{
    for(unsigned int i = 0; i < num; i++) {
        led_off(leds[i]);
    }
}

void configure_led(int pin)
{
    nrf_gpio_cfg_output(pin);
    nrf_gpio_pin_write(pin, 1);
}

// void blink(int led)
// {
//     bsp_board_led_invert(led);
//     nrf_delay_ms(DELAY);   
//     bsp_board_led_invert(led);
//     nrf_delay_ms(DELAY);  
// }

void config_pin_as_led(int32_t pin)
{
    nrf_gpio_cfg_output(pin);
    nrf_gpio_pin_write(pin, 1);
}

void config_pins_as_leds(int num, const int32_t pins[num]) 
{
    for(int i = 0; i < num; i++) {
        config_pin_as_led(pins[i]);
    }
}