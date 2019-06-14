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

#endif
