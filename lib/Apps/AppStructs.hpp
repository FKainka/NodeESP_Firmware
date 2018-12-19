#include "CmdHandler.hpp"
#include <ArduinoJson.h> //https://arduinojson.org/assistant/?utm_source=github&utm_medium=issues

/*Example APPS*/
void btn_exmpl(),touch_exmpl(), led_exmpl(),pwm_exmpl(),adc_exmpl();

/*OLD Example List*/
bool list_apps(JsonObject& msg, JsonObject& answ, bool& hasAnsw);
void dflt_init(), firmware_update();
void udp_test_init(),speaker_test(), neoPixel_test(), ir_recv_test(), adc_test(), btn_n_touch_test(), btn_n_led_test(), rf_test(), ir_test(), mqtt_test(), all_test();
void tvLight();

/*IoT Calendar 2018*/
void iot18_day2(), iot18_day3(),iot18_day4(), iot18_day5(),iot18_day6(), iot18_day7(),iot18_day8(), iot18_day9(), iot18_day10();
void iot18_day11(), iot18_day12(), iot18_day13(),iot18_day14(), iot18_day15(),iot18_day16(), iot18_day17(), iot18_day18(), iot18_day19(), iot18_day20();
void iot18_day21(), iot18_day22(),iot18_day23(),iot18_day24();

void d5_task(void * parameter);


/**
 *  * paramter:  (AppStructs.hpp)
  *       cmd_handler_s* functions; //Function to be executed
  *       connection_s* con;   {true, true, false} //Websockets; UDP, MQTT (Config.hpp)       //connections used for Communication
  *       uint16_t delay;
 */
typedef struct {
        cmd_handler_s* functions;
        connection_s* con;
        uint16_t delay;
} task_parameter_s;

typedef struct {
        String app;
        void (*func)();
        String info;
} apps_s;

typedef struct {
        String cmd;
        bool (*func)(JsonObject& msg, JsonObject&  answer);
} app_cmd_handler_s;

typedef struct {
        apps_s* apps_pnt;
        String info;
}app_lists_s;


