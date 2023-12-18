#include <stdint.h>

#include "sdk_config.h"
#include "nrfx_pwm.h"

#include "my_leds.h"
#include "my_button.h"
#include "my_blink.h"
#include "nrfx_pwm.h"
#include "hsv.h"
#include "pwm.h"
#include "states.h"
#include "set_color.h"
#include "click_manager.h"
#include "nrfx_nvmc.h"
#include "nvmc.h"

#include "app_usbd.h"
#include "app_usbd_serial_num.h"
#include "app_usbd_cdc_acm.h"

#define READ_SIZE 1
#define MAX_COMMAND_LENGTH 20

static char m_rx_buffer[READ_SIZE];

char commandBuffer[MAX_COMMAND_LENGTH];
size_t commandLength = 0;

static void usb_ev_handler(app_usbd_class_inst_t const * p_inst,
                           app_usbd_cdc_acm_user_event_t event);

/* Make sure that they don't intersect with LOG_BACKEND_USB_CDC_ACM */
#define CDC_ACM_COMM_INTERFACE  2
#define CDC_ACM_COMM_EPIN       NRF_DRV_USBD_EPIN3

#define CDC_ACM_DATA_INTERFACE  3
#define CDC_ACM_DATA_EPIN       NRF_DRV_USBD_EPIN4
#define CDC_ACM_DATA_EPOUT      NRF_DRV_USBD_EPOUT4

APP_USBD_CDC_ACM_GLOBAL_DEF(usb_cdc_acm,
                            usb_ev_handler,
                            CDC_ACM_COMM_INTERFACE,
                            CDC_ACM_DATA_INTERFACE,
                            CDC_ACM_COMM_EPIN,
                            CDC_ACM_DATA_EPIN,
                            CDC_ACM_DATA_EPOUT,
                            APP_USBD_CDC_COMM_PROTOCOL_NONE);



void checkCommand(const char *commandBuffer, size_t length) {
    const char *helpCommand = "help";
    const char *rgbCommand = "RGB";
    const char *hsvCommand = "HSV";

    const char *messageHelp = "RGB <red> <green> <blue> - the device sets current color to specified one.\r\n"
                              "HSV <hue> <saturation> <value> - the same with RGB, but color is specified in HSV.\r\n"
                              "help - print information about supported commands\r\n";

    const char *messageRGB = "RGB values should be in the range [0...255]\r\n";

    const char *messageHSV = "HSV values should be within these limits: hue [0...360], staturation and value [0...100]\r\n";

    char messageSetRGB[50];
    char messageSetHSV[50];


    if (length >= strlen(helpCommand) && strncmp(commandBuffer, helpCommand, strlen(helpCommand)) == 0) 
    {
        NRF_LOG_INFO("HELP COMMAND");
        app_usbd_cdc_acm_write(&usb_cdc_acm, messageHelp, strlen(messageHelp));
    } else if (length >= strlen(rgbCommand) && strncmp(commandBuffer, rgbCommand, strlen(rgbCommand)) == 0) 
    {
        NRF_LOG_INFO("RGB COMMAND");
        int red, green, blue;
        if (sscanf(commandBuffer, "RGB %d %d %d", &red, &green, &blue) == 3) 
        {
            if (red >= 0 && red <= 255 && green >= 0 && green <= 255 && blue >= 0 && blue <= 255) 
            {
                sprintf(messageSetRGB, "Color set to R=%d G=%d B=%d\r\n", red, green, blue);
                app_usbd_cdc_acm_write(&usb_cdc_acm, messageSetRGB, strlen(messageSetRGB));
                set_rgb_color(red, green, blue, &hsv_color);

            } else {
                app_usbd_cdc_acm_write(&usb_cdc_acm, messageRGB, strlen(messageRGB));
            }
        }
    } else if (length >= strlen(hsvCommand) && strncmp(commandBuffer, hsvCommand, strlen(hsvCommand)) == 0) 
    {
        NRF_LOG_INFO("HSV COMMAND");
        int hue, saturation, value;
        if (sscanf(commandBuffer, "HSV %d %d %d", &hue, &saturation, &value) == 3) 
        {
            if (hue >= 0 && hue <= 360 && saturation >= 0 && saturation <= 100 && value >= 0 && value <= 100) 
            {
                sprintf(messageSetHSV, "Color set to H=%d S=%d V=%d\r\n", hue, saturation, value);
                app_usbd_cdc_acm_write(&usb_cdc_acm, messageSetHSV, strlen(messageSetHSV));
                set_hsv_color(hue, saturation, value, &hsv_color);
            } else 
            {
                app_usbd_cdc_acm_write(&usb_cdc_acm, messageHSV, strlen(messageHSV));
            }
        }
    }
}

