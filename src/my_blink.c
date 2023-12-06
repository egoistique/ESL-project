#include "nrfx_systick.h"
#include "nrf_delay.h"
#include "my_blink.h"
#include "my_leds.h"

void wait_microseconds(int32_t us) 
{
    nrf_delay_us(us);
}

void smooth_blink(int32_t led, int32_t duration_ms, volatile bool * enable)
{
    int32_t num_steps = (duration_ms * 1000) / (2 * PERIOD); 
    int32_t step = PERIOD / num_steps; 
    int32_t current_duty_cycle = 0; 

    bool direction = true;

    nrfx_systick_state_t state;

    while (true) {
        if (*enable) {
            if (direction && (current_duty_cycle >= PERIOD)) {
                direction = false;
            }

            current_duty_cycle += direction ? step : -step;

            if (current_duty_cycle <= 0) {
                break;
            }
        }
        led_clear(led);
        nrfx_systick_get(&state);
        wait_microseconds(current_duty_cycle);
        led_set(led);
        nrfx_systick_get(&state);
        wait_microseconds(PERIOD - current_duty_cycle);
        
    }
}

void smooth_blink_any_times(int32_t led , int32_t num, int32_t period_ms, volatile bool *enable)
{
    for(unsigned int i = 0; i < num; i++){
        smooth_blink(led, period_ms, enable);
        nrf_delay_ms(period_ms);
    }
}