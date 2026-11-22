#ifndef HOMEASSISTANT_MQTT_H
#define HOMEASSISTANT_MQTT_H

#include "esp_err.h"
#include "RAK3172.h"

esp_err_t homeassistant_mqtt_start(void);
void homeassistant_mqtt_publish_measurements(const RAK3172_Message_t *msg);

#endif
