#ifndef CMDIR_HPP
#define CMDIR_HPP

#include "Peripherals.hpp"
#include "MQTT.hpp"
#include "Udp.hpp"
#include "Websockets.hpp"

bool ir_send(JsonObject& msg, JsonObject& answ, bool& hasAnsw){
        msg.prettyPrintTo(Serial);
        if (irsend == NULL) return false;
        ir_send(msg["type"], msg["value"], msg["length"]);
        return true;
}



bool ir_recv(JsonObject& msg, JsonObject& answ, bool& hasAnsw){
        decode_results results;
        if (irrecv->decode(&results)) {
                answ["cmd"]= "ir_recv";
                ir_result_toJson(answ,&results);
                irrecv->resume(); // resume receiver
                hasAnsw = true;
        }
        return true;
}

bool play_speaker_wav(HANDLER_PARAMETER){

        if (DacAudio == NULL) return false;
        if (taskSongHandle != NULL) return false;

        msg.prettyPrintTo(Serial);
        int len = msg["length"];

        unsigned char* song = (unsigned char*)malloc(len);

        Serial.printf("Ready to recieve Wav. Pointer %d \n\n", (int)song);

        udp.parsePacket();

        Serial.println("recieving");
        long recv = udp.read(song, len);
        Serial.println("recieved "+ String(recv));
        xTaskCreate(wav_task,"Song",10000,(void*)song,3,&taskSongHandle);
        return true;

}


bool play_speaker_note(HANDLER_PARAMETER){
        if (DacAudio == NULL) return false;
        if (taskSongHandle != NULL) return false;

        int8_t note= msg["note"];
        int8_t* song = (int8_t*)malloc(2);

        song[0] = note;
        song[1] = 127;

        xTaskCreate(audio_task,"Song",10000,(void*)song,3,&taskSongHandle);
        return true;
}

bool play_speaker_notes(HANDLER_PARAMETER){
        if (DacAudio == NULL) return false;
        if (taskSongHandle != NULL) return false;
        msg.prettyPrintTo(Serial);
        int len = msg["length"];

        int8_t* song = (int8_t*)malloc(len);
        for (int i =0; i<len; i++) {
                song[i] = msg["song"][i];
        }
        xTaskCreate(audio_task,"Song",10000,(void*)song,3,&taskSongHandle);

        return true;
}


bool set_alarm(HANDLER_PARAMETER){
        if(alarm_pin == NULL) return false;

        bool alarm = msg["value"];
        alarm_set(alarm);

        answ["cmd"] ="set_alarm";
        answ["value"] = alarm;
        hasAnsw = true;
        return true;
}

#endif
