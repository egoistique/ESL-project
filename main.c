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
void invert_led_state(int);
bool button_pressed(int pin_n);

int main(void)
{
    bsp_board_init(BSP_INIT_LEDS);

    configure_led(LED1_PIN);
    configure_led(LED2_PIN);
    configure_led(LED3_PIN);
    configure_led(LED4_PIN);
    configure_button(BUTTON_PIN);

    uint8_t blinkCounts[4] = {6, 5, 8, 1};
    int current_led = 0;
    bool button_was_pressed = false; 

    while (true)
    {
        if (button_pressed(BUTTON_PIN)) {
            if (!button_was_pressed) {
                button_was_pressed = true;
                // Если кнопка была нажата, переключаемся на следующий светодиод
                current_led = (current_led + 1) % LED_NUMBER;
            }

            for (int j = 0; j < blinkCounts[current_led]; j++)
            {
                blink(current_led);
                if (!button_pressed(BUTTON_PIN)) {
                    // Если кнопка отпущена во время мигания, прерываем цикл мигания
                    button_was_pressed = false;
                    break;
                }
            }
        } else {
            // Кнопка не нажата, сбрасываем флаг и ожидаем нажатия
            button_was_pressed = false;
        }

        nrf_delay_ms(1000);  
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
    nrf_gpio_cfg_input(pin, NRF_GPIO_PIN_PULLUP);
}

void invert_led_state(int pin){
    int current_state = nrf_gpio_pin_out_read(pin);
    nrf_gpio_pin_write(pin, current_state ? 0 : 1);
}


bool button_pressed(int pin_n)
{
    return nrf_gpio_pin_read(pin_n) == 0;
}