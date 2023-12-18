#include <math.h>
#include "hsv.h"
#include "nordic_common.h"
#include "my_button.h"
#include "pwm.h"
#include "states.h"

#define PWM_MAX 1000


void rgb2hsv(struct hsv *hsv_color, struct RGB *color_rgb) {
    // Нормализация значений R, G, B
    float red = color_rgb->red / (float)PWM_MAX;
    float green = color_rgb->green / (float)PWM_MAX;
    float blue = color_rgb->blue / (float)PWM_MAX;

    float max_color = fmax(red, fmax(green, blue));
    float min_color = fmin(red, fmin(green, blue));

    // Вычисление значения яркости (value)
    hsv_color->value = max_color * 100;

    // Если цвета равны, значит оттенок (hue) равен 0
    if (max_color == min_color) {
        hsv_color->hue = 0;
        hsv_color->saturation = 0;
        return;
    }

    // Вычисление насыщенности (saturation)
    float delta = max_color - min_color;
    hsv_color->saturation = (delta / max_color) * 100;

    // Вычисление оттенка (hue)
    float hue;
    if (max_color == red) {
        hue = 60 * fmod(((green - blue) / delta), 6);
    } else if (max_color == green) {
        hue = 60 * (((blue - red) / delta) + 2);
    } else {
        hue = 60 * (((red - green) / delta) + 4);
    }

    // Обработка отрицательного значения оттенка
    if (hue < 0) {
        hue += 360;
    }

    hsv_color->hue = hue;
}


void hsv2rgb(struct hsv color_hsv, struct RGB *color_rgb)
{
    // Convert saturation and value to floats
    float saturation = color_hsv.saturation / 100.0f;
    float value = color_hsv.value / 100.0f;
    // Calculate chroma
    float chroma = saturation * value;
    float a = color_hsv.hue / 60.0;
    float x = chroma * (1 - fabs(fmod((a), 2.0) - 1));
    float m = value - chroma;

    // Initialize r, g, b to 0
    float r = 0;
    float g = 0;
    float b = 0;

    // Determine which hue range to use
    int hue_range = color_hsv.hue / 60;

    // Set r, g, b based on hue range
    switch (hue_range) {
        case 0: 
            r = chroma; 
            g = x; 
            break;
        case 1: 
            r = x; 
            g = chroma; 
            break;
        case 2:
            g = chroma; 
            b = x; 
            break;
        case 3:
            g = x; 
            b = chroma; 
            break;
        case 4:
            r = x; 
            b = chroma; 
            break;
        case 5:
            r = chroma; 
            b = x; 
            break;
        default:
            break;
    }

    // Set RGB values and scale by PWM_MAX
    color_rgb->red = (r + m) * PWM_MAX;
    color_rgb->green = (g + m) * PWM_MAX;
    color_rgb->blue = (b + m) * PWM_MAX;
}



