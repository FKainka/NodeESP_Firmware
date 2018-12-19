<<<<<<< HEAD
#define VERSION "v1.4_16122018-Smarthome"
=======
/**
 * Generall Informations
 *
 * This is the main file, meaning that this is kind of the programm start. It is saved as ino-file so it can also be opened in the Ardino IDE.
 *
 */

//Current Version of APP, Spiff version is saved in config
#define VERSION "v1.4_16122018"
>>>>>>> refs/remotes/origin/master
#define SPIFFS_VERSION "v1.2_16122018"

#include <SPIFFS.h>

#include "StatusLed.hpp"    //Status LED functions
#include "Config.hpp"       //configuration loading, saving and changing
#include "Commands.hpp"     //handles most commands
#include "Apps.hpp"         //handles execution of "apps"

void setup() {
        Serial.begin(115200);
        Serial.setDebugOutput(true);
<<<<<<< HEAD
        Serial.println("\n\nRUN\n");

=======
        Serial.println("\n\nRUN");

        #ifdef AUTHORMODE
          Serial.println("Author Mode!"); 
        #endif

        //initialise the RGB LED to show status
>>>>>>> refs/remotes/origin/master
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
