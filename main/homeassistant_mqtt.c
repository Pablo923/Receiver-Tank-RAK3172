#include <stdbool.h>
#include <stdio.h>

#include "esp_event.h"
#include "esp_log.h"
#include "mqtt_client.h"
#include "homeassistant_mqtt.h"

#define MQTT_STATE_TOPIC       "estanque/nivel/state"
#define MQTT_RSSI_STATE_TOPIC  "estanque/rssi/state"
#define MQTT_SNR_STATE_TOPIC   "estanque/snr/state"
#define MQTT_AVAILABILITY_TOPIC "estanque/nivel/status"
#define HA_DISCOVERY_TOPIC     "homeassistant/sensor/receiver_tank_rak3172/nivel/config"
#define HA_RSSI_DISCOVERY_TOPIC "homeassistant/sensor/receiver_tank_rak3172/rssi/config"
#define HA_SNR_DISCOVERY_TOPIC  "homeassistant/sensor/receiver_tank_rak3172/snr/config"

static const char *TAG = "[HA_MQTT]";
static esp_mqtt_client_handle_t s_mqtt_client;
static volatile bool s_mqtt_connected;

static const char s_discovery_payload[] =
    "{\"name\":\"Nivel del estanque\","
    "\"unique_id\":\"receiver_tank_level\","
    "\"state_topic\":\"" MQTT_STATE_TOPIC "\","
    "\"availability_topic\":\"" MQTT_AVAILABILITY_TOPIC "\","
    "\"payload_available\":\"online\","
    "\"payload_not_available\":\"offline\","
    "\"unit_of_measurement\":\"L\","
    "\"state_class\":\"measurement\","
    "\"suggested_display_precision\":2,"
    "\"device\":{"
        "\"identifiers\":[\"receiver_tank_rak3172\"],"
        "\"name\":\"Receptor del estanque\","
        "\"manufacturer\":\"Espressif\","
        "\"model\":\"ESP32-C6 + RAK3172\""
    "}}";

static const char s_rssi_discovery_payload[] =
    "{\"name\":\"RSSI LoRa\","
    "\"unique_id\":\"receiver_tank_rssi\","
    "\"state_topic\":\"" MQTT_RSSI_STATE_TOPIC "\","
    "\"availability_topic\":\"" MQTT_AVAILABILITY_TOPIC "\","
    "\"payload_available\":\"online\","
    "\"payload_not_available\":\"offline\","
    "\"unit_of_measurement\":\"dBm\","
    "\"state_class\":\"measurement\","
    "\"suggested_display_precision\":0,"
    "\"device\":{"
        "\"identifiers\":[\"receiver_tank_rak3172\"],"
        "\"name\":\"Receptor del estanque\","
        "\"manufacturer\":\"Espressif\","
        "\"model\":\"ESP32-C6 + RAK3172\""
    "}}";

static const char s_snr_discovery_payload[] =
    "{\"name\":\"SNR LoRa\","
    "\"unique_id\":\"receiver_tank_snr\","
    "\"state_topic\":\"" MQTT_SNR_STATE_TOPIC "\","
    "\"availability_topic\":\"" MQTT_AVAILABILITY_TOPIC "\","
    "\"payload_available\":\"online\","
    "\"payload_not_available\":\"offline\","
    "\"unit_of_measurement\":\"dB\","
    "\"state_class\":\"measurement\","
    "\"suggested_display_precision\":0,"
    "\"device\":{"
        "\"identifiers\":[\"receiver_tank_rak3172\"],"
        "\"name\":\"Receptor del estanque\","
        "\"manufacturer\":\"Espressif\","
        "\"model\":\"ESP32-C6 + RAK3172\""
    "}}";

static void enqueue_message(const char *topic, const char *payload, int qos)
{
    int message_id = esp_mqtt_client_enqueue(
        s_mqtt_client, topic, payload, 0, qos, true, false);

    if (message_id < 0) {
        ESP_LOGW(TAG, "Could not queue MQTT message for topic %s", topic);
    }
}

