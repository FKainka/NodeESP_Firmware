/**
 * StatusLed.hpp
 * Functions for easy handling the Board integrated Status LED
 *
 * Usage:
 * - Use statusLedInit(true) to enable Status LED
 * - Use statusLed( [enum status_led] ) to set LED Color or Mode
 *
 * Posible settings:
 * off, red, green, yellow,
 * toggle_red, toggle_green, toggle_yellow,
 * blink_red, blink_green, blink_yellow,                 (100 ms frequency)
 * blink_red_slow, blink_green_slow, blink_yellow_slow   (200 ms frequency)
 */

#ifndef STATUS_LED_H
#define STATUS_LED_H
#include "Config.hpp"       //Includes struct gen_set_s gen_set which has the wifi settings

#define red_led 4                     //Green LED on Pin 4
#define green_led 2                   //Red LED on Pin 2
#define blue_led 0                    //Blue on Pin 0, BOOT BUTTON!!!

bool status_led = true;               //Enables/Disables status LED
bool toggle;                          //Toggle Bit for Blinking
TaskHandle_t taskBlinkHandle = NULL;  //Handler for Blink Task, needed to Delete Task


struct s_status_led_blink {     // Parameter for Blink Task
        bool red = true;        //Use red LED for Blinking
        bool green = false;     //Use green LED for Blinking
        int frequency = 100;    //Blink frequency
} status_led_blink;


enum status_led {  // Possible status LED Settings
        off, red, green, yellow,
        toggle_red, toggle_green,toggle_yellow,
        blink_red, blink_green, blink_yellow,
        blink_red_slow, blink_green_slow, blink_yellow_slow
};

/**
 * FreeRTOS Task for LED Blinking
 * @param parameter [struct s_status_led_blink with blink Parameter]
 *
 * Task runs until terminated extern
 */
void taskBlink(void * parameter){
        while(1) {
                if(((s_status_led_blink*)parameter)->red) digitalWrite(red_led, toggle);
                if(((s_status_led_blink*)parameter)->green) digitalWrite(green_led, toggle);
                toggle = !toggle;
                delay(((s_status_led_blink*)parameter)->frequency);
        }
}

/**
 * Set Status LED setting by passing an enum to this function
 * @param status [enum status_led status with status]
 */
void statusLed(enum status_led status){
        if (status_led & gen_set.status_led) { //Setting from config.hpp
                if( taskBlinkHandle != NULL ) { // Terminate Blinking Task if aktive
                        vTaskDelete( taskBlinkHandle );
                        taskBlinkHandle = NULL;
                }

                //Turn both LEDs off (HIGH = OFF here)
                digitalWrite(green_led, HIGH);
                digitalWrite(red_led, HIGH);

                //Select Status Mode
                switch (status) {
                case red:
                        digitalWrite(red_led, LOW);
                        break;
                case green:
                        digitalWrite(green_led, LOW);
                        break;
                case yellow:
                        digitalWrite(green_led, LOW);
                        digitalWrite(red_led, LOW);
                        break;
                case toggle_red:
                        digitalWrite(red_led, toggle);
                        toggle = !toggle;
                        break;
                case toggle_green:
                        digitalWrite(green_led, toggle);
                        toggle = !toggle;
                        break;
                case toggle_yellow:
                        digitalWrite(green_led, toggle);
                        digitalWrite(red_led, toggle);
                        toggle = !toggle;
                        break;
                case blink_red:
                        //Change LEDs Blinking & frequency
                        status_led_blink.red = true; status_led_blink.green = false; status_led_blink.frequency = 100;
                        //Create Task to be executed
                        //Parameters: Task Function, Task Name, Stack Size of Task, Parameter for Task Function, Task Priority (1 = Lowest), Task Handle for Termination
                        xTaskCreate(taskBlink,"blink", 500, (void*)&status_led_blink, 1,&taskBlinkHandle);
                        break;
                case blink_green:
                        status_led_blink.red = false; status_led_blink.green = true; status_led_blink.frequency = 100;
                        xTaskCreate(taskBlink,"blink", 500, (void*)&status_led_blink, 1,&taskBlinkHandle);
                        break;
                case blink_yellow:
                        status_led_blink.red = true; status_led_blink.green = true; status_led_blink.frequency = 100;
                        xTaskCreate(taskBlink,"blink", 500, (void*)&status_led_blink, 1,&taskBlinkHandle);
                        break;
                case blink_red_slow:
                        status_led_blink.red = true; status_led_blink.green = false; status_led_blink.frequency = 200;
                        xTaskCreate(taskBlink,"blink", 500, (void*)&status_led_blink, 1,&taskBlinkHandle);
                        break;
                case blink_green_slow:
                        status_led_blink.red = false; status_led_blink.green = true; status_led_blink.frequency = 200;
                        xTaskCreate(taskBlink,"blink", 500, (void*)&status_led_blink, 1,&taskBlinkHandle);
                        break;
                case blink_yellow_slow:
                        status_led_blink.red = true; status_led_blink.green = true; status_led_blink.frequency = 200;
                        xTaskCreate(taskBlink,"blink", 500, (void*)&status_led_blink, 1,&taskBlinkHandle);
                        break;
                default:  //Also Off
                        break;
                }
        }
}

/**
 * Initiate the Status LED
 * @param status_led_s [true if status LED should be used]
 */
void statusLedInit(){
        pinMode(red_led, OUTPUT);
        pinMode(green_led, OUTPUT);

        digitalWrite(red_led, HIGH);
        digitalWrite(green_led, HIGH);
}
#endif
