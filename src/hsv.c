#include <math.h>
#include "hsv.h"
#include "nordic_common.h"
#include "my_button.h"
#include "pwm.h"
#include "states.h"


#define PWM_COUNT_TOP 1200
#define PWM_COLOR_STEP (PWM_COUNT_TOP / 120.0f)

#define MAX_VAL_PTR(a, b) ((*(a) > *(b)) ? a : b)

 struct hsv hsv_color = {
    .hue = 100,
    .saturation = 100,
    .value = 100
};


// void set_hue(struct hsv *color){
//     if(360 <= ++color->hue) {
//         color->hue = 0;
//      }
// }

void hsv_to_rgb(struct hsv color, union rgb *rgb_color)
{
    float s = color.saturation / 100.0f;
    float v = color.value / 100.0f;

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

