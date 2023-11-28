#include "hsv.h"
#include "nordic_common.h"


#define PWM_COUNT_TOP 1200
#define PWM_COLOR_STEP (PWM_COUNT_TOP / 120.0f)

#define MAX_VAL_PTR(a, b) ((*(a) > *(b)) ? a : b)



union rgb hue_to_rgb(hue_t hue)
{
    union rgb rgb = {
        .red = 0,
        .green = 0,
        .blue = 0,
    };

    hue = hue % 360;

    uint16_t second_color = PWM_COLOR_STEP * (hue % 120);
    uint16_t first_color = PWM_COUNT_TOP - second_color;

    switch (hue / 120) {
        case 0:
            rgb.red = first_color;
            rgb.green = second_color;
            break;
        case 1:
            rgb.green = first_color;
            rgb.blue = second_color;
            break;
        case 2:
            rgb.blue = first_color;
            rgb.red = second_color;
    }

    return rgb;
}


void set_brightness(uint8_t brightness, union rgb *rgb)
{
    if(!rgb){
        return;
    }

    if(MAX_BRIGHTNESS < brightness){
        brightness = MAX_BRIGHTNESS;
    }

    for(unsigned int i = 0; i < ARRAY_SIZE(rgb->components); i++) {
        if(rgb->components[i]){
            rgb->components[i] = ((float)rgb->components[i] / MAX_BRIGHTNESS) * brightness;
        }
    }
}


void set_saturation(uint8_t saturation, union rgb *rgb)
{
    uint16_t *max_val_ptr = MAX_VAL_PTR(&rgb->components[0], MAX_VAL_PTR(&rgb->components[1], &rgb->components[2]));
    unsigned int max_val_idx = (max_val_ptr - &rgb->components[0]) / sizeof(rgb->components[0]);
    unsigned int zero_val_idx = 0;
    unsigned int mid_val_idx = 0;
    unsigned int zero_comp_num = 0;
    uint16_t satur_val = 0;

    float steps[ARRAY_SIZE(rgb->components)] = {0.0};

    for(unsigned int i = 0; i < ARRAY_SIZE(rgb->components); i++) {
        if(0 == rgb->components[i]) {
            zero_comp_num++;
            zero_val_idx = i;
        }
    }

    if(1 < zero_comp_num) {
        satur_val = *max_val_ptr;
        for(unsigned int i = 0; i < ARRAY_SIZE(rgb->components); i++) {
            if(i != max_val_idx) {
                steps[i] = satur_val / MAX_SATURATION;
            }
        }
    }
    else {
        mid_val_idx = 3 - (zero_val_idx + max_val_idx);
        satur_val = (rgb->components[max_val_idx] + rgb->components[mid_val_idx]) / 2;
        steps[max_val_idx] = ((float)rgb->components[max_val_idx] - satur_val) / MAX_SATURATION;
        steps[mid_val_idx] = ((float)satur_val - rgb->components[mid_val_idx]) / MAX_SATURATION;
        steps[zero_val_idx] = (float)satur_val / MAX_SATURATION;
    }

    rgb->components[max_val_idx] -= steps[max_val_idx] * (MAX_SATURATION - saturation);
    rgb->components[mid_val_idx] += steps[mid_val_idx] * (MAX_SATURATION - saturation);
    rgb->components[zero_val_idx] = steps[zero_val_idx] * (MAX_SATURATION - saturation);
}