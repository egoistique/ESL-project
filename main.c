#include <stdbool.h>
#include <stdint.h>
#include "boards.h"
#include "nrf_delay.h"
#include "nrf_gpio.h"
#include "nrfx_systick.h"
#include "nrfx_gpiote.h"
#include "app_timer.h"


#define LED1_PIN NRF_GPIO_PIN_MAP(0, 6)
#define LED2_PIN NRF_GPIO_PIN_MAP(0, 8)
#define LED3_PIN NRF_GPIO_PIN_MAP(1, 9)
#define LED4_PIN NRF_GPIO_PIN_MAP(0, 12)
#define CUSTOM_LEDS_LIST {LED1_PIN, LED2_PIN, LED3_PIN, LED4_PIN}

#define BUTTON_PIN NRF_GPIO_PIN_MAP(1, 6)

#define LED_NUMBER 4
#define DELAY 500

#define BLINK_FREQ      100 
#define BLINK_PERIOD_US (1000000/BLINK_FREQ) 

#define PERIOD 1000
#define DEBOUNCE_TIME_MS 10

static const unsigned int device_id[] = {2, 5, 8, 1};
static const int32_t leds_list[] = CUSTOM_LEDS_LIST;


static volatile bool blink_enable = true;

static volatile bool button_state = true;  // Используйте pull-up, поэтому начальное состояние true
static volatile int32_t last_button_press_time = 0;


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

void led_on(int32_t pin)
{
    nrf_gpio_pin_clear(pin);
}

void led_off(int32_t pin)
{
    nrf_gpio_pin_set(pin);
}

bool button_pressed(int pin)
{
    return nrf_gpio_pin_read(pin) == 0;
}

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

void config_pin_as_button(int32_t pin)
{
    nrf_gpio_cfg_input(pin, NRF_GPIO_PIN_PULLUP);
}

void wait_microseconds(int32_t us) {
    nrf_delay_us(us);
}

void smooth_blink(int32_t led, int32_t duration_ms, volatile bool * enable)
{
    int32_t num_shots = (duration_ms * 1000) / (2 * BLINK_PERIOD_US); 
    int32_t duty_cycle_step = BLINK_PERIOD_US / num_shots; 
    int32_t current_duty_cycle = 0; 

    bool forward_direction = true;

    nrfx_systick_state_t state;

    while (true) {
        if (*enable) {
            if (forward_direction && (current_duty_cycle >= BLINK_PERIOD_US)) {
                forward_direction = false;
            }

            current_duty_cycle += forward_direction ? duty_cycle_step : -duty_cycle_step;

            if (current_duty_cycle <= 0) {
                break;
            }
        }

        if (current_duty_cycle > 0) {
            led_on(led);
            nrfx_systick_get(&state);
            wait_microseconds(current_duty_cycle);
            led_off(led);
            nrfx_systick_get(&state);
            wait_microseconds(BLINK_PERIOD_US - current_duty_cycle);
        }
    }

}

void smooth_blink_any_times(int32_t led, int32_t num, int32_t half_period_ms, volatile bool *enable)
{
    for(unsigned int i = 0; i < num; i++){
        while (*enable) {
            smooth_blink(led, half_period_ms, enable);
            nrf_delay_ms(half_period_ms);
        }
        nrf_delay_ms(half_period_ms);  // Дополнительная задержка после заморозки
    }
}


void all_leds_off(unsigned int num, const int32_t leds[num])
{
    for(unsigned int i = 0; i < num; i++) {
        led_off(leds[i]);
    }
}


void check_double_click(void)
{
    if (!button_pressed(BUTTON_PIN)) {
        // Кнопка не нажата
        return;
    }

    int32_t current_time = app_timer_cnt_get();

    int32_t time_since_last_press = current_time - last_button_press_time;

    if (button_state && (time_since_last_press > DEBOUNCE_TIME_MS) && (time_since_last_press < (2 * DEBOUNCE_TIME_MS))) {
        // Обработка двойного нажатия
        blink_enable = !blink_enable;  // Инвертируем состояние
        while (!button_pressed(BUTTON_PIN));  // Ждем, пока кнопка не отпущена
    }

    button_state = button_pressed(BUTTON_PIN);
    last_button_press_time = current_time;
}


int main(void)
{
    config_pins_as_leds(sizeof(leds_list)/sizeof(*leds_list), leds_list);
    all_leds_off(sizeof(leds_list)/sizeof(*leds_list), leds_list);

    config_pin_as_button(BUTTON_PIN);

    nrfx_systick_init();

    while(true) {

        check_double_click();

        for (int i = 0; i < sizeof(device_id)/sizeof(*device_id); i++)
        {
            smooth_blink_any_times(leds_list[i], device_id[i], PERIOD, &blink_enable);
        }
    }
}


