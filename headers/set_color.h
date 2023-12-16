
#include "states.h"
#include "pwm.h"
#include "nrfx_nvmc.h"

#include "nordic_common.h"
#include "boards.h"

#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"

#include "nrf_log_backend_usb.h"

#include "app_usbd.h"
#include "app_usbd_serial_num.h"

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
