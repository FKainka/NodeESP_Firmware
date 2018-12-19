#ifndef CMDGEN_HPP
#define CMDGEN_HPP

#include "Peripherals.hpp"
#include "MQTT.hpp"
#include "Udp.hpp"
#include "Websockets.hpp"

bool test(JsonObject& msg, JsonObject& answ, bool& hasAnsw){
        answ["cmd"] ="test reply";
        hasAnsw = true;
        return true;
}

bool testtopic(JsonObject& msg, JsonObject& answ, bool& hasAnsw){
        answ["cmd"] ="testtopic";
        answ["value"] = "Hello from your NodeESP";
        hasAnsw = true;
        return true;
}

bool light_on(JsonObject& msg, JsonObject& answ, bool& hasAnsw){
        if (len_leds <= 0 || leds == NULL) return false; //No LEDs Definded
        leds[0].pin_state = true;
        digital_state();
        hasAnsw = false;
        return true;
}

bool light_off(JsonObject& msg, JsonObject& answ, bool& hasAnsw){
        if (len_leds <= 0 || leds == NULL) return false; //No LEDs Definded
        leds[0].pin_state = false;
        digital_state();
        hasAnsw = false;
        return true;
}

bool light_pwm(JsonObject& msg, JsonObject& answ, bool& hasAnsw){
        set_single_pin_state_json(leds, msg);
        pwm_state(leds, 1);
        hasAnsw = false;
        return true;
}



bool get_button_config(HANDLER_PARAMETER){
        if (len_buttons <= 0 || buttons == NULL) return false; //No Buttons Definded
        answ["cmd"] = msg["cmd"];
        answ["len"] = len_buttons;

        JsonArray& pin_array_json = answ.createNestedArray("pin_names");
        for (int i =0; i<len_buttons; i++) {
                //  JsonObject& pin_json = pin_array_json.createNestedObject();
                pin_array_json.add(buttons[i].pin_name.c_str());
        }
        get_pin_state_json(buttons,len_buttons,answ);
        hasAnsw = true;
        return true;
}


bool get_button_state(HANDLER_PARAMETER){
        if (len_buttons <= 0 || buttons == NULL) return false; //No Buttons Definded
        if (!button_change) buttons_state();

        answ["cmd"] = "get_button_state";
        hasAnsw = true;
        if (len_buttons > 1) {
                get_pin_state_json(buttons,len_buttons,answ);
                button_change = false;
        }
        else{
                answ[buttons[0].pin_name]= buttons[0].pin_state;
                button_change = false;
        }

        return true;
}

bool get_button_change(HANDLER_PARAMETER){
        buttons_state();
        if (button_change) {
                get_button_state(msg, answ, hasAnsw);
                button_change = false;
        }
        return true;
}

bool get_touch_config(HANDLER_PARAMETER){
        if (len_touchs <= 0 || touchs == NULL) return false; //No Buttons Definded
        answ["cmd"] = "get_touch_config";
        answ["len"] = len_touchs;

        JsonArray& pin_array_json = answ.createNestedArray("pin_names");
        for (int i =0; i<len_touchs; i++) {
                //  JsonObject& pin_json = pin_array_json.createNestedObject();
                pin_array_json.add(touchs[i].pin_name.c_str());
        }
        get_pin_state_json(touchs,len_touchs,answ);
        hasAnsw = true;
        return true;
}


bool get_touch_state(HANDLER_PARAMETER){
        if (len_touchs <= 0 || touchs == NULL) return false; //No touchs Definded
        if (!touch_change) touch_state();
        answ["cmd"] = "get_touch_state";
        hasAnsw = true;
        if (len_touchs > 1) {
                get_pin_state_json(touchs,len_touchs,answ);
                touch_change = false;
        }
        else{
                answ[touchs[0].pin_name]= touchs[0].pin_state;
                touch_change = false;
        }
        return true;
}

bool get_touch_change(HANDLER_PARAMETER){
        touch_state();
        if (touch_change) {
                get_touch_state(msg, answ, hasAnsw);
                touch_change = false;
        }
        return true;
}


