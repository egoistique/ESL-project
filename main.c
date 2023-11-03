#include <stdbool.h>
#include <stdint.h>
#include "boards.h"
#include "nrf_delay.h"
#include "nrf_gpio.h"

#define LED1_PIN NRF_GPIO_PIN_MAP(0, 6)
#define LED2_PIN NRF_GPIO_PIN_MAP(0, 8)
#define LED3_PIN NRF_GPIO_PIN_MAP(1, 9)
#define LED4_PIN NRF_GPIO_PIN_MAP(0, 12)

#define BUTTON_PIN NRF_GPIO_PIN_MAP(1, 6)

#define LED_NUMBER 4
#define DELAY 500


void blink(int);
void configure_led(int);
void configure_button(int);


int main(void)
{
    bsp_board_init(BSP_INIT_LEDS);

    uint8_t blinkCounts[4] = {6, 5, 8, 1};

    while (true)
    {
        for (int i = 0; i < LEDS_NUMBER; i++)
        {
            for (int j = 0; j < blinkCounts[i]; j++)
            {
                bsp_board_led_invert(i);
                nrf_delay_ms(500);   
                bsp_board_led_invert(i);
                nrf_delay_ms(500);   
            }
            nrf_delay_ms(1000);  
        }

    }
}


void blink(int led){
    bsp_board_led_invert(led);
    nrf_delay_ms(DELAY);   
    bsp_board_led_invert(led);
    nrf_delay_ms(DELAY);  
}

void configure_led(int pin){
    nrf_gpio_cfg_output(pin);
    nrf_gpio_pin_write(pin, 1);
}

void configure_button(int pin){
    nrf_gpio_cfg_input(pin);
}
