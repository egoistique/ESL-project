#include <math.h>
#include "hsv.h"
#include "nordic_common.h"


#define PWM_COUNT_TOP 1200
#define PWM_COLOR_STEP (PWM_COUNT_TOP / 120.0f)

#define MAX_VAL_PTR(a, b) ((*(a) > *(b)) ? a : b)

 struct hsv hsv_color = {
    .hue = 292,
    .saturation = 100,
    .brightness = 100
};

void hsv_to_rgb(struct hsv color, union rgb *rgb_color)
{
    float s = color.saturation / 100.0f;
    float v = color.brightness / 100.0f;

    float c = s * v;

    float x = c * (1 - fabs(fmod((color.hue / 60.0), 2.0) - 1));

    float m = v - c;

    float r = 0;
    float g = 0;
    float b = 0;

    switch (color.hue / 60) {
        case 0: 
            r = c; g = x; b = 0;
            break;
        case 1: 
            r = x; g = c; b = 0;
            break;
        case 2:
            r = 0; g = c; b = x;
            break;
        case 3:
            r = 0; g = x; b = c;
            break;
        case 4:
            r = x; g = 0; b = c;
            break;
        case 5:
            r = c; g = 0; b = x;
            break;
        default:
            break;
    }

    rgb_color->red = (r + m) * PWM_COUNT_TOP;
    rgb_color->green = (g + m) * PWM_COUNT_TOP;
    rgb_color->blue = (b + m) * PWM_COUNT_TOP;
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