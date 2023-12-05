#include <stdint.h>
#include <stdint.h>

#define MAX_VALUE 100
#define MAX_SATURATION 100

struct hsv hsv_color;

union rgb {
    uint16_t components[3];
    struct {
        uint16_t red;
        uint16_t green;
        uint16_t blue;
    };
};

void hsv_to_rgb(struct hsv color, union rgb *rgb_color);
//void set_hue(struct hsv *color);



