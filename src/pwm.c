#include "pwm.h"
#include "my_button.h"
#include "states.h"

volatile nrf_pwm_values_individual_t pwm_values;
nrfx_pwm_config_t pwm_config = NRFX_PWM_DEFAULT_CONFIG;
nrfx_pwm_t pwm_inst = NRFX_PWM_INSTANCE(0);
nrf_pwm_sequence_t pwm_sequence = {
    .values = (nrf_pwm_values_t){.p_individual = (nrf_pwm_values_individual_t *)&pwm_values},
    .length = NRF_PWM_VALUES_LENGTH(pwm_values),
    .repeats = 100,
};

volatile uint16_t status_indicator_step = 0;

