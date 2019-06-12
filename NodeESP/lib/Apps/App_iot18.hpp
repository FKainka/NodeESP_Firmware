/**
 * Apps of the IOT Adventcalendar 2018
 */
#ifndef APPIOT18_HPP
#define APPIOT18_HPP

#include <WiFiUdp.h>
#include <ArduinoJson.h> //https://arduinojson.org/assistant/?utm_source=github&utm_medium=issues

void dflt_init();
void task_ref(void* parameter);
void opt_dflt_init();

/**
 * On Board RGB-LED control via website
 */
void iot18_day2(){
        dflt_init();
        rgb_init(); //deactivates WPS Button if onBoard LED auto!
        server.serveStatic("/app", SPIFFS, "/apps/rgb1.htm");
        functions.app_page = true;
}

/**
 * Sends a UPD-MSG every 5 Sekconds, Task created direktly
 */
void iot18_day3(){ //UDP Msg , 5 Sec
        dflt_init();
        udp_init();
        xTaskCreate([](void * parameter){
                while(1) {
                        udp_send_direct("Hello from your NodeESP :)", udp_set.udp_ip, udp_set.udp_port_send);
                        delay(5000);
                }
        }, "task_day3", 2000, NULL, 2, NULL);
}

/**
 * Send msgs on Button Press
 */
void iot18_day4(){ //UDP Msg , Boot press
        dflt_init();
        wps_enable(false); //Changes 21.03.19
        udp_init();
        xTaskCreate([](void * parameter){
                while(1) {
                        if(!digitalRead(wps_pin)) {
                                StaticJsonBuffer<128> jsonBuffer; // Allocate the memory pool on the stack
                                JsonObject &root = jsonBuffer.createObject();
                                root["cmd"] = "boot_pressed";
                                udp_send_json(root, udp_set.udp_ip, udp_set.udp_port_send);
                                delay(1000);
                        }
                }
        }, "task_d4", 5000, NULL, 2, NULL);
}

/**
 * Message send on Button1 (18) press
 */
void iot18_day5(){
        dflt_init();
        udp_init();
        buttons_init(buttons_dflt,1, false); //len_leds so only 2!

        xTaskCreate([](void * parameter){
                while(1) {
                        buttons_state();
                        if(button_change && (!buttons[0].pin_state)) {
                                StaticJsonBuffer<128> jsonBuffer; // Allocate the memory pool on the stack
                                JsonObject &root = jsonBuffer.createObject();
                                root["cmd"] = "btn1_pressed";
                                udp_send_json(root, udp_set.udp_ip, udp_set.udp_port_send);
                                delay(100);
                                button_change = false;
                        }

                }
        }, "task_d5", 2000, NULL, 2, NULL);
}

/*
   Touch Buttons, Text to Speech
 */
void iot18_day6(){
        dflt_init();
        udp_init();

        touch_init(touch_dflt,len_touch_dflt, true);
        while(1) {
                if (touch_change) {
                        for(int i = 0; i< len_touchs; i++) if (touchs[i].pin_state) {
                                        DynamicJsonBuffer jsonBuffer; // Allocate the memory pool on the stack
                                        JsonObject &root = jsonBuffer.createObject();

                                        String msg = touchs[i].pin_name;
                                        msg += " touched.";

                                        root["cmd"] = "touch_text";
                                        root["text"] = msg.c_str();

                                        udp_send_json(root, udp_set.udp_ip, udp_set.udp_port_send);

                                        delay(2000);
                                        touch_change = false;
                                        break;
                                }
                }
        }
}


/*
   Staircase Light - Task
 */
void d7_task(void * parameter ){
        while(1) {
                buttons_state();
                if(button_change && (!buttons[0].pin_state)) {
                        DynamicJsonBuffer jsonBuffer;                     // Allocate the memory pool on the stack
                        JsonObject &root = jsonBuffer.createObject();

                        root["cmd"] = "trigger";

                        udp_send_json(root, udp_set.udp_ip, udp_set.udp_port_send);
                        delay(100);
                        button_change = false;
                }
                delay(100);
        }
}

/**
 * Day7 - Staircase
 */
void iot18_day7(){
        dflt_init();
        udp_init();

        buttons_init(buttons_dflt,1);
        digital_init(leds_dflt, 1);

        xTaskCreate(d7_task, "d7_task",2000, NULL, 3, NULL);
}

/**
 * Task for Day8
 */
void d8_task(void * parameter ){
        while(1) {
                adc_read();
                DynamicJsonBuffer jsonBuffer;                             // Allocate the memory pool on the stack
                JsonObject &root = jsonBuffer.createObject();

                root["cmd"] = "adc";
                root["value"] = adcs[0].pin_value;

                udp_send_json(root, udp_set.udp_ip, udp_set.udp_port_send);

                delay(100);
        }
}

/**
 * Day8 - ADC-READ and Staircase (Day7 task)
 */
