
#ifndef COMMMANDS_HPP
#define COMMMANDS_HPP


#define HANDLER_PARAMETER JsonObject& msg, JsonObject&  answ, bool& hasAnsw

typedef struct {
        String cmd;
        bool (*func)(HANDLER_PARAMETER);
} cmd_handler_s;


//---------------------------------Color Functions---------------------------------
//RGB Led
bool set_rgb_hex(HANDLER_PARAMETER);
bool get_rgb_hex(HANDLER_PARAMETER);
bool set_rgb_rgb(HANDLER_PARAMETER);
bool get_rgb_rgb(HANDLER_PARAMETER);
bool set_rgb_hsv(HANDLER_PARAMETER);
bool get_rgb_hsv(HANDLER_PARAMETER);
bool set_rgb_fade(HANDLER_PARAMETER);
bool get_rgb_config(HANDLER_PARAMETER);

//neoPixel
bool set_pixel_hex(HANDLER_PARAMETER);
bool get_pixel_hex(HANDLER_PARAMETER);
bool set_pixel_rgb(HANDLER_PARAMETER);
bool get_pixel_rgb(HANDLER_PARAMETER);
bool set_pixel_hsv(HANDLER_PARAMETER);
bool get_pixel_hsv(HANDLER_PARAMETER);
bool set_pixel_fade(HANDLER_PARAMETER);

bool set_pixel_array(HANDLER_PARAMETER);
bool set_array_fade(HANDLER_PARAMETER);
bool get_pixel_set(HANDLER_PARAMETER);
bool set_pixel_set(HANDLER_PARAMETER);
bool set_pixel_bright(HANDLER_PARAMETER);

//IR
bool ir_recv(HANDLER_PARAMETER);
bool ir_send(HANDLER_PARAMETER);

//------------------------------General---------------------
bool test(HANDLER_PARAMETER);
bool testtopic(HANDLER_PARAMETER);

bool get_intern_temp(HANDLER_PARAMETER);
bool get_intern_hall(HANDLER_PARAMETER);

bool light_on(HANDLER_PARAMETER);
bool light_off(HANDLER_PARAMETER);
bool light_pwm(HANDLER_PARAMETER);

bool get_button_state(HANDLER_PARAMETER);
bool get_button_config(HANDLER_PARAMETER);
bool get_button_change(HANDLER_PARAMETER);

bool get_touch_state(HANDLER_PARAMETER);
bool get_touch_config(HANDLER_PARAMETER);
bool get_touch_change(HANDLER_PARAMETER);

bool get_led_state(HANDLER_PARAMETER);
bool get_led_config(HANDLER_PARAMETER);
bool set_led_state(HANDLER_PARAMETER);

bool get_pwm_state(HANDLER_PARAMETER);
bool get_pwm_config(HANDLER_PARAMETER);
bool set_pwm_state(HANDLER_PARAMETER);

bool get_adc_state(HANDLER_PARAMETER);
bool get_adc_config(HANDLER_PARAMETER);
bool get_hum_state(HANDLER_PARAMETER);

bool play_speaker_notes(HANDLER_PARAMETER);
bool play_speaker_wav(HANDLER_PARAMETER);

bool set_alarm(HANDLER_PARAMETER);


//--------------------------------Settings----------------------------------
bool get_wifi_status(JsonObject& msg, JsonObject& answ, bool& hasAnsw);
bool get_wifi_scan(JsonObject& msg, JsonObject& answ, bool& hasAnsw);

bool get_gen_set(JsonObject& msg, JsonObject& answ, bool& hasAnsw);
bool get_udp_set(JsonObject& msg, JsonObject& answ, bool& hasAnsw);
bool get_mqtt_set(JsonObject& msg, JsonObject& answ, bool& hasAnsw);
bool get_app_set(JsonObject& msg, JsonObject& answ, bool& hasAnsw);

bool set_app_set(JsonObject& msg, JsonObject& answ, bool& hasAnsw);
bool set_gen_set(JsonObject& msg, JsonObject& answ, bool& hasAnsw);
bool set_udp_set(JsonObject& msg, JsonObject& answ, bool& hasAnsw);
bool set_mqtt_set(JsonObject& msg, JsonObject& answ, bool& hasAnsw);
bool set_wifi(JsonObject& msg, JsonObject& answ, bool& hasAnsw);
bool wifi_wps(JsonObject& msg, JsonObject& answ, bool& hasAnsw);