apps_s apps_iot18[]= {
        {"iot18_day2",iot18_day2,"[IoT Calendar 2018 - Day2] See <a href='http://iot.fkainka.de/day2-2018'>iot.fkainka.de/day2-2018</a> OnBoard LED (WS, AP) "},
        {"iot18_day3",iot18_day3,"[IoT Calendar 2018 - Day3] See <a href='http://iot.fkainka.de/day3-2018'>iot.fkainka.de/day3-2018</a> (UDP, WS, WPS)"},
        {"iot18_day4",iot18_day4,"[IoT Calendar 2018 - Day4] See <a href='http://iot.fkainka.de/day4-2018'>iot.fkainka.de/day4-2018</a> (UDP, WS, WPS)"},
        {"iot18_day5",iot18_day5,"[IoT Calendar 2018 - Day5] See <a href='http://iot.fkainka.de/day5-2018'>iot.fkainka.de/day5-2018</a> Button (UDP, WS, WPS)"},
        {"iot18_day6",iot18_day6,"[IoT Calendar 2018 - Day6] See <a href='http://iot.fkainka.de/day6-2018'>iot.fkainka.de/day6-2018</a> Touch (UDP, WS, WPS)"},
        {"iot18_day7",iot18_day7,"[IoT Calendar 2018 - Day7] See <a href='http://iot.fkainka.de/day7-2018'>iot.fkainka.de/day7-2018</a> LED (UDP, WS, WPS)"},
        {"iot18_day8",iot18_day8,"[IoT Calendar 2018 - Day8] See <a href='http://iot.fkainka.de/day8-2018'>iot.fkainka.de/day8-2018</a> ADC (UDP, WS, WPS)"},
        {"iot18_day9",iot18_day9,"[IoT Calendar 2018 - Day9] See <a href='http://iot.fkainka.de/day9-2018'>iot.fkainka.de/day9-2018</a> ADC (UDP, WS, WPS)"},
        {"iot18_day10",iot18_day10,"[IoT Calendar 2018 - Day10] See <a href='http://iot.fkainka.de/day10-2018'>iot.fkainka.de/day10-2018</a> Deep Sleep (UDP, ADC)"},

        {"iot18_day11",iot18_day11,"[IoT Calendar 2018 - Day11] See <a href='http://iot.fkainka.de/day11-2018'>iot.fkainka.de/day11-2018</a> (MQTT, WS, WPS)"},
        {"iot18_day12",iot18_day12,"[IoT Calendar 2018 - Day12] See <a href='http://iot.fkainka.de/day12-2018'>iot.fkainka.de/day12-2018</a> Button (MQTT, WS, WPS)"},
        {"iot18_day13",iot18_day13,"[IoT Calendar 2018 - Day13] See <a href='http://iot.fkainka.de/day13-2018'>iot.fkainka.de/day13-2018</a> Touch (MQTT, WS, WPS)"},
        {"iot18_day14",iot18_day14,"[IoT Calendar 2018 - Day14] See <a href='http://iot.fkainka.de/day14-2018'>iot.fkainka.de/day14-2018</a> LED (MQTT, WS, WPS)"},
        {"iot18_day15",iot18_day15,"[IoT Calendar 2018 - Day15] See <a href='http://iot.fkainka.de/day15-2018'>iot.fkainka.de/day15-2018</a> IR (MQTT, WPS)"},
        {"iot18_day16",iot18_day16,"[IoT Calendar 2018 - Day16] See <a href='http://iot.fkainka.de/day16-2018'>iot.fkainka.de/day16-2018</a> IR (MQTT, WPS)"},
        {"iot18_day17",iot18_day17,"[IoT Calendar 2018 - Day17] See <a href='http://iot.fkainka.de/day17-2018'>iot.fkainka.de/day17-2018</a> ADC (MQTT, WS, WPS)"},
        {"iot18_day18",iot18_day18,"[IoT Calendar 2018 - Day18] See <a href='http://iot.fkainka.de/day18-2018'>iot.fkainka.de/day18-2018</a> ADC (MQTT, WS, WPS)"},
        {"iot18_day19",iot18_day19,"[IoT Calendar 2018 - Day19] See <a href='http://iot.fkainka.de/day19-2018'>iot.fkainka.de/day19-2018</a> Cheerlights (MQTT, WS, WPS)"},
        {"iot18_day20",iot18_day20,"[IoT Calendar 2018 - Day20] See <a href='http://iot.fkainka.de/day20-2018'>iot.fkainka.de/day20-2018</a> Plant (MQTT, WS, WPS)"},

        {"iot18_day21",iot18_day21,"[IoT Calendar 2018 - Day21] See <a href='http://iot.fkainka.de/day21-2018'>iot.fkainka.de/day21-2018</a> Sound (UDP, WS, WPS) "},
        {"iot18_day22",iot18_day22,"[IoT Calendar 2018 - Day22] See <a href='http://iot.fkainka.de/day22-2018'>iot.fkainka.de/day22-2018</a> Clima (MQTT, WS, WPS) "},
        {"iot18_day23",iot18_day23,"[IoT Calendar 2018 - Day23] See <a href='http://iot.fkainka.de/day23-2018'>iot.fkainka.de/day23-2018</a> Alarm (MQTT, WS, WPS)"},
        {"iot18_day24",iot18_day24,"[IoT Calendar 2018 - Day24] See <a href='http://iot.fkainka.de/day24-2018'>iot.fkainka.de/day24-2018</a> RGB (MQTT, WS, WPS)"},
        {"",dflt_init,""} //marks end of handlers
};