void iot18_day8(){
        dflt_init();
        udp_init();
        adc_init(adcs_dflt,1);


        buttons_init(buttons_dflt,1, false); //len_leds so only 2!
        digital_init(leds_dflt, 1);
        xTaskCreate(d7_task, "d7_task",2000, NULL, 3, NULL);
        xTaskCreate(d8_task, "d8_task",2000, NULL, 3, NULL);
}

/**
 *  ADC to RGB LED (OnBoard)
 */
void iot18_day9(){
        dflt_init();
        udp_init();

        rgb_init(); //deactivates WPS Button if onBoard LED auto!
        adc_init(adcs_dflt,1);

        connection_s* udp_con = new connection_s{false, true, false};
        cmd_handler_s* check_adc = new cmd_handler_s[2]{{"get_adc_state",get_adc_state},{"",NULL}};
        task_parameter_s* task_parameter = new task_parameter_s{check_adc,udp_con,2000};
        xTaskCreate(task_ref,"ADC",10000,(void*)task_parameter,1,NULL);
}

/**
 * DEEP Sleep
 */
void iot18_day10(){
        opt_dflt_init();

        wifiInit();
        udp_init();

        adc_init(adcs_dflt,1);

        DynamicJsonBuffer jsonBuffer;         // Allocate the memory pool on the stack
        JsonObject &root = jsonBuffer.createObject();

        bool hasAnsw = false;
        get_adc_state(root, root, hasAnsw);

        root.prettyPrintTo(Serial);

        if (hasAnsw) udp_send_json(root, udp_set.udp_ip, udp_set.udp_port_send);
        delay(100);

        esp_sleep_enable_timer_wakeup(20 * 1000000);
        esp_deep_sleep_start();
}

/**
 * MQTT Test
 */
void iot18_day11(){
        dflt_init();
        mqtt_init();

        mqtt_subscribe_topic("testtopic");
}

/**
 * Two Buttons MQTT
 */
void iot18_day12(){
        dflt_init();
        mqtt_init();

        buttons_init(buttons_dflt,2);
        support_init();

        cmd_handler_s* check_buttons = new cmd_handler_s[2]{{"get_button_change",get_button_change},{"",NULL}};
        task_parameter_s* task_parameter = new task_parameter_s{check_buttons,&connection,100};
        xTaskCreate(task_ref,"Buttons",10000,(void*)task_parameter,1,NULL);
}

/**
 * Touch Buttons + Deep Sleep
 */
void iot18_day13(){
        touch_pad_t touchPin;
        touchPin = esp_sleep_get_touchpad_wakeup_status();

        opt_dflt_init();

        int len_touch_d13 = 4;//  sizeof(touch_d14)/sizeof(touch_d14[0]);
        pin_config_s* touch_d13= new pin_config_s[len_touch_d13]{
          {9,T9,false,0,"Touch1"},
          {8,T8,false,0,"Touch2"},
          {7,T7,false,0,"Touch3"},
          {6,T6,false,0,"Touch4"},
        };

        touch_init(touch_d13, len_touch_d13,true);

        for(int i = 0; i<len_touch_d13; i++) {
                if (touch_d13[i].pin_id == (int)touchPin) {
                        DynamicJsonBuffer jsonBuffer;
                        JsonObject &rootTch = jsonBuffer.createObject();

                        rootTch["cmd"] = "get_touch_state";

                        rootTch["pin_name"] =touch_d13[i].pin_name.c_str();
                        rootTch["pin_id"] = touch_d13[i].pin_id;
                        rootTch["pin_nr"] =touch_d13[i].pin_nr;
                        rootTch["pin_state"] = true;

                        rootTch.prettyPrintTo(Serial);

                        if (!wifiInit()) ESP.restart();
                        mqtt_init();
                        mqtt_publish_json(rootTch);

                        break;
                }
        }

        delay(100);
        esp_sleep_enable_touchpad_wakeup();
        esp_deep_sleep_start();
}

/**
 * Smart light (Touch)
 */
void iot18_day14(){
        dflt_init();
        mqtt_init();

        int len_touch_d14 = 4;//  sizeof(touch_d14)/sizeof(touch_d14[0]);
        pin_config_s*touch_d14= new pin_config_s[len_touch_d14]{
                {9,T9,false,0,"Touch1"},
                {8,T8,false,0,"Touch2"},
                {7,T7,false,0,"Touch3"},
                {6,T6,false,0,"Touch4"},
        };
        touch_init(touch_d14, len_touch_d14,true);
        pwm_init();
        buttons_init(buttons_dflt,2);

        mqtt_subscribe_topic("pwm/state/set");

        cmd_handler_s* check_buttons = new cmd_handler_s[3]{{"get_button_change",get_button_change},{"get_touch_change",get_touch_change},{"",NULL}};
        task_parameter_s* task_parameter = new task_parameter_s{check_buttons,&connection,100};
        xTaskCreate(task_ref,"Buttons",10000,(void*)task_parameter,1,NULL);
}

/**
 * IR Reciever
 */