bool get_mqtt_check(JsonObject& msg, JsonObject& answ, bool& hasAnsw);

bool con_restore(JsonObject& msg, JsonObject& answ, bool& hasAnsw);
bool do_restore(JsonObject& msg, JsonObject& answ, bool& hasAnsw);
bool con_firmware_update(JsonObject& msg, JsonObject& answ, bool& hasAnsw);
bool do_firmware_update(JsonObject& msg, JsonObject& answ, bool& hasAnsw);
bool ping(JsonObject& msg, JsonObject& answ, bool& hasAnsw);

//in App-hpp
bool list_apps(JsonObject& msg, JsonObject& answ, bool& hasAnsw);

//---------------------------------Settings---------------------------------
cmd_handler_s cmd_handler[]= {
        {"ping",ping},

        {"get_wifi_status",get_wifi_status},
        {"get_mqtt_set", get_mqtt_set},
        {"get_gen_set",get_gen_set},
        {"get_udp_set",get_udp_set},
        {"get_wifi_scan",get_wifi_scan},
        {"get_app_set",get_app_set},

        {"set_app_set", set_app_set},
        {"set_mqtt_set", set_mqtt_set},
        {"set_gen_set",set_gen_set},
        {"set_udp_set",set_udp_set},

        {"get_mqtt_check",get_mqtt_check},
        {"set_wifi",set_wifi},
        {"wifi_wps",wifi_wps},

        {"con_restore",con_restore},
        {"do_restore", do_restore},
        {"con_firmware_update",con_firmware_update},
        {"do_firmware_update", do_firmware_update},
        {"",NULL} //marks end of handlers
};


//---------------------------------Peripherie---------------------------------


cmd_handler_s cmd_handler_rgb[]= {
        {"set_rgb_hex",set_rgb_hex},
        {"get_rgb_hex",get_rgb_hex},
        {"set_rgb_rgb",set_rgb_rgb},
        {"get_rgb_rgb",get_rgb_rgb},
        {"set_rgb_hsv",set_rgb_hsv},
        {"get_rgb_hsv",get_rgb_hsv},


        {"",NULL} //marks end of handlers
};

cmd_handler_s cmd_handler_rgbw[]= {
        {"set_rgb_hex",set_rgb_hex},
        {"get_rgb_hex",get_rgb_hex},
        /*    {"set_rgb_rgb",set_rgb_rgb},
            {"get_rgb_rgb",get_rgb_rgb},
            {"set_rgb_hsv",set_rgb_hsv},
            {"get_rgb_hsv",get_rgb_hsv},*/
        {"",NULL} //marks end of handlers
};


cmd_handler_s cmd_handler_pixel[]= {
        /*      {"set_pixel_array",set_pixel_array},
              {"get_pixel_set",get_pixel_set},
              {"set_pixel_set",set_pixel_set},
              {"set_pixel_bright",set_pixel_bright},*/
        {"",NULL} //marks end of handlers
};

cmd_handler_s cmd_handler_onBoardLED[]= {
        {"set_rgb_hex",set_rgb_hex},
        {"get_rgb_hex",get_rgb_hex},
        {"",NULL} //marks end of handlers
};

cmd_handler_s cmd_handler_TvLight[]= {
        {"set_pixel_hex",set_pixel_hex},
        {"get_pixel_hex",get_pixel_hex},
        {"set_pixel_rgb",set_pixel_rgb},
        {"get_pixel_rgb",get_pixel_rgb},
        {"set_pixel_hsv",set_pixel_hsv},
        {"get_pixel_hsv",get_pixel_hsv},
        {"set_pixel_fade",set_pixel_fade},

        {"set_pixel_array",set_pixel_array},
        {"get_pixel_set",get_pixel_set},
        {"set_pixel_set",set_pixel_set},
        {"set_pixel_bright",set_pixel_bright},

        {"",NULL} //marks end of handlers
};


