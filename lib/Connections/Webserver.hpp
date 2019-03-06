#ifndef WEBSERVER_HPP
#define WEBSERVER_HPP

#include <WiFi.h>
#include <ESPAsyncWebServer.h>//https://github.com/me-no-dev/ESPAsyncWebServer
#include <FS.h>
#include <SPIFFS.h>
#include <SD.h>
#include <SPI.h>
#include <AsyncJson.h>
#include <ArduinoJson.h>

#include "Websockets.hpp"

//#include <SPIFFSEditor.h>
#include <FSEditor.h>

AsyncWebServer server(80);

void responseFunctions(AsyncWebServerRequest *request){
              DynamicJsonBuffer jsonBuffer;
              JsonObject& root = jsonBuffer.createObject();
              get_functions(root);
              AsyncResponseStream *response = request->beginResponseStream("text/json");
              root.printTo(*response);
              request->send(response);
}

bool webserver_init(){
        ws.onEvent(onWsEvent);
        server.addHandler(&ws);
        server.addHandler(new FSEditor(SPIFFS, SD, functions.sd_card));
        server.serveStatic("/", SPIFFS, "/").setDefaultFile("index.htm");
        server.serveStatic("/intern", SPIFFS, "/").setDefaultFile("index.htm");
        server.on("/functions", responseFunctions);


        if (functions.sd_card) { // Prior SD?, Handle Upload etc
                Serial.println("Webserver with SD Card support");
                server.serveStatic("/sd", SD, "/").setDefaultFile("index.htm").setCacheControl("max-age=600");
        }

        server.onNotFound([](AsyncWebServerRequest *request){
                Serial.println("NOT_FOUND: " + request->url());
                AsyncWebServerResponse *response = request->beginResponse(SPIFFS, "/404.htm");
                response->setCode(404);
                request->send(response);
        });

        Serial.println("Webserver running!");
        server.begin();
        connection.ws = true;
        return true;
}

#endif