static void mqtt_event_handler(void *handler_args,
                               esp_event_base_t base,
                               int32_t event_id,
                               void *event_data)
{
    (void)handler_args;
    (void)base;
    (void)event_data;

    switch ((esp_mqtt_event_id_t)event_id) {
    case MQTT_EVENT_CONNECTED:
        s_mqtt_connected = true;
        ESP_LOGI(TAG, "Connected to MQTT broker");
        enqueue_message(HA_DISCOVERY_TOPIC, s_discovery_payload, 1);
        enqueue_message(HA_RSSI_DISCOVERY_TOPIC, s_rssi_discovery_payload, 1);
        enqueue_message(HA_SNR_DISCOVERY_TOPIC, s_snr_discovery_payload, 1);
        enqueue_message(MQTT_AVAILABILITY_TOPIC, "online", 1);
        break;

    case MQTT_EVENT_DISCONNECTED:
        s_mqtt_connected = false;
        ESP_LOGW(TAG, "Disconnected from MQTT broker");
        break;

    case MQTT_EVENT_ERROR:
        ESP_LOGE(TAG, "MQTT client reported an error");
        break;

    default:
        break;
    }
}

esp_err_t homeassistant_mqtt_start(void)
{
    esp_mqtt_client_config_t mqtt_config = {
        .broker.address.uri = CONFIG_HA_MQTT_BROKER_URI,
        .credentials.client_id = "receiver-tank-rak3172",
        .session.last_will.topic = MQTT_AVAILABILITY_TOPIC,
        .session.last_will.msg = "offline",
        .session.last_will.qos = 1,
        .session.last_will.retain = true,
    };

    s_mqtt_client = esp_mqtt_client_init(&mqtt_config);
    if (s_mqtt_client == NULL) {
        ESP_LOGE(TAG, "Failed to create MQTT client");
        return ESP_FAIL;
    }

    esp_err_t err = esp_mqtt_client_register_event(
        s_mqtt_client, ESP_EVENT_ANY_ID, mqtt_event_handler, NULL);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to register MQTT event handler: %s", esp_err_to_name(err));
        esp_mqtt_client_destroy(s_mqtt_client);
        s_mqtt_client = NULL;
        return err;
    }

    err = esp_mqtt_client_start(s_mqtt_client);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to start MQTT client: %s", esp_err_to_name(err));
        esp_mqtt_client_destroy(s_mqtt_client);
        s_mqtt_client = NULL;
    }

    return err;
}

void homeassistant_mqtt_publish_measurements(float level_liters, int rssi_dbm, int snr_db)
{
    if (!s_mqtt_connected || s_mqtt_client == NULL) {
        return;
    }

    char payload[24];
    int length = snprintf(payload, sizeof(payload), "%.2f", (double)level_liters);
    if (length < 0 || (size_t)length >= sizeof(payload)) {
        ESP_LOGE(TAG, "Failed to format tank level MQTT payload");
        return;
    }

    enqueue_message(MQTT_STATE_TOPIC, payload, 1);

    char radio_payload[12];
    length = snprintf(radio_payload, sizeof(radio_payload), "%d", rssi_dbm);
    if (length > 0 && (size_t)length < sizeof(radio_payload)) {
        enqueue_message(MQTT_RSSI_STATE_TOPIC, radio_payload, 1);
    } else {
        ESP_LOGE(TAG, "Failed to format RSSI MQTT payload");
    }

    length = snprintf(radio_payload, sizeof(radio_payload), "%d", snr_db);
    if (length > 0 && (size_t)length < sizeof(radio_payload)) {
        enqueue_message(MQTT_SNR_STATE_TOPIC, radio_payload, 1);
    } else {
        ESP_LOGE(TAG, "Failed to format SNR MQTT payload");
    }
}
