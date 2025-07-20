#pragma once

#include "esp_err.h"

esp_err_t dht11_read(int gpio, int *temperature, int *humidity);
