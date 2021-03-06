# NodeESP Firmware
Firmware of the NodeESP Development Board.

More Information: www.iot.fkainka.de/nodeesp

![NodeESP Board](https://iot.fkainka.de/wp-content/uploads/2018/10/board1-1024x402.jpg)

## Changelog:
  * 28.02.2019 Updated Framework (1.7) + Libraries (TCPAsync 1.0.3)
  * 01.03.2019 Back to Framework (1.0.2) +  TCPAsync 1.0
    * added Selected App DIV in website, changed app stile to title +  info,
    * added SD Card Support (call nodeesp.local/sd)
  * 14.06.2019 Changed OTA Updates -> files are directly loaded from GitHub (only for development mode)
  * 17.10.2019: Smarthome Update
  *    + Added Smarthome Apps
  *     + Added Editor
  *     + Added SD-Card support
  *     + Extended App selection info view
  *     + bug fixes 
  *     + New OTA Update source (directly from GITHUB)
  *     + Spiffs: Added Editor
  * 04.12.2019: Stability Update
  *    + New ESP32 Arduino Core
  *    + Updated libraries
  *    + Better Config
  *    + Added Fritzing Part to /doc
  *    + Better Update
  * 05.12.2019: Hotfix Pin18 interfering with new SD-Card Library
  

## FIX for error: C99 in esp_wps.cpp:
  Info: https://github.com/espressif/esp-idf/pull/2869/files

  change file .platformio/packages/framework-arduinoespressif32/tools/sdk/include/esp32/esp_wps.h\
  line 81ff:\
    {.manufacturer = "ESPRESSIF"},\\ \
    {.model_number = "ESP32"},\\  \
    {.model_name = "ESPRESSIF IOT"},\\  \
    {.device_name = "ESP STATION"},\\  \

## Used Libraries
  * Webserver:    https://github.com/me-no-dev/ESPAsyncWebServer
  * Arduino JSON: https://arduinojson.org/assistant/?utm_source=github&utm_medium=issues
  * MQTT:         https://github.com/knolleary/pubsubclient
  * DAC:          https://www.xtronical.com/the-dacaudio-library-download-and-installation/
  * IR Remote:    https://github.com/SensorsIot/Arduino-IRremote


