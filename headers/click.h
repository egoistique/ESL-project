#include "app_timer.h"
#include "nrf_drv_clock.h"
#include "nrfx_glue.h"
#include "nrfx_gpiote.h"
//#include "states.h"
#include "my_blink.h"

#define DEBOUNCE_TIME_MS 10
#define DOUBLE_CLICK_TIME_MS 1000

typedef enum {
    DEFAULT_MODE,
    HUE_MODE,
    SATURATION_MODE,
    VALUE_MODE,
} hsv_control_state_t;

extern volatile bool blink_enable;
extern volatile bool button_long_pressed;
extern hsv_control_state_t settings_state;

void double_click_handler(void *context);
void debounce_handler(void *context);
void gpio_handler(nrfx_gpiote_pin_t pin, nrf_gpiote_polarity_t action);
void lfclk_request(void);
void timers_init(void);
