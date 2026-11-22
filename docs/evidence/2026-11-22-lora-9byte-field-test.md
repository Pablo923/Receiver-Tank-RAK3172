# Prueba en terreno — payload LoRa de 9 bytes

- **Fecha:** 2026-11-22 (UTC-3)
- **Receptor:** ESP32-C6 + RAK3172, rama `feature/lora-9byte-payload` (commit `f36bc65`)
- **Transmisor:** ESP32_Water_Tank_Level, rama `feature/lora-9byte-payload` (commit `3405878`), instalado en la ubicación remota
- **Fuente:** suscripción al broker MQTT `192.168.1.224:1883`, topics `estanque/+/state` y `estanque/nivel/status`

## Resumen

| Métrica | Sesión 1 (13:30:09–13:31:36) | Sesión 2 (13:49:14–13:54:12) |
| --- | --- | --- |
| Paquetes recibidos | 18 | 60 de 60 esperados (cada ~5 s, máx. 6 s entre paquetes) |
| RSSI | −93 a −72 dBm | −91 a −81 dBm (promedio −84) |
| SNR | 12–13 dB (uno con −9 dB) | 7–13 dB (promedio 12,3) |
| Temperatura/humedad válidas | 3 de 18 (36 °C / 27 %RH) | 8 de 60 (33–34 °C / 28 %RH) |
| Nivel reportado | 2402–3612 L | 1997–3754 L (promedio 3165) |

## Conclusión

- El enlace LoRa desde la ubicación remota es estable: en la sesión 2 llegaron
  los 60 paquetes esperados en 5 minutos, con SNR ≈ 12 dB.
- El receptor decodifica la trama de 9 bytes y publica nivel, RSSI, SNR,
  temperatura y humedad en Home Assistant.

## Pendientes en el transmisor

- **Nivel inestable:** varía hasta ~1700 L entre paquetes consecutivos; revisar
  el sensor ultrasónico o promediar lecturas.
- **DHT11:** solo ~13–17 % de lecturas válidas; en el resto se envía el
  marcador "sin dato" y el receptor lo omite (comportamiento esperado).
- **Corte de 13:31:36 a ≥13:33:57:** no llegaron paquetes en ese intervalo; a
  las 13:49 el enlace estaba activo. Causa sin confirmar (el transmisor no
  envía si el nivel está fuera de rango).

## Captura MQTT — sesión 2 (5 minutos)

