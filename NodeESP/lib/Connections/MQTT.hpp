#ifndef MQTT_HPP
#define MQTT_HPP

#include <ArduinoJson.h> //https://arduinojson.org/assistant/?utm_source=github&utm_medium=issues
#include <PubSubClient.h>

#include "Config.hpp"
#include "CmdHandler.hpp"
#include "Commands.hpp"

/* create an instance of PubSubClient client */
WiFiClient espClient;
PubSubClient client(espClient);
DynamicJsonBuffer jsonBufferMqtt;
//Prototype
bool handle_cmd_list(JsonObject& msg, JsonObject& answ, bool& hasAnsw,  cmd_handler_s** handler_list);

#define MAX_SUBS 20
String subscribed_topics[MAX_SUBS] = {""};

// PREFIX           /TYPE/ ID / Additional
// NodeESP/XXXXXXXX/TYPE/NAME/X
typedef struct {
        int type;
        bool (*func)(const char * devcicename, const char* additionalTopic, const char * value);
} mqtt_handler_s;


char JSONmessageBuffer[500];

void topicToCmd(const char* topic, char* cmd){


        char specifier[20] = {'\0'};
        char device[20] = {'\0'};
        char direction[20] = {'\0'};
        char rest[20] = {'\0'};

        //NodeESP/ID/ TYPE  /device /speicifier/direction
        int parsed = 0;
        parsed = sscanf (topic,"%20*[^/]/%20*[^/]/%20[^/]/%20[^/]/%20[^/]/%s",device,specifier,direction,rest);
        if (strlen(rest) >0) return;

        if (strlen(specifier)>0) {
                if (strlen(direction)>0) {
                        sprintf(cmd,"%s_%s_%s",direction, device, specifier);
                }
                else{
                        sprintf(cmd,"%s_%s",device, specifier);
                }
        }else{
                sprintf(cmd,"%s", device);
        }

}

void cmdToTopic(const char* cmd, char* topic){

        char specifier[20] = {'\0'};
        char device[20] = {'\0'};
        char direction[20] = {'\0'};

        //NodeESP/ID/ TYPE  /device /speicifier/direction
        int parsed = 0;
        parsed = sscanf (cmd,"%20[^_]_%20[^_]_%20[^_]",direction, device, specifier);

        if (strlen(device)>0) {
                if (strlen(specifier)>0) {
                        sprintf(topic,"%s/%s/%s",device,  specifier, direction);
                }
                else{
                        sprintf(topic,"%s/%s",direction,device);
                }
        }else{
                sprintf(topic,"%s", direction);
        }
}

void mqtt_publish_json(JsonObject& msg){
        char topic[100]  ={'\0'};
        sprintf(topic,"%s/%s/%s", "NodeESP",mqtt_set.client_id,"out");

        msg.printTo(JSONmessageBuffer,sizeof(JSONmessageBuffer));

        //  Serial.printf("Topic: %s \t\t Payload: ",topic);
        //  msg.prettyPrintTo(Serial);
        //  if (client.connected()) client.publish(topic, JSONmessageBuffer);

        if (client.connected())  client.publish(topic, JSONmessageBuffer);

        if(msg.containsKey("cmd")) {
                char topic_v[30]  ={'\0'};
                cmdToTopic( msg["cmd"], topic_v);
                sprintf(topic,"%s/%s/%s/%s", "NodeESP",mqtt_set.client_id,topic_v,"out");

                if ((msg.size() == 2) and (msg.containsKey("value"))) {
                        msg["value"].printTo(JSONmessageBuffer,sizeof(JSONmessageBuffer));
                        if (client.connected())  client.publish(topic, JSONmessageBuffer);
                }
                else{
                    if (client.connected())  client.publish(topic, JSONmessageBuffer);
                }
        }

}

bool resubscribe(){
        for (int i = 0; i<MAX_SUBS; i++) {
                if (subscribed_topics[i] != "") {
                        bool succ = client.subscribe(subscribed_topics[i].c_str());
                        succ ? Serial.printf("Subscribed to %s\n",subscribed_topics[i]) : Serial.printf("error sub to %s\n",subscribed_topics[i]);
                }
        }
}

bool mqtt_subscribe_topic(const char* topic_end){
        char topic[100]  ={'\0'};
        sprintf(topic,"%s/%s/%s", "NodeESP",mqtt_set.client_id,topic_end);

        bool succ = client.subscribe(topic);

        succ ? Serial.printf("Subscribed to %s\n",topic) : Serial.printf("error sub to %s\n",topic);
        for (int i = 0; i<MAX_SUBS; i++) {
                if (subscribed_topics[i]== "") {
                        subscribed_topics[i] = topic; break;
                }
        }
        return succ;

}

