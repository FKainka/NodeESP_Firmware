 //******************************************************************************
 // NodeESP Firmware
 // Version 2.0 17.10.2019
 //
 // Copyright 2019 Fabian Kainka
 // Licensed under GNU GPL 3.0 and later, under this license absolutely no warranty given
 //
 // // See www.iot.fkainka.de for documentation
 //
 // Changelog:
 // - 28.02.2019 Updated Framework (1.7) + Libraries (TCPAsync 1.0.3)
 // - 01.03.2019 Back to Framework (1.0.2) +  TCPAsync 1.0
 //    + added Selected App DIV in website, changed app stile to title +  info,
 //    + added SD Card Support (call nodeesp.local/sd)
 // - 17.10.2019: Smarthome Update
 //     + Added Smarthome Apps
 //     + Added Editor
 //     + Added SD-Card support
 //     + Extended App selection info view
 //     + bug fixes 
 //     + New OTA Update source (directly from GITHUB)
 //     + spiffs: Added Editor
 // - 02.11.2019: Smarthome SD-Card Links
 //     + Added Smarthome SD-Card Links
 //
 // FIX for error: C99 in esp_wps.cpp:
 // https://github.com/espressif/esp-idf/pull/2869/files
 // change file .platformio/packages/framework-arduinoespressif32/tools/sdk/include/esp32/esp_wps.h
 // line 81ff:
 // {.manufacturer = "ESPRESSIF"},
 // {.model_number = "ESP32"},  \
 // {.model_name = "ESPRESSIF IOT"},  \
 // {.device_name = "ESP STATION"},  \

 //******************************************************************************


//Current Version of APP, Spiff version is also saved in config file
#define VERSION "v2.2_04122019"
#define SPIFFS_VERSION "v2.0_17102019"

#define LOG_LOCAL_LEVEL ESP_LOG_VERBOSE
#include "esp_log.h"

#include <SPIFFS.h>

#include "StatusLed.hpp"    //Status LED functions
#include "Config.hpp"       //configuration loading, saving and changing
#include "Commands.hpp"     //handles most commands
#include "Apps.hpp"         //handles execution of "apps"

void setup() {
        Serial.begin(115200);
        Serial.setDebugOutput(true);
        Serial.println("\n\nRUN\n");

        #ifdef AUTHORMODE
          Serial.println("Author Mode!");
        #endif

        //initialise the RGB LED to show status
        statusLedInit();

        //Spiffs is the file system on the Board. It includes every file from the onboard website and the config
        if (!SPIFFS.begin()) {
                Serial.println("SPIFFS Mount Failed!");
                statusLed(blink_red);      //Blink red on fatal error and stop everything
                while(1);
        }

        //Loads the configuration from the file system and sets parameter
        if (!loadConfiguration()) {
                Serial.println("Error Loading Configuration");
                statusLed(blink_red);    //Blink red on fatal error and stop everything
                while(1);
        }

        //Print out the current APP- and Spiffs-Version
        Serial.printf("APP Version: %s \nSPIFFS Version: %s\n\n", ota_set.app_version, ota_set.spiffs_version);

        //Execute selected app. Function in Apps.hpp
        exec_app(app_set.app_exec);
}

void loop() {
//Nothing happens in the Loop, main programm is in seperated processes
}
