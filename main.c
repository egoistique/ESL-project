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
#include "my_blink.h"
#include "nrfx_pwm.h"
#include "nrf_drv_clock.h"
#include "nrfx_glue.h"
#include "click.h"

#define LED_PIN NRF_GPIO_PIN_MAP(0, 6)

static const int32_t leds[] = LEDS;

static nrfx_pwm_t m_pwm0 = NRFX_PWM_INSTANCE(0);
static volatile bool m_pwm_ready = true;

void pwm_handler(nrfx_pwm_evt_type_t event_type)
{
    m_pwm_ready = true;
}

void pwm_init(void)
{
    nrfx_pwm_config_t const config =
    {
        .output_pins =
        {
            LED_PIN,              // Channel 0
            NRFX_PWM_PIN_NOT_USED, // Channel 1
            NRFX_PWM_PIN_NOT_USED, // Channel 2
            NRFX_PWM_PIN_NOT_USED, // Channel 3
        },
        .irq_priority = APP_IRQ_PRIORITY_LOWEST,
        .base_clock   = NRF_PWM_CLK_16MHz,
        .count_mode   = NRF_PWM_MODE_UP,
        .top_value    = 1000, // Set maximum value for the counter (controls brightness)
        .load_mode    = NRF_PWM_LOAD_COMMON,
        .step_mode    = NRF_PWM_STEP_AUTO
    };
    
    nrfx_pwm_init(&m_pwm0, &config, pwm_handler);
}

void pwm_set_duty_cycle(uint16_t duty_cycle)
{
    nrf_pwm_values_individual_t seq_values =
    {
        .channel_0 = duty_cycle,
        .channel_1 = 0,
        .channel_2 = 0,
        .channel_3 = 0
    };
    
    nrf_pwm_sequence_t const seq =
    {
        .values.p_individual = &seq_values,
        .length              = NRF_PWM_VALUES_LENGTH(seq_values),
        .repeats             = 0,
        .end_delay           = 0
    };
    
    nrfx_pwm_simple_playback(&m_pwm0, &seq, 1, NRFX_PWM_FLAG_STOP);
}

int main(void)
{

    configure_leds(sizeof(leds) / sizeof(*leds), leds);
    configure_button(BUTTON_PIN);

    lfclk_request();

    nrfx_systick_init();

    timers_init();
 

    nrf_gpio_cfg_output(LED_PIN);

    pwm_init();

    while (true)
    {
        for (uint16_t brightness = 0; brightness <= 1000; brightness += 10)
        {
            while (!m_pwm_ready)
            {
                // Wait for the previous sequence to finish
            }
            pwm_set_duty_cycle(brightness);
            nrf_delay_ms(10); // Adjust delay for desired transition speed
        }
    }
}