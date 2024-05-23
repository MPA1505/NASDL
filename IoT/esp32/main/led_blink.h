#ifndef LED_BLINK_H
#define LED_BLINK_H

void turn_on_red_LED(void);
void turn_on_green_LED(void);
void turn_on_blue_LED(void);
void init_LED(void);

typedef void (*door_status_callback_t)(bool locked);

void door_register_status_callback(door_status_callback_t callback);
void set_lock_status(bool locked);

#endif