void iot18_day15(){
        opt_dflt_init(); //option to make dlft when 23 is open;

        wps_enable(true);
        wifiInit();
        //udp_init();
        mqtt_init();

        //Set Pin to 3.3V
        //support_init(&(ir_recv_pins[1])); //Support High
        pinMode(ir_recv_pins[1].pin_nr, OUTPUT);
        digitalWrite(ir_recv_pins[1].pin_nr, HIGH);

        ir_recv_init();

        task_parameter_s* task_parameter = new task_parameter_s{cmd_handler_ir_task,&connection,100};
        xTaskCreate(task_ref,"IR Task",10000,(void*)task_parameter,1,NULL);
}

/**
 * IR Send Recive
 */
void iot18_day16(){
        opt_dflt_init();   //option to make dlft when 23 is open;
        wps_enable(true);
        wifiInit();
        mqtt_init();

        //Set Pin to 3.3V
        pinMode(ir_recv_pins[1].pin_nr, OUTPUT);
        digitalWrite(ir_recv_pins[1].pin_nr, HIGH);

        //Set Pin5 (GND IR-Send) to GND
        pinMode(5, OUTPUT);
        digitalWrite(5, LOW);

        ir_recv_init();
        ir_send_init();

        mqtt_subscribe_topic("ir/send");
        task_parameter_s* task_parameter = new task_parameter_s{cmd_handler_ir_task,&connection,100};
        xTaskCreate(task_ref,"IR Task",10000,(void*)task_parameter,1,NULL);
}

/**
 * Time set light
 */
void iot18_day17(){
        dflt_init();
        mqtt_init();

        //ADC Auflösung auf 12 gesetzt
        pwm_init(leds_dflt, len_leds_dflt,5000, 12);

        adcs_dflt[0].pin_name="Poti_Led2";
        adcs_dflt[1].pin_name="Poti_Led1";

        adc_init(adcs_dflt,len_adcs_dflt);
        buttons_init(buttons_dflt,2);

        mqtt_subscribe_topic("pwm/state/set");

        cmd_handler_s* check_buttons = new cmd_handler_s[2]{{"get_button_change",get_button_change},{"",NULL}};
        task_parameter_s* task_parameter = new task_parameter_s{check_buttons,&connection,100};
        xTaskCreate(task_ref,"Buttons",10000,(void*)task_parameter,1,NULL);

        task_parameter_s* task_parameter_adc = new task_parameter_s{cmd_handler_adc_task,&connection,500};
        xTaskCreate(task_ref,"ADC Task",10000,(void*)task_parameter_adc,1,NULL);
}

/**
 * Candle watch
 */
void iot18_day18(){
        dflt_init();
        mqtt_init();

        adc_init(adcs_dflt,1);
        adcs[0].pin_name = "Candle";
        task_parameter_s* task_parameter = new task_parameter_s{cmd_handler_adc_task,&connection,1000};
        xTaskCreate(task_ref,"Fire",10000,(void*)task_parameter,1,NULL);
}

/**
 * Cheerlights
 */
void iot18_day19(){
        dflt_init();
        mqtt_init();

        neoPixel_init(NeoPixelType_Rgb, 1, neo_pixel_pin.pin_nr);
        mqtt_subscribe_topic("pixel/+/set");
}

/**
 * Plant watch
 */
void iot18_day20(){
        dflt_init();
        mqtt_init();
        udp_init();

        rf_init();
        adc_init();

        task_parameter_s* task_parameter = new task_parameter_s{cmd_handler_clima,&connection,1000};
        xTaskCreate(task_ref,"Clima",10000,(void*)task_parameter,1,NULL);
}

/**
 * Speaker
 */
void iot18_day21(){
      //  opt_dflt_init(); //option to make dlft when 23 is open;
      dflt_init();

      udp_init();
      speaker_init();
}

/**
 * Climate
 */
void iot18_day22(){
        dflt_init();
        mqtt_init();

        rf_init();
        adc_init();

        task_parameter_s* task_parameter = new task_parameter_s{cmd_handler_clima,&connection,2000};
        xTaskCreate(task_ref,"Clima",10000,(void*)task_parameter,1,NULL);
}

/**
* Alarm
 */
void iot18_day23(){
        dflt_init();
        mqtt_init();

        pin_config_s* btn_d23= new pin_config_s[1]{
            {3,21,false,0,"Tilt"},
        };

        buttons_init(btn_d23,1);
        alarm_init();


        cmd_handler_s* check_buttons = new cmd_handler_s[2]{{"get_button_change",get_button_change},{"",NULL}};
        task_parameter_s* task_parameter = new task_parameter_s{check_buttons,&connection,100};
        xTaskCreate(task_ref,"Buttons",10000,(void*)task_parameter,1,NULL);
}

/**
 * Christmas Light
 */
void iot18_day24(){
        dflt_init();
        //  mqtt_init();
        udp_init();

        neoPixel_init(NeoPixelType_Rgb, 2, neo_pixel_pin.pin_nr);

}

#endif
