#include "dht.h"
#include "driver/gpio.h"
#include "esp_event.h"
#include "esp_http_server.h"
#include "esp_log.h"
#include "esp_netif.h"
#include "esp_wifi.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "nvs_flash.h"

#include <stdio.h>

#define DHT_GPIO GPIO_NUM_4

// WiFi credentials are now defined at compile time via CMakeLists.txt
#ifndef WIFI_SSID
    #error "WIFI_SSID not defined. Please set environment variable and rebuild."
#endif

#ifndef WIFI_PASS
    #error "WIFI_PASS not defined. Please set environment variable and rebuild."
#endif

static const char *TAG = "main";

// Serve sensor data as a webpage
esp_err_t data_get_handler(httpd_req_t *req)
{
    int16_t temp, hum;
    char resp[200];
    esp_err_t ret;

    ESP_LOGI(TAG, "Attempting to read DHT11 sensor...");

    // Try reading sensor multiple times if it fails
    for(int attempts = 0; attempts < 3; attempts++) {
        ret = dht_read_data(DHT_TYPE_DHT11, DHT_GPIO, &hum, &temp);
        if(ret == ESP_OK) {
            snprintf(resp, sizeof(resp),
                     "{\"temperature\":\"%.1f°C\", "
                     "\"humidity\":\"%.1f%%\"}",
                     temp / 10.0, hum / 10.0);
            ESP_LOGI(TAG, "DHT11 read successful on attempt %d: T=%.1f°C, H=%.1f%%", attempts + 1,
                     temp / 10.0, hum / 10.0);
            break;
        } else {
            ESP_LOGW(TAG, "DHT11 read failed on attempt %d", attempts + 1);
            if(attempts < 2) {
                vTaskDelay(pdMS_TO_TICKS(2000));  // Wait 2 seconds before retry
            }
        }
    }

    if(ret != ESP_OK) {
        snprintf(resp, sizeof(resp),
                 "{\"error\":\"Failed to read DHT11 sensor after 3 attempts\"}");
        ESP_LOGE(TAG, "All DHT11 read attempts failed");
    }

    httpd_resp_send(req, resp, HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}

httpd_handle_t start_webserver(void)
{
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    httpd_handle_t server = NULL;

    if(httpd_start(&server, &config) == ESP_OK) {
        httpd_uri_t data_uri = {
            .uri = "/", .method = HTTP_GET, .handler = data_get_handler, .user_ctx = NULL};
        httpd_register_uri_handler(server, &data_uri);
    }
    return server;
}

void wifi_init(void)
{
    esp_netif_init();
    esp_event_loop_create_default();
    esp_netif_create_default_wifi_sta();

    wifi_init_config_t wifi_cfg = WIFI_INIT_CONFIG_DEFAULT();
    esp_wifi_init(&wifi_cfg);
    esp_wifi_set_mode(WIFI_MODE_STA);

    wifi_config_t sta_config = {
        .sta =
            {
                .ssid = WIFI_SSID,
                .password = WIFI_PASS,
                .threshold.authmode = WIFI_AUTH_WPA2_PSK,
            },
    };

    esp_wifi_set_config(ESP_IF_WIFI_STA, &sta_config);
    esp_wifi_start();
    esp_wifi_connect();
}

void app_main(void)
{
    ESP_ERROR_CHECK(nvs_flash_init());

    // Initialize GPIO for DHT11
    ESP_LOGI(TAG, "Initializing DHT11 sensor on GPIO %d", DHT_GPIO);

    wifi_init();

    // Wait a bit for WiFi to stabilize
    vTaskDelay(pdMS_TO_TICKS(2000));

    start_webserver();
    ESP_LOGI(TAG, "Web server started");

    // Test sensor reading once at startup
    int16_t test_temp, test_hum;
    if(dht_read_data(DHT_TYPE_DHT11, DHT_GPIO, &test_hum, &test_temp) == ESP_OK) {
        ESP_LOGI(TAG, "Initial DHT11 test successful: T=%.1f°C, H=%.1f%%", test_temp / 10.0,
                 test_hum / 10.0);
    } else {
        ESP_LOGW(TAG, "Initial DHT11 test failed - sensor may need time to stabilize");
    }
}
