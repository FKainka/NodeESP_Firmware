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

void returnFail(AsyncWebServerRequest *request, String msg) {
        request->send(500, "text/plain", msg + "\r\n");
}

void returnOK(AsyncWebServerRequest *request) {
        request->send(200, "text/plain", "");
}

void printRequest(AsyncWebServerRequest *request){
        Serial.printf("\nREQ - url: %s, #param: %d, #arg: %d \n\n",request->url().c_str(), request->params(), request->args());
}

/**
 * [createPathJson description]
 * @param  fs   [description]
 * @param  path ohne SD Prefix
 * @param  sd   [description]
 * @param  msg  [description]
 * @return      [description]
 */
bool createPathJson(FS fs, String path, bool sd, JsonArray& msg){
        Serial.println("path CP: "+ path);
        File dir = fs.open(path);

        if (!dir.isDirectory()) {
                dir.close();
                return false;
        }
        dir.rewindDirectory();

        while(true) {
                File entry = dir.openNextFile();
                if (!entry) break;

                JsonObject& msg_element = msg.createNestedObject();


                String filename = entry.name();
                Serial.println(filename);

                bool directory = entry.isDirectory();
                msg_element["type"] = directory ? "dir" : "file";

                if (sd) filename = "/sd"+filename;
                else filename = "/intern"+filename;
                msg_element["name"] = filename;

                entry.close();
        }
        dir.close();

        return true;
}

void printDirectory(AsyncWebServerRequest *request) {
        if (!request->hasArg("dir")) return returnFail(request,"BAD ARGS");

        String path = request->arg("dir");
        Serial.println("path: "+ path);

        DynamicJsonBuffer jsonBuffer;
        JsonArray& root = jsonBuffer.createArray();

        if (path == "/") {
                JsonObject& msg_element = root.createNestedObject();
                msg_element["type"] ="dir";
                msg_element["name"] = "/intern/";

                JsonObject& msg_element2 = root.createNestedObject();
                msg_element2["type"] ="dir";
                msg_element2["name"] = "/sd/";

        }
        else{
                if (path.startsWith("/sd/")) {
                        path.replace("/sd/", "/");
                        Serial.println("path: "+ path);
                        if (!createPathJson(SD, path,true, root)) Serial.println("Error");
                }
                else if (path.startsWith("/intern/")) {
                        Serial.println("path: "+ path);
                        path.replace("/intern/", "/");
                        if (!createPathJson(SPIFFS, path,false, root)) Serial.println("Error");
                }
        }

        AsyncResponseStream *response = request->beginResponseStream("text/json");
        root.printTo(*response);
        request->send(response);
}

void handleUpload(AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final){
        printRequest(request);

        Serial.println("filename: " +filename);
        Serial.println("index: " +String(index));
        Serial.println("length: " +String(len));
          //Serial.println("data: " +data);

        if (!filename.startsWith("/sd/")) return returnFail(request,"Upload to SD only!");
        filename.replace("/sd/", "/");

        if ((index == 0) &&SD.exists(filename)) SD.remove(filename);
        File uploadFile = SD.open(filename, FILE_APPEND);

        if(!index) {
                Serial.printf("UploadStart: %s\n", filename.c_str());
        }
        if (uploadFile) {
                uploadFile.write(data,len);
                uploadFile.close();
        }
        if(final) {
                Serial.printf("UploadEnd: %s, %u B\n", filename.c_str(), index+len);
        }
}

/*
   void handleFileUpload(AsyncWebServerRequest *request) {
   if (request->uri() != "/edit") return;
   HTTPUpload& upload = request->upload();
   if (upload.status == UPLOAD_FILE_START) {
    if (SD.exists((char *)upload.filename.c_str())) SD.remove((char *)upload.filename.c_str());
    uploadFile = SD.open(upload.filename.c_str(), FILE_WRITE);
    Serial.print("Upload: START, filename: "); Serial.println(upload.filename);
   } else if (upload.status == UPLOAD_FILE_WRITE) {
    if (uploadFile) uploadFile.write(upload.buf, upload.currentSize);
    Serial.print("Upload: WRITE, Bytes: "); Serial.println(upload.currentSize);
   } else if (upload.status == UPLOAD_FILE_END) {
    if (uploadFile) uploadFile.close();
    Serial.print("Upload: END, Size: "); Serial.println(upload.totalSize);
   }
   }*/

void deleteRecursive(String path) {
        File file = SD.open(path);
        if (!file.isDirectory()) {
                file.close();
                SD.remove(path);
                return;
        }

        file.rewindDirectory();
        while (true) {
                File entry = file.openNextFile();
                if (!entry) break;
                String entryPath = path + "/" + entry.name();
                if (entry.isDirectory()) {
                        entry.close();
                        deleteRecursive(entryPath);
                } else {
                        entry.close();
                        SD.remove(entryPath);
                }
                yield();
        }

        SD.rmdir(path);
        file.close();
}

void handleDelete(AsyncWebServerRequest *request) {
        printRequest(request);
        if (request->args() == 0) return returnFail(request,"BAD ARGS");
        String path = request->arg("path");
        Serial.println("path: " +path);
        if (!path.startsWith("/sd/")) return returnFail(request,"Create on SD only!");

        path.replace("/sd/", "/");
        Serial.println("path: "+path);

        if (path == "/" || !SD.exists(path)) {
                returnFail(request,"BAD PATH");
                return;
        }
        deleteRecursive(path);
        returnOK(request);
}

void handleCreate(AsyncWebServerRequest *request) {
        printRequest(request);
        if (request->args() == 0) return returnFail(request,"BAD ARGS");
        String path = request->arg("path");
        Serial.println("path: " +path);
        if (!path.startsWith("/sd/")) return returnFail(request,"Create on SD only!");

        path.replace("/sd/", "/");
        Serial.println("path: "+path);

        if (path == "/" || SD.exists(path)) {
                returnFail(request,"BAD PATH");
                return;
        }

        if (path.indexOf('.') > 0) {
                File file = SD.open(path, FILE_WRITE);
                if (file) {

                        file.close();
                }
        } else {
                SD.mkdir(path);
        }
        returnOK(request);
}


bool webserver_init(){
        ws.onEvent(onWsEvent);
        server.addHandler(&ws);

        server.addHandler(new FSEditor(SPIFFS, SD, functions.sd_card,"don","test"));
        server.serveStatic("/", SPIFFS, "/").setDefaultFile("index.htm").setCacheControl("max-age=600");;

        if (functions.sd_card) { // Prior SD?, Handle Upload etc
                Serial.println("Webserver with SD Card support");
                server.serveStatic("/sd", SD, "/").setDefaultFile("index.htm").setCacheControl("max-age=600");
                server.serveStatic("/intern", SPIFFS, "/").setDefaultFile("index.htm");

                /*server.on("/list", printDirectory);

                server.on("/edit", HTTP_DELETE, handleDelete);
                server.on("/edit", HTTP_PUT, handleCreate);
                server.onFileUpload(handleUpload);*/


        }

        server.onNotFound([](AsyncWebServerRequest *request){
                if (request->method() == HTTP_POST){
                  returnOK(request);
                  return;
                }
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
