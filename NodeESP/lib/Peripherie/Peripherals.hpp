#ifndef PERIP_HPP
#define PERIP_HPP

//#include <NeoPixelBus.h> // https://github.com/Makuna/NeoPixelBus/wiki/API-Reference

#include <ArduinoJson.h> // https://arduinojson.org/assistant/?utm_source=github&utm_medium=issues
#include <IRremote.h> // https://github.com/z3t0/Arduino-IRremote //https://github.com/SensorsIot/Definitive-Guide-to-IR
#include <driver/dac.h>

#include "NeoPixelWrapper.hpp"
#include "MusicDefinitions.h"
#include "SoundData.h"
#include "XT_DAC_Audio.h"

#define KETTE

typedef struct {
        uint8_t pin_id;
        uint8_t pin_nr;
        bool pin_state;
        uint16_t pin_value;
        String pin_name;
}pin_config_s;

int len_buttons =0; pin_config_s* buttons =NULL;
int len_touchs = 0; pin_config_s* touchs =NULL;
int len_leds = 0; pin_config_s* leds =NULL;
int len_pwms = 0; pin_config_s* pwms =NULL;
int len_adcs = 0; pin_config_s* adcs =NULL;
pin_config_s* rgb =NULL;
pin_config_s* rf =NULL;
pin_config_s* alarm_pin =NULL;
TaskHandle_t taskRgbFadeHandle = NULL;  //Handler for Blink Task, needed to Delete Task
TaskHandle_t taskSireneHandle = NULL;  //Handler for Blink Task, needed to Delete Task

//for intern Temperatur Sensor
#ifdef __cplusplus
extern "C" {
#endif
uint8_t temprature_sens_read();
#ifdef __cplusplus
}
#endif

typedef struct  {
        RgbColor fromColor;
        RgbColor toColor;
        int duration;
}fade_parameter_s;

//#ifdef BAND
#define METHOD NeoEsp32BitBang800KbpsMethod //NeoEsp32BitBangWs2813Method  // NeoEsp32BitBang400KbpsMethod NeoEsp32BitBang800KbpsMethod
#define FEATURE NeoGrbFeature // NeoRgbFeature
#define NEOCLASS  NeoPixelBrightnessBus // NeoPixelBus
#define NUMMER_PIXEL 82

/*
                          // KETTE
 #define METHOD NeoEsp32BitBangWs2813Method  // NeoEsp32BitBang400KbpsMethod
 #define FEATURE NeoRgbFeature // NeoRgbFeature
 #define NEOCLASS NeoPixelBrightnessBus // NeoPixelBus
 #define NUMMER_PIXEL 40
                          /*
 #elseif RINGE
 #define METHOD NeoEsp32BitBangWs2813Method  // NeoEsp32BitBang400KbpsMethod
 #define FEATURE NeoGrbFeature // NeoRgbFeature
 #define NEOCLASS NeoPixelBrightnessBus // NeoPixelBus
 #define NUMMER_PIXEL 16
 #endif
 */
NeoPixelWrapper* strip = NULL;
int len_pixel = 0;
RgbColor curColor;
TaskHandle_t taskPixelFadeHandle = NULL;  //Handler for Blink Task, needed to Delete Task
const char* cfg_neoPixel_path = "/apps/neo_config.cfg";

// IR Stuff
IRrecv* irrecv;
IRsend* irsend;
bool ir_toggle; //For RC5/RC6

//Speaker
XT_DAC_Audio_Class* DacAudio;    // Create the main player class object. Use GPIO 25, one of the 2 DAC pins and timer 0
TaskHandle_t taskSongHandle = NULL;  //Handler for Blink Task, needed to Delete Task
/*typedef struct {
        unsigned long value;
        int code_type; //decode_type_t          decode_type;  // UNKNOWN, NEC, SONY, RC5, ...
        int code_len;
        unsigned int address;
   }ir_code_s;
 */

//Easy IO
volatile bool button_change = false;
volatile bool touch_change = false;
int threshold;



//id, pin_nr(io), state bool, value int, string cleanname;
pin_config_s buttons_dflt[]= {
        {1,18,false,0,"Button1"},
        {2,16,false,0,"Button2"},
        {3,23,false,0,"Tilt"},
}; int len_buttons_dflt =  sizeof(buttons_dflt)/sizeof(buttons_dflt[0]);

