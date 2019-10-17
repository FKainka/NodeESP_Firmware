/**
 *  Config.hpp
 *
 * Load and Save Configuration. Also generates the MQTT-ID from MAC Adresse.
 */

#ifndef CONFIG_H_APP
#define CONFIG_H_APP

#define MQTT_ID_AUTHOR "[PID]" //only in AUTHORMODE
#define RESEST_PIN 13          //pin for the Config-Reset

#include <ArduinoJson.h>   //https://arduinojson.org/assistant/?utm_source=github&utm_medium=issues
#include <FS.h>
#include <SPIFFS.h>
#include <WiFi.h>
#include <esp_wifi.h>

const char* con_file = "/config/config.cfg";          //Main Config-File
const char* dflt_con_file =  "/config/dflt_config.cfg";   //Default Config-File

char baseMacChr[13] = {0};
char topic[27] = {0};

StaticJsonBuffer<3000> jsonBuffer;       // !!! Allocate the memory pool on the sta

struct functions_s {       //Which Functions have been initialised successfully
        bool wifi = false;
        bool wifi_ap = false;
        bool wps = false;
        bool webserver = false;
        bool udp = false;
        bool mqtt = false;
        bool sd_card = false;
        bool app_page = false;
} functions;

struct connection_s {       //Which Connetions should be established...
        bool ws;
        bool udp;
        bool mqtt;
} connection;

struct app_set_s {          //App Executtion settings
        const char* app_exec = "dflt_init";
} app_set;

struct ota_set_s {        //Update Settings
        const char* app_version = VERSION; //hardcoded in sketch!
        const char* spiffs_version = SPIFFS_VERSION;
        const char* update_url = "http://ota.fkainka.de/dev.php";
        const char* version_url = "https://raw.githubusercontent.com/FKainka/NodeESP_Firmware/master/version.json";
        bool make_update = false;
} ota_set;

struct gen_set_s {        //Generall Settings
        uint8_t wifi_con_time = 30; //Time board waits for wifi connectin
        int wifi_mode = 3;
        const char* host_name = "NodeESP";
        const char* mdns_name = "nodeesp";
        bool status_led = true;
        const char* ap_ssid = "NodeESP";
        const char* ap_pwd = "";
        uint8_t touch_threshold = 30;
} gen_set;

struct set_udp_s {        //UDP Settings
        uint16_t udp_port_recv = 55057;
        uint16_t udp_port_send = 55057;
        const char* udp_ip = "192.168.178.255";
        bool ack = true;
} udp_set;

struct mqtt_set_s {       //MQTT Settings
        const char* server_address = "broker.fkainka.de";
        uint16_t port  = 1883;
        const char* client_id ="";
        bool enable = false;

        const char* user_name = "";
        const char* user_pw = "";

        const char* con_msg = "NanoESP32 connected";
        const char* con_topic = "";
        bool con_msg_retain = false;

        const char* last_will_msg  = "NanoESP32 disconnected";
        const char* last_will_topic = "";
        bool last_will_retain  = false;
} mqtt_set;

//Prototypes
bool loadConfiguration();
bool saveConfiguration(bool reboot);

bool load_gen_set(JsonObject &root);
bool save_gen_set(JsonObject &root);

void load_mqtt_set(JsonObject &root);
void save_mqtt_set(JsonObject &root);
void dflt_set_mqtt();

bool load_udp_set(JsonObject &root);
bool save_udp_set(JsonObject &root);

bool load_ota_set(JsonObject &root);
bool save_ota_set(JsonObject &root);

bool load_app_set(JsonObject &root);
bool save_app_set(JsonObject &root);

void loadWifiSet();
bool restore_settings(bool reboot);


/**
 * Loads the configuration from file
 * @return true if successfull
 */
bool loadConfiguration() {
        //Resset Settings if Reset_Pin is low
        pinMode(RESEST_PIN, INPUT_PULLUP);
        delay(10);
        if (digitalRead(RESEST_PIN)==LOW) restore_settings(false);

        //Load default on first run
        bool def_loaded = false;
        const char* cfg_path = con_file;
        if (!SPIFFS.exists(cfg_path)) {
                //First Run. No Config so load default
                Serial.println(F("No config file found. Loading default"));
                cfg_path = dflt_con_file;
                if (!SPIFFS.exists(cfg_path)) {
                        Serial.println(F("No default config file found!"));
                        return false;
                }
                def_loaded = true;
        }

        //Load file and parse JSON
        File file = SPIFFS.open(cfg_path);         // Open file for reading
        JsonObject &root = jsonBuffer.parseObject(file);     // Parse the root object

        //On error  in JSON-Parsing -> return false
        if (!root.success()) {
                Serial.println(F("Failed to read JSON Configuration!"));
                file.close();
                return false;
        }

        //Get Subset of settings
        JsonObject& gen_set_json = root["gen_set"];
        load_gen_set(gen_set_json);

        JsonObject& app_set_json = root["app_set"];
        load_app_set(app_set_json);

        JsonObject& udp_set_json = root["udp_set"];
        load_udp_set(udp_set_json);

        JsonObject& ota_set_json = root["ota_set"];
        load_ota_set(ota_set_json);

        JsonObject& mqtt_set_json = root["mqtt_set"];
        load_mqtt_set(mqtt_set_json);

        //Fix for Authormode Bug -> Author may use [PID] instead of Mac
        #ifndef AUTHORMODE
        String auth_id = "[PID]";
        if (String(mqtt_set.client_id) == auth_id) {
                Serial.println("Fix Author MQTT ID");
                def_loaded = true;
        }
        #endif

        if (def_loaded) dflt_set_mqtt();    //only on default load -> generates ID from MAC

        file.close();                       // Close the file (File's destructor doesn't close the file)
        if (def_loaded) saveConfiguration(true);    //Save if changes where made (on first load for example)

        return true;
}

