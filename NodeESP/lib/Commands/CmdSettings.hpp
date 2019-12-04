#ifndef CMDSETTINGS_HPP
#define CMDSETTINGS_HPP

#include <WiFi.h>
#include <esp_wps.h>

//https://github.com/me-no-dev/ESPAsyncWebServer
#include <ESPAsyncWebServer.h>

#include "Commands.hpp"
#include "Apps.hpp"

#include "AsyncJson.h"
#include "ArduinoJson.h"
#include "Config.hpp"
#include "WifiSetup.hpp"
#include "OTA.hpp"

#include "MQTT.hpp"

//bool (*additional_handler)(JsonObject& msg, JsonObject& answ, bool& hasAnsw) = NULL;

bool con_firmware_update(JsonObject &msg, JsonObject &answ, bool &hasAnsw)
{
        bool new_spiffs, new_app;
        ota_check_update(new_spiffs, new_app);

        if (new_spiffs | new_app)
        {
                answ["cmd"] = "confirm";
                answ["msg"] = "Newer firmware version found. Update now?";
                answ["confirm"] = "do_firmware_update";
                answ["alert"] = "Updating board firmware. This will take a while (~5 Minutes).";
        }
        else
        {
                answ["cmd"] = "alert";
                answ["msg"] = "NodeESP ist up to date.";
        }
        hasAnsw = true;
        return true;
}

bool ping(JsonObject &msg, JsonObject &answ, bool &hasAnsw)
{
        answ["cmd"] = "pong";
        hasAnsw = true;
        return true;
}

bool do_firmware_update(JsonObject &msg, JsonObject &answ, bool &hasAnsw)
{
        strcpy(app_set.app_exec, "firmware_update");
        //  app_set.app_exec = "firmware_update";
        saveConfiguration(true);
        return true;
}

bool con_restore(JsonObject &msg, JsonObject &answ, bool &hasAnsw)
{
        answ["cmd"] = "confirm";
        answ["msg"] = "Do you realy want to restore to default settings?";
        answ["confirm"] = "do_restore";
        answ["alert"] = "Restoring to default settings. Board will reset.";
        hasAnsw = true;
        return true;
}

bool do_restore(JsonObject &msg, JsonObject &answ, bool &hasAnsw)
{
        restore_settings();
        return true;
}

bool get_wifi_status(JsonObject &msg, JsonObject &answ, bool &hasAnsw)
{
        answ["cmd"] = "get_wifi_status";
        answ["stat_station"] = WiFi.SSID();
        answ["stat_ip"] = WiFi.localIP().toString();
        answ["stat_connected"] = (WiFi.status() == WL_CONNECTED);
        hasAnsw = true;
        return true;
}

bool get_gen_set(JsonObject &msg, JsonObject &answ, bool &hasAnsw)
{
        //Save config to JSON!
        answ["cmd"] = "get_gen_set";
        save_gen_set(answ);
        answ.printTo(Serial);
        hasAnsw = true;
        return true;
}

bool get_udp_set(JsonObject &msg, JsonObject &answ, bool &hasAnsw)
{
        //Save config to JSON!
        answ["cmd"] = "get_udp_set";
        save_udp_set(answ);
        answ.printTo(Serial);
        hasAnsw = true;
        return true;
}

bool get_mqtt_set(JsonObject &msg, JsonObject &answ, bool &hasAnsw)
{
        //Save config to JSON!
        answ["cmd"] = "get_mqtt_set";
        save_mqtt_set(answ);
        answ.printTo(Serial);
        hasAnsw = true;
        return true;
}

bool set_app_set(JsonObject &msg, JsonObject &answ, bool &hasAnsw)
{
        if (!msg.containsKey("app_select"))
                return false;
        strcpy(app_set.app_exec, msg["app_select"]);
        //app_set.app_exec = msg["app_select"];
        saveConfiguration(true);
        return true;
}

bool get_mqtt_check(JsonObject &msg, JsonObject &answ, bool &hasAnsw)
{
        answ["cmd"] = "get_mqtt_check";
        answ["connected"] = test_mqtt_connection();
        hasAnsw = true;
        return true;
}

bool set_gen_set(JsonObject &msg, JsonObject &answ, bool &hasAnsw)
{
        //Load JSON to Structs!
        load_gen_set(msg);
        saveConfiguration(true);
        return true;
}

bool set_udp_set(JsonObject &msg, JsonObject &answ, bool &hasAnsw)
{
        //Load JSON to Structs!
        load_udp_set(msg);
        saveConfiguration(true);
        return true;
}

bool set_mqtt_set(JsonObject &msg, JsonObject &answ, bool &hasAnsw)
{
        //Load JSON to Structs!
        load_mqtt_set(msg);
        saveConfiguration(true);
        return true;
}

bool wifi_wps(JsonObject &msg, JsonObject &answ, bool &hasAnsw)
{
        xTaskCreate(taskWpsSetup, "wpssetup", 2000, NULL, 3, NULL);
        return true;
}

bool set_wifi(JsonObject &msg, JsonObject &answ, bool &hasAnsw)
{
        const char *ssid = msg["wifi_ssid"];
        const char *password = msg["wifi_pwd"];

        Serial.printf("SSID: %s\t PWD:&s ", ssid, password);
        WiFi.begin(ssid, password);

        saveConfiguration(true);
        return true;
}

bool get_wifi_scan(JsonObject &msg, JsonObject &answ, bool &hasAnsw)
{
        int n = WiFi.scanNetworks();
        Serial.println("scan done");
        if (n == 0)
                Serial.println("no networks found");
        else
        {
                answ["cmd"] = "get_wifi_scan";
                JsonArray &ssids = answ.createNestedArray("ssids");
                for (int i = 0; i < n; ++i)
                {
                        ssids.add(WiFi.SSID(i));
                        delay(10);
                }
                answ.prettyPrintTo(Serial);
        }
        hasAnsw = true;
        return true;
}

bool get_app_set(JsonObject &msg, JsonObject &answ, bool &hasAnsw)
{
        return list_apps(msg, answ, hasAnsw);
}

#endif
