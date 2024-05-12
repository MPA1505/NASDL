#include <stdio.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "driver/gpio.h"

#define DELAY_MS 1000
#define PIN 21
static const char* TAG = "LED_BLINK_TASK";

void TaskBlink(void *pvParameters)
{
    uint8_t state = 1;
    while (1)
    {
        gpio_set_level(PIN, state);
        ESP_LOGI(TAG, "LED state set to: %d", state);
        vTaskDelay(pdMS_TO_TICKS(DELAY_MS));
        state = !state;
    }
}

void app_main(void)
{
    gpio_reset_pin(PIN);
    gpio_set_direction(PIN, GPIO_MODE_OUTPUT);

    ESP_LOGI(TAG, "Starting LED blink task on pin %d", PIN);
    xTaskCreate(TaskBlink, "BlinkLED", 2048, NULL, 1, NULL);
}
