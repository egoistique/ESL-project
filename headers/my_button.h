#include <stdbool.h>
#include "nrf_gpio.h"
#include "nrfx_gpiote.h"
#include "app_timer.h"
#include "nrf_drv_clock.h"
#include "nrfx_glue.h"

#define BUTTON_PIN NRF_GPIO_PIN_MAP(1, 6)
#define DEBOUNCE_TIME_MS 10
#define DOUBLE_CLICK_TIME_MS 1000

extern volatile bool blink_enable;
extern volatile bool button_long_pressed;

enum button_state {
    DEFAULT_UNKNOWN,
    SINGLE_CLICK_PRESSED,
    DOUBLE_CLICK_PRESSED,
    SINGLE_CLICK_RELEASED,
    DOUBLE_CLICK_RELEASED
};

typedef struct {
    uint32_t pin;
    volatile enum button_state button_state;
    volatile unsigned int current_click_num;
} custom_button_context_t;

extern custom_button_context_t main_button;


struct hsv_control_state {
    custom_button_context_t *button;
    bool saturation_direction;
    bool brightness_direction;
};


void double_click_handler(void *context);
void debounce_handler(void *context);
void gpio_handler(nrfx_gpiote_pin_t pin, nrf_gpiote_polarity_t action);
void lfclk_request(void);
void timers_init(void);

void configure_button(int pin);
bool button_pressed(int pin);
bool button_te_is_pressed(uint32_t pin);
bool button_te_is_released(uint32_t pin);
