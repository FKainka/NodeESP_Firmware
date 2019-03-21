/**
 * Some Example apps
 *
 * Examples apps behavoid will be shown on Website
 */
#ifndef APPEXAMP_HPP
#define APPEXAMP_HPP

#include <WiFiUdp.h>
#include <ArduinoJson.h>   //https://arduinojson.org/assistant/?utm_source=github&utm_medium=issues

void dflt_init();
void task_ref(void* parameter);
void opt_dflt_init();

void onboard_led_exmpl(){
        dflt_init();
        rgb_init(); //deactivates WPS Button if onBoard LED auto!
        server.serveStatic("/app", SPIFFS, "/apps/rgb1.htm");
        functions.app_page = true;
}


void sd_intern_temp_exmpl(){
        dflt_init();

        if (functions.sd_card) {
                if (SD.exists("/gauge/interntemp.htm")) {
                        server.serveStatic("/app", SD, "/gauge/interntemp.htm");
                        functions.app_page  = true;
                }
        }
}


void sd_intern_hall_exmpl(){
        dflt_init();

        if (functions.sd_card) {
                if (SD.exists("/gauge/magnet_ws.htm")) {
                        server.serveStatic("/app", SD, "/gauge/magnet_ws.htm");
                        functions.app_page  = true;
                }
        }

        connection_s* udp_con = new connection_s{true, false, false}; //ws, upd, mqtt
        cmd_handler_s* check_hall = new cmd_handler_s[2]{{"get_intern_hall",get_intern_hall},{"",NULL}};
        task_parameter_s* task_parameter = new task_parameter_s{check_hall,udp_con,200};
        xTaskCreate(task_ref,"HALL",10000,(void*)task_parameter,1,NULL);
}



void mqtt_blue_led(){
  dflt_init();

  pin_config_s* leds= new pin_config_s[1]{
    {1,blue_led,false,0,"LED"},
  };

  digital_init(leds,1);

  mqtt_init();
  mqtt_subscribe_topic("light/+");
}



/**
 * Test 2 Buttons (UDP & MQTT)
 */
void btn_exmpl(){  //
        dflt_init();
        udp_init();
        mqtt_init();

        buttons_init(buttons_dflt,2);
}

/**
 *  Test Touch Buttons (UDP & MQTT)
 */
void touch_exmpl(){
        dflt_init();
        udp_init();
        mqtt_init();

        touch_init(touch_dflt,2);

        cmd_handler_s* check_buttons = new cmd_handler_s[2]{{"get_touch_change",get_touch_change},{"",NULL}};
        task_parameter_s* task_parameter = new task_parameter_s{check_buttons,&connection,100};
        xTaskCreate(task_ref,"Buttons",10000,(void*)task_parameter,1,NULL);
}

/**
 *  Test LED  (UDP & MQTT)
 */
void led_exmpl(){
        dflt_init();
        udp_init();
        mqtt_init();

        leds_init();
}

/**
 *  Test PWM Buttons (UDP & MQTT)
 */
void pwm_exmpl(){
        dflt_init();
        udp_init();
        mqtt_init();

        pwm_init(); //8 Bit
}

/**
 * Test ADC Buttons (UDP & MQTT)
 */
void adc_exmpl(){
        dflt_init();
        udp_init();
        mqtt_init();

        adc_init();

        task_parameter_s* task_parameter_adc = new task_parameter_s{cmd_handler_adc_task,&connection,500};
        xTaskCreate(task_ref,"ADC Task",10000,(void*)task_parameter_adc,1,NULL);
}

#endif
