#include "wifi.h"
#include "logging.h"
#include "led_blink.h"
#include "mqtt.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <stdbool.h>

bool locked = true;

void set_lock(bool lock_door) {
    locked = lock_door;
}

void LED_indicate_door_status(void){
    if (locked) {
        turn_on_red_LED();
    } else {
        turn_on_green_LED();
    }
}

void wifi_status_handler(bool connected) {
    if (!connected) {
        turn_on_blue_LED();
    } 
    vTaskDelay(pdMS_TO_TICKS(500));
    LED_indicate_door_status();
}

void app_main(void) {
    // Initialize Logging
    init_logging();

    // Initialize LEDs
    init_LED();
    door_register_status_callback(set_lock);

    // Initialize Wi-Fi
    turn_on_blue_LED();
    vTaskDelay(pdMS_TO_TICKS(2000));

    wifi_register_status_callback(wifi_status_handler);
	wifi_init();

    LED_indicate_door_status();

    // Initialize MQTT
    mqtt_init();
}
