
#include "states.h"
#include "pwm.h"

extern bool saturation_direction;
extern bool brightness_direction;

void application_state_handler();
void process_hsv_state(struct hsv *color);
void custom_pwm_handler(nrfx_pwm_evt_type_t event_type);