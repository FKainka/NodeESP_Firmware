#ifndef UDP_HPP
#define UDP_HPP

#include <WiFiUdp.h>
#include <ArduinoJson.h> //https://arduinojson.org/assistant/?utm_source=github&utm_medium=issues

#include "../Config/Config.hpp"
#include "../Commands/CmdHandler.hpp"
#include "../Commands/Commands.hpp"
WiFiUDP udp;

#define UDP_BUFFER_SIZE 2048

char incomingPacket[UDP_BUFFER_SIZE];
void (*handle_upd_direct)(char* msg) = NULL; //handler may be registered for direkt acces to udp msg

bool handle_cmd_list(JsonObject& msg, JsonObject& answ, bool& hasAnsw,  cmd_handler_s** handler_list);

bool udp_send_direct(const char* msg, String ip = udp_set.udp_ip, uint16_t port = udp_set.udp_port_send){
        udp.beginPacket(ip.c_str(),port);
        udp.printf(msg);
        udp.endPacket();
}

bool udp_send_json(JsonObject& msg, String ip = udp_set.udp_ip, uint16_t port = udp_set.udp_port_send){
        udp.beginPacket(ip.c_str(),port);
        msg.printTo(udp);
        udp.endPacket();
}

void udp_recv_task(void * parameter ){
        DynamicJsonBuffer jsonBufferUdp;
        while(1) {
                int packetSize = udp.parsePacket();

                if (packetSize)
                {
                        // receive incoming UDP packets
                        Serial.printf("Received %d bytes from %s, port %d\n", packetSize, udp.remoteIP().toString().c_str(), udp.remotePort());

                        int len = udp.read(incomingPacket, UDP_BUFFER_SIZE);
                        if (len > 0)   {    incomingPacket[len] = 0;    }

                        Serial.printf("UDP packet contents: %s\n", incomingPacket);

                        if (handle_upd_direct != NULL) {
                                handle_upd_direct(incomingPacket);

                        }
                        else{

                                JsonObject& msg = jsonBufferUdp.parseObject((char*)incomingPacket);
                                JsonObject& answ = jsonBufferUdp.createObject();

                                bool hasAnsw = false;
                                if (handle_cmd_list(msg,answ,hasAnsw,cmd_handler_list.udp_list)) {
                                        if (hasAnsw) {
                                                udp_send_json(answ, udp.remoteIP().toString().c_str(), udp.remotePort());
                                        }
                                }
                                if(!hasAnsw) {
                                        if (udp_set.ack) {
                                                answ["cmd"] = "ack";
                                                udp_send_json(answ, udp.remoteIP().toString().c_str(), udp.remotePort());
                                        }
                                }
                                jsonBufferUdp.clear();

                        }

                }
                delay(100);
        }
}



void udp_init() {
        connection.udp = true;
        udp.begin(udp_set.udp_port_recv);
        xTaskCreate(udp_recv_task,"TaskUDP",4000,NULL,1,NULL);
}

#endif
