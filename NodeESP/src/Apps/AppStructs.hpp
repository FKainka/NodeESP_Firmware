/**
 * AppStrucs.hpp
 *
 * Here all APPS are predefined and added to a list including Informations
 */
#include <ArduinoJson.h> //https://arduinojson.org/assistant/?utm_source=github&utm_medium=issues

#include "../Commands/CmdHandler.hpp"

/*Example APPS*/
void onboard_led_exmpl(), mqtt_blue_led();
void btn_exmpl(),touch_exmpl(), led_exmpl(),pwm_exmpl(),adc_exmpl();

/*Default Apps*/
bool list_apps(JsonObject& msg, JsonObject& answ, bool& hasAnsw);
void dflt_init(), firmware_update();

/*IoT Calendar 2018*/
void iot18_day2(), iot18_day3(), iot18_day4(), iot18_day5(),iot18_day6(), iot18_day7(),iot18_day8(), iot18_day9(), iot18_day10();
void iot18_day11(), iot18_day12(), iot18_day13(),iot18_day14(), iot18_day15(),iot18_day16(), iot18_day17(), iot18_day18(), iot18_day19(), iot18_day20();
void iot18_day21(), iot18_day22(),iot18_day23(),iot18_day24();

/* APPS for testing - not included most times*/
void udp_test_init(),speaker_test(), neoPixel_test(), ir_recv_test(), adc_test(), btn_n_touch_test(), btn_n_led_test(), rf_test(), ir_test(), mqtt_test(), all_test();
void tvLight(),test_test();
void sd_test_init();

// TASK
void d5_task(void * parameter);

/**
 *  Parameter for a Task
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

/*
 Typedef of an app
 */
typedef struct {
        String app;     //APP Name/Identifier
        void (*func)(); //Pointer to actual app
        String title;
        String info;    //Aditional Informations to show on Webpage
} apps_s;

/*
 Typedef if an Command
 */
typedef struct {
        String cmd;
        bool (*func)(JsonObject& msg, JsonObject&  answer);
} app_cmd_handler_s;

/*
Typedef of an list of list of apps
 */
typedef struct {
        apps_s* apps_pnt;
        String info;
}app_lists_s;

/**
 * APPS Adventcalendar IoT 2018
 */
