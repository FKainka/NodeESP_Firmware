#ifndef COLOR_FUNC_HPP
#define COLOR_FUNC_HPP

#include "Peripherals.hpp"
#include "MQTT.hpp"
#include "Udp.hpp"
#include "Websockets.hpp"


//*************************** RGB LED *******************************************
bool get_rgb_hex(HANDLER_PARAMETER){
        if (rgb == NULL) return false;

        answ["cmd"] ="get_rgb_hex";

        HtmlColor color = rgb_led_get_html();

        char color_value[20];
        color.ToNumericalString(color_value,20);
        answ["value"] = color_value;

        hasAnsw = true;
        return true;
}

bool set_rgb_hex(HANDLER_PARAMETER){
        if (rgb == NULL ) return false;

        HtmlColor color_html; const char* color = msg["value"];
        uint8_t result = color_html.Parse<HtmlColorNames>(color);
        rgb_led_set(color_html);
        return get_rgb_hex(msg, answ, hasAnsw);
}


bool get_rgb_rgb(HANDLER_PARAMETER){
        if (rgb == NULL) return false;

        answ["cmd"] ="get_rgb_rgb";

        RgbColor color = rgb_led_get();

        char color_value[20];
        sprintf(color_value, "rgb(%d,%d,%d)",color.R,color.G,color.B);
        answ["value"] = color_value;

        hasAnsw = true;
        return true;
}

bool set_rgb_rgb(HANDLER_PARAMETER){
        if (rgb == NULL ) return false;

        int r,g,b, parsed;
        const char* color_raw = msg["value"];

        parsed =sscanf(color_raw, "rgb(%d,%d,%d)",&r,&g,&b);
        if (parsed == 3) {
                RgbColor color(r,g,b);
                rgb_led_set(color);
        }
        return get_rgb_rgb(msg, answ, hasAnsw);
}

bool get_rgb_hsv(HANDLER_PARAMETER){
        if (rgb == NULL) return false;

        answ["cmd"] ="get_rgb_hsv";

        HsbColor color = rgb_led_get_hsv(rgb);

        char color_value[20];
        Serial.printf("HSV(H: %f, S: %f, V: %f)\n\n", color.H, color.S, color.B);
        //sprintf(color_value, "hsv(%f,%f,%f)",color.H,color.S,color.B);
        sprintf(color_value, "hsv(%d,%d,%d)",(int)(color.H*360),(int)(color.S*100),(int)(color.B*100));
        answ["value"] = color_value;

        hasAnsw = true;
        return true;
}

bool set_rgb_hsv(HANDLER_PARAMETER){
        if (rgb == NULL ) return false;

        int h,s,v, parsed;
        const char* color_raw = msg["value"];

        parsed =sscanf(color_raw, "hsv(%d,%d,%d)",&h,&s,&v);
        if (parsed != 3) parsed =sscanf(color_raw, "hsv(%d,%d%%,%d%%)",&h,&s,&v);

        if (parsed == 3) {
                float hue = ((float)h)/360; float sac = ((float)s)/100; float val =((float)v) /100; //Convert to float 0..1 for Library
                HsbColor color(hue,sac,val);
                rgb_led_set(color);
        }
        return get_rgb_hsv(msg, answ, hasAnsw);
}

bool set_rgb_fade(HANDLER_PARAMETER){
        if (rgb == NULL ) return false;

        int r,g,b, parsed;
        const char* color_raw = msg["value"];

        parsed =sscanf(color_raw, "rgb(%d,%d,%d)",&r,&g,&b);
        if (parsed != 3) return false;

        RgbColor toColor(r,g,b);
        int duration = 1;
        if (msg.containsKey("duration")) duration = msg["duration"];
        rgb_led_fade(toColor,  duration);
        return true;
}

bool get_rgb_config(HANDLER_PARAMETER){
        if (rgb == NULL) return false; //No Buttons Definded
        answ["cmd"] = "get_rgb_config";
        get_pin_state_json(rgb,3,answ);
        hasAnsw = true;
        return true;
}


//********************************************* PIXEL STRIP *************************************************

bool get_pixel_hex(HANDLER_PARAMETER){
        if (strip == NULL ) return false;

        answ["cmd"] ="get_pixel_hex";

        HtmlColor color = get_pixel_hex();
        char color_value[20];
        color.ToNumericalString(color_value,20);
        answ["value"] = color_value;

        hasAnsw = true;
        return true;
}

