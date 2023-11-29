#include "my_leds.h"
#include "boards.h"
#include "nrf_delay.h"


void led_clear(int32_t pin)
{
    nrf_gpio_pin_clear(pin);
}

void led_set(int32_t pin)
{
    nrf_gpio_pin_set(pin);
}

void all_leds_off(unsigned int num, const int32_t leds[num])
{
    for(unsigned int i = 0; i < num; i++) {
        led_set(leds[i]);
    }
}

void configure_led(int pin)
{
    nrf_gpio_cfg_output(pin);
    nrf_gpio_pin_write(pin, 1);
}

void blink(int led)
{
    bsp_board_led_invert(led);
    nrf_delay_ms(DELAY);   
    bsp_board_led_invert(led);
    nrf_delay_ms(DELAY);  
}


void configure_leds(int num, const int32_t pins[num]) 
{
    for(int i = 0; i < num; i++) {
        configure_led(pins[i]);
    }
}