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
#include "hsv.h"
//#include "states.h"

#define LED_PIN NRF_GPIO_PIN_MAP(0, 6)

#define PERIOD_MS 1000
#define PWM_PLAYBACK_COUNT 1

#define PWM_DEFAULT_MODE_IND_INC    0
#define DEFAULT_INDIC_COUNT_VAL 0
#define HUE_INDIC_COUNT_VAL 5
#define SATURATION_INDIC_COUNT_VAL 10
#define BRIGHTNESS_INDIC_COUNT_VAL NRFX_PWM_DEFAULT_CONFIG_TOP_VALUE

#define HUE_VAL_PER_DEGREE (NRFX_PWM_DEFAULT_CONFIG_TOP_VALUE / 360)

static const int32_t leds[] = LEDS;

static volatile nrf_pwm_values_individual_t pwm_values = {
    .channel_0 = 0,
    .channel_1 = 0,
    .channel_2 = 0,
    .channel_3 = 0
};

static nrfx_pwm_config_t pwm_config = NRFX_PWM_DEFAULT_CONFIG;
static nrfx_pwm_t pwm_inst = NRFX_PWM_INSTANCE(0);
static nrf_pwm_sequence_t pwm_sequence = {
    .values = (nrf_pwm_values_t){.p_individual = (nrf_pwm_values_individual_t *)&pwm_values},
    .length = NRF_PWM_VALUES_LENGTH(pwm_values),
    .repeats = 100,
    .end_delay = 0
};

// typedef enum {
//     DEFAULT_MODE,
//     HUE_MODE,
//     SATURATION_MODE,
//     VALUE_MODE,
// } hsv_control_state_t;

hsv_control_state_t settings_state = DEFAULT_MODE;

static volatile uint16_t status_indicator_step = 10;

static volatile bool pwm_is_finished = true;
static struct hsv hsv_color = {
    .hue = 292,
    .saturation = 100,
    .brightness = 100
};

void custom_pwm_handler(nrfx_pwm_evt_type_t event_type)
{
    pwm_is_finished = true;
}

int main(void)
{
    configure_leds(sizeof(leds) / sizeof(*leds), leds);
    configure_button(BUTTON_PIN);
    lfclk_request();

   // nrfx_systick_init();

    timers_init();

    nrf_gpio_cfg_output(LED_PIN);

    nrfx_pwm_init(&pwm_inst, &pwm_config, custom_pwm_handler);
    nrfx_pwm_simple_playback(&pwm_inst, &pwm_sequence, PWM_PLAYBACK_COUNT, NRFX_PWM_FLAG_LOOP);



    union rgb rgb_color = hue_to_rgb(hsv_color.hue);
    set_saturation(hsv_color.saturation, &rgb_color);

    pwm_values.channel_1 = rgb_color.red;
    pwm_values.channel_2 = rgb_color.green;
    pwm_values.channel_3 = rgb_color.blue;
    blink(0);

    while (true)
    {
        switch (settings_state) {
            case DEFAULT_MODE:
                pwm_values.channel_0 = PWM_DEFAULT_MODE_IND_INC; 
                break;
            case HUE_MODE:
                
                break;
            case SATURATION_MODE:
                break;
            case VALUE_MODE:
                break;
            default:
                break;
         }
     }
    
}