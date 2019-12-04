/**
 * Apps of the IOT Adventcalendar 2018
 */
#ifndef APPSH19_HPP
#define APPSH19_HPP

void dflt_init();
void task_ref(void* parameter);
void opt_dflt_init();


void sh19_fire(){
        dflt_init();
        mqtt_init();

        adc_init(&adcs_dflt[1],1);
        task_parameter_s* task_parameter = new task_parameter_s{cmd_handler_adc_task,&connection,1000};
        xTaskCreate(task_ref,"Fire",10000,(void*)task_parameter,1,NULL);
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

#endif
