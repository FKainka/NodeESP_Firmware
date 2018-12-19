#ifndef CMDHANLDER_HPP
#define CMDHANLDER_HPP

#include <WiFi.h>
#include <esp_wps.h>

//https://github.com/me-no-dev/ESPAsyncWebServer
#include <ESPAsyncWebServer.h>

#include "Commands.hpp"


#include "AsyncJson.h"
#include "ArduinoJson.h"

#include "CmdSettings.hpp"
#include "CmdColors.hpp"
#include "CmdGeneral.hpp"
#include "CmdIR.hpp"

bool handle_cmd(JsonObject& msg, JsonObject& answ, bool& hasAnsw, cmd_handler_s* handler){
        bool cmd_handled = false;

        String cmd = msg["cmd"];
        while(handler->cmd != "") {
                if (handler->cmd == cmd) {
                        cmd_handled = handler->func(msg,answ,hasAnsw);
                        break;
                }
                handler++;
        }

        return cmd_handled;
}

bool handle_cmd_list(JsonObject& msg, JsonObject& answ, bool& hasAnsw,  cmd_handler_s** handler_list ){

        if (!msg.containsKey("cmd")) {
                Serial.println("Unknown JSON Format");
                msg.prettyPrintTo(Serial);
                return false;
        }

        if (handler_list == NULL) return false;

        hasAnsw = false;
        int i=0;
        while(handler_list[i] != NULL) {
                if (handle_cmd(msg, answ, hasAnsw,handler_list[i] )) return true;
                i++;
        }

        Serial.println("Unknown command.");
        msg.prettyPrintTo(Serial);
        return false;
}


#endif
