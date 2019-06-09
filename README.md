# NodeESP Firmware
Firmware of the NodeESP Development Board.

![NodeESP Board](https://iot.fkainka.de/wp-content/uploads/2018/10/board1-1024x402.jpg)

More Information: www.iot.fkainka.de/nodeesp


## Changelog:
  * 28.02.2019 Updated Framework (1.7) + Libraries (TCPAsync 1.0.3)
  * 01.03.2019 Back to Framework (1.0.2) +  TCPAsync 1.0
    * added Selected App DIV in website, changed app stile to title +  info,
    * added SD Card Support (call nodeesp.local/sd)


## FIX for error: C99 in esp_wps.cpp:
  Info: https://github.com/espressif/esp-idf/pull/2869/files

  change file .platformio/packages/framework-arduinoespressif32/tools/sdk/include/esp32/esp_wps.h
  line 81ff:
    {.manufacturer = "ESPRESSIF"},
    {.model_number = "ESP32"},  \
    {.model_name = "ESPRESSIF IOT"},  \
    {.device_name = "ESP STATION"},  \
