#define VERSION "v1.4_16122018"
#define SPIFFS_VERSION "v1.2_16122018"

#include <SPIFFS.h>

#include "StatusLed.hpp"    //Status LED functions
#include "Config.hpp"
#include "Commands.hpp"
#include "Apps.hpp"
#include "Test.hpp"

void setup() {
        Serial.begin(115200);
        Serial.setDebugOutput(true);
        Serial.println("\n\nRUN\n");

        statusLedInit();

        if (!SPIFFS.begin()) {

                Serial.println("SPIFFS Mount Failed!");
                statusLed(blink_red); //Blink while trying to establish connection
                while(1);
        }

        //SPIFFS.remove("/config/config.cfg");

        if (!loadConfiguration()) {
                Serial.println("Error Loading Configuration");
                statusLed(blink_red);   //Blink while trying to establish connection
                while(1);
        }

        Serial.printf("\n\nAPP Version: %s \nSPIFFS Version: %s\n\n", ota_set.app_version, ota_set.spiffs_version);
        exec_app(app_set.app_exec); //exec_app("dflt_init");
}


void loop() {


}
