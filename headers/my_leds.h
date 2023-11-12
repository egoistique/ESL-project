#include "nrf_gpio.h"

#define LED1_PIN NRF_GPIO_PIN_MAP(0, 6)
#define LED2_PIN NRF_GPIO_PIN_MAP(0, 8)
#define LED3_PIN NRF_GPIO_PIN_MAP(1, 9)
#define LED4_PIN NRF_GPIO_PIN_MAP(0, 12)
#define CUSTOM_LEDS_LIST {LED1_PIN, LED2_PIN, LED3_PIN, LED4_PIN}

#define DELAY 500

void led_on(int32_t pin);
void led_off(int32_t pin);

void all_leds_off(unsigned int num, const int32_t leds[num]);
void configure_led(int pin);
//void blink(int led);
void config_pin_as_led(int32_t pin);
void config_pins_as_leds(int num, const int32_t pins[num]) ;