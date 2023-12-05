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

extern volatile enum button_state {
    DEFAULT_UNKNOWN,
    SINGLE_CLICK_PRESSED,
    DOUBLE_CLICK_PRESSED,
    SINGLE_CLICK_RELEASED,
    DOUBLE_CLICK_RELEASED
} button_state;

struct hsv{
    uint16_t hue;          
    uint8_t saturation; 
    uint8_t brightness; 
};


extern uint32_t main_button_pin;
extern enum hsv_mode_state_t app_state;

#endif 

