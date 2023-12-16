
#include <stdint.h>
#include <string.h>
#include "nrfx_nvmc.h"
#include "nrf_dfu_types.h"
#include "app_util.h"
#include "states.h"

void save_data_to_nvm(struct hsv *data);
void read_data_from_nvm(struct hsv *data);
