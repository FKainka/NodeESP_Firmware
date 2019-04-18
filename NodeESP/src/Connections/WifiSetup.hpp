/**
 * WiFiSetup.hpp
 * Functions for establishing WiFi Connection and handling WPS
 *
 * WiFiEvent()            Prints out and shows Wifi and WPS events via Terminal and Status LED
 * taskWpsSetup           Task for enabling WPS
 * wps_pin_change()       Interrupt Routine for detecting long button press on BOOT Button (GPIO0)
 * wifiInit()             Init Function for setting up WiFi and establish connection to wifi station/router
 * firstInit()            Initiates predfined settings
 */

#ifndef WIFI_SETUP_H
#define WIFI_SETUP_H

#include <WiFi.h>
#include <esp_wps.h>
#include <esp_wifi.h>
#include <ESPmDNS.h>

#include "../Peripherie/StatusLed.hpp"    //Status LED functions
#include "../Config/Config.hpp"          //Includes struct gen_set_s gen_set which has the general settings

#define wps_pin 0                             //WPS Pin = GPIO0, BOOT onBoard button
#define ESP_WPS_MODE WPS_TYPE_PBC            //WPS Mode = PBS (Push Button Configuration)
esp_wps_config_t config; // = WPS_CONFIG_INIT_DEFAULT(ESP_WPS_MODE);  //WPS
bool disconnect_msg = false;


//Variables for detecting long WPS Buttons press
volatile long wps_pin_press_duration = 0;
volatile long wps_pin_press_down_time = 0;

//Prototypes - predfined functions implemented later
bool wifiInit();
void WiFiEvent(WiFiEvent_t event, system_event_info_t info);
void taskWpsSetup(void * parameter );
void IRAM_ATTR wps_pin_change();
void firstInit();

/**
 * Initiate Wifi Connection and enable WPS button
 * @return true if Connection was established directly
 */
bool wifiInit(){
        WiFi.onEvent(WiFiEvent); //Wifi Event hanlder: Print WiFi Status Messages

        if (gen_set.wifi_mode==2 | gen_set.wifi_mode==3) {
                strlen(gen_set.ap_pwd) ? WiFi.softAP(gen_set.ap_ssid, gen_set.ap_pwd) : WiFi.softAP(gen_set.ap_ssid);
        }

        wifi_config_t conf;
        esp_wifi_get_config(WIFI_IF_STA,&conf);   //IF unintitialised set to ap mode!
        String ssid = String(reinterpret_cast<const char*>(conf.sta.ssid));

        if (ssid == "") {  //if no Wifi-Data is saved in memory
                Serial.println("No Wifi Data - AP Mode"); //Seems to work but no Message? !!!
                WiFi.mode(WIFI_AP);
                statusLed(yellow);
        }
        else{
                WiFi.mode(intToMode(gen_set.wifi_mode));

                if (gen_set.wifi_mode==1 | gen_set.wifi_mode==3) {
                        WiFi.begin(); //Start Wifi

                        Serial.print("Trying to establish Wifi Connection.");
                        statusLed(blink_green); //Blink while trying to establish connection

                        disconnect_msg = false;   //No ´Debug msg
                        int failed_attempts = 0;
                        for (int i = gen_set.wifi_con_time * 10; i; i--) { //Wait for WiFi Connection, gen_set = Setting from config.hpp
                              if (WiFi.isConnected()) Serial.println("CONNECTED");
                          //    Serial.println(WiFi.localIP());
                              if ( WiFi.status() == WL_CONNECTED || WiFi.status() == WL_CONNECT_FAILED ) break;

                              /* only 1.7.0
                              // Fix for bug https://github.com/espressif/arduino-esp32/issues/2501
                              if (WiFi.status() == WL_CONNECT_FAILED){
                                Serial.println("Failed: " + failed_attempts);
                                if (failed_attempts++ >3) break;
                                String pw = String(reinterpret_cast<const char*>(conf.sta.password));
                                WiFi.disconnect(true);
                                WiFi.begin(ssid.c_str(), pw.c_str());
                              }*/

                                Serial.print(".");
                                delay(100);
                        }
                        disconnect_msg = true;

                        if (WiFi.status() == WL_CONNECTED) {
                              WiFi.setHostname(gen_set.mdns_name);
                                MDNS.begin(gen_set.mdns_name) ? Serial.println("MDNS OK") : Serial.println("Error MDNS!");
                                MDNS.addService("_http", "_tcp", 80);
                              //  MDNS.addService("http", "tcp", 80);


                                statusLed(green);
                                return true;
                        }
                        else{
                                statusLed(red);
                                return false;
                        }
                }
        }
        return false;
}

