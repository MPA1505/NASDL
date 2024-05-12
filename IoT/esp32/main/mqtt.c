#include <stdbool.h>
#include "esp_log.h"
#include "mqtt_client.h"
#include <inttypes.h>
#include "sdkconfig.h"
#include <time.h>
#include <string.h>
#include "led_blink.h"

#define MQTT_BROKER CONFIG_MQTT_BROKER
#define MQTT_COMMAND_TOPIC CONFIG_MQTT_COMMAND_TOPIC
#define MQTT_RESPONSE_TOPIC CONFIG_MQTT_RESPONSE_TOPIC

static esp_mqtt_client_handle_t client = NULL;
static const char *TAG = "MQTT";

static void log_error_if_nonzero(const char *message, int error_code)
{
    if (error_code != 0) {
        ESP_LOGE(TAG, "Last error %s: 0x%x", message, error_code);
    }
}

// Helper function to parse ISO 8601 formatted date-time and convert it to time_t
static time_t parse_iso8601(const char *timestamp) {
    struct tm tm;
    strptime(timestamp, "%Y-%m-%dT%H:%M:%S", &tm);
    time_t t = mktime(&tm);  // Convert tm structure to time_t
    return t;
}

void handle_command(char *data) {
    char *token = strtok(data, ",");
    if (token && strcmp(token, "unlock") == 0) {
        token = strtok(NULL, ",");
        if (token) {
            time_t command_time = parse_iso8601(token);
            time_t current_time = time(NULL);  // Current time in seconds since the epoch
            if (difftime(current_time, command_time) <= 10) {  // Check if the difference is less than or equal to 10 seconds
                // Command is recent, within the last 10 seconds
                int msg_id = esp_mqtt_client_publish(client, MQTT_RESPONSE_TOPIC, "unlocked", 0, 0, 0);
                ESP_LOGI(TAG, "sent publish unlocked, msg_id=%d", msg_id);
                turn_on_green_LED();

                // Wait for 10 seconds
                vTaskDelay(10000 / portTICK_PERIOD_MS);

                // Then publish "locked"
                msg_id = esp_mqtt_client_publish(client, MQTT_RESPONSE_TOPIC, "locked", 0, 0, 0);
                ESP_LOGI(TAG, "sent publish locked, msg_id=%d", msg_id);
                turn_on_red_LED();
            }
        }
    }
}

static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
{
    ESP_LOGD(TAG, "Event dispatched from event loop base=%s, event_id=%" PRIi32 "", base, event_id);
    esp_mqtt_event_handle_t event = event_data;
    esp_mqtt_client_handle_t client = event->client;
    int msg_id;
    switch ((esp_mqtt_event_id_t)event_id) {
    case MQTT_EVENT_CONNECTED:
        ESP_LOGI(TAG, "MQTT_EVENT_CONNECTED");
        msg_id = esp_mqtt_client_subscribe(client, MQTT_COMMAND_TOPIC, 0);
        ESP_LOGI(TAG, "sent subscribe successful, msg_id=%d", msg_id);
        break;
    case MQTT_EVENT_DISCONNECTED:
        ESP_LOGI(TAG, "MQTT_EVENT_DISCONNECTED. Attempting reconnect...");
        vTaskDelay(pdMS_TO_TICKS(500));
        esp_mqtt_client_reconnect(client);
        break;
    case MQTT_EVENT_SUBSCRIBED:
        ESP_LOGI(TAG, "MQTT_EVENT_SUBSCRIBED, msg_id=%d", event->msg_id);
        break;
    case MQTT_EVENT_UNSUBSCRIBED:
        ESP_LOGI(TAG, "MQTT_EVENT_UNSUBSCRIBED, msg_id=%d", event->msg_id);
        break;
    case MQTT_EVENT_PUBLISHED:
        ESP_LOGI(TAG, "MQTT_EVENT_PUBLISHED, msg_id=%d", event->msg_id);
        break;
    case MQTT_EVENT_DATA:
        ESP_LOGI(TAG, "MQTT_EVENT_DATA");
        printf("TOPIC=%.*s\r\n", event->topic_len, event->topic);
        printf("DATA=%.*s\r\n", event->data_len, event->data);
        if (event->topic_len && strncmp(event->topic, MQTT_COMMAND_TOPIC, event->topic_len) == 0) {
            char *command = strndup(event->data, event->data_len);
            handle_command(command);
            free(command);
        }
        break;
    case MQTT_EVENT_ERROR:
        ESP_LOGI(TAG, "MQTT_EVENT_ERROR");
        if (event->error_handle->error_type == MQTT_ERROR_TYPE_TCP_TRANSPORT) {
            log_error_if_nonzero("reported from esp-tls", event->error_handle->esp_tls_last_esp_err);
            log_error_if_nonzero("reported from tls stack", event->error_handle->esp_tls_stack_err);
            log_error_if_nonzero("captured as transport's socket errno",  event->error_handle->esp_transport_sock_errno);
            ESP_LOGI(TAG, "Last errno string (%s)", strerror(event->error_handle->esp_transport_sock_errno));

        }
        break;
    default:
        ESP_LOGI(TAG, "Other event id:%d", event->event_id);
        break;
    }
}

void mqtt_init(void)
{
    esp_mqtt_client_config_t mqtt_cfg = {
        .broker.address.uri = MQTT_BROKER,
    };

    client = esp_mqtt_client_init(&mqtt_cfg);

    esp_mqtt_client_register_event(client, ESP_EVENT_ANY_ID, mqtt_event_handler, NULL);
    esp_mqtt_client_start(client);
}