#include <stdint.h>
#include <stdint.h>
#include "states.h"

#define MAX_VALUE 100
#define MAX_SATURATION 100

struct RGB {
    uint16_t red;
    uint16_t green;
    uint16_t blue;
};

void rgb2hsv(struct hsv *hsv_color, struct RGB *color_rgb);
void hsv2rgb(struct hsv color_hsv, struct RGB *color_rgb);








