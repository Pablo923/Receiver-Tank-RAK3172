#ifndef HOMEASSISTANT_MQTT_H
#define HOMEASSISTANT_MQTT_H

#include "esp_err.h"

esp_err_t homeassistant_mqtt_start(void);
void homeassistant_mqtt_publish_measurements(float level_liters, int rssi_dbm, int snr_db);

#endif
