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

AsyncWebServer server(80);


bool webserver_init(){
        ws.onEvent(onWsEvent);
        server.addHandler(&ws);

        server.serveStatic("/", SPIFFS, "/").setDefaultFile("index.htm");

        server.onNotFound([](AsyncWebServerRequest *request){
                        Serial.println("NOT_FOUND: " + request->url());
                        AsyncWebServerResponse *response = request->beginResponse(SPIFFS, "/404.htm");
                        response->setCode(404);
                        request->send(response);
        });

        Serial.println("Webserver running!");
        server.begin();
        connection.ws = true;



}

#endif