void receivedCallback(char* topic, byte* payload, unsigned int length) {
        ((char*)payload)[length] = '\0';
        Serial.printf("[MQTT] Topic: %s \t\t Payload: %s", topic, (char*)payload);

        char cmd[30] ={'\0'};
        topicToCmd(topic, cmd);
        Serial.printf("Command: %s\n\n",cmd);



        JsonObject& answ = jsonBufferMqtt.createObject();
        JsonObject& msg = jsonBufferMqtt.parseObject((char*)payload);
        bool hasAnsw =0, handled = 0;
        if (strcmp(cmd,"in")==0) {
                //    Serial.println("In Channel:");
                //      msg.prettyPrintTo(Serial);
                handled =  handle_cmd_list(msg, answ, hasAnsw, cmd_handler_list.mqtt_list);
        }
        else{
                if (msg.size() >0) {
                        //          Serial.println("IS JSON:");
                        if (  msg.containsKey("cmd")) {//is a JSON
                                //                Serial.println("IS CMD FORMATED:");
                                //Check for topic
                                if (strcmp(msg["cmd"],cmd) != 0) {
                                        Serial.println("Wrong Topic");
                                }
                                else{
                                        handled =  handle_cmd_list(msg, answ, hasAnsw, cmd_handler_list.mqtt_list);
                                }
                        }
                        else{
                                //        Serial.println("IS Not CMD FORMATED:");
                                msg["cmd"] = cmd;
                                //  msg.prettyPrintTo(Serial);
                                handled =  handle_cmd_list(msg, answ, hasAnsw, cmd_handler_list.mqtt_list);
                        }
                }
                else{
                        Serial.println("IS Not JSON:");
                        JsonObject& msg_n = jsonBufferMqtt.createObject();
                        msg_n["cmd"] = cmd;
                        msg_n["value"] = (char*)payload;

                        //  msg_n.prettyPrintTo(Serial);
                        handled =  handle_cmd_list(msg_n, answ, hasAnsw, cmd_handler_list.mqtt_list);
                }
        }

        if (handled) {
                Serial.println("Command handled");
                if (hasAnsw) {
                        mqtt_publish_json(answ);
                }
        }

        jsonBufferMqtt.clear();
}

bool mqttconnect() {

        Serial.print("MQTT connecting ...");

        /* connect now */
        //!!! Currently ohne Passwordund username -> fix 16122018
        boolean conn_est = false;
        //    Serial.printf("LEN %d",strlen(mqtt_set.user_name));
        //    Serial.printf("COM %d",strcmp(mqtt_set.user_name,""));
        if ( strlen(mqtt_set.user_name) > 0) {
                conn_est =   client.connect (mqtt_set.client_id, mqtt_set.user_name, mqtt_set.user_pw, mqtt_set.last_will_topic, 0, mqtt_set.last_will_retain, mqtt_set.last_will_msg);
          //      Serial.println("MQTT with PW");
        }
        else{
                conn_est = client.connect(mqtt_set.client_id,mqtt_set.last_will_topic, 0, mqtt_set.last_will_retain, mqtt_set.last_will_msg);
            //    Serial.println("MQTT without PW");
        }
        //boolean
        if (conn_est) {
                //if (client.connect(mqtt_set.client_id, mqtt_set.user_name, mqtt_set.user_pw,mqtt_set.last_will_topic, 0, mqtt_set.last_will_retain, mqtt_set.last_will_msg))            {
                Serial.println("connected");
                /* subscribe topic with default QoS 0*/
                if (client.connected()) client.publish(mqtt_set.con_topic, mqtt_set.con_msg, mqtt_set.con_msg_retain);

                return true;
        } else {
                delay(500);         //wait a while to reconnect
                return false;
        }

}

void mqtt_loop(void * parameter){
        while(1) {
                if (!client.connected()) {
                  //    Serial.printf("MQTT Status code: %d\n",client.state());
                        mqttconnect();
                        resubscribe();
                }
                /* this function will listen for incomming
                   subscribed topic-process-invoke receivedCallback */
                client.loop();
                //!!! Check Task Stack!
        }
}





void mqtt_init() {
        /* configure the MQTT server with IPaddress and port */
        client.setServer(mqtt_set.server_address, mqtt_set.port);
        /* this receivedCallback function will be invoked
           when client received subscribed topic */
        client.setCallback(receivedCallback);
        Serial.println("Connecting to MQTT Broker");
        mqttconnect();
        while(!client.connected()) {
                delay(10); Serial.print(".");
        }
        Serial.println("connected!");
        //  Serial.println("Max Packet Size: "+String(MQTT_MAX_PACKET_SIZE));
        connection.mqtt = true;
        mqtt_subscribe_topic("in");
        xTaskCreate(mqtt_loop,"TaskMQTT",40000,NULL,2,NULL);
}

bool test_mqtt_connection() {
        if(client.connected()) return true;
        /* configure the MQTT server with IPaddress and port */
        client.setServer(mqtt_set.server_address, mqtt_set.port);
        bool connected = mqttconnect();
        if (connected) client.disconnect();
        return connected;
}



#endif
