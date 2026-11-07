## WiFi Configuration

Before compilation, copy template and add own credentials:

```bash
cp sdkconfig.defaults.template sdkconfig.defaults
```

Edit `sdkconfig.defaults` in `CONFIG_WIFI_STA_SSID` and `CONFIG_WIFI_STA_PASSWORD` with own SSID and password.

## VS Code shortcuts

The `.vscode/tasks.json` tasks build, flash, and open the serial monitor with ESP-IDF. To bind F16, F17, and F18 to those tasks, open **Preferences: Open Keyboard Shortcuts (JSON)** and copy the entries from `.vscode/keybindings.example.json` into your user `keybindings.json`.

- **F16:** Build
- **F17:** Flash (prompts for the serial port)
- **F18:** Monitor (prompts for the serial port)
