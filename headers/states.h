#ifndef STATES_H
#define STATES_H
#include <stdint.h>
#include <stdbool.h>

enum hsv_mode_state_t {
    DEFAULT_MODE,
    HUE_MODE,
    SATURATION_MODE,
    VALUE_MODE,
    MODE_NUMBER,
};

enum button_state {
    DEFAULT_UNKNOWN,
    SINGLE_CLICK_PRESSED,
    DOUBLE_CLICK_PRESSED,
    SINGLE_CLICK_RELEASED,
    DOUBLE_CLICK_RELEASED
};

struct hsv{
    uint16_t hue;          
    uint8_t saturation; 
    uint8_t brightness; 
};


typedef struct {
    uint32_t pin;
    volatile enum button_state button_state;
} custom_button_context_t;

extern custom_button_context_t main_button;

extern struct hsv_control_state hsv_ctrl_state_ctx;

struct hsv_control_state {
    custom_button_context_t *button;

};

extern enum hsv_mode_state_t app_state;

#endif 

