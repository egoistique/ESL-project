#include "nrfx_pwm.h"

#define PWM_HUE_MODE 40
#define PWM_SATURATION_MODE 150
#define PWM_VALUE_MODE 500
#define PWM_PLAYBACK_COUNT 1

extern volatile nrf_pwm_values_individual_t pwm_values;
extern nrfx_pwm_config_t pwm_config;
extern nrfx_pwm_t pwm_inst;
extern nrf_pwm_sequence_t pwm_sequence;