pin_config_s touch_dflt[]= { //!!! leerzeichen eingeführt
        {7,T7,false,0,"Touch 3"},
        {8,T8,false,0,"Touch 2"},
        {9,T9,false,0,"Touch 1"},
}; int len_touch_dflt =  sizeof(touch_dflt)/sizeof(touch_dflt[0]);


pin_config_s leds_dflt[]= {
        {1,22,false,0,"LED1"},
        {2,15,false,0,"LED2"},
}; int len_leds_dflt = sizeof(leds_dflt)/sizeof(leds_dflt[0]);

pin_config_s onBoard_led[]= {
        {1,red_led,false,0,"red"},
        {2,green_led,false,0,"green"},
        {3,blue_led,false,0,"blue"},
}; int onBoard_led_len = sizeof(onBoard_led)/sizeof(onBoard_led[0]);

pin_config_s adcs_dflt[]= {
        {1,35,false,0,"Light"},
        {2,36,false,0,"Temp"},
        //{3,26,false,0,"Light"},
}; int len_adcs_dflt = sizeof(adcs_dflt)/sizeof(adcs_dflt[0]);

pin_config_s rf_dflt[]= {
        {1,32,false,0,"HUMIDITY"}, //Former: 34/39 but they dont have pullup
        {2,33,false,0,"GND"},
}; int len_rf_dflt = sizeof(rf_dflt)/sizeof(rf_dflt[0]);

pin_config_s ir_recv_pins[]= {
        {1,19,false,0,"DOUT"},
        {2,21,true,0,"3V3"},
}; int ir_recv_pins_len = sizeof(ir_recv_pins)/sizeof(ir_recv_pins[0]);

pin_config_s ir_send_pin=  {1,17,false,0,"IR"};
pin_config_s speaker_pin=  {1,25,false,0,"SPEAKER"};
pin_config_s neo_pixel_pin= {1,14,false,0,"NeoPixel"};
pin_config_s gnd_support_pin= {1,5,false,0,"GND"};

portMUX_TYPE mux_buttons = portMUX_INITIALIZER_UNLOCKED;
portMUX_TYPE mux_buttons_int = portMUX_INITIALIZER_UNLOCKED;

//---SPEAKER----
hw_timer_t * speaker_timer = NULL;
portMUX_TYPE timerMuxS = portMUX_INITIALIZER_UNLOCKED;
#define SOUND_BUFFER_LEN 2000
#define SOUND_BUFFER_PAGES 10
uint8_t sound_buffer[SOUND_BUFFER_LEN];
long sound_read_pointer = 0;
long sound_read_len =0;
bool sound_play = false;

bool get_pin_state_json(pin_config_s* pin_config, int len,  JsonObject& msg){
        JsonArray& pin_array_json = msg.createNestedArray("value");

        for (int i =0; i<len; i++) {
                JsonObject& pin_json = pin_array_json.createNestedObject();
                pin_json["pin_name"] = pin_config[i].pin_name.c_str();
                pin_json["pin_id"] = pin_config[i].pin_id;
                pin_json["pin_nr"] = pin_config[i].pin_nr;
                pin_json["pin_state"] = pin_config[i].pin_state;
                pin_json["pin_value"] = pin_config[i].pin_value;
        }
        //    msg.prettyPrintTo(Serial);
        return true;
}

bool set_pin_state_json(pin_config_s* pin_config, int len,  JsonObject& msg){
        //  JsonArray& pin_array_json = msg.createNestedArray("pin_state");
        for (int i =0; i<len; i++) {
                const char* pinName =pin_config[i].pin_name.c_str();
                if(msg.containsKey(pinName)) {
                        if (msg.is<bool>(pinName)) pin_config[i].pin_state = msg[pinName];
                        if (msg.is<unsigned int>(pinName)) pin_config[i].pin_value = msg[pinName];
                        if (msg.is<const char*>(pinName)) {
                                if (strcmp(msg[pinName], "on") == 0) pin_config[i].pin_state =true;
                                if (strcmp(msg[pinName], "ON")== 0) pin_config[i].pin_state =true;
                                if (strcmp(msg[pinName], "off")== 0) pin_config[i].pin_state =false;
                                if (strcmp(msg[pinName], "OFF")== 0) pin_config[i].pin_state =false;
                        }
                }
        }
        return true;
}

//Single pin!
bool set_single_pin_state_json(pin_config_s* pin_config,  JsonObject& msg){
        if (msg.containsKey("pin_state")) pin_config->pin_state= msg["pin_state"];
        if (msg.containsKey("pin_value")) pin_config->pin_value= msg["pin_value"];
        return true;
}

