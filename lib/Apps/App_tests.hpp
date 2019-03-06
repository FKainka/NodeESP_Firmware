/**
 * TEST APPS - IGNORE FOR NOW
 */
#ifndef APPTESTS_HPP
#define APPTESTS_HPP

#include <WiFiUdp.h>
#include <ArduinoJson.h> //https://arduinojson.org/assistant/?utm_source=github&utm_medium=issues
#include <SdCard.hpp>


void dflt_init();
void task_ref(void* parameter);
void opt_dflt_init();

/*--------------------------------*/

void sd_test_init(){
  dflt_init();
  if (functions.sd_card) test_sd();
}

void test_test(){
        dflt_init();

        pin_config_s* leds= new pin_config_s[1]{
          {1,blue_led,false,0,"LED"},
        };

        digital_init(leds,1);

        mqtt_init();
        mqtt_subscribe_topic("light/+");
}


void mqtt_test(){
        dflt_init();
        mqtt_init();

        dflt_cmd_mqtt[0] = cmd_handler;

        /*  cmd_handler_list.mqtt_list = new cmd_handler_s*[2]  {
                                               cmd_handler, NULL
                                       };
         */


        mqtt_subscribe_topic("+");
        mqtt_subscribe_topic("+/+");
        mqtt_subscribe_topic("+/+/+");

}

void udp_test_task(void * parameter ){
        while(1) {
                StaticJsonBuffer<256> jsonBuffer; // Allocate the memory pool on the stack
                JsonObject &root = jsonBuffer.createObject();
                root["cmd"] = "test send";
                udp_send_json(root, udp_set.udp_ip, udp_set.udp_port_send);
                delay(10000);
        }
}

void udp_test_init(){
        dflt_init();
        udp_init();

        cmd_handler_list.udp_list = new cmd_handler_s*[3]  {
                                            cmd_handler, cmd_handler_gen,NULL
                                    };
        xTaskCreate(udp_test_task,"TaskTest",10000,NULL,1,NULL);

}

void tvLight(){
        dflt_init();
        server.serveStatic("/app", SPIFFS, "/apps/rgbArray.htm");
        functions.app_page = true;
        neoPixel_load();
        cmd_handler_list.ws_list = new cmd_handler_s*[3]  {
                                           cmd_handler, cmd_handler_TvLight,NULL
                                   };
}

void neoPixel_test(){
        dflt_init();
        // # LED, Pin
        neoPixel_init(NeoPixelType_Grb, 2, neo_pixel_pin.pin_nr);

        RgbColor red(255, 0, 0);
        RgbColor blue(0,0,255);
        RgbColor green(0,255,0);
        delay(500);
        strip->SetPixelColor(0,blue);
        strip->SetPixelColor(1,red);
        strip->Show();
        delay(500);
        strip->SetPixelColor(0,red);
        strip->SetPixelColor(1,blue);
        strip->Show();
        delay(500);
        strip->SetPixelColor(0,red);
        strip->SetPixelColor(1,green);
        strip->Show();
}

void ir_test(){
        opt_dflt_init();     //option to make dlft when 23 is open;
        wps_enable(true);
        wifiInit();
        udp_init();
        mqtt_init();

        //Set Pin to 3.3V
        pinMode(ir_recv_pins[1].pin_nr, OUTPUT);
        digitalWrite(ir_recv_pins[1].pin_nr, HIGH);

        ir_recv_init();
        ir_send_init();

        dflt_cmd_mqtt[0] = cmd_handler_ir;
        dflt_cmd_list[1] =  cmd_handler_ir;

        task_parameter_s* task_parameter = new task_parameter_s{cmd_handler_ir_task,&connection,100};
        xTaskCreate(task_ref,"IR Task",10000,(void*)task_parameter,1,NULL);


        //ir_recv_test();

//  ir_send_init(39);
//ir_test();

}


void btn_n_led_test(){
        dflt_init();
        buttons_init(buttons_dflt,len_leds_dflt); //len_leds so only 2!
        //Spport Pin! !!!
        pinMode(gnd_support_pin.pin_nr, OUTPUT); digitalWrite(gnd_support_pin.pin_nr, LOW);
        buttons_state();
        for(int i = 0; i< len_buttons; i++) Serial.printf("Button %d is %d\n", buttons[i].pin_id,buttons[i].pin_state);

        digital_init(leds_dflt, len_leds_dflt);
        leds[0].pin_state = HIGH;
        digital_state(leds_dflt,len_leds_dflt);

        while(1) {
                buttons_state();
                if (button_change) {
                        for(int i = 0; i< len_leds; i++) {
                                Serial.printf("Button %d is %d\n", buttons[i].pin_id,buttons[i].pin_state);
                                leds[i].pin_state = buttons[i].pin_state;
                        }
                        digital_state(leds_dflt,len_leds_dflt);

                        Serial.println("-");
                        delay(500); //debounce
                        button_change = false;
                }
        }
}


