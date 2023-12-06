
#include "states.h"
#include "pwm.h"

extern bool saturation_increases;
extern bool value_increases;
extern volatile uint16_t yellow_led_step;


int high_value_yellow_led;

void set_mode();
void rgb_led_set_state(struct hsv *color);
void pwm_handler(nrfx_pwm_evt_type_t event_type);
void yellow_led_sync();
void yellow_led_set_state(void);

void increase_yellow_led(void);
void decrease_yellow_led(void);

void set_hue(struct hsv *color);
void set_saturation(struct hsv *color);
void set_value(struct hsv *color);