bool get_led_config(HANDLER_PARAMETER){
        if (len_leds <= 0 || leds == NULL) return false; //No leds Definded
        answ["cmd"] = msg["cmd"];
        answ["len"] = len_leds;

        JsonArray& pin_array_json = answ.createNestedArray("pin_names");
        for (int i =0; i<len_leds; i++) {
                //  JsonObject& pin_json = pin_array_json.createNestedObject();
                pin_array_json.add(leds[i].pin_name.c_str());
        }
        get_pin_state_json(leds,len_leds,answ);
        hasAnsw = true;
        return true;
}


bool get_led_state(HANDLER_PARAMETER){
        if (len_leds <= 0 || leds == NULL) return false; //No Buttons Definded
        answ["cmd"] = "get_led_state";
        hasAnsw = true;
        if (len_leds > 1) {
                get_pin_state_json(leds,len_leds,answ);
        }
        else{
                answ[leds[0].pin_name]= leds[0].pin_state;
        }
        return true;
}

bool set_led_state(HANDLER_PARAMETER){
        if (len_leds <= 0 || leds == NULL) return false; //No Buttons Definded
        set_pin_state_json(leds, len_leds, msg);
        leds_state();
        get_led_state(msg,answ, hasAnsw);
        return true;
}

bool get_pwm_config(HANDLER_PARAMETER){
        if (len_pwms <= 0 || pwms == NULL) return false; //No pwms Definded
        answ["cmd"] = msg["cmd"];
        answ["len"] = len_pwms;

        JsonArray& pin_array_json = answ.createNestedArray("pin_names");
        for (int i =0; i<len_pwms; i++) {
                //  JsonObject& pin_json = pin_array_json.createNestedObject();
                pin_array_json.add(pwms[i].pin_name.c_str());
        }
        get_pin_state_json(pwms,len_pwms,answ);
        hasAnsw = true;
        return true;
}


bool get_pwm_state(HANDLER_PARAMETER){
        if (len_pwms <= 0 || pwms == NULL) return false; //No Buttons Definded
        answ["cmd"] = "get_pwm_state";
        hasAnsw = true;
        if (len_pwms > 1) {
                get_pin_state_json(pwms,len_pwms,answ);
        }
        else{
                answ[pwms[0].pin_name]= pwms[0].pin_value;
        }
        return true;
}

bool set_pwm_state(HANDLER_PARAMETER){
        if (len_pwms <= 0 || pwms == NULL) return false; //No Buttons Definded
        set_pin_state_json(pwms, len_pwms, msg);
        pwm_state();
        answ["cmd"] = "get_pwm_state";
        hasAnsw = true;
        get_pin_state_json(pwms,len_pwms,answ);
        //get_pwm_state(msg, answ, hasAnsw);
        return true;
}

bool get_adc_config(HANDLER_PARAMETER){
        if (len_adcs <= 0 || adcs == NULL) return false; //No adcs Definded
        answ["cmd"] = "get_adc_config";
        answ["len"] = len_adcs;

        JsonArray& pin_array_json = answ.createNestedArray("pin_names");
        for (int i =0; i<len_adcs; i++) {
                //  JsonObject& pin_json = pin_array_json.createNestedObject();
                pin_array_json.add(adcs[i].pin_name.c_str());
        }
        get_pin_state_json(adcs,len_adcs,answ);
        hasAnsw = true;
        return true;
}


bool get_adc_state(HANDLER_PARAMETER){
        if (len_adcs <= 0 || adcs == NULL) return false; //No Buttons Definded
        answ["cmd"] = "get_adc_state";
        hasAnsw = true;
        adc_read();

        for(int i = 0; i< len_adcs; i++) {
                answ[adcs[i].pin_name]= adcs[i].pin_value;
        }

        /*
           if (len_adcs > 1) {
                get_pin_state_json(adcs,len_adcs,answ);
           }
           else{

           }*/
        return true;
}

bool get_hum_state(HANDLER_PARAMETER){
        if (rf == NULL) return false; //No RF Definded
        answ["cmd"] = "get_hum_state";
        hasAnsw = true;
        int hum = get_humidity();
        answ[rf[0].pin_name]= hum;

        return true;
}



#endif
