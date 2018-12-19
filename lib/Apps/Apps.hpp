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
#include "APP_examples.hpp"

//app_cmd_handler_s* app_cmd_handler[] = NULL;


bool list_apps(JsonObject& msg, JsonObject& answ, bool& hasAnsw){
        answ["cmd"] = "get_app_set";

        JsonObject& app_array_json = answ.createNestedObject("apps");

        for (int i = 0; i< len_app_lists; i++) {

                JsonArray& app_list_array_json = app_array_json.createNestedArray(app_lists[i].info.c_str()); //trick to create array on pther buffer
                apps_s* exc_app = app_lists[i].apps_pnt;
                while(exc_app->app != "") {
                        JsonObject& app_json = app_list_array_json.createNestedObject();
                        app_json["app"] = (exc_app->app).c_str();
                        app_json["info"] = (exc_app->info).c_str();
                        if  (String(app_set.app_exec) == exc_app->app) answ["app_running"] = exc_app->info;
                        exc_app++;
                }

        }

//  answ.prettyPrintTo(Serial);
        hasAnsw = true;
        return true;
}

/*
   bool list_apps(JsonObject& msg, JsonObject& answ, bool& hasAnsw){
        answ["cmd"] = "get_app_set";

        JsonArray& app_array_json = answ.createNestedArray("apps");

        apps_s* exc_app = apps;
        while(exc_app->app != "") {
                JsonObject& app_json = app_array_json.createNestedObject();
                app_json["app"] = (exc_app->app).c_str();
                app_json["info"] = (exc_app->info).c_str();
                if  (String(app_set.app_exec) == exc_app->app) answ["app_running"] = exc_app->info;
                exc_app++;
        }
   //  answ.prettyPrintTo(Serial);
        hasAnsw = true;
        return true;
   }
 */

void exec_app(const char* app_c){
        String app = app_c;
        bool executed = false;
        apps_s* exc_app;
        for (int i = 0; i< len_app_lists; i++) {
                exc_app = app_lists[i].apps_pnt;
                while(exc_app->app != "") {
                        if (exc_app->app == app) {
                                executed = true;
                                Serial.printf("Running: %s\n",app_c);
                                exc_app->func();
                                break;
                        }
                        exc_app++;
                }
                if (executed) break;
        }
        if (!executed) {
                exc_app = apps_intern;
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
        if (!executed) {
                Serial.printf("App %s not found. Running default app.",app_c);
                dflt_init();
        }
}

/**
 * Standart Task for apps
 *
 * @param task parameter,
 * paramter:  (AppStructs.hpp)
 *       cmd_handler_s* functions; //Function to be executed
 *       connection_s* con;         //connections used for Communication
 *       uint16_t delay;            // delay between two executions
 */
void task_ref(void* parameter){
        task_parameter_s* para;
        para= (task_parameter_s*) parameter;
        DynamicJsonBuffer jsonBuffer;         // Allocate the memory pool on the stack
        while(1) {

                JsonObject &root = jsonBuffer.createObject();
                bool hasAnsw = false;    int i = 0;


                while(para->functions[i].cmd != "") {
                        para->functions[i].func(root,root,hasAnsw);
                        i++;
                }

                if (hasAnsw) {
                        if (!root.containsKey("cmd")) root["cmd"] = para->functions[0].cmd;
                        if  (para->con->udp) udp_send_json(root, udp_set.udp_ip, udp_set.udp_port_send);
                        if  (para->con->ws) wsSendJson(root);
                        if  (para->con->mqtt) mqtt_publish_json(root);
                }
                jsonBuffer.clear();
              //  Serial.printf("Delay: %d", para->delay);
                delay(para->delay);
        }
}

void firmware_update(){
        app_set.app_exec = "dflt_init";
        saveConfiguration(false);
        if (wifiInit()) {
                bool new_spiffs, new_app;
                ota_check_update(new_spiffs, new_app);
                ota_make_update(new_spiffs,new_app);
        }
}

void dflt_init(){
        wps_enable(true);
        wifiInit();
        webserver_init();
}

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
