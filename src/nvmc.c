#include "nvmc.h"
#include <stdint.h>

void save_data_to_nvm(struct hsv *data) {
    uint32_t f_addr = 0x0007f000;
    
    const uint32_t k[3] = {data->hue, data->saturation, data->value};
    
    nrfx_nvmc_page_erase(f_addr);
    nrfx_nvmc_words_write(f_addr, k, 3);
}

void read_data_from_nvm(struct hsv *data) {
    uint32_t f_addr = 0x0007f000;
    uint32_t *p_addr = (uint32_t *)f_addr;

    for (int i = 0; i < 3; i++) {
        if (i == 0) {
            data->hue = *(p_addr + i);
        } else if (i == 1) {
            data->saturation = *(p_addr + i);
        } else if (i == 2) {
            data->value = *(p_addr + i);
        }
    }
}