/**
 * Writes the configuration to File on SPIFF
 * @param  reboot after writing (Should be true in most cases for consistens)
 * @return        true if successfull
 */
bool saveConfiguration(bool reboot = true) {
        if (SPIFFS.exists(con_file)) SPIFFS.remove(con_file);  // Delete existing file, otherwise the configuration is appended to the file

        // Open file for writing
        File file = SPIFFS.open(con_file, FILE_WRITE);
        if (!file) {
                Serial.println(F("Failed to create file"));
                return false;
        }

        // Create the root object, create nestet json with sub settings
        JsonObject &root = jsonBuffer.createObject();

        JsonObject& gen_set_json = root.createNestedObject("gen_set");
        save_gen_set (gen_set_json);

        JsonObject& udp_set_json = root.createNestedObject("udp_set");
        save_udp_set (udp_set_json);

        JsonObject& app_set_json = root.createNestedObject("app_set");
        save_app_set(app_set_json);

        JsonObject& ota_set_json = root.createNestedObject("ota_set");
        save_ota_set (ota_set_json);

        JsonObject& mqtt_set_json = root.createNestedObject("mqtt_set");
        save_mqtt_set (mqtt_set_json);


        if (root.printTo(file) == 0) {         // Serialize JSON to file
                Serial.println(F("Failed to write to file"));
        }
        file.close();   // Close the file (File's destructor doesn't close the file)
        Serial.println("Config updatet. Restarting.");
        delay(2000);
        if (reboot) ESP.restart();  //Mandatory. new values only on stack! will be loaded in json buffer
}

/**
 * Load General Settings
 * @param  root Json object
 * @return true if sucessfull
 */
bool load_gen_set(JsonObject &root){
        if (root.containsKey("wifi_mode")) gen_set.wifi_mode = root["wifi_mode"];
        if (root.containsKey("wifi_con_time")) gen_set.wifi_con_time = root["wifi_con_time"];
        if (root.containsKey("host_name")) gen_set.host_name = root["host_name"];
        if (root.containsKey("mdns_name")) gen_set.mdns_name = root["mdns_name"];
        if (root.containsKey("status_led")) gen_set.status_led = root["status_led"];
        if (root.containsKey("ap_ssid")) gen_set.ap_ssid = root["ap_ssid"];
        if (root.containsKey("ap_pwd")) gen_set.ap_pwd = root["ap_pwd"];
        if (root.containsKey("touch_threshold")) gen_set.touch_threshold = root["touch_threshold"];
        return true;
}

/**
 * Load Over The Air Update Settings
 * @param  root Json object
 * @return true if sucessfull
 */
bool load_ota_set(JsonObject &root){
        ota_set.spiffs_version = root["spiffs_version"];
        #if AUTHORMODE
        //ToDo: Change to actual script index.php
        ota_set.update_url = "http://ota.fkainka.de/dev.php/?branch=development"; //update script
        ota_set.version_url = "https://raw.githubusercontent.com/FKainka/NodeESP_Firmware/development/version.json";   //Verion file
        #else
        ota_set.update_url = root["update_url"];
        ota_set.version_url = root["version_url"];
        #endif
        ota_set.make_update = root["make_update"];
        return true;
}

/**
 * Save Over The Air Update Settings to JSON Object
 * @param  root Json object
 * @return true if sucessfull
 */
bool save_ota_set(JsonObject &root){
        root["spiffs_version"] = ota_set.spiffs_version;
        root["update_url"] =   ota_set.update_url;
        root["version_url"] =   ota_set.version_url;
        root["make_update"]= ota_set.make_update;
        return true;
}

/**
 * Save General Settings to JSON Object
 * @param  root Json object
 * @return true if sucessfull
 */
bool save_gen_set(JsonObject &root){
        root["wifi_mode"] =   gen_set.wifi_mode;
        root["wifi_con_time"] = gen_set.wifi_con_time;
        root["host_name"] = gen_set.host_name;
        root["mdns_name"] = gen_set.mdns_name;
        root["status_led"] = gen_set.status_led;
        root["ap_ssid"] =   gen_set.ap_ssid;
        root["ap_pwd"] = gen_set.ap_pwd;
        root["touch_threshold"] = gen_set.touch_threshold;
        return true;
}

/**
 * Load UDP Settings
 * @param  root Json object
 * @return true if sucessfull
 */
