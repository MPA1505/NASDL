#include <stdio.h>
#include <inttypes.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "driver/gpio.h"
#include <hal/gpio_types.h>

#define PIN_GREEN 23
#define PIN_BLUE 22
#define PIN_RED 21

static const char* TAG = "LED_BLINK_TASK";

void turn_on_red_LED(void)
{
    uint8_t on = 0;

    // Turn off green LED
    gpio_set_level(PIN_GREEN, !on);
    ESP_LOGI(TAG, "Green LED turned off");

    // Turn off blue LED
    gpio_set_level(PIN_BLUE, !on);
    ESP_LOGI(TAG, "Blue LED turned off");

    // Toggle red LED
    gpio_set_level(PIN_RED, on);
    ESP_LOGI(TAG, "Red LED state set to: %d", on);
}

void turn_on_green_LED(void)
{
    uint8_t on = 0;

    // Turn off red LED
    gpio_set_level(PIN_RED, !on);
    ESP_LOGI(TAG, "RED LED turned off");

    // Turn off blue LED
    gpio_set_level(PIN_BLUE, !on);
    ESP_LOGI(TAG, "Blue LED turned off");

    // Toggle green LED
    gpio_set_level(PIN_GREEN, on);
    ESP_LOGI(TAG, "Green LED state set to: %d", on);
}

void turn_on_blue_LED(void)
{
    uint8_t on = 0;

    // Turn off red LED
    gpio_set_level(PIN_RED, !on);
    ESP_LOGI(TAG, "RED LED turned off");

    // Turn off green LED
    gpio_set_level(PIN_GREEN, !on);
    ESP_LOGI(TAG, "Green LED turned off");

    // Toggle blue LED
    gpio_set_level(PIN_BLUE, on);
    ESP_LOGI(TAG, "Blue LED state set to: %d", on);
}

void init_LED(void)
{
    gpio_reset_pin(PIN_GREEN);
    gpio_set_direction(PIN_GREEN, GPIO_MODE_OUTPUT);
    gpio_reset_pin(PIN_BLUE);
    gpio_set_direction(PIN_BLUE, GPIO_MODE_OUTPUT);
    gpio_reset_pin(PIN_RED);
    gpio_set_direction(PIN_RED, GPIO_MODE_OUTPUT);
}