apps_s apps_iot18[]= {
        {"iot18_day2",iot18_day2,"[IoT Calendar 2018 - Day2]","See <a href='http://iot.fkainka.de/day2-2018'>iot.fkainka.de/day2-2018</a> OnBoard LED (WS, AP) "},
        {"iot18_day3",iot18_day3,"[IoT Calendar 2018 - Day3]","See <a href='http://iot.fkainka.de/day3-2018'>iot.fkainka.de/day3-2018</a> (UDP, WS, WPS)"},
        {"iot18_day4",iot18_day4,"[IoT Calendar 2018 - Day4]","See <a href='http://iot.fkainka.de/day4-2018'>iot.fkainka.de/day4-2018</a> (UDP, WS, WPS)"},
        {"iot18_day5",iot18_day5,"[IoT Calendar 2018 - Day5]","See <a href='http://iot.fkainka.de/day5-2018'>iot.fkainka.de/day5-2018</a> Button (UDP, WS, WPS)"},
        {"iot18_day6",iot18_day6,"[IoT Calendar 2018 - Day6]","See <a href='http://iot.fkainka.de/day6-2018'>iot.fkainka.de/day6-2018</a> Touch (UDP, WS, WPS)"},
        {"iot18_day7",iot18_day7,"[IoT Calendar 2018 - Day7]","See <a href='http://iot.fkainka.de/day7-2018'>iot.fkainka.de/day7-2018</a> LED (UDP, WS, WPS)"},
        {"iot18_day8",iot18_day8,"[IoT Calendar 2018 - Day8]","See <a href='http://iot.fkainka.de/day8-2018'>iot.fkainka.de/day8-2018</a> ADC (UDP, WS, WPS)"},
        {"iot18_day9",iot18_day9,"[IoT Calendar 2018 - Day9]","See <a href='http://iot.fkainka.de/day9-2018'>iot.fkainka.de/day9-2018</a> ADC (UDP, WS, WPS)"},
        {"iot18_day10",iot18_day10,"[IoT Calendar 2018 - Day10]","See <a href='http://iot.fkainka.de/day10-2018'>iot.fkainka.de/day10-2018</a> Deep Sleep (UDP, ADC)"},

        {"iot18_day11",iot18_day11,"[IoT Calendar 2018 - Day11]","See <a href='http://iot.fkainka.de/day11-2018'>iot.fkainka.de/day11-2018</a> (MQTT, WS, WPS)"},
        {"iot18_day12",iot18_day12,"[IoT Calendar 2018 - Day12]","See <a href='http://iot.fkainka.de/day12-2018'>iot.fkainka.de/day12-2018</a> Button (MQTT, WS, WPS)"},
        {"iot18_day13",iot18_day13,"[IoT Calendar 2018 - Day13]","See <a href='http://iot.fkainka.de/day13-2018'>iot.fkainka.de/day13-2018</a> Touch (MQTT, WS, WPS)"},
        {"iot18_day14",iot18_day14,"[IoT Calendar 2018 - Day14]","See <a href='http://iot.fkainka.de/day14-2018'>iot.fkainka.de/day14-2018</a> LED (MQTT, WS, WPS)"},
        {"iot18_day15",iot18_day15,"[IoT Calendar 2018 - Day15]","See <a href='http://iot.fkainka.de/day15-2018'>iot.fkainka.de/day15-2018</a> IR (MQTT, WPS)"},
        {"iot18_day16",iot18_day16,"[IoT Calendar 2018 - Day16]","See <a href='http://iot.fkainka.de/day16-2018'>iot.fkainka.de/day16-2018</a> IR (MQTT, WPS)"},
        {"iot18_day17",iot18_day17,"[IoT Calendar 2018 - Day17]","See <a href='http://iot.fkainka.de/day17-2018'>iot.fkainka.de/day17-2018</a> ADC (MQTT, WS, WPS)"},
        {"iot18_day18",iot18_day18,"[IoT Calendar 2018 - Day18]","See <a href='http://iot.fkainka.de/day18-2018'>iot.fkainka.de/day18-2018</a> ADC (MQTT, WS, WPS)"},
        {"iot18_day19",iot18_day19,"[IoT Calendar 2018 - Day19]","See <a href='http://iot.fkainka.de/day19-2018'>iot.fkainka.de/day19-2018</a> Cheerlights (MQTT, WS, WPS)"},
        {"iot18_day20",iot18_day20,"[IoT Calendar 2018 - Day20]","See <a href='http://iot.fkainka.de/day20-2018'>iot.fkainka.de/day20-2018</a> Plant (MQTT, WS, WPS)"},

        {"iot18_day21",iot18_day21,"[IoT Calendar 2018 - Day21]","See <a href='http://iot.fkainka.de/day21-2018'>iot.fkainka.de/day21-2018</a> Sound (UDP, WS, WPS) "},
        {"iot18_day22",iot18_day22,"[IoT Calendar 2018 - Day22]","See <a href='http://iot.fkainka.de/day22-2018'>iot.fkainka.de/day22-2018</a> Clima (MQTT, WS, WPS) "},
        {"iot18_day23",iot18_day23,"[IoT Calendar 2018 - Day23]","See <a href='http://iot.fkainka.de/day23-2018'>iot.fkainka.de/day23-2018</a> Alarm (MQTT, WS, WPS)"},
        {"iot18_day24",iot18_day24,"[IoT Calendar 2018 - Day24]","See <a href='http://iot.fkainka.de/day24-2018'>iot.fkainka.de/day24-2018</a> RGB (MQTT, WS, WPS)"},
        {"",dflt_init,""} //marks end of handlers
};

/**
 * APPS Smarthome Lernpaket
 */