```
13:49:13 [retained] estanque/nivel/state = 2322.37
13:49:13 [retained] estanque/rssi/state = -80
13:49:13 [retained] estanque/snr/state = 12
13:49:13 [retained] estanque/temperatura/state = 34.00
13:49:13 [retained] estanque/humedad/state = 28.00
13:49:13 [retained] estanque/nivel/status = online
13:49:14 estanque/nivel/state = 3600.66
13:49:14 estanque/rssi/state = -82
13:49:14 estanque/snr/state = 13
13:49:19 estanque/nivel/state = 3061.66
13:49:19 estanque/rssi/state = -81
13:49:19 estanque/snr/state = 12
13:49:24 estanque/nivel/state = 2493.90
13:49:24 estanque/rssi/state = -84
13:49:24 estanque/snr/state = 12
13:49:24 estanque/temperatura/state = 34.00
13:49:24 estanque/humedad/state = 28.00
13:49:29 estanque/nivel/state = 3434.56
13:49:29 estanque/rssi/state = -85
13:49:29 estanque/snr/state = 12
13:49:34 estanque/nivel/state = 3060.58
13:49:34 estanque/rssi/state = -87
13:49:34 estanque/snr/state = 7
13:49:39 estanque/nivel/state = 2800.03
13:49:39 estanque/rssi/state = -88
13:49:39 estanque/snr/state = 13
13:49:44 estanque/nivel/state = 3254.36
13:49:44 estanque/rssi/state = -87
13:49:44 estanque/snr/state = 12
13:49:49 estanque/nivel/state = 2588.34
13:49:49 estanque/rssi/state = -82
13:49:49 estanque/snr/state = 12
13:49:49 estanque/temperatura/state = 33.00
13:49:49 estanque/humedad/state = 28.00
13:49:54 estanque/nivel/state = 3402.00
13:49:54 estanque/rssi/state = -85
13:49:54 estanque/snr/state = 13
13:49:59 estanque/nivel/state = 3192.48
13:49:59 estanque/rssi/state = -83
13:49:59 estanque/snr/state = 12
13:50:05 estanque/nivel/state = 3184.33
13:50:05 estanque/rssi/state = -81
13:50:05 estanque/snr/state = 13
13:50:10 estanque/nivel/state = 3302.12
13:50:10 estanque/rssi/state = -83
13:50:10 estanque/snr/state = 12
13:50:15 estanque/nivel/state = 2987.84
13:50:15 estanque/rssi/state = -85
13:50:15 estanque/snr/state = 12
13:50:20 estanque/nivel/state = 3266.30
13:50:20 estanque/rssi/state = -83
13:50:20 estanque/snr/state = 12
13:50:25 estanque/nivel/state = 3420.99
13:50:25 estanque/rssi/state = -83
13:50:25 estanque/snr/state = 13
13:50:30 estanque/nivel/state = 3041.04
13:50:30 estanque/rssi/state = -82
13:50:30 estanque/snr/state = 13
13:50:35 estanque/nivel/state = 3585.46
13:50:35 estanque/rssi/state = -81
13:50:35 estanque/snr/state = 12
13:50:40 estanque/nivel/state = 3273.90
13:50:40 estanque/rssi/state = -82
13:50:40 estanque/snr/state = 13
13:50:44 estanque/nivel/state = 3281.50
13:50:44 estanque/rssi/state = -82
13:50:44 estanque/snr/state = 12
13:50:49 estanque/nivel/state = 3171.31
13:50:49 estanque/rssi/state = -83
13:50:49 estanque/snr/state = 12
13:50:54 estanque/nivel/state = 1996.69
13:50:54 estanque/rssi/state = -88
13:50:54 estanque/snr/state = 12
13:50:54 estanque/temperatura/state = 34.00
13:50:54 estanque/humedad/state = 28.00
13:50:59 estanque/nivel/state = 3753.73
13:50:59 estanque/rssi/state = -90
13:50:59 estanque/snr/state = 12
13:51:04 estanque/nivel/state = 2976.44
13:51:04 estanque/rssi/state = -88
13:51:04 estanque/snr/state = 12
13:51:10 estanque/nivel/state = 3168.05
13:51:10 estanque/rssi/state = -89
13:51:10 estanque/snr/state = 12
13:51:15 estanque/nivel/state = 3473.65
13:51:15 estanque/rssi/state = -85
13:51:15 estanque/snr/state = 12
13:51:20 estanque/nivel/state = 2808.18
13:51:20 estanque/rssi/state = -85
13:51:20 estanque/snr/state = 13
13:51:25 estanque/nivel/state = 3038.87
13:51:25 estanque/rssi/state = -83
13:51:25 estanque/snr/state = 12
13:51:30 estanque/nivel/state = 3254.90
13:51:30 estanque/rssi/state = -83
13:51:30 estanque/snr/state = 12
13:51:35 estanque/nivel/state = 3438.36
13:51:35 estanque/rssi/state = -85
13:51:35 estanque/snr/state = 13
13:51:40 estanque/nivel/state = 3392.23
13:51:40 estanque/rssi/state = -85
13:51:40 estanque/snr/state = 13
13:51:45 estanque/nivel/state = 2563.37
13:51:45 estanque/rssi/state = -83
13:51:45 estanque/snr/state = 12
13:51:45 estanque/temperatura/state = 33.00
13:51:45 estanque/humedad/state = 28.00
13:51:50 estanque/nivel/state = 3473.65
13:51:50 estanque/rssi/state = -85
13:51:50 estanque/snr/state = 13
13:51:55 estanque/nivel/state = 3140.91
13:51:55 estanque/rssi/state = -82
13:51:55 estanque/snr/state = 13
13:52:00 estanque/nivel/state = 3124.08
13:52:00 estanque/rssi/state = -87
13:52:00 estanque/snr/state = 12
13:52:05 estanque/nivel/state = 2934.65
13:52:05 estanque/rssi/state = -89
13:52:06 estanque/snr/state = 12
13:52:11 estanque/nivel/state = 3373.23
13:52:11 estanque/rssi/state = -84
13:52:11 estanque/snr/state = 13
13:52:16 estanque/nivel/state = 3248.38
13:52:16 estanque/rssi/state = -86
13:52:16 estanque/snr/state = 13
13:52:21 estanque/nivel/state = 3254.90
13:52:21 estanque/rssi/state = -84
13:52:21 estanque/snr/state = 13
13:52:26 estanque/nivel/state = 2447.76
13:52:26 estanque/rssi/state = -82
13:52:26 estanque/snr/state = 13
13:52:26 estanque/temperatura/state = 33.00
13:52:26 estanque/humedad/state = 28.00
13:52:31 estanque/nivel/state = 3078.49
13:52:31 estanque/rssi/state = -85
13:52:31 estanque/snr/state = 13
13:52:36 estanque/nivel/state = 3337.40
13:52:36 estanque/rssi/state = -85
13:52:36 estanque/snr/state = 13
13:52:41 estanque/nivel/state = 3194.11
13:52:41 estanque/rssi/state = -86
13:52:41 estanque/snr/state = 12
13:52:46 estanque/nivel/state = 3197.36
13:52:46 estanque/rssi/state = -83
13:52:46 estanque/snr/state = 13
13:52:50 estanque/nivel/state = 3649.51
13:52:50 estanque/rssi/state = -83
13:52:50 estanque/snr/state = 13
13:52:55 estanque/nivel/state = 3574.06
13:52:55 estanque/rssi/state = -83
13:52:55 estanque/snr/state = 12
13:52:55 estanque/temperatura/state = 33.00
13:52:55 estanque/humedad/state = 28.00
13:53:00 estanque/nivel/state = 3083.92
13:53:00 estanque/rssi/state = -81
13:53:00 estanque/snr/state = 12
13:53:05 estanque/nivel/state = 3201.70
13:53:06 estanque/rssi/state = -81
13:53:06 estanque/snr/state = 12
13:53:11 estanque/nivel/state = 2425.50
13:53:11 estanque/rssi/state = -83
13:53:11 estanque/snr/state = 12
13:53:11 estanque/temperatura/state = 33.00
13:53:11 estanque/humedad/state = 28.00
13:53:16 estanque/nivel/state = 2473.27
13:53:16 estanque/rssi/state = -82
13:53:16 estanque/snr/state = 13
13:53:16 estanque/temperatura/state = 33.00
13:53:16 estanque/humedad/state = 28.00
13:53:21 estanque/nivel/state = 3436.19
13:53:21 estanque/rssi/state = -81
13:53:21 estanque/snr/state = 13
13:53:26 estanque/nivel/state = 2896.11
13:53:26 estanque/rssi/state = -82
13:53:26 estanque/snr/state = 12
13:53:31 estanque/nivel/state = 3717.90
13:53:31 estanque/rssi/state = -87
13:53:31 estanque/snr/state = 12
13:53:36 estanque/nivel/state = 3013.90
13:53:36 estanque/rssi/state = -91
13:53:36 estanque/snr/state = 13
13:53:41 estanque/nivel/state = 3017.70
13:53:41 estanque/rssi/state = -84
13:53:41 estanque/snr/state = 12
13:53:46 estanque/nivel/state = 3343.92
13:53:46 estanque/rssi/state = -83
13:53:46 estanque/snr/state = 12
13:53:51 estanque/nivel/state = 3458.99
13:53:51 estanque/rssi/state = -81
13:53:51 estanque/snr/state = 12
13:53:56 estanque/nivel/state = 3536.07
13:53:56 estanque/rssi/state = -81
13:53:56 estanque/snr/state = 12
13:54:01 estanque/nivel/state = 3139.28
13:54:01 estanque/rssi/state = -86
13:54:01 estanque/snr/state = 13
13:54:06 estanque/nivel/state = 3307.55
13:54:06 estanque/rssi/state = -83
13:54:06 estanque/snr/state = 12
13:54:12 estanque/nivel/state = 3563.21
13:54:12 estanque/rssi/state = -81
13:54:12 estanque/snr/state = 12
```

