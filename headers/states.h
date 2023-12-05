#ifndef STATES_H
#define STATES_H
#include <stdint.h>
#include <stdbool.h>

enum mode_state {
    DEFAULT_MODE,
    HUE_MODE,
    SATURATION_MODE,
    VALUE_MODE,
};

extern volatile enum button_state {
    DEFAULT_UNKNOWN,
    LONG_CLICK_PRESSED,
    DOUBLE_CLICK_PRESSED,
    LONG_CLICK_RELEASED,
    DOUBLE_CLICK_RELEASED
} button_state;

struct hsv{
    uint16_t hue;          
    uint8_t saturation; 
    uint8_t value; 
};

extern enum mode_state state;

#endif 

