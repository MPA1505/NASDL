#include <stdbool.h>
#ifndef MQTT_H
#define MQTT_H

void mqtt_init(void);
bool check_if_unlock_door(void);
void mqtt_publish_log(const char *message);

#endif
