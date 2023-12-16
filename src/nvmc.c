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

    bool memory_empty = true;

    // проверяем память на наличие данных
    for (int i = 0; i < 3; i++) {
        if (*(p_addr + i) != 0xFFFFFFFF) {
            memory_empty = false;
            break;
        }
    }

    if (memory_empty) {
        // если память пуста, записываем дефолтный цвет и сразу считываем его
        const uint32_t default_values[3] = {292, 100, 100};
        nrfx_nvmc_page_erase(f_addr);
        nrfx_nvmc_words_write(f_addr, default_values, 3);

        for (int i = 0; i < 3; i++) {
            if (i == 0) {
                data->hue = default_values[i];
            } else if (i == 1) {
                data->saturation = default_values[i];
            } else if (i == 2) {
                data->value = default_values[i];
            }
        }
    } else {
        // иначе считываем цвет
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
}

