# nESP-thermostat

This is my project to build a Wifi enabled smart thermostat based on the ESP8266/ESP32 microcontroller. This project is designed to support various devices including temperature sensors, displays, relays, LEDs, and buttons. This is a very early release, so not a long list of options, but we'll see how it goes.

# Supported Devices

Currently, support is in place for the DHTxx and BMP280 temperature sensors. The only display in place is the SSD1306 128x64 monochrome display. These are the devices I just have around, so that's what I built. I also have a 2.4 inch TFT display that I would like to add too.

# Required Libraries

The exact libraries required by nESP depend on what is enabled in the userConfig.h file, located in the src/ directory. The following libraries are used when the associated devices are enabled:

  - U8g2 by olikraus                    https://github.com/olikraus/u8g2
  - Adafruit Unified Sensor Driver      https://github.com/adafruit/Adafruit_Sensor
  - Adafruit DHT Sensor Library         https://github.com/adafruit/DHT-sensor-library
  - Adafruit BMP280 Library             https://github.com/adafruit/Adafruit_BMP280_Library
  - IoTWebConf by Balázs Kelemen        https://github.com/prampec/IotWebConf
  - ArduinoJson by Benoît Blanchon      https://github.com/bblanchon/ArduinoJson
  - MQTT by Joël Gähwiler               https://github.com/256dpi/arduino-mqtt
  - OneButton by Matthias Hertel        https://github.com/mathertel/OneButton
  
All of these are available through the Platformio library management.

# To Do

  - JSON for state updates
  - Configure conventional HVAC switches
  - Configure web interface
  - Improve logging
  - Add more sensors/displays
  - Add libs to platformio.ini
