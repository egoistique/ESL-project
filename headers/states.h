#ifndef STATES_H
#define STATES_H

typedef enum {
    DEFAULT_MODE,
    HUE_MODE,
    SATURATION_MODE,
    VALUE_MODE,
    MODE_NUMBER,
} hsv_control_state_t;

extern hsv_control_state_t app_state;
extern struct hsv_control_state hsv_ctrl_state_ctx;

#endif 

