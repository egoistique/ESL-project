#ifndef STATES_H
#define STATES_H
#include <stdint.h>
#include <stdint.h>

typedef enum {
    DEFAULT_MODE,
    HUE_MODE,
    SATURATION_MODE,
    VALUE_MODE,
    MODE_NUMBER,
} hsv_control_state_t;

typedef uint16_t hue_t;

struct hsv{
    hue_t hue;          
    uint8_t saturation; 
    uint8_t brightness; 
};

// enum button_state {
//     DEFAULT_UNKNOWN,
//     SINGLE_CLICK_PRESSED,
//     DOUBLE_CLICK_PRESSED,
//     SINGLE_CLICK_RELEASED,
//     DOUBLE_CLICK_RELEASED
// };

// typedef struct {
//     uint32_t pin;
//     volatile enum button_state button_state;
//     volatile unsigned int current_click_num;
// } custom_button_context_t;

// extern custom_button_context_t main_button;

// extern struct hsv_control_state hsv_ctrl_state_ctx;

// struct hsv_control_state {
//     custom_button_context_t *button;
//     bool saturation_direction;
//     bool brightness_direction;
// };

extern hsv_control_state_t app_state;
extern struct hsv_control_state hsv_ctrl_state_ctx;

#endif 

