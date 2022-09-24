# PPTempCheck
simple platform.io project, build for a [01Space/ESP32-C3-0.42LCD](https://github.com/01Space/ESP32-C3-0.42LCD) development board used with a DHT11 Sensor to measure temperature and publish it to a mqtt server, for cosumption by [Home Assistants MQTT integration ](https://www.home-assistant.io/integrations/mqtt/) (supports discovery).

## Installation
### Sensor End
Clone project, copy include/config.h-dist to include/config.h, edit config to fit your setup (PIN, Wifi, MQTT Server/Sensor Name/ID) get dependenies with platform io, build and flash to your development board.

It should flash a connecting to Wi-Fi message a few times (if it keeps doing that it can't), then it will flash a connecting to MQTT server (if it keeps doing that it can't), and ultimately it will publish and show the measured temperature (if constant at -1 while not being at -1, then its not reading anything).

This is always in celcius.

### Home Assistant End
All you need to do is have an MQTT Broker available (I use the mosquito add-on for home assistant), the MQTT Integration enabled, and discover mode on, and the sensor will automatically appear as an entity, if everything is working.


## License
MIT style, see [LICENSE.md] file


## Links
* Author [Peter Petermann](https://devedge.eu)
* Framework used [PlatformIO](https://platformio.org/)
* Development Board [01Space/ESP32-C3-0.42LCD](https://github.com/01Space/ESP32-C3-0.42LCD) - [Buy Here](https://www.banggood.com/custlink/DmvcV3BiwP)
* Sensor [DHT11](https://components101.com/sensors/dht11-temperature-sensor)
* [Home Assistant](https://www.home-assistant.io/)