void btn_n_touch_test(){
        dflt_init();
        udp_init();
        mqtt_init();

        //buttons_init();
        buttons_init();
        pinMode(gnd_support_pin.pin_nr, OUTPUT); digitalWrite(gnd_support_pin.pin_nr, LOW);
        touch_init(touch_dflt, len_touch_dflt, true);

        dflt_cmd_list[1] = cmd_handler_gen;
        dflt_cmd_mqtt[0] = cmd_handler_gen;

        cmd_handler_tasks[0].cmd ="get_button_change";
        cmd_handler_tasks[0].func =get_button_change;
        cmd_handler_tasks[1].cmd ="get_touch_change";
        cmd_handler_tasks[1].func =get_touch_change;

        task_parameter_s* task_parameter = new task_parameter_s{cmd_handler_tasks,&connection,100};
        xTaskCreate(task_ref,"Buttons",10000,(void*)task_parameter,1,NULL);
}


void all_test(){
        dflt_init();
        udp_init();
        mqtt_init();

        pinMode(gnd_support_pin.pin_nr, OUTPUT); digitalWrite(gnd_support_pin.pin_nr, LOW);
        buttons_init();
        touch_init();
        leds_init();
        adc_init();

        cmd_handler_s* check_buttons = new cmd_handler_s[3]{{"get_button_change",get_button_change},{"get_touch_change",get_touch_change},{"",NULL}};
        cmd_handler_s* read_task = new cmd_handler_s[2]{{"get_adc_state",get_adc_state},{"",NULL}};
        connection_s* ws_con = new connection_s{true,false,false,};
        connection_s* udpMqtt_con = new connection_s{false,true,true};


        task_parameter_s* task_parameter = new task_parameter_s{check_buttons,&connection,100};
        xTaskCreate(task_ref,"Buttons",10000,(void*)task_parameter,1,NULL);

        task_parameter_s* task_parameter_adc_ws = new task_parameter_s{read_task,ws_con,500};
        xTaskCreate(task_ref,"ADCs_ws",10000,(void*)task_parameter_adc_ws,1,NULL);

        task_parameter_s* task_parameter_adc = new task_parameter_s{read_task,udpMqtt_con,3000};
        xTaskCreate(task_ref,"ADCs_udpmqtt",10000,(void*)task_parameter_adc,1,NULL);
}



void adc_test(){
        dflt_init();
        adc_init();
        while(1) {
                adc_read();
                for(int i = 0; i< len_adcs; i++) Serial.printf("ADC %d (%s) = %d\n",adcs[i].pin_id, adcs[i].pin_name.c_str(), adcs[i].pin_value);
                Serial.println("-");
                delay(2000);
        }
}

void rf_test(){
        dflt_init();
        rf_init();
        adc_init();
        while(1) {
                int hum  = get_humidity();
                Serial.printf("Humidity: %d \n--\n",hum);
                adc_read();
                for(int i = 0; i< len_adcs; i++) Serial.printf("ADC %d (%s) = %d\n",adcs[i].pin_id, adcs[i].pin_name.c_str(), adcs[i].pin_value);
                Serial.println("-");
                double t = get_temp(adcs[1].pin_value);
                Serial.printf("Temp: %f \n\n", t);
                delay(2000);
        }

}

void speaker_test(){
        opt_dflt_init();   //option to make dlft when 23 is open;
        //wps_enable(true);

        wifiInit();
        wps_enable(false);
        udp_init();
        mqtt_init();

        speaker_init();


        speaker_play_twinkle();


        //server.begin();
        /*

           if (SPIFFS.exists("/sounds/demo.raw")) {
                File file = SPIFFS.open("/sounds/demo.raw");
                if (file) speaker_play_stream(&file);
                else Serial.println("Error opening File");
                file.close();

           }
           else{
                Serial.println("File not found!");
           }

           HTTPClient http;
           //http.begin("http://halckemy.s3.amazonaws.com/uploads/attachments/444742/demo.raw");
           //http.begin("http://www.wavsource.com/snds_2018-01-14_3453803176249356/movie_stars/cagney/say_your_prayers.wav");
           http.begin("http://ota.fkainka.de/meet_parents_bring_u_down.raw");



           Serial.print("[HTTP] GET...\n");

           int httpCode = http.GET();       // start connection and send HTTP header
           if(httpCode > 0) {

                Serial.printf("[HTTP] GET... code: %d\n", httpCode); // HTTP header has been send and Server response header has been handled
                if(httpCode == HTTP_CODE_OK) { // file found at server

                        int len = http.getSize();         // get lenght of document (is -1 when Server sends no Content-Length header)
                        Serial.println(len);

                        // get tcp stream
                        WiFiClient* stream = http.getStreamPtr();
                        if (stream) speaker_play_stream(stream);

                }
           }
         */
}

#endif