bool load_udp_set(JsonObject &root){
        udp_set.udp_port_recv = root["udp_port_recv"];
        udp_set.udp_ip= root["udp_ip"];
        udp_set.udp_port_send = root["udp_port_send"];
        udp_set.ack = root["ack"];
        return true;
}

/**
 * Save UDP Settings to JSON Object
 * @param  root Json object
 * @return true if sucessfull
 */
bool save_udp_set(JsonObject &root){
        root["udp_ip"] =   udp_set.udp_ip;
        root["udp_port_recv"] = udp_set.udp_port_recv;
        root["udp_port_send"] = udp_set.udp_port_send;
        root["ack"] =     udp_set.ack;
        return true;
}

/**
 * Load APP Settings (App to execute)
 * @param  root Json object
 * @return true if sucessfull
 */
bool load_app_set(JsonObject &root){
        if (root.containsKey("app_exec")) app_set.app_exec = root["app_exec"];
        return true;
}

/**
 * Save  APP Settings (App to execute) to JSON Object
 * @param  root Json object
 * @return true if sucessfull
 */
bool save_app_set(JsonObject &root){
        root["app_exec"] =   app_set.app_exec;
        return true;
}

/**
 * Generate Client ID from MAC-Adress and set topics based on ID
 */
void dflt_set_mqtt(){
        uint8_t baseMac[6];
        esp_read_mac(baseMac, ESP_MAC_WIFI_STA); // Get MAC address for WiFi station

        sprintf(baseMacChr, "%02X%02X%02X%02X%02X%02X", baseMac[0], baseMac[1], baseMac[2], baseMac[3], baseMac[4], baseMac[5]);
        mqtt_set.client_id = baseMacChr;

        sprintf(topic, "NodeESP/%s/state",baseMacChr);

        mqtt_set.con_topic = topic;
        mqtt_set.last_will_topic =topic;
}

/**
 * Load MQTT Settings
 * @param  root Json object
 * @return true if sucessfull
 */
void load_mqtt_set(JsonObject &root){
        mqtt_set.server_address =  root["server_address"];
        mqtt_set.port  =  root["port"];

        #ifdef AUTHORMODE
        mqtt_set.client_id = MQTT_ID_AUTHOR;    //on Author Mode use [PID]
        #else
        mqtt_set.client_id = root["client_id"];
        #endif
        mqtt_set.enable =   root["enable"];

        mqtt_set.user_name =  root["user_name"];
        mqtt_set.user_pw =   root["user_pw"];

        mqtt_set.con_msg =   root["con_msg"];
        mqtt_set.con_topic =   root["con_topic"];
        mqtt_set.con_msg_retain =  root["con_msg_retain"];

        mqtt_set.last_will_msg  =   root["last_will_msg"];
        mqtt_set.last_will_topic =   root["last_will_topic"];
        mqtt_set.last_will_retain  =   root["last_will_retain"];
}

/**
 * Save MQTT Settings to JSON Object
 * @param  root Json object
 * @return true if sucessfull
 */
void save_mqtt_set(JsonObject &root){
        root["server_address"] =   mqtt_set.server_address;
        root["port"]= mqtt_set.port;
        root["client_id"]=   mqtt_set.client_id;
        root["enable"]=   mqtt_set.enable;

        root["user_name"]=   mqtt_set.user_name;
        root["user_pw"]= mqtt_set.user_pw;

        root["con_msg"]= mqtt_set.con_msg;
        root["con_topic"]= mqtt_set.con_topic;
        root["con_msg_retain"]= mqtt_set.con_msg_retain;

        root["last_will_msg"]= mqtt_set.last_will_msg;
        root["last_will_topic"]= mqtt_set.last_will_topic;
        root["last_will_retain"]= mqtt_set.last_will_retain;
}


/**
 * Save Over The Air Update Settings to JSON Object
 * @param  root Json object
 * @return true if sucessfull
 */
bool get_functions(JsonObject &root){
        root["wifi_ap"] = functions.wifi_ap;
        root["wps"] =   functions.wps;
        root["webserver"] =   functions.webserver;
        root["udp"]= functions.udp;
        root["mqtt"] = functions.mqtt;
        root["sd_card"] =   functions.sd_card;
        root["app_page"] =   functions.app_page;
        return true;
}

/**
 * REstore default settings by deleting settings and reboot
 * @param  reboot if true (should be true almost always)
 * @return       true if susccesfull
 */
bool restore_settings(bool reboot = true){
        if (SPIFFS.remove(con_file)) Serial.println("Config file deleted");
        if (reboot) ESP.restart();
        return true;
}

/**
 * Convert Wifi-Mode settings to ENUM
 * @param  mode as integer
 * @return      mode as Enumeration
 */
wifi_mode_t intToMode(int mode){
        switch (mode) {
        case 1: return WIFI_MODE_STA;
        case 2: return WIFI_MODE_AP;
        case 3: return WIFI_MODE_APSTA;
        case 4: return WIFI_MODE_MAX;
        default: return WIFI_MODE_NULL;
        }
}
#endif
