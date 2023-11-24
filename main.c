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

static const unsigned int device_id[] = {6, 5, 8, 1};
static const int32_t leds[] = LEDS;


void pwm_init(void) {
    nrfx_pwm_config_t pwm_config = NRFX_PWM_DEFAULT_CONFIG;
    nrfx_pwm_t pwm_instance = NRFX_PWM_INSTANCE(0);
    nrfx_pwm_init(&pwm_instance, &pwm_config, NULL);
}

int main(void) {
    configure_leds(sizeof(leds) / sizeof(*leds), leds);
    configure_button(BUTTON_PIN);

    lfclk_request();

    nrfx_systick_init();

    timers_init();
 
    pwm_init();

    while (true) {
        for (int i = 0; i < sizeof(device_id) / sizeof(*device_id); i++) {
           smooth_blink_any_times(leds[i], device_id[i], PERIOD, &blink_enable);
        }
    }
}