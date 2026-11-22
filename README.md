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

## VS Code shortcuts

The `.vscode/tasks.json` tasks build, flash, and open the serial monitor with ESP-IDF. To bind F16, F17, and F18 to those tasks, open **Preferences: Open Keyboard Shortcuts (JSON)** and copy the entries from `.vscode/keybindings.example.json` into your user `keybindings.json`.

- **F16:** Build
- **F17:** Flash (prompts for the serial port)
- **F18:** Monitor (prompts for the serial port)
