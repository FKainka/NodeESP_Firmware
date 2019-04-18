#ifndef WEBSOCKETS_HPP
#define WEBSOCKETS_HPP

#include <WiFi.h>
#include <esp_wps.h>

//https://github.com/me-no-dev/ESPAsyncWebServer
#include <ESPAsyncWebServer.h>
#include <AsyncJson.h>
#include <ArduinoJson.h>

#include "../Config/Config.hpp"   
#include "../Commands/CmdHandler.hpp"
#include "../Commands/Commands.hpp"

AsyncWebSocket ws("/ws");
DynamicJsonBuffer jsonBufferWs;

void sendShortMsg(String cmd, String valueType, String value, AsyncWebSocketClient* client = NULL ){
        DynamicJsonBuffer jsonBuffer;
        JsonObject& root = jsonBuffer.createObject();
        root["cmd"] =cmd;
        root[valueType] =value;
        root.printTo(Serial);
        size_t len = root.measureLength();
        AsyncWebSocketMessageBuffer * buffer = ws.makeBuffer(len); //  creates a buffer (len + 1) for you.
        if (buffer) {
                root.printTo((char *)buffer->get(), len + 1);
                client ?  client->text(buffer) : ws.textAll(buffer);
        }
}

void wsSendJson(JsonObject& msg, AsyncWebSocketClient* client = NULL){
  size_t len = msg.measureLength();
  AsyncWebSocketMessageBuffer * buffer = ws.makeBuffer(len); //  creates a buffer (len + 1) for you.
  if (buffer) {
          msg.printTo((char *)buffer->get(), len + 1);
          client ?  client->text(buffer) : ws.textAll(buffer);
  }
}

void onWsEvent(AsyncWebSocket * server, AsyncWebSocketClient * client, AwsEventType type, void * arg, uint8_t *data, size_t len){
        if(type == WS_EVT_CONNECT) {
                Serial.printf("ws[%s][%u] connect\n", server->url(), client->id());
                sendShortMsg("log","msg","connected", client);
                //client->ping();

        } else if(type == WS_EVT_DISCONNECT) {
                Serial.printf("ws[%s][%u] disconnect: %u\n", server->url(), client->id());

        } else if(type == WS_EVT_ERROR) {
                Serial.printf("ws[%s][%u] error(%u): %s\n", server->url(), client->id(), *((uint16_t*)arg), (char*)data);

        } else if(type == WS_EVT_PONG) {
                Serial.printf("ws[%s][%u] pong[%u]: %s\n", server->url(), client->id(), len, (len) ? (char*)data : "");

        } else if(type == WS_EVT_DATA) {
                AwsFrameInfo * info = (AwsFrameInfo*)arg;
                String msg = "";

                if(info->final && info->index == 0 && info->len == len) {
                        //the whole message is in a single frame and we got all of it's data
                        Serial.printf("ws[%s][%u] %s-message[%llu]: ", server->url(), client->id(), (info->opcode == WS_TEXT) ? "text" : "binary", info->len);
                        //  Serial.printf("%s\n",(char*)msg);

                        if(info->opcode == WS_TEXT) {
                                data[len] = 0;
                                Serial.printf("%s\n", (char*)data);


                                JsonObject& msg = jsonBufferWs.parseObject((char*)data);
                                JsonObject& answ = jsonBufferWs.createObject();

                                bool hasAnsw;

                                if (handle_cmd_list(msg,answ,hasAnsw,cmd_handler_list.ws_list)) {
                                        if (hasAnsw) {
                                                size_t len = answ.measureLength();
                                                AsyncWebSocketMessageBuffer * buffer = ws.makeBuffer(len); //  creates a buffer (len + 1) for you.
                                                if (buffer) {
                                                        answ.printTo((char *)buffer->get(), len + 1);
                                                        client ?  client->text(buffer) : ws.textAll(buffer);
                                                }
                                        }
                                }
                                jsonBufferWs.clear();
                        }

                } else {
                        //message is comprised of multiple frames or the frame is split into multiple packets
                        if(info->index == 0) {
                                if(info->num == 0)
                                        Serial.printf("ws[%s][%u] %s-message start\n", server->url(), client->id(), (info->message_opcode == WS_TEXT) ? "text" : "binary");
                                Serial.printf("ws[%s][%u] frame[%u] start[%llu]\n", server->url(), client->id(), info->num, info->len);
                        }

                        Serial.printf("ws[%s][%u] frame[%u] %s[%llu - %llu]: ", server->url(), client->id(), info->num, (info->message_opcode == WS_TEXT) ? "text" : "binary", info->index, info->index + len);

                        if(info->opcode == WS_TEXT) {
                                Serial.printf("%s\n", (char*)data);
                                JsonObject& msg = jsonBufferWs.parseObject((char*)data);
                                JsonObject& answ = jsonBufferWs.createObject();
                                bool hasAnsw;
                                if (handle_cmd_list(msg,answ,hasAnsw,cmd_handler_list.ws_list)) {
                                        if (hasAnsw) {
                                                size_t len = answ.measureLength();
                                                AsyncWebSocketMessageBuffer * buffer = ws.makeBuffer(len); //  creates a buffer (len + 1) for you.
                                                if (buffer) {
                                                        answ.printTo((char *)buffer->get(), len + 1);
                                                        client ?  client->text(buffer) : ws.textAll(buffer);
                                                }
                                        }
                                }
                                jsonBufferWs.clear();

                        }
                }
        }
}

#endif
