#include <stdint.h>

#include <stdint.h>

#define MAX_BRIGHTNESS 100
#define MAX_SATURATION 100

typedef uint16_t hue_t;

struct hsv{
    hue_t hue;          
    uint8_t saturation; 
    uint8_t brightness; 
};

union rgb {
    uint16_t components[3];
    struct {
        uint16_t red;
        uint16_t green;
        uint16_t blue;
    };
};

void hsv_to_rgb(struct hsv color, union rgb *rgb_color);