void IRAM_ATTR buttons_state(pin_config_s* button_config = buttons, int len = len_buttons){
        portENTER_CRITICAL(&mux_buttons);
        if (button_config != NULL) for (int i =0; i<len; i++) {
                        bool state = digitalRead(button_config[i].pin_nr);
                        if ( button_config[i].pin_state != state) {
                                button_config[i].pin_state = state;
                                button_change = true;
                        }
                }
        portEXIT_CRITICAL(&mux_buttons);
}

void IRAM_ATTR buttons_interrupt(){
        buttons_state();
}

void buttons_init(pin_config_s* button_config_v = buttons_dflt, int len_v= len_buttons_dflt, bool interrupt = false){
        buttons = button_config_v;
        len_buttons = len_v;

        for (int i =0; i<len_buttons; i++) {
                pinMode(buttons[i].pin_nr,INPUT_PULLUP);
                if (interrupt) attachInterrupt(digitalPinToInterrupt(buttons[i].pin_nr), buttons_interrupt, CHANGE);
        }

        if(connection.mqtt == true) mqtt_subscribe_topic("button/+/get");
}

void IRAM_ATTR touch_state(pin_config_s* button_config= touchs, int len = len_touchs){
        for (int i =0; i<len; i++) {
                button_config[i].pin_value =touchRead(button_config[i].pin_nr);

                bool state = button_config[i].pin_value <= threshold;
                if ( button_config[i].pin_state != state) {
                        button_config[i].pin_state= state;
                        touch_change = true;
                }
        }
}

void IRAM_ATTR touch_interrupt(){
        touch_state();
}

void touch_init(pin_config_s* button_config = touch_dflt, int len = len_touch_dflt, bool interrupt = true, int threshold_v = gen_set.touch_threshold){
        touchs = button_config;
        len_touchs = len;
        threshold = threshold_v;

        for (int i =0; i<len; i++) {
                if (interrupt) touchAttachInterrupt(button_config[i].pin_nr, touch_interrupt, threshold);
        }

        if(connection.mqtt == true) mqtt_subscribe_topic("touch/+/get");
}

void support_init(pin_config_s* button_config = &gnd_support_pin, int len = 1){
        for (int i =0; i<len; i++) {
                pinMode(button_config[i].pin_nr,OUTPUT);
                digitalWrite(button_config[i].pin_nr, button_config[i].pin_value);
        }
}

void digital_state(pin_config_s* button_config = leds, int len = len_leds){
        for (int i =0; i<len; i++) digitalWrite(button_config[i].pin_nr, button_config[i].pin_state);
}

void digital_init(pin_config_s* button_config = leds_dflt, int len = len_leds_dflt){
        leds = button_config;
        len_leds = len;
        for (int i =0; i<len; i++) {
                pinMode(button_config[i].pin_nr,OUTPUT);
        }

}

void leds_state(pin_config_s* button_config = leds, int len = len_leds){
        digital_state(leds, len_leds);
}

void leds_init(pin_config_s* button_config = leds_dflt, int len = len_leds_dflt){
        digital_init(button_config, len);
        if(connection.mqtt == true) mqtt_subscribe_topic("led/+/+");
}

void adc_init(pin_config_s* button_config  = adcs_dflt, int len = len_adcs_dflt){
        adcs = button_config;
        len_adcs = len;

        if(connection.mqtt == true)  mqtt_subscribe_topic("adc/+/get");
}

void adc_read(pin_config_s* button_config = adcs, int len = len_adcs){
        for (int i =0; i<len; i++) button_config[i].pin_value = analogRead(button_config[i].pin_nr);
}

void pwm_state(pin_config_s* button_config = pwms, int len = len_pwms){
        for (int i =0; i<len; i++) {
                if (button_config[i].pin_state == true) {
                        ledcWrite(button_config[i].pin_id, button_config[i].pin_value);
                }
                else{
                        ledcWrite(button_config[i].pin_id, 0);
                }
        }
}

void pwm_state_byte(pin_config_s* button_config = pwms, int len = len_pwms){
        for (int i =0; i<len; i++) {
                if (button_config[i].pin_state == true) {
                        ledcWrite(button_config[i].pin_id, (uint8_t)button_config[i].pin_value);
                }
                else{
                        ledcWrite(button_config[i].pin_id, 0);
                }
        }
}

