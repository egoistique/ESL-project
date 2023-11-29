#include "set_color.h"

// void set_hue(struct hsv *color){
//     if(360 <= ++color->hue) {
//         color->hue = 0;
//      }
// }

// void set_saturation(struct hsv *color, struct hsv_control_state *state){
//     if(state->saturation_direction) {
//          if(MAX_SATURATION <= ++color->saturation) {
//                         state->saturation_direction = false;
//                     }
//                 }
//                 else {
//                     if(0 >= --color->saturation) {
//                         state->saturation_direction = true;
//                     }
//      }
// }

// void process_hsv_state(struct hsv *color, struct hsv_control_state *state)
// {
//     if(SINGLE_CLICK_PRESSED == state->button->button_state) {
//         switch(app_state) {
//             case HUE_MODE:
//                 // if(360 <= ++color->hue) {
//                 //     color->hue = 0;
//                 // }
//                  break;
//             case SATURATION_MODE:
//                 // if(state->saturation_direction) {
//                 //     if(MAX_SATURATION <= ++color->saturation) {
//                 //         state->saturation_direction = false;
//                 //     }
//                 // }
//                 // else {
//                 //     if(0 >= --color->saturation) {
//                 //         state->saturation_direction = true;
//                 //     }
//                 // }
//                 break;
//             case VALUE_MODE: 
//                 if(state->brightness_direction) {
//                     if(MAX_SATURATION <= ++color->brightness) {
//                         state->brightness_direction = false;
//                     }
//                 }
//                 else {
//                     if(0 >= --color->brightness) {
//                         state->brightness_direction = true;
//                     }
//                 }
//                 break;
//             default:
//                 break;
//         }
//     }

//     union rgb the_color = { .components = {0} };

//     hsv_to_rgb(hsv_color, &the_color);

//     pwm_values.channel_1 = the_color.red;
//     pwm_values.channel_2 = the_color.green;
//     pwm_values.channel_3 = the_color.blue;
// }