bool set_pixel_hex(HANDLER_PARAMETER){
        if (strip == NULL ) return false;

        Serial.println("PIXEL HEX");

        HtmlColor color_html; const char* color = msg["value"];
        uint8_t result = color_html.Parse<HtmlColorNames>(color);
        pixel_set(color_html);
        return get_pixel_hex(msg, answ, hasAnsw);
}

bool get_pixel_rgb(HANDLER_PARAMETER){
        if (strip == NULL ) return false;

        answ["cmd"] ="get_pixel_rgb";

        RgbColor color = get_pixel_rgb();

        char color_value[20];
        sprintf(color_value, "rgb(%d,%d,%d)",color.R,color.G,color.B);
        answ["value"] = color_value;

        hasAnsw = true;
        return true;

}

bool set_pixel_rgb(HANDLER_PARAMETER){
        if (strip == NULL ) return false;
        int r,g,b, parsed;
        const char* color_raw = msg["value"];

        parsed =sscanf(color_raw, "rgb(%d,%d,%d)",&r,&g,&b);
        if (parsed == 3) {
                RgbColor color(r,g,b);
                pixel_set(color);
        }
        return get_pixel_rgb(msg, answ, hasAnsw);
}

bool get_pixel_hsv(HANDLER_PARAMETER){
        if (strip == NULL ) return false;

        answ["cmd"] ="get_pixel_hsv";

        HsbColor color = get_pixel_hsv();

        char color_value[20];
        Serial.printf("HSV(H: %f, S: %f, V: %f)\n\n", color.H, color.S, color.B);
        //sprintf(color_value, "hsv(%f,%f,%f)",color.H,color.S,color.B);
        sprintf(color_value, "hsv(%d,%d,%d)",(int)(color.H*360),(int)(color.S*100),(int)(color.B*100));
        answ["value"] = color_value;

        hasAnsw = true;
        return true;
}

bool set_pixel_hsv(HANDLER_PARAMETER){
        if (strip == NULL ) return false;

        int h,s,v, parsed;
        const char* color_raw = msg["value"];

        parsed =sscanf(color_raw, "hsv(%d,%d,%d)",&h,&s,&v);
        if (parsed != 3) parsed =sscanf(color_raw, "hsv(%d,%d%%,%d%%)",&h,&s,&v);

        if (parsed == 3) {
                float hue = ((float)h)/360; float sac = ((float)s)/100; float val =((float)v) /100; //Convert to float 0..1 for Library
                HsbColor color(hue,sac,val);
                pixel_set(color);
        }
        return get_pixel_hsv(msg, answ, hasAnsw);

}

bool set_pixel_fade(HANDLER_PARAMETER){
        if (strip == NULL ) return false;
        return true;
}

bool set_array_fade(HANDLER_PARAMETER){
        if (strip == NULL ) return false;
        return true;
}




bool set_pixel_array(HANDLER_PARAMETER){
        if (strip == NULL ) return false;
        neoPixel_array_fromJson(msg);
        strip->Show();
      //  neoPixel_save(); !!!
        hasAnsw = false;
        return true;
}

bool get_pixel_set(HANDLER_PARAMETER){
        if (strip == NULL ) return false;
        answ["cmd"] = "get_pixel_set";

        neoPixel_set_toJson(answ);
        neoPixel_array_toJson(answ);
        //answ["max_bright"] =
        //    answ.prettyPrintTo(Serial);
        hasAnsw = true;
        return true;
}

bool set_pixel_set(HANDLER_PARAMETER){
        if (strip == NULL ) return false;
        neoPixel_set_fromJson(msg);
        neoPixel_save();
        delete strip;
        neoPixel_init();
        answ["cmd"] = "get_pixel_set";

        neoPixel_set_toJson(answ);
        neoPixel_array_toJson(answ);
        //answ["max_bright"] =
        //    answ.prettyPrintTo(Serial);
        hasAnsw = true;
        return true;
}

bool set_pixel_bright(HANDLER_PARAMETER){
        if (strip == NULL ) return false;
        neopixel_set.brightness = msg["brightness"];
        strip->SetBrightness(neopixel_set.brightness);
        strip->Show();
        return true;
}




#endif