//8 bit and invertert, for example for onBoard LED
void pwm_state_byte_inv(pin_config_s* button_config = pwms, int len = len_pwms){
        for (int i =0; i<len; i++) {
                if (button_config[i].pin_state ==true) {
                        ledcWrite(button_config[i].pin_id, 255-(uint8_t)button_config[i].pin_value);
                }
                else{
                        ledcWrite(button_config[i].pin_id, 255);
                }
        }
}

void pwm_init(pin_config_s* button_config =leds_dflt, int len=len_leds_dflt, int freq = 5000, int resolution = 8){
        pwms = button_config; len_pwms = len;

        for (int i =0; i<len; i++) {
                pinMode(button_config[i].pin_id, OUTPUT);
                button_config[i].pin_state = true;
                ledcSetup(button_config[i].pin_id, freq, resolution);
                ledcAttachPin(button_config[i].pin_nr, button_config[i].pin_id);
        }

              if(connection.mqtt == true)  mqtt_subscribe_topic("pwm/+/+");
}

//---------------rgb-------------------
void rgb_init(pin_config_s* button_config =onBoard_led,  int freq = 5000, int resolution = 8){
        rgb = button_config;
        pwm_init(rgb,3,freq, resolution);

        if (button_config == onBoard_led) {
                wps_enable(false); //Auto deactivate WPS Taste if onBoard LED
                pwm_state_byte_inv (rgb,3);
        }
}


void rgb_led_set(RgbColor color, pin_config_s* pins = rgb, int pins_len = 3, bool inverted = true){
        if( taskRgbFadeHandle != NULL ) { // Terminate Fade Task if aktive
                vTaskDelete( taskRgbFadeHandle );
                taskRgbFadeHandle = NULL;
        }

        pins[0].pin_value = color.R;
        pins[1].pin_value = color.G;
        pins[2].pin_value = color.B;

        if (inverted) pwm_state_byte_inv (pins,pins_len);
        else pwm_state_byte(pins,pins_len);
}

void rgb_led_set_fade(RgbColor color, pin_config_s* pins = rgb, int pins_len = 3, bool inverted = true){
        pins[0].pin_value = color.R;
        pins[1].pin_value = color.G;
        pins[2].pin_value = color.B;

        if (inverted) pwm_state_byte_inv (pins,pins_len);
        else pwm_state_byte(pins,pins_len);
}


void rgb_led_set(HtmlColor color_html, pin_config_s* pins = rgb, int pins_len = 3,  bool inverted = true){
        RgbColor color = color_html;
        rgb_led_set(color, pins, pins_len, inverted);
}

void rgb_led_set(HsbColor color_hsv, pin_config_s* pins = rgb, int pins_len = 3,  bool inverted = true){
        RgbColor color = color_hsv;
        rgb_led_set(color, pins, pins_len, inverted);
}

RgbColor rgb_led_get(pin_config_s* pins = rgb){
        return RgbColor((uint8_t) pins[0].pin_value, (uint8_t) pins[1].pin_value, (uint8_t) pins[2].pin_value);
}

HtmlColor rgb_led_get_html(pin_config_s* pins = rgb){
        return HtmlColor(rgb_led_get(pins));
}

HsbColor rgb_led_get_hsv(pin_config_s* pins = rgb){
        return HsbColor(rgb_led_get(pins));
}


void rgb_led_fade_task(void * parameter) {
        fade_parameter_s* fade;
        fade= (fade_parameter_s*) parameter;
        while(1) {
                for (float i = 0; i<1; i+=0.01) { //100 Steps
                        RgbColor color = RgbColor::LinearBlend(fade->fromColor, fade->toColor, i);
                        rgb_led_set_fade(color);
                        delay(10*fade->duration); //min 1Sek
                }
                taskRgbFadeHandle = NULL;
                vTaskDelete(NULL);
        }
}

void rgb_led_fade(RgbColor toColor, int duration=1){
        if( taskRgbFadeHandle != NULL ) { // Terminate Fade Task if aktive
                vTaskDelete( taskRgbFadeHandle );
                taskRgbFadeHandle = NULL;
        }

        RgbColor fromColor = rgb_led_get();
        fade_parameter_s* task_parameter = new fade_parameter_s{fromColor,toColor,duration};
        xTaskCreate(rgb_led_fade_task,"Fade",10000,(void*)task_parameter,1,&taskRgbFadeHandle);
}