apps_s apps_examples[]= {
        {"dflt_init",dflt_init,"[init_dflt] The default Applikation (Webserver, WPS)"},
        {"btn_exmpl",btn_exmpl,"[Button Example] Two Buttons (Webserver, WPS, UDP, MQTT)"},
        {"touch_exmpl",touch_exmpl,"[Touch Example] Two Touch-Buttons (Webserver, WPS, UDP, MQTT)"},
        {"led_exmpl",led_exmpl,"[LED Example] Two LEDs (Webserver, WPS, UDP, MQTT)"},
        {"pwm_exmpl",pwm_exmpl,"[PWM-LED Example] Two PWM-LEDs (Webserver, WPS, UDP, MQTT)"},
        {"adc_exmpl",adc_exmpl,"[ADC Example] Two ADCs (Webserver, WPS, UDP, MQTT)"},
        {"onboard_led",iot18_day2,"[OnBoard RGB-LED Example] <a href='../app'>OnBoard LED App Page</a>(UDP, WPS, Webserver)"},
      //  {"tvLight",tvLight,"[TV Light] Use 30 WS2812 LEDs as TV Light"},
        {"",dflt_init,""} //marks end of handlers
};

apps_s apps_coming[]= {
        {"x",dflt_init,"[Makey Makey] See <a href='http://iot.fkainka.de/day2'>iot.fkainka.de/day2</a> (UDP, WPS, Webserver)"},
        {"x",dflt_init,"[OnBoard RGB-LED Example] See <a href='http://iot.fkainka.de/day2'>iot.fkainka.de/day2</a> (UDP, WPS, Webserver)"},
        {"x",dflt_init,"[Everything Example] See <a href='http://iot.fkainka.de/day2'>iot.fkainka.de/day2</a> (UDP, WPS, Webserver)"},
        {"x",dflt_init,"[BLE Example] See <a href='http://iot.fkainka.de/day2'>iot.fkainka.de/day2</a> (UDP, WPS, Webserver)"},
        {"x",dflt_init,"[Programmer] See <a href='http://iot.fkainka.de/day2'>iot.fkainka.de/day2</a> (UDP, WPS, Webserver)"},
        {"x",dflt_init,"[Do It] See <a href='http://iot.fkainka.de/day2'>iot.fkainka.de/day2</a> (UDP, WPS, Webserver)"},

        /*--------------*/
        {"tvLight",tvLight,"[TV Light] Use 30 WS2812 LEDs as TV Light"},

        {"udp_test_init",udp_test_init,"[UDP Test] Sends a test messages and response to cmd test"},
        {"btn_n_touch_test",btn_n_touch_test,"[Buttons and Touch Test] Uses buttons 16,18, 23 and touch T7, T8, T9 and prints changes on Serial"},
        {"btn_n_led_test",btn_n_led_test,"[Buttons and LED Test] Uses buttons 16,18 and LEDs on 15,22"},

        {"adc_test",adc_test,"[ADC Test] Prints ADCs value on Serial"},
        {"rf_test",rf_test,"[Humidity Sensor Test] Prints humidity value on Serial"},
        {"ir_test",ir_test,"[IR Recieve Test] Prints Recievd Code on Serial"},
        {"all_test",all_test,"[All Test] Prints Recievd Code on Serial"},
        {"speaker_test",speaker_test,"[Speaker Test]"},
        {"neoPixel_test",neoPixel_test,"[Neo Pixel Test]"},

        {"mqtt_test",mqtt_test,"[MQTT Test]"},

        {"",dflt_init,""} //marks end of handlers
};

apps_s apps_intern[]= {
        {"firmware_update",firmware_update,""},
        {"test",udp_test_init,""},
        {"",dflt_init,""} //marks end of handlers
};

int len_app_lists = 2;
app_lists_s app_lists[2] = {  {apps_examples,"Example Apps"},
                              {apps_iot18,"IoT Adventcalendar 2018"}
                            //  {apps_coming,"ToDo Apps"}
                            };
