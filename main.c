#include <stdbool.h>
#include <stdint.h>
#include "boards.h"
#include "nrf_delay.h"
#include "nrf_gpio.h"
#include "nrfx_systick.h"
#include "nrfx_gpiote.h"
#include "app_timer.h"
#include "my_leds.h"
#include "my_button.h"

#define BLINK_FREQ      100 
#define BLINK_PERIOD_US (1000000/BLINK_FREQ) 

#define PERIOD 1000


#define BLINK_TIMER_INTERVAL APP_TIMER_TICKS(20) // Интервал таймера для управления дребезгом

static const unsigned int device_id[] = {2, 5, 8, 1};
static const int32_t leds_list[] = CUSTOM_LEDS_LIST;
static volatile bool blink_enable = true;
static app_timer_id_t blink_timer_id;

static volatile bool button_state = false;
static volatile uint32_t button_press_time = 0;
static volatile bool double_click_flag = false;

// Прототипы функций
static void blink_timer_handler(void * p_context);
static void button_handler(nrfx_gpiote_pin_t pin, nrf_gpiote_polarity_t action);

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

void smooth_blink_any_times(int32_t led , int32_t num, int32_t half_period_ms, volatile bool *enable)
{
    for(unsigned int i = 0; i < num; i++){
        smooth_blink(led, half_period_ms, enable);
        nrf_delay_ms(half_period_ms);
    }
}

static void blink_timer_handler(void * p_context) {
    // Обработчик таймера для управления дребезгом
    nrfx_gpiote_in_event_disable(BUTTON_PIN);
    if (nrfx_gpiote_in_is_set(BUTTON_PIN)) {
        button_handler(BUTTON_PIN, NRF_GPIOTE_POLARITY_HITOLO);
    } else {
        button_handler(BUTTON_PIN, NRF_GPIOTE_POLARITY_LOTOHI);
    }
    nrfx_gpiote_in_event_enable(BUTTON_PIN, true);
}

static void button_handler(nrfx_gpiote_pin_t pin, nrf_gpiote_polarity_t action) {
    static int32_t prev_button_press_time = 0;

    if (action == NRF_GPIOTE_POLARITY_LOTOHI) {
        // Кнопка нажата
        int32_t now = app_timer_cnt_get();
        
        if (now - prev_button_press_time > APP_TIMER_TICKS(500)) {
            // Если прошло более 500 миллисекунд с предыдущего нажатия, считаем, что это новое нажатие
            double_click_flag = false;
        }

        if (!double_click_flag) {
            // Если флаг двойного нажатия не установлен, обрабатываем нажатие
            button_state = true;
            button_press_time = now;
        } else {
            // Если флаг двойного нажатия установлен, сбрасываем его
            double_click_flag = false;
        }
    } else {
        // Кнопка отпущена
        uint32_t now = app_timer_cnt_get();
        if (now - button_press_time > APP_TIMER_TICKS(500)) {
            // Если прошло более 500 миллисекунд с предыдущего нажатия, устанавливаем флаг двойного нажатия
            double_click_flag = true;
        }

        // Сбрасываем состояние кнопки
        button_state = false;
        prev_button_press_time = now;
    }
}


int main(void)
{
    config_pins_as_leds(sizeof(leds_list)/sizeof(*leds_list), leds_list);
    all_leds_off(sizeof(leds_list)/sizeof(*leds_list), leds_list);

    configure_button(BUTTON_PIN);

    nrfx_systick_init();

    // Инициализация таймера для управления дребезгом
    app_timer_init();
    app_timer_create(&blink_timer_id, APP_TIMER_MODE_REPEATED, blink_timer_handler);
    app_timer_start(blink_timer_id, BLINK_TIMER_INTERVAL, NULL);

    while(true) {
        if (button_pressed(BUTTON_PIN)) {
            smooth_blink_any_times(leds_list[1], device_id[1], PERIOD, &blink_enable);
        } 
    }
}