//--------------------------------------------------------------------------------

void rf_init(pin_config_s* rf_config = rf_dflt){
        rf = rf_config;
}

int get_humidity(pin_config_s* rf_config = rf) { //get Humidity, source: http://www.elektronik-labor.de/AVR/Rmessung.html#rx2
        long U, R;
        double F;
        uint8_t pin_hum = rf_config[0].pin_nr;
        uint8_t pin_gnd = rf_config[1].pin_nr;


//Geändertes Messverfahren,
        pinMode(pin_gnd, INPUT_PULLUP); //  Ddrb.3 = 0      'Eingang
        //  pinMode(pin_hum, INPUT_PULLUP); //  Ddrb.3 = 0      'Eingang
        delay(2);
        U = analogRead(pin_hum); //  U = Getadc(3)
        digitalWrite(pin_gnd, LOW); //  Portb.3 = 0     'Pullup Aus
        pinMode(pin_gnd, OUTPUT);   //  Ddrb.3 = 1 'Nierohmig


        //  U = U / 2;
        R = 37000 * U;           //'R=35k*(U/4095-U)
        U = 4095 - U;
        R = (U > 0) ? R / U : 999999; //alternativ writing to: if(U>0) R = R/U; else R=999999;
        //  R = R - 450;                //- interner Bahnwiderstand 0,45 k

        //Calculate Humidity
        F = R / 280;
        F = log(F);                                          //ln f !!!
        F = F * 8.9;
        F = 100 - F;
        F = round(F);

        Serial.println(String(R) + " Ohm     " + String(F) + "%");

        return F;
}


double get_temp(int RawADC) {
        //Source: http://playground.arduino.cc/ComponentLib/Thermistor2
        double Temp;
        Temp = log(10000.0 * ((4095.0 / RawADC )));
        //         =log(10000.0/(1024.0/RawADC-1)) // for pull-up configuration
        Temp = 1 / (0.001129148 + (0.000234125 + (0.0000000876741 * Temp * Temp )) * Temp );
        Temp =  273.15-Temp;      // Convert Kelvin to Celcius
        // Temp = (Temp * 9.0)/ 4.7 + 32.0; // Convert Celcius to Fahrenheit

        Temp = round(Temp*10);
        Temp = Temp/ 10;
        return Temp;
}


//--------------------------------------------------------------------------------
//Intern Sensors NodeESP

int get_intern_hall(){
  return hallRead();
}

float get_intern_temp(){
    uint8_t temp_farenheit = temprature_sens_read();
    return ((temp_farenheit - 32) / 1.8);
}



//--------------------------------------------------------------------------------

IRrecv* ir_recv_init(uint8_t recv_pin = ir_recv_pins[0].pin_nr){
        irrecv = new IRrecv (recv_pin);
        irrecv->enableIRIn();
        return irrecv;
}


IRrecv* ir_recv_init(uint8_t recv_pin, uint8_t blink_pin){
        irrecv = new IRrecv (recv_pin, blink_pin);
        irrecv->enableIRIn();
        return irrecv;
}

IRsend* ir_send_init(uint8_t send_pin = ir_send_pin.pin_nr){
        irsend = new IRsend(send_pin);
        Serial.println(send_pin);
        return irsend;
}

void ir_send(int codeType, unsigned long codeValue, int codeLen){
        Serial.println((int)irsend);
        if (irsend == NULL) return;
        //    portDISABLE_INTERRUPTS();  //Interrupt disable because timing issues und esp32
        String cType ="BEFORE";
        //    Serial.printf("Send  %s, %#x\n", cType.c_str(), codeValue);

/*
   codeType = 3;
   codeValue 16758855;
   codeLen = 32;
 */

        if (codeType == RC5 || codeType == RC6) {
                ir_toggle = 1 - ir_toggle;
                // Put the toggle bit into the code to send
                codeValue = codeValue & ~(1 << (codeLen - 1));
                codeValue = codeValue | (ir_toggle << (codeLen - 1));
        }

        switch (codeType) {
        case NEC:      irsend->sendNEC(codeValue, codeLen); cType = "NEC"; break;
        case SONY:     irsend->sendSony(codeValue, codeLen); cType = "SON"; break;
        case PANASONIC:     irsend->sendPanasonic(codeValue, codeLen); cType = "PAN"; break;
        case JVC:      irsend->sendJVC(codeValue, codeLen, false); cType = "JVC"; break;
        case RC5:      irsend->sendRC5(codeValue, codeLen); cType = "RC5"; break;
        case RC6:      irsend->sendRC6(codeValue, codeLen); cType = "RC6"; break;
        default:       cType = "UNKNOWN";
        }

        //      portENABLE_INTERRUPTS();
        //      Serial.printf("Send  %s, %#x\n\n", cType.c_str(), codeValue);
}



