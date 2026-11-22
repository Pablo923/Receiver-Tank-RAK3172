## WiFi Configuration

Before compilation, copy template and add own credentials:

```bash
cp sdkconfig.defaults.template sdkconfig.defaults
```

Copy `components/wifi/include/wifi_credentials.local.h.example` to
`components/wifi/include/wifi_credentials.local.h` and set `WIFI_LOCAL_SSID`
and `WIFI_LOCAL_PASSWORD` there. The local file is ignored by Git. The Wi-Fi
component uses it when present and falls back to the ESP-IDF Kconfig values if
it is absent.

## LoRa payload

The receiver decodes the 9-byte payload sent by `ESP32_Water_Tank_Level`
(little-endian):

| Bytes | Field | Type / unit | No-data value |
| --- | --- | --- | --- |
| 0 | Device ID | `uint8` | n/a |
| 1–4 | Water level | `float32`, liters | n/a |
| 5–6 | Temperature | `int16`, hundredths of °C | `0x8000` |
| 7–8 | Humidity | `uint16`, hundredths of %RH | `0xFFFF` |

The legacy 4-byte payload (water level only) is still accepted. Level, RSSI,
SNR, temperature and humidity are published to Home Assistant over MQTT;
temperature and humidity are skipped when the transmitter sends the no-data
marker, so Home Assistant keeps the last valid reading.

## VS Code shortcuts

The `.vscode/tasks.json` tasks build, flash, and open the serial monitor with ESP-IDF. To bind F16, F17, and F18 to those tasks, open **Preferences: Open Keyboard Shortcuts (JSON)** and copy the entries from `.vscode/keybindings.example.json` into your user `keybindings.json`.

- **F16:** Build
- **F17:** Flash (prompts for the serial port)
- **F18:** Monitor (prompts for the serial port)