## Captura MQTT — sesión 1

```
13:30:06 [retained] estanque/nivel/state = 3182.71
13:30:06 [retained] estanque/rssi/state = -87
13:30:06 [retained] estanque/snr/state = 12
13:30:06 [retained] estanque/temperatura/state = 36.00
13:30:06 [retained] estanque/humedad/state = 27.00
13:30:06 [retained] estanque/nivel/status = online
13:30:09 estanque/nivel/state = 3611.52
13:30:09 estanque/rssi/state = -86
13:30:09 estanque/snr/state = 13
13:30:14 estanque/nivel/state = 3315.15
13:30:14 estanque/rssi/state = -90
13:30:14 estanque/snr/state = 12
13:30:19 estanque/nivel/state = 3146.34
13:30:19 estanque/rssi/state = -90
13:30:19 estanque/snr/state = 12
13:30:25 estanque/nivel/state = 3137.11
13:30:25 estanque/rssi/state = -85
13:30:25 estanque/snr/state = 13
13:30:30 estanque/nivel/state = 3416.65
13:30:30 estanque/rssi/state = -82
13:30:30 estanque/snr/state = 13
13:30:35 estanque/nivel/state = 3101.29
13:30:35 estanque/rssi/state = -82
13:30:35 estanque/snr/state = 13
13:30:40 estanque/nivel/state = 2414.65
13:30:40 estanque/rssi/state = -82
13:30:40 estanque/snr/state = 13
13:30:40 estanque/temperatura/state = 36.00
13:30:40 estanque/humedad/state = 27.00
13:30:45 estanque/nivel/state = 2402.16
13:30:45 estanque/rssi/state = -84
13:30:45 estanque/snr/state = 12
13:30:45 estanque/temperatura/state = 36.00
13:30:45 estanque/humedad/state = 27.00
13:30:50 estanque/nivel/state = 3124.08
13:30:50 estanque/rssi/state = -87
13:30:50 estanque/snr/state = 12
13:30:55 estanque/nivel/state = 3372.14
13:30:55 estanque/rssi/state = -93
13:30:55 estanque/snr/state = 12
13:31:00 estanque/nivel/state = 3462.25
13:31:00 estanque/rssi/state = -91
13:31:00 estanque/snr/state = 12
13:31:05 estanque/nivel/state = 3442.16
13:31:05 estanque/rssi/state = -90
13:31:05 estanque/snr/state = 12
13:31:10 estanque/nivel/state = 2670.31
13:31:10 estanque/rssi/state = -88
13:31:10 estanque/snr/state = 12
13:31:10 estanque/temperatura/state = 36.00
13:31:10 estanque/humedad/state = 27.00
13:31:36 estanque/nivel/state = 3245.67
13:31:36 estanque/rssi/state = -72
13:31:36 estanque/snr/state = -9
```

## Captura serial en banco (antes de instalar, 60 s)

```
I (16250) [RAK3172]: UART <- +EVT:RXP2P:-31:13:0196969B458C0A800C
I (16250) [MAIN]: ID=1 RSSI=-31 SNR=13 Level=4978.82 Temp=27.00 Hum=32.00
I (21290) [RAK3172]: UART <- +EVT:RXP2P:-34:13:01A22BA345280AE40C
I (21290) [MAIN]: ID=1 RSSI=-34 SNR=13 Level=5221.45 Temp=26.00 Hum=33.00
I (26330) [RAK3172]: UART <- +EVT:RXP2P:-32:14:0196969B45280AE40C
I (26330) [MAIN]: ID=1 RSSI=-32 SNR=14 Level=4978.82 Temp=26.00 Hum=33.00
```
