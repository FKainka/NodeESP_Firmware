/**
 * Apps.hpp
 *
 * Makes a JSON Response of all Apps, execute selected app, has default app and handles update
 *
 */

#ifndef APP_HPP
#define APP_HPP

#include <WiFiUdp.h>
#include <ArduinoJson.h> //https://arduinojson.org/assistant/?utm_source=github&utm_medium=issues
#include "CmdHandler.hpp"
#include "AppStructs.hpp"

#include "WiFiSetup.hpp"
#include "Webserver.hpp"
#include "Udp.hpp"
#include "MQTT.hpp"
#include "OTA.hpp"

#include "Peripherals.hpp"

#include "APP_iot18.hpp"
#include "APP_sh19.hpp"
#include "APP_examples.hpp"
#include "APP_tests.hpp"

/**
 * Adds a list of all available APPs to a JSON Object - Response to JSON-Request
 * @param  msg     JSON-Request
 * @param  answ    JSON-Response -> here the apps will be added
 * @param  hasAnsw if somethings has been added to the JSON-Response this should be true
 * @return         true if susccesfull
 */
bool list_apps(JsonObject& msg, JsonObject& answ, bool& hasAnsw){
        answ["cmd"] = "get_app_set";    //the answear is of type: get_app_set

        JsonObject& app_array_json = answ.createNestedObject("apps"); //Create nestet array of APPSs
        for (int i = 0; i< len_app_lists; i++) {
                JsonArray& app_list_array_json = app_array_json.createNestedArray(app_lists[i].info.c_str()); //trick to create array on other buffer
                apps_s* exc_app = app_lists[i].apps_pnt;
                while(exc_app->app != "") {
                        JsonObject& app_json = app_list_array_json.createNestedObject();
                        app_json["app"] = (exc_app->app).c_str();
                        app_json["title"] = (exc_app->title).c_str();
                        app_json["info"] = (exc_app->info).c_str();
                        if  (String(app_set.app_exec) == exc_app->app) answ["app_running"] = exc_app->title+"<br>"+exc_app->info;
                        exc_app++;
                }
        }
        hasAnsw = true;
        return true;
}

/**
 * Function that calls the "APP" that should be executed
 * @param app_c Strong of APP that should be called
 */
void exec_app(const char* app_c){
        String app = app_c;
        bool executed = false;
        apps_s* exc_app;      //Pointer to APP
        for (int i = 0; i< len_app_lists; i++) {    //Go through lists of list of app (list_app_lists is only the list of all sub lists)
                exc_app = app_lists[i].apps_pnt;    //get pointer
                while(exc_app->app != "") {         // "" marks the end of the list
                        if (exc_app->app == app) {   // if app was found...
                                executed = true;
                                Serial.printf("Running: %s\n",app_c);
                                exc_app->func();      //... execute APP
                                break;
                        }
                        exc_app++;
                }
                if (executed) break;
        }
        if (!executed) {          //App was not found -> look if it is an "inter app"
                exc_app = apps_intern;        //Intern apps handle things like fw updates or dflt apps
                while(exc_app->app != "") {
                        if (exc_app->app == app) {
                                executed = true;
                                Serial.printf("Running: %s\n",app_c);
                                exc_app->func();
                                break;
                        }
                        exc_app++;
                }
        }
        if (!executed) {      //if nothing was called at all load the dflt app
                Serial.printf("App %s not found. Running default app.",app_c);
                dflt_init();
        }
}

/**
 * Standart Task for apps, is a task to shedule (Multitasking)
 *
 * @param task parameter,
 * paramter:  (AppStructs.hpp)
 *       cmd_handler_s* functions; //Function to be executed
 *       connection_s* con;         //connections used for Communication
 *       uint16_t delay;            // delay between two executions
 */
void task_ref(void* parameter){
        task_parameter_s* para;
        para= (task_parameter_s*) parameter;    //Get Task parameter
        DynamicJsonBuffer jsonBuffer;         // Allocate the memory pool on the stack
        while(1) {

                JsonObject &root = jsonBuffer.createObject();   //tasks will mostly generate JSON-MSGs,
                bool hasAnsw = false;    int i = 0;

                while(para->functions[i].cmd != "") {
                        para->functions[i].func(root,root,hasAnsw);   //Call function defined in Task parameter
                        i++;
                }

                if (hasAnsw) {        //if called function had an answear -> send through defined communcation port defined in task parameter
                        if (!root.containsKey("cmd")) root["cmd"] = para->functions[0].cmd;
                        if  (para->con->udp) udp_send_json(root, udp_set.udp_ip, udp_set.udp_port_send);
                        if  (para->con->ws) wsSendJson(root);
                        if  (para->con->mqtt) mqtt_publish_json(root);
                }
                jsonBuffer.clear();
                delay(para->delay);     //Wait for defined time
        }
}

/**
 * APP for firmware update
 */
void firmware_update(){
        app_set.app_exec = "dflt_init";
        saveConfiguration(false);
        if (wifiInit()) {     //Wait for wifi
                bool new_spiffs, new_app;
                ota_check_update(new_spiffs, new_app);  //check if new update available (fw or spiff)
                ota_make_update(new_spiffs,new_app);     //do update (fw or spiff)
        }
}

/**
 * Default APP:  enables WPS, Establish wifi, Check for SD Card, Initiate Webserver
 */
void dflt_init(){
        wps_enable(true);
        functions.wifi = wifiInit();
        functions.sd_card = init_sd();
        webserver_init();
}

/**
 * Optional Default APP:  if Pin23 is is not set call dflt_app, (USED for APPS without Webserver by default to be sure the user knows what he does)
 */
void opt_dflt_init(){
        pinMode(23,INPUT_PULLUP);
        delay(10);
        if (digitalRead(23)) {
                Serial.println("default");
                dflt_init();
                while(1);
        }
}

#endif
