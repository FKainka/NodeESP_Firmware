#ifndef OTA_H
#define OTA_H

#define OTA_DEBUG Serial

#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ESP32httpUpdate.h>
#include <ArduinoJson.h> //https://arduinojson.org/assistant/?utm_source=github&utm_medium=issues
#include <Update.h>
#include <StatusLed.hpp>

#include "Config.hpp"

void ota_print_return(t_httpUpdate_return ret)
{
  switch (ret)
  {
  case HTTP_UPDATE_FAILED:
    Serial.printf("HTTP_UPDATE_FAILD Error (%d): %s", ESPhttpUpdate.getLastError(), ESPhttpUpdate.getLastErrorString().c_str());
    break;

  case HTTP_UPDATE_NO_UPDATES:
    Serial.println("HTTP_UPDATE_NO_UPDATES");
    break;

  case HTTP_UPDATE_OK:
    Serial.println("HTTP_UPDATE_OK");
    break;
  }
}

bool ota_make_update(bool &new_spiffs, bool &new_app)
{
  if (new_spiffs | new_app)
  {
    statusLed(yellow);  

    if (new_spiffs)
    {
      Serial.println("Updating SPIFFS. Grap a coffe.");
    //  statusLed(blink_yellow_slow); //Start blinking while trying to estblish connection

      t_httpUpdate_return retSpiffs = ESPhttpUpdate.updateSpiffs(ota_set.update_url, ota_set.spiffs_version);
      ota_print_return(retSpiffs);
    }
    if (new_app)
    {
      Serial.println("Updating Firmware. Grap a coffe.");
   //   statusLed(blink_yellow_slow); //Start blinking while trying to estblish connection

      t_httpUpdate_return retApp = ESPhttpUpdate.update(ota_set.update_url, ota_set.app_version);
      ota_print_return(retApp);
    }

    ESP.restart();
  }
  return true;
}

bool ota_check_update(bool &new_spiffs, bool &new_app)
{
  new_spiffs = false;
  new_app = false;
  HTTPClient http;

  http.begin(ota_set.version_url); //Specify the URL
  int httpCode = http.GET();       //Make the request

  if (httpCode > 0)
  { //Check for the returning code
    DynamicJsonBuffer jsonBufferVersion;

    JsonObject &version = jsonBufferVersion.parseObject(http.getString());
    JsonObject &spiffs_json = version["spiffs"];
    JsonObject &app_json = version["sketch"];

    const char *spiffs_version = spiffs_json["version"];
    const char *app_version = app_json["version"];

    if (strcmp(spiffs_version, ota_set.spiffs_version) != 0)
    {
      Serial.println("New SPIFFS Version found!");
      new_spiffs = true;
    }
    if (strcmp(app_version, ota_set.app_version) != 0)
    {
      Serial.println("New Firmware Version found!");
      new_app = true;
    }
  }

  else
  {
    Serial.println("Error on HTTP request");
  }

  http.end(); //Free the resources
}

#endif