cmd_handler_s cmd_handler_gen[]= {
        {"test",test},
        {"testtopic",testtopic},

        {"light_on",light_on},
        {"light_off",light_off},
        {"light_pwm",light_pwm},

        {"get_intern_temp", get_intern_temp},
        {"get_intern_hall", get_intern_hall},

        {"get_button_state",get_button_state},
        {"get_button_config",get_button_config},
        {"get_button_change",get_button_change},

        {"get_touch_state",get_touch_state},
        {"get_touch_config",get_touch_config},
        {"get_touch_change",get_touch_change},

        {"get_led_state",get_led_state},
        {"get_led_config",get_led_config},
        {"set_led_state",set_led_state},

        {"get_pwm_state",get_pwm_state},
        {"get_pwm_config",get_pwm_config},
        {"set_pwm_state",set_pwm_state},

        {"get_adc_state",get_adc_state},
        {"get_adc_config",get_adc_config},

        {"get_hum_state",get_hum_state},

        {"set_rgb_hex",set_rgb_hex},
        {"get_rgb_hex",get_rgb_hex},
        {"set_rgb_rgb",set_rgb_rgb},
        {"get_rgb_rgb",get_rgb_rgb},
        {"set_rgb_hsv",set_rgb_hsv},
        {"get_rgb_hsv",get_rgb_hsv},
        {"set_rgb_fade",set_rgb_fade},
        {"get_rgb_config",get_rgb_config},

        {"set_pixel_hex",set_pixel_hex},
        {"get_pixel_hex",get_pixel_hex},
        {"set_pixel_rgb",set_pixel_rgb},
        {"get_pixel_rgb",get_pixel_rgb},
        {"set_pixel_hsv",set_pixel_hsv},
        {"get_pixel_hsv",get_pixel_hsv},
        {"set_pixel_fade",set_pixel_fade},
        {"set_pixel_bright",set_pixel_bright},
        {"get_pixel_set",get_pixel_set},
        {"set_pixel_set",set_pixel_set},
        {"set_pixel_array",set_pixel_array},
        {"set_array_fade",set_pixel_fade},

        {"ir_send",ir_send},

        {"play_speaker_notes",play_speaker_notes},
        {"play_speaker_wav",play_speaker_wav},

        {"set_alarm",set_alarm},

        {"",NULL} //marks end of handlers
};

cmd_handler_s cmd_handler_ir[]= {
        {"ir_send",ir_send},
        {"",NULL} //marks end of handlers
};

cmd_handler_s cmd_handler_ir_task[]= {
        {"ir_recv",ir_recv},
        {"",NULL} //marks end of handlers
};


cmd_handler_s cmd_handler_adc_task[]= {
        {"get_adc_state",get_adc_state},
        {"",NULL} //marks end of handlers
};

cmd_handler_s cmd_handler_clima[]= {
        {"get_adc_state",get_adc_state},
        {"get_hum_state",get_hum_state},
        {"",NULL} //marks end of handlers
};


cmd_handler_s cmd_handler_tests[]= {
        {"test",test},
        {"testtopic",testtopic},
        {"",NULL} //marks end of handlers
};


cmd_handler_s cmd_handler_short[]= {
        {"",NULL},
        {"",NULL},
        {"",NULL},
        {"",NULL} //marks end of handlers
};

cmd_handler_s cmd_handler_tasks[]= {
        {"",NULL},
        {"",NULL},
        {"",NULL},
        {"",NULL} //marks end of handlers
};


/*
   cmd_handler_s cmd_handler_rgb[]= {
        {"set_rgb_hex",set_rgb_hex},
        {"get_rgb_hex", get_rgb_hex},

        {"",NULL} //marks end of handlers
   };

   cmd_handler_s cmd_handler_neo[]= {
        {"get_neo_set",get_neo_set},
        {"set_neo_bright", set_neo_bright},
        {"set_neo_array",set_neo_array},

        {"",NULL} //marks end of handlers
   };

 */
cmd_handler_s* dflt_cmd_list[] =  {cmd_handler,cmd_handler_gen, NULL};
cmd_handler_s* dflt_cmd_mqtt[] =  {cmd_handler_gen,NULL,};

struct  {
        cmd_handler_s** udp_list = dflt_cmd_list;
        cmd_handler_s** ws_list = dflt_cmd_list;
        cmd_handler_s** mqtt_list = dflt_cmd_mqtt;
} cmd_handler_list;


#endif
