#include <stdbool.h>
#include <stdint.h>
#include "my_leds.h"
#include "my_button.h"
#include "my_blink.h"
#include "nrfx_pwm.h"
#include "hsv.h"
#include "pwm.h"
#include "states.h"
#include "set_color.h"
#include "click_manager.h"
#include "nrfx_nvmc.h"
#include "nvmc.h"

static const int32_t leds[] = LEDS;

int main(void)
{
    configure_leds(sizeof(leds) / sizeof(*leds), leds);
    configure_button(BUTTON_PIN);
    lfclk_request();
    timers_init();

    struct RGB the_color;

    //save_data_to_nvm(&hsv_color);
    read_data_from_nvm(&hsv_color);

    hsv2rgb(hsv_color, &the_color);
    pwm_values.channel_1 = the_color.red;
    pwm_values.channel_2 = the_color.green;
    pwm_values.channel_3 = the_color.blue;

    nrfx_pwm_init(&pwm_inst, &pwm_config, pwm_handler);
    nrfx_pwm_simple_playback(&pwm_inst, &pwm_sequence, PWM_PLAYBACK_COUNT, NRFX_PWM_FLAG_LOOP);

    while (true)
    {
    }
}