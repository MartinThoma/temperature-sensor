# Temperature- and Humidity Sensor

## Hardware

* ESP32 Development Board
* DHT11 Temperature and Humidity Sensor

## Setup

1. Copy the environment variables template:
   ```bash
   cp .envrc.example .envrc
   ```

2. Edit `.envrc` and set your WiFi credentials:
   ```bash
   export WIFI_SSID="YourWiFiNetworkName"
   export WIFI_PASS="YourWiFiPassword"
   ```

3. Source the environment variables before building:
   ```bash
   source .envrc
   ```

## Development

Requirements:

```
. $HOME/esp/esp-idf/export.sh
```

Make sure to source your WiFi credentials and then build and flash:

```bash
source .envrc
idf.py -p /dev/ttyUSB0 flash monitor
```

### TODO

* Run with battery
* Configure WiFi credentials via Bluetooth
* Use less energy