bool ir_result_debug(decode_results *results){
        String cType;
        switch (results->decode_type) {
        case NEC:  if (results->value != REPEAT) cType = "NEC"; break;
        case SONY:  cType = "SON"; break;
        case PANASONIC:  cType = "PAN"; break;
        case JVC:   cType = "JVC"; break;
        case RC5:   cType = "RC5"; break;
        case RC6:   cType = "RC6"; break;
        default:    cType = "UNKNOWN";
        }
        Serial.print("Recieved "+cType+" :");  Serial.println(results->value, HEX);

}

void ir_result_toJson(JsonObject& root, decode_results *results){
        ir_result_debug(results);
        root["type"] = (int) results->decode_type;
        root["value"] = results->value;
        root["length"] = results->bits;
}


void ir_recv_test(){
        decode_results results;
        while(1) {

                if (irrecv->decode(&results)) {
                        String cStr;
                        ir_result_debug(&results);
                        irrecv->resume(); // resume receiver
                }
        }
}

//-----------------------NEO PIXEL-------------------------

void neoPixel_init(int type =neopixel_set.type, uint16_t countPixels = neopixel_set.number_pixel, uint8_t pin = neopixel_set.pin){
        strip = new NeoPixelWrapper();
        len_pixel= countPixels;
        //      neopixel_set.number_pixel = countPixels;
        //      NeoPixelType
        strip->Begin((NeoPixelType)type, countPixels, pin);
        strip->SetBrightness(neopixel_set.brightness);
        strip->Show();
}

void pixel_set(RgbColor color){
        if( taskPixelFadeHandle != NULL ) { // Terminate Fade Task if aktive
                vTaskDelete( taskPixelFadeHandle );
                taskPixelFadeHandle = NULL;
        }

        for (int i = 0; i< len_pixel; i++) {
                strip->SetPixelColor(i,color);
        }
        strip->Show();
        curColor = color;
}

void pixel_set(HtmlColor color){
        if( taskPixelFadeHandle != NULL ) { // Terminate Fade Task if aktive
                vTaskDelete( taskPixelFadeHandle );
                taskPixelFadeHandle = NULL;
        }

        for (int i = 0; i< len_pixel; i++) {
                strip->SetPixelColor(i,color);
        }
        strip->Show();
        curColor = RgbColor(color);
}

void pixel_set(HsbColor color){
        if( taskPixelFadeHandle != NULL ) { // Terminate Fade Task if aktive
                vTaskDelete( taskPixelFadeHandle );
                taskPixelFadeHandle = NULL;
        }

        curColor = RgbColor(color);
        for (int i = 0; i< len_pixel; i++) {
                strip->SetPixelColor(i,curColor);
        }
        strip->Show();

}

RgbColor get_pixel_rgb(){
        return curColor;
}

HtmlColor get_pixel_hex(){
        return HtmlColor(curColor);
}

HsbColor get_pixel_hsv(){
        return HsbColor(curColor);
}


void pixel_fade(){
}

void pixel_fade_array(){
}


void neoPixel_array_toJson(JsonObject& root){
        JsonArray& rgb_array = root.createNestedArray("rgb_array");

        for(int i = 0; i< neopixel_set.number_pixel; i++) {
                RgbColor color_rgb =  strip->GetPixelColor(i);
                HtmlColor color_html = HtmlColor(color_rgb);

                char string[10] = {0};
                color_html.ToNumericalString(string, 20);
                //Serial.println(string);

                rgb_array.add(string);
        }

//        root.prettyPrintTo(Serial);
}

void neoPixel_array_fromJson(JsonObject& root){

        for(int i = 0; i< neopixel_set.number_pixel; i++) {
                HtmlColor color = HtmlColor(0);
                const char* color_value   = root["rgb_array"][i];
                if (color_value) color.Parse<HtmlColorNames>(color_value);
                strip->SetPixelColor(i, color);
        }
}


