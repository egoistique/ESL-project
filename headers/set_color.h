
#include "states.h"

#include "pwm.h"

void application_state_handler(const custom_button_context_t *button);
void process_hsv_state(struct hsv *color, struct hsv_control_state *state);
void custom_pwm_handler(nrfx_pwm_evt_type_t event_type);