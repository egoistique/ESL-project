#include <stdint.h>
#include <stdint.h>

#define MAX_BRIGHTNESS 100
#define MAX_SATURATION 100

// typedef uint16_t hue_t;

// struct hsv{
//     hue_t hue;          
//     uint8_t saturation; 
//     uint8_t brightness; 
// };

struct hsv hsv_color;

union rgb {
    uint16_t components[3];
    struct {
        uint16_t red;
        uint16_t green;
        uint16_t blue;
    };
};
//void process_hsv_state(struct hsv *color, struct hsv_control_state *state);

void hsv_to_rgb(struct hsv color, union rgb *rgb_color);
void set_hue(struct hsv *color);
//void set_saturation(struct hsv *color, struct hsv_control_state *state);



