#include "nvmc.h"
#include <stdint.h>

void save_data_to_nvm(struct hsv *data) {
    uint32_t f_addr = 0x0007f000;
    
    // Добавляем фиктивное значение в начало данных
    const uint32_t fake_value = 0xFFFFFFFF;
    const uint32_t k[4] = {fake_value, data->hue, data->saturation, data->value};

    // Записываем данные в память
    nrfx_nvmc_page_erase(f_addr);
    nrfx_nvmc_words_write(f_addr, k, 4);
}


void read_data_from_nvm(struct hsv *data) {
    uint32_t f_addr = 0x0007f000;
    uint32_t *p_addr = (uint32_t *)f_addr;

    // Проверяем, пуста ли память (проверка первого значения)
    bool memory_empty = (*p_addr == 0xFFFFFFFF); // Используем фиктивное значение для проверки

    if (memory_empty) {
        // Если память пуста, сохраняем дефолтные данные
        save_data_to_nvm(&hsv_color);
    } else {
        // Если данные уже сохранены, читаем из памяти
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
