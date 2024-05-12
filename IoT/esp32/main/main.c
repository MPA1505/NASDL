#include "wifi.h"
#include "led_blink.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

void app_main(void) {
    // Initialize LEDs
    init_LED();

    // Initialize Wi-Fi
    turn_on_blue_LED();
    vTaskDelay(pdMS_TO_TICKS(2000));

	wifi_init();

    // Check Wi-Fi connection status
    bool wifi_connected = false;
    while (!wifi_connected) {
        wifi_connected = check_wifi_connected();
        if (wifi_connected) {
            turn_on_green_LED();
        } else {
            turn_on_red_LED();
            vTaskDelay(pdMS_TO_TICKS(1000));
        }
    }

}
