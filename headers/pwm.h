#include "nrfx_pwm.h"

#define PERIOD_MS 1000

#define PWM_BASE_CLOCK_HZ 125000 
#define PWM_BASE_PERIOD_US (1000000 / PWM_BASE_CLOCK_HZ) //8
#define PWM_TIME_BETWEEN_HANDLERS_US (1200 * PWM_BASE_PERIOD_US) //9600

#define PWM_DEFAULT_MODE_IND_INC 0
#define PWM_HUE_MODE_IND_INC (PERIOD_MS / 2 * 1000 / PWM_TIME_BETWEEN_HANDLERS_US) //52
#define PWM_SATURATION_MODE_IND_INC (PERIOD_MS * 1000 / PWM_TIME_BETWEEN_HANDLERS_US) //104
#define PWM_VALUE_MODE_IND_INC 1200
#define PWM_PLAYBACK_COUNT 1

extern volatile nrf_pwm_values_individual_t pwm_values;
extern nrfx_pwm_config_t pwm_config;
extern nrfx_pwm_t pwm_inst;
extern nrf_pwm_sequence_t pwm_sequence;
extern volatile uint16_t status_indicator_step;