void wps_enable(bool enable){
        //Attach Interrupt for Long Button Press on WPS Button
        //Parameter: Pin, Function to call, Events (RISING, FALLING, CHANGE)
        if (enable) attachInterrupt(digitalPinToInterrupt(wps_pin), wps_pin_change, CHANGE);
        else detachInterrupt(wps_pin);
        functions.wps = enable;
}

/**
 * Task for setting up WPS
 */
void taskWpsSetup(void * parameter ){
        Serial.println("\nStarting WPS");
        WiFi.mode(WIFI_MODE_STA);          //WPS works oly in Station Mode!

        esp_err_t err = esp_wifi_wps_enable(&config);
        Serial.println("Error: "+ String(err));

        if (esp_wifi_wps_start(0)== 0) {
                disconnect_msg= false;
                statusLed(blink_yellow);   //Start blinking while trying to estblish connection
        }
        else{
                esp_wifi_wps_disable();
                WiFi.mode(intToMode(gen_set.wifi_mode));     //Restore default if wps fails
        }
        vTaskDelete( NULL );               //Delete this task on end
}

/**
 * Interrupt Routine to detect a long WPS Button press
 */
void IRAM_ATTR wps_pin_change() {
        if (!digitalRead(wps_pin)) {
                wps_pin_press_down_time = millis();                                  //Save ms when button is pressed
        }
        else{
                wps_pin_press_duration = millis() -  wps_pin_press_down_time;        //calc press duration when released
                if (wps_pin_press_duration >=2000 && wps_pin_press_duration <=10000) {                                 //when button pressed for longer than 2 seconds start wps
                        //Parameter: Task Function, Task Name, Stack Size, Task Function Parameter, Task Priority, Task Handle
                        xTaskCreate(taskWpsSetup,"wpssetup",2000,NULL,3,NULL);
                }
                else if (wps_pin_press_duration >10000) {
                        Serial.println("Restoring Settings");
                        restore_settings();
                }
        }
}


/**
 * Handler for Wifi Status Events (and WPS)
 * Prints out Messages on Serial Monitor and sets Status LED
 *
 * @param event, info
 */
void WiFiEvent(WiFiEvent_t event, system_event_info_t info){
        //Serial.println("WiFi Event: "+ String(event));

        switch(event) {
        case SYSTEM_EVENT_STA_GOT_IP:           //WiFi Connection established and IP assigned
                Serial.println("\n\nConnected to: " + String(WiFi.SSID()));
                Serial.print("IP: ");
                Serial.println(WiFi.localIP());
                statusLed(green);
                break;
        case SYSTEM_EVENT_STA_DISCONNECTED:    //Diconnected from Station/Router
                if (disconnect_msg) {
                        statusLed(red);
                        Serial.println("\nWiFi disconnected");
                        WiFi.reconnect(); //Try to reconnect on connection loss
                }
                break;
        case SYSTEM_EVENT_STA_WPS_ER_SUCCESS:   //WPS succesfull
                Serial.println("WPS Successfull, reseting.");
                esp_wifi_wps_disable();
                disconnect_msg= true;
                //gen_set.wifi_mode = 3;
                //saveConfiguration();
                statusLed(green);
                delay(1000);
                ESP.restart();
                break;
        case SYSTEM_EVENT_STA_WPS_ER_FAILED:    //WPS failed
                Serial.println("WPS Failed");
                statusLed(red);
                esp_wifi_wps_disable();
                disconnect_msg= true;
                WiFi.mode(intToMode(gen_set.wifi_mode));
                break;
        case SYSTEM_EVENT_STA_WPS_ER_TIMEOUT:   //WPS failed
                Serial.println("WPS Timedout");
                statusLed(red);
                esp_wifi_wps_disable();
                disconnect_msg= true;
                WiFi.mode(intToMode(gen_set.wifi_mode));
                //  WiFi.begin();
                break;
        }
}

#endif