void neoPixel_set_toJson(JsonObject& root){
        root["number"] = neopixel_set.number_pixel;
        root["max_brightness"] = neopixel_set.max_brightness;
        root["brightness"] = neopixel_set.brightness;
        root["pin"] = neopixel_set.pin;
        root["type"] = neopixel_set.type;
}

void neoPixel_set_fromJson(JsonObject& root){
        if (root.containsKey("number")) neopixel_set.number_pixel =   root["number"];
        if (root.containsKey("max_brightness")) neopixel_set.max_brightness = root["max_brightness"];
        if (root.containsKey("brightness")) neopixel_set.brightness =   root["brightness"];
        if (root.containsKey("pin")) neopixel_set.pin = root["pin"];
        if (root.containsKey("type")) neopixel_set.type =   root["type"];
}



bool neoPixel_load(){
        if (!SPIFFS.exists(cfg_neoPixel_path)) {
                //First Run. No Config so load default
                Serial.println(F("No config file found. Loading default"));
                neoPixel_init();
                strip->Show();

        }
        else{
                File file = SPIFFS.open(cfg_neoPixel_path); // Open file for reading
                DynamicJsonBuffer jsonBuffer_neo;
                JsonObject &root = jsonBuffer_neo.parseObject(file); // Parse the root object


                if (!root.success()) {
                        Serial.println(F("Failed to read JSON Configuration!"));
                        file.close();
                        neoPixel_init();
                        strip->Show();
                        return false;
                }

                neoPixel_set_fromJson(root);
                neoPixel_init();
                neoPixel_array_fromJson(root);
                strip->Show();

                file.close();   // Close the file (File's destructor doesn't close the file)
        }
}

bool neoPixel_save(){
        if (SPIFFS.exists(cfg_neoPixel_path)) SPIFFS.remove(cfg_neoPixel_path); // Delete existing file, otherwise the configuration is appended to the file

        // Open file for writing
        File file = SPIFFS.open(cfg_neoPixel_path, FILE_WRITE);
        if (!file) {
                Serial.println(F("Failed to create file"));
                return false;
        }

        else{
                DynamicJsonBuffer jsonBuffer_neo;   // Allocate the memory pool on the stack.
                JsonObject &root = jsonBuffer_neo.createObject(); // Parse the root object

                neoPixel_set_toJson(root);
                neoPixel_array_toJson(root);

                if (root.printTo(file) == 0) {         // Serialize JSON to file
                        Serial.println(F("Failed to write to file"));
                }
                file.close();   // Close the file (File's destructor doesn't close the file)
        }
}

//--------------------------------------------------------------

void alarm_init(pin_config_s* button_config = &speaker_pin){
        alarm_pin = button_config;
        ledcSetup(alarm_pin->pin_id, 5000, 8);
        ledcAttachPin(alarm_pin->pin_nr, alarm_pin->pin_id);

}

void alarm_set(bool alarm){
        if (alarm) {
                if (taskSireneHandle == NULL) {
                        xTaskCreate([](void* pm){
                                while(true) {
                                        ledcWriteNote(alarm_pin->pin_id, NOTE_C, 6);
                                        delay(500);
                                        ledcWriteNote(alarm_pin->pin_id, NOTE_D, 6);
                                        delay(500);
                                }
                        },"TaskSirene",5000,NULL,2,&taskSireneHandle);
                }
        }else{
                if (taskSireneHandle != NULL) {
                        vTaskDelete(taskSireneHandle);
                        taskSireneHandle = NULL;
                        ledcWrite(alarm_pin->pin_id,0);
                }
        }
}

void speaker_init(){
        DacAudio = new XT_DAC_Audio_Class(25,1);    // Create the main player class object. Use GPIO 25, one of the 2 DAC pins and timer 1
}



void wav_task (void* parameter){
        unsigned char* song;
        song= (unsigned char*) parameter;

        XT_Wav_Class Music(song); // The music score object, pass in the Music data
        DacAudio->Play(&Music);
        while(Music.Playing==true) {
                DacAudio->FillBuffer();
        }


        Serial.println("Song Finished");
        delete &Music;
        free(song);
        taskSongHandle = NULL;
        vTaskDelete(NULL);
}

void audio_task (void* parameter){
        int8_t* song;
        song= (int8_t*) parameter;

        XT_MusicScore_Class Music(song,TEMPO_ALLEGRO,INSTRUMENT_PIANO,1); // The music score object, pass in the Music data
        DacAudio->Play(&Music);
        while(Music.Playing) DacAudio->FillBuffer();

        free(song);
        taskSongHandle = NULL;
        Serial.println("Song Finished");
        vTaskDelete(NULL);
}


