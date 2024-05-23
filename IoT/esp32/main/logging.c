#include "wifi.h"
#include <esp_wifi.h>
#include <esp_system.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "esp_timer.h"

uint64_t startUpAt = 0;

// Function prototypes
void heartbeat_loop(void *parameter);
void mqtt_publish_log(const char* msg);
char* getResourceUsageLog();
char* getNetworkActivityLog();

// Functions to capture system logs
void init_logging() {
  startUpAt = esp_timer_get_time() / 1000;
  xTaskCreate(heartbeat_loop, "heartbeat_loop", 4096, NULL, 5, NULL);
}

char* getResourceUsageLog() {
  uint32_t freeHeap = esp_get_free_heap_size();
  uint32_t minFreeHeap = esp_get_minimum_free_heap_size();
  char* buffer = (char*)malloc(128);
  if (buffer != NULL) {
    snprintf(buffer, 128, "Free Heap: %lu bytes, Min Free Heap: %lu bytes", (unsigned long)freeHeap, (unsigned long)minFreeHeap);
  }
  return buffer;
}

char* log_network_activity() {
  return get_network_activity();
}

void heartbeat_loop(void *parameter) {
  char msg[512];
  while (true)
  {
    uint64_t millis_now = esp_timer_get_time() / 1000;
    snprintf(msg, sizeof(msg), "Heartbeat at %llu ms\n", millis_now);

    char* resource_usage = getResourceUsageLog();
    if (resource_usage != NULL) {
      strncat(msg, resource_usage, sizeof(msg) - strlen(msg) - 1);
      strncat(msg, "\n", sizeof(msg) - strlen(msg) - 1);
      free(resource_usage);
    }

    char* network_activity = log_network_activity();
    if (network_activity != NULL) {
      strncat(msg, network_activity, sizeof(msg) - strlen(msg) - 1);
      free(network_activity);
    }

    mqtt_publish_log(msg);

    vTaskDelay(pdMS_TO_TICKS(10000));
  }
}

