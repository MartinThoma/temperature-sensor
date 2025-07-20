#include "dht11.h"

#include "driver/gpio.h"
#include "esp_log.h"
#include "esp_rom_sys.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "soc/rtc_cntl_reg.h"
#include "soc/soc.h"

#define TAG "DHT11"

#define DHT_MAX_TIMINGS 85

esp_err_t dht11_read(int gpio, int *temperature, int *humidity)
{
    uint8_t data[5] = {0, 0, 0, 0, 0};
    int i, j = 0;
    int high_time, low_time;

    // Initialize GPIO
    gpio_config_t io_config = {.pin_bit_mask = (1ULL << gpio),
                               .mode = GPIO_MODE_OUTPUT,
                               .pull_up_en = GPIO_PULLUP_ENABLE,
                               .pull_down_en = GPIO_PULLDOWN_DISABLE,
                               .intr_type = GPIO_INTR_DISABLE};
    gpio_config(&io_config);

    // Disable interrupts during critical timing section
    portDISABLE_INTERRUPTS();

    // Start signal: pull low for 18ms
    gpio_set_level(gpio, 0);
    esp_rom_delay_us(18000);  // 18ms delay

    // Pull high for 20-40us
    gpio_set_level(gpio, 1);
    esp_rom_delay_us(30);

    // Switch to input
    io_config.mode = GPIO_MODE_INPUT;
    gpio_config(&io_config);

    // Wait for DHT11 to pull low (response signal)
    int timeout = 0;
    while(gpio_get_level(gpio) == 1) {
        esp_rom_delay_us(1);
        timeout++;
        if(timeout > 100) {
            portENABLE_INTERRUPTS();
            ESP_LOGE(TAG, "DHT11 no response");
            return ESP_FAIL;
        }
    }

    // Wait for DHT11 to pull high
    timeout = 0;
    while(gpio_get_level(gpio) == 0) {
        esp_rom_delay_us(1);
        timeout++;
        if(timeout > 100) {
            portENABLE_INTERRUPTS();
            ESP_LOGE(TAG, "DHT11 response timeout 1");
            return ESP_FAIL;
        }
    }

    // Wait for DHT11 to pull low (end of response)
    timeout = 0;
    while(gpio_get_level(gpio) == 1) {
        esp_rom_delay_us(1);
        timeout++;
        if(timeout > 100) {
            portENABLE_INTERRUPTS();
            ESP_LOGE(TAG, "DHT11 response timeout 2");
            return ESP_FAIL;
        }
    }

    // Read 40 bits of data
    for(i = 0; i < 40; i++) {
        // Wait for start of data bit (low to high transition)
        timeout = 0;
        while(gpio_get_level(gpio) == 0) {
            esp_rom_delay_us(1);
            timeout++;
            if(timeout > 100) {
                portENABLE_INTERRUPTS();
                ESP_LOGE(TAG, "Data bit %d timeout waiting for high", i);
                return ESP_FAIL;
            }
        }

        // Measure high time to determine if bit is 0 or 1
        high_time = 0;
        while(gpio_get_level(gpio) == 1) {
            esp_rom_delay_us(1);
            high_time++;
            if(high_time > 100) {
                portENABLE_INTERRUPTS();
                ESP_LOGE(TAG, "Data bit %d timeout during high", i);
                return ESP_FAIL;
            }
        }

        // Store the bit
        data[i / 8] <<= 1;
        if(high_time > 30) {  // > 30us means '1', <= 30us means '0'
            data[i / 8] |= 1;
        }
    }

    // Re-enable interrupts
    portENABLE_INTERRUPTS();

    // Print received data for debugging
    ESP_LOGI(TAG, "Data received: [0]=%02X [1]=%02X [2]=%02X [3]=%02X [4]=%02X", data[0], data[1],
             data[2], data[3], data[4]);

    // Verify checksum
    uint8_t checksum = (data[0] + data[1] + data[2] + data[3]) & 0xFF;
    if(data[4] != checksum) {
        ESP_LOGE(TAG, "Checksum failed: calculated=0x%02X, received=0x%02X", checksum, data[4]);
        return ESP_FAIL;
    }

    // Extract temperature and humidity
    *humidity = data[0];
    *temperature = data[2];

    ESP_LOGI(TAG, "DHT11 read successful: T=%d°C, H=%d%%", *temperature, *humidity);
    return ESP_OK;
}