apps_s apps_sh_lp[]= {
        {"k1_p1",iot18_day2,"[Treppenhauslicht]","Flow: <a href='https://pastebin.com/R2MtBdVb'>'https://pastebin.com/R2MtBdVb</a> OnBoard LED (WS, AP) "},
        {"k1_p2",iot18_day3,"[Testlampe]"," Flow: <a href='https://pastebin.com/R2MtBdVb'>'https://pastebin.com/R2MtBdVb</a> (UDP, WS, WPS)"},
        {"iot18_day4",iot18_day4,"[IoT Calendar 2018 - Day4]","See <a href='http://iot.fkainka.de/day4-2018'>iot.fkainka.de/day4-2018</a> (UDP, WS, WPS)"},
        {"iot18_day5",iot18_day5,"[IoT Calendar 2018 - Day5]","See <a href='http://iot.fkainka.de/day5-2018'>iot.fkainka.de/day5-2018</a> Button (UDP, WS, WPS)"},
        {"iot18_day6",iot18_day6,"[IoT Calendar 2018 - Day6]","See <a href='http://iot.fkainka.de/day6-2018'>iot.fkainka.de/day6-2018</a> Touch (UDP, WS, WPS)"},
        {"iot18_day7",iot18_day7,"[IoT Calendar 2018 - Day7]","See <a href='http://iot.fkainka.de/day7-2018'>iot.fkainka.de/day7-2018</a> LED (UDP, WS, WPS)"},
        {"iot18_day8",iot18_day8,"[IoT Calendar 2018 - Day8]","See <a href='http://iot.fkainka.de/day8-2018'>iot.fkainka.de/day8-2018</a> ADC (UDP, WS, WPS)"},
        {"iot18_day9",iot18_day9,"[IoT Calendar 2018 - Day9]","See <a href='http://iot.fkainka.de/day9-2018'>iot.fkainka.de/day9-2018</a> ADC (UDP, WS, WPS)"},
        {"iot18_day10",iot18_day10,"[IoT Calendar 2018 - Day10]","See <a href='http://iot.fkainka.de/day10-2018'>iot.fkainka.de/day10-2018</a> Deep Sleep (UDP, ADC)"},

        {"iot18_day11",iot18_day11,"[IoT Calendar 2018 - Day11]","See <a href='http://iot.fkainka.de/day11-2018'>iot.fkainka.de/day11-2018</a> (MQTT, WS, WPS)"},
        {"iot18_day12",iot18_day12,"[IoT Calendar 2018 - Day12]","See <a href='http://iot.fkainka.de/day12-2018'>iot.fkainka.de/day12-2018</a> Button (MQTT, WS, WPS)"},
        {"iot18_day13",iot18_day13,"[IoT Calendar 2018 - Day13]","See <a href='http://iot.fkainka.de/day13-2018'>iot.fkainka.de/day13-2018</a> Touch (MQTT, WS, WPS)"},
        {"iot18_day14",iot18_day14,"[IoT Calendar 2018 - Day14]","See <a href='http://iot.fkainka.de/day14-2018'>iot.fkainka.de/day14-2018</a> LED (MQTT, WS, WPS)"},
        {"iot18_day15",iot18_day15,"[IoT Calendar 2018 - Day15]","See <a href='http://iot.fkainka.de/day15-2018'>iot.fkainka.de/day15-2018</a> IR (MQTT, WPS)"},
        {"iot18_day16",iot18_day16,"[IoT Calendar 2018 - Day16]","See <a href='http://iot.fkainka.de/day16-2018'>iot.fkainka.de/day16-2018</a> IR (MQTT, WPS)"},
        {"iot18_day17",iot18_day17,"[IoT Calendar 2018 - Day17]","See <a href='http://iot.fkainka.de/day17-2018'>iot.fkainka.de/day17-2018</a> ADC (MQTT, WS, WPS)"},
        {"iot18_day18",iot18_day18,"[IoT Calendar 2018 - Day18]","See <a href='http://iot.fkainka.de/day18-2018'>iot.fkainka.de/day18-2018</a> ADC (MQTT, WS, WPS)"},
        {"iot18_day19",iot18_day19,"[IoT Calendar 2018 - Day19]","See <a href='http://iot.fkainka.de/day19-2018'>iot.fkainka.de/day19-2018</a> Cheerlights (MQTT, WS, WPS)"},
        {"iot18_day20",iot18_day20,"[IoT Calendar 2018 - Day20]","See <a href='http://iot.fkainka.de/day20-2018'>iot.fkainka.de/day20-2018</a> Plant (MQTT, WS, WPS)"},

        {"iot18_day21",iot18_day21,"[IoT Calendar 2018 - Day21]","See <a href='http://iot.fkainka.de/day21-2018'>iot.fkainka.de/day21-2018</a> Sound (UDP, WS, WPS) "},
        {"iot18_day22",iot18_day22,"[IoT Calendar 2018 - Day22]","See <a href='http://iot.fkainka.de/day22-2018'>iot.fkainka.de/day22-2018</a> Clima (MQTT, WS, WPS) "},
        {"iot18_day23",iot18_day23,"[IoT Calendar 2018 - Day23]","See <a href='http://iot.fkainka.de/day23-2018'>iot.fkainka.de/day23-2018</a> Alarm (MQTT, WS, WPS)"},
        {"iot18_day24",iot18_day24,"[IoT Calendar 2018 - Day24]","See <a href='http://iot.fkainka.de/day24-2018'>iot.fkainka.de/day24-2018</a> RGB (MQTT, WS, WPS)"},
        {"",dflt_init,""} //marks end of handlers
};

/**
* Exampe APPS
 */