void speaker_play_twinkle(){
        int8_t PROGMEM TwinkleTwinkle[] = {
                NOTE_C5,NOTE_C5,NOTE_G5,NOTE_G5,NOTE_A5,NOTE_A5,NOTE_G5,BEAT_2,
                NOTE_F5,NOTE_F5,NOTE_E5,NOTE_E5,NOTE_D5,NOTE_D5,NOTE_C5,BEAT_2,
                NOTE_G5,NOTE_G5,NOTE_F5,NOTE_F5,NOTE_E5,NOTE_E5,NOTE_D5,BEAT_2,
                NOTE_G5,NOTE_G5,NOTE_F5,NOTE_F5,NOTE_E5,NOTE_E5,NOTE_D5,BEAT_2,
                NOTE_C5,NOTE_C5,NOTE_G5,NOTE_G5,NOTE_A5,NOTE_A5,NOTE_G5,BEAT_2,
                NOTE_F5,NOTE_F5,NOTE_E5,NOTE_E5,NOTE_D5,NOTE_D5,NOTE_C5,BEAT_4,
                NOTE_SILENCE,BEAT_5,SCORE_END
        };

        int len = 51;

        DynamicJsonBuffer test;
        JsonObject& answ = test.createObject();
        answ["cmd"]  = "play_speaker_notes";
        answ["length"] = len;
        JsonArray& array = answ.createNestedArray("song");
        array.copyFrom(TwinkleTwinkle);
        answ.prettyPrintTo(Serial);


        Serial.println(TwinkleTwinkle[10]);
        //    return;


        //XT_Wav_Class ForceWithYou(Force);           // create WAV object and pass in the WAV data

        XT_MusicScore_Class Music(TwinkleTwinkle,TEMPO_ALLEGRO,INSTRUMENT_PIANO,1); // The music score object, pass in the Music data

        XT_MultiPlay_Class Sequence;

//  Sequence.AddPlayItem(&ForceWithYou);      // Add the first sound item, this will play first
        Sequence.AddPlayItem(&Music); // Add the music score, this will play after the first item

        DacAudio->Play(&Sequence);
        while(Sequence.Playing) {

                DacAudio->FillBuffer(); // This needs only be in your main loop once, suggest here at the top.

        }


}
/*
   void IRAM_ATTR speaker_timer_itr(){
        portENTER_CRITICAL_ISR(&timerMuxS);
        // play data:
        if (sound_play) {
                dac_output_voltage(DAC_CHANNEL_1, sound_buffer[sound_read_pointer]);
                sound_read_pointer++;
                if (sound_read_pointer == sound_read_len) {
                        sound_read_pointer = 0;
                        sound_play = false;
                }
        }
        portEXIT_CRITICAL_ISR(&timerMuxS);
   }


   void speaker_initx(){



        dac_output_enable(DAC_CHANNEL_1);
        // Use 1st timer of 4 (counted from zero).
   // Set 80 divider for prescaler (see ESP32 Technical Reference Manual for more
   // info).
        speaker_timer = timerBegin(0, 80, true); // use a prescaler of 2
        timerAttachInterrupt(speaker_timer, &speaker_timer_itr, true); // Attach onTimer function to our timer.
        // Set alarm to call onTimer function every second (value in microseconds).
   // Repeat the alarm (third parameter)
        timerAlarmWrite(speaker_timer, 125, true);
        dacWrite(DAC_CHANNEL_1,0x7f);
        timerAlarmEnable(speaker_timer);


        /*
              dac_output_enable(DAC_CHANNEL_1);
              speaker_timer = timerBegin(0, 2, true); // use a prescaler of 2
              timerAttachInterrupt(speaker_timer, &speaker_timer_itr, true);
              timerAlarmWrite(speaker_timer, 5000, true);
 */
/*
   }

   void speaker_play_stream(Stream* stream){
   //  https://www.hackster.io/julianfschroeter/stream-your-audio-on-the-esp32-2e4661

        while(stream->available()) {
                if(!sound_play) {
                        sound_read_len = (stream->available()>SOUND_BUFFER_LEN) ? SOUND_BUFFER_LEN : stream->available();
                        stream->readBytes(sound_buffer,SOUND_BUFFER_LEN);
                        sound_play = true;
                }
        }

   }
 */
#endif
