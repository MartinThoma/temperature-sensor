# Temperature- and Humidity Sensor

The idea behind this project is to create a temperature and humidity sensor that can be easily integrated into a home automation system, such as Home Assistant.

In a private house every room should have at
least one sensor, potentially more to get the
temperature gradient within a room (from floor
to ceiling, from window to door).

It is primarily designed to be used indoors.

As one home might have 15 rooms and every room might have 1-3 sensors, the price should be low
enough to allow for multiple sensors without breaking the bank.

For the same reason, the sensor should be low power, so it can run on batteries for a long time.

It also should not break the wi-fi by having too many sensors connected to the same network.


## Hardware

### Components: 9.63€


<table>
    <tr>
        <th>Component</th>
        <th>Specs</th>
        <th>Price</th>
        <th>Price per piece</th>
    </tr>
    <tr>
        <td><a href="https://www.espressif.com/sites/default/files/documentation/esp32-wroom-32_datasheet_en.pdf">ESP-WROOM-32 Development board</a></td>
        <td>two low-power Xtensa® 32-bit LX6, 520 KB of on-chip SRAM; Wi-Fi 802.11 b/g/n (802.11n up to 150 Mbps); Bluetooth v4.2 BR/EDR + BLE; –40 °C to +85 °C</td>
        <td>7€</td>
        <td>7€</td>
    </tr>
    <tr>
        <td><a href="https://www.mouser.com/datasheet/2/758/DHT11-Technical-Data-Sheet-Translated-Version-1143054.pdf?srsltid=AfmBOorywJsEEEBtBxdXiNRqFNXPa3f5OPcv28YAAwzZxsxg1NEnql1v">DHT11 Temperature and Humidity Sensor</a></td>
        <td>20-90%RH and 0-50 ℃; ±5％RH; ±2℃; Resolution of 1°C and 1% RH; Requires 3-5.5V DC</td>
        <td>3x for 7.89€</td>
        <td>2.63€</td>
    </tr>
</table>


### Assembly

1. Connect the DHT11 sensor to the ESP32:
   - VCC to 3.3V (3V3)
   - GND to GND
   - Data pin to GPIO 4 (D4)

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

## Communication

Home Assistant can get the data from the sensor via HTTP requests (polling a REST API).

### TODO

* Integrate into Home Assistant
* Make a rigid case
* Run with battery
* Configure WiFi credentials via Bluetooth
* Use less energy