static void usb_ev_handler(app_usbd_class_inst_t const * p_inst,
                           app_usbd_cdc_acm_user_event_t event)
{
    switch (event) {
    case APP_USBD_CDC_ACM_USER_EVT_PORT_OPEN: {
        ret_code_t ret;
        ret = app_usbd_cdc_acm_read(&usb_cdc_acm, m_rx_buffer, READ_SIZE);
        UNUSED_VARIABLE(ret);
        break;
    }
    case APP_USBD_CDC_ACM_USER_EVT_PORT_CLOSE: {
        break;
    }
    case APP_USBD_CDC_ACM_USER_EVT_TX_DONE: {
        NRF_LOG_INFO("tx done");
        break;
    }
    case APP_USBD_CDC_ACM_USER_EVT_RX_DONE: {
        ret_code_t ret;
        do {
            /*Get amount of data transfered*/
            size_t size = app_usbd_cdc_acm_rx_size(&usb_cdc_acm);
            NRF_LOG_INFO("rx size: %d", size);

            /* It's the simple version of an echo. Note that writing doesn't
             * block execution, and if we have a lot of characters to read and
             * write, some characters can be missed.
             */
            if (commandLength < MAX_COMMAND_LENGTH - 1) {
                commandBuffer[commandLength] = m_rx_buffer[0];            
                commandLength++;
                commandBuffer[commandLength] = '\0'; 
            }

            if (m_rx_buffer[0] == '\r' || m_rx_buffer[0] == '\n') {
                ret = app_usbd_cdc_acm_write(&usb_cdc_acm, "\r\n", 2);

                checkCommand(commandBuffer, commandLength);

                memset(commandBuffer, 0, sizeof(commandBuffer));
                commandLength = 0;
            }
            else {
                ret = app_usbd_cdc_acm_write(&usb_cdc_acm,
                                             m_rx_buffer,
                                             READ_SIZE);
            }

            /* Fetch data until internal buffer is empty */
            ret = app_usbd_cdc_acm_read(&usb_cdc_acm,
                                        m_rx_buffer,
                                        READ_SIZE);
        } while (ret == NRF_SUCCESS);

        break;
    }
    default:
        break;
    }
}

static const int32_t leds[] = LEDS;

void logs_init()
{
    ret_code_t ret = NRF_LOG_INIT(NULL);
    APP_ERROR_CHECK(ret);

    NRF_LOG_DEFAULT_BACKENDS_INIT();
}

int main(void)
{
    logs_init();
    configure_leds(sizeof(leds) / sizeof(*leds), leds);
    configure_button(BUTTON_PIN);
    //lfclk_request();
    timers_init();

    app_usbd_class_inst_t const * class_cdc_acm = app_usbd_cdc_acm_class_inst_get(&usb_cdc_acm);
    ret_code_t ret = app_usbd_class_append(class_cdc_acm);
    APP_ERROR_CHECK(ret);

    struct RGB the_color;

    read_data_from_nvm(&hsv_color);

    hsv2rgb(hsv_color, &the_color);
    pwm_values.channel_1 = the_color.red;
    pwm_values.channel_2 = the_color.green;
    pwm_values.channel_3 = the_color.blue;

    nrfx_pwm_init(&pwm_inst, &pwm_config, pwm_handler);
    nrfx_pwm_simple_playback(&pwm_inst, &pwm_sequence, PWM_PLAYBACK_COUNT, NRFX_PWM_FLAG_LOOP);


    while(true) {
        LOG_BACKEND_USB_PROCESS();
        NRF_LOG_PROCESS();
    }   
}