apps_s apps_examples[]= {
        {"dflt_init",dflt_init,"[init_dflt]","The default Applikation<br>(Webserver, WPS)"},
        {"onboard_led",onboard_led_exmpl,"[Onboard RGB-LED]","<a href='../app'>OnBoard LED Application Page</a><br>Try out diffrent colors of the onboard RGB-LED (WPS-Button will deactivated)<br>See <a href='http://iot.fkainka.de/day2-2018'>iot.fkainka.de/day2-2018</a> for details"},
        {"udp_auto_test",iot18_day3,"[UDP Test - Automated Message]","Recieve a Board Message every 5 Seconds via UDP<br>See <a href='http://iot.fkainka.de/day3-2018'>iot.fkainka.de/day3-2018</a> for details"},
        {"upd_press_test",iot18_day4,"[UDP Test - Button Press]","Recieve a Board Message via UDP every time you press the boot button<br>See <a href='http://iot.fkainka.de/day4-2018'>iot.fkainka.de/day4-2018</a> for details"},
        {"mqtt_test",iot18_day11,"[MQTT Test - Testtopic]","Try out the MQTT Connection<br>See <a href='http://iot.fkainka.de/day11-2018'>iot.fkainka.de/day11-2018</a> for details"},
        {"mqtt_blue_led",mqtt_blue_led,"[MQTT - Blue LED]","Try out the MQTT Connection and controll the blue onboard LED (commands :light_on/light_off, paramteter inverted (inv): true/false)"},

        {"upd_press_test",iot18_day4,"[UDP Test - Button Press]","Recieve a Board Message via UDP every time you press the boot button<br>See <a href='http://iot.fkainka.de/day4-2018'>iot.fkainka.de/day4-2018</a> for details"},
        {"btn_exmpl",btn_exmpl,"[Button Example]","Two Buttons <br>(Webserver, WPS, UDP, MQTT)"},
        {"touch_exmpl",touch_exmpl,"[Touch Example]","Two Touch-Buttons <br>(Webserver, WPS, UDP, MQTT)"},
        {"led_exmpl",led_exmpl,"[LED Example]","Two LEDs <br>(Webserver, WPS, UDP, MQTT)"},
        {"pwm_exmpl",pwm_exmpl,"[PWM-LED Example]","Two PWM-LEDs <br>(Webserver, WPS, UDP, MQTT)"},
        {"adc_exmpl",adc_exmpl,"[ADC Example]","Two ADCs <br>(Webserver, WPS, UDP, MQTT)"},

      //  {"tvLight",tvLight,"[TV Light]","Use 30 WS2812 LEDs as TV Light"},
        {"",dflt_init,""} //marks end of handlers
};

/**
 *  Test apps and Ideas -> Ignore for now
 */
apps_s apps_coming[]= {
        /*--------------*/
        {"test_test", test_test, "[TEST TEST]", "Test what you want"},
        {"tvLight",tvLight,"[TV Light]","Use 30 WS2812 LEDs as TV Light"},
        {"sd_test_init",sd_test_init,"[SD Card Test]","Test a SD Card by Serial output etc"},


        {"udp_test_init",udp_test_init,"[UDP Test]","Sends a test messages and response to cmd test"},
        {"btn_n_touch_test",btn_n_touch_test,"[Buttons and Touch Test]","Uses buttons 16,18, 23 and touch T7, T8, T9 and prints changes on Serial"},
        {"btn_n_led_test",btn_n_led_test,"[Buttons and LED Test]","Uses buttons 16,18 and LEDs on 15,22"},

        {"adc_test",adc_test,"[ADC Test]","Prints ADCs value on Serial"},
        {"rf_test",rf_test,"[Humidity Sensor Test]","Prints humidity value on Serial"},
        {"ir_test",ir_test,"[IR Recieve Test]","Prints Recievd Code on Serial"},
        {"all_test",all_test,"[All Test]","Prints Recievd Code on Serial"},
        {"speaker_test",speaker_test,"[Speaker Test]"},
        {"neoPixel_test",neoPixel_test,"[Neo Pixel Test]"},

        {"mqtt_test",mqtt_test,"[MQTT Test]"},

        {"",dflt_init,""} //marks end of handlers
};

/**
 * Inter apps for Updates etc
 */
apps_s apps_intern[]= {
        {"firmware_update",firmware_update,""},
        {"test",udp_test_init,""},
        {"",dflt_init,""} //marks end of handlers
};

//List of all lists that should be available public (Selectable by Website)
const int len_app_lists = 3;
app_lists_s app_lists[len_app_lists]= {  {apps_examples,"Example Apps"},
                          //   {apps_iot18,"IoT Adventcalendar 2018"},
                            {apps_sh_lp,"Smart Home Lernpaket"},
                            {apps_coming,"ToDo Apps"}
                            };