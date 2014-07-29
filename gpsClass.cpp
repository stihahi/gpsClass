//
//  gpsClass.cpp
//  
//
//  Created by 岩井聡 on 2014/07/28.
//
//

#include "gpsClass.h"
#include "Arduino.h"

void gpsClass::auto_detect_baud_rate(void){
    const unsigned int bauds[] = {
        57600, 38400, 28800, 14400, 9600, 4800 };
    
    Serial.print("auto detect... ");
    
    for(int i=0; i<(sizeof(bauds)/sizeof(bauds[0]));i++){
        int p = 0;
        int r = 0;
        begin(bauds[i]);
        flush();
        do{
            if(available()){
                if(isprint(read())){
                    p++;
                }
                r++;
            }
        }
        while(r < 20);
        if(p > 15){
            Serial.print(bauds[i]);
            Serial.println(" ok");
            return;
        }
        delay(100);
    }
    
    Serial.println("fail");
    while(1);//エラー終了。
}

void gpsClass::serialSetup(void){
    auto_detect_baud_rate();
    char sendPack[256];
    sprintf(sendPack,"PMTK220,%d",reloadSec * 1000);//秒数
    send_pmtk_packet(sendPack);
    send_pmtk_packet("PMTK314,0,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0");
    send_pmtk_packet("PMTK251,9600");
    auto_detect_baud_rate();
}
void gpsClass::readData(void){
    if(available()){
        Serial.print((char)read());
    }
}

char* gpsClass::gpsFetch(void){
    if (available()) {
        int preamble = 0;
        char readData[256];
        int index = 0;
        while (!available()) {}//wait for data to come.
        unsigned long last_time = millis();
        do{
            char c=read();
            if (c == '$') {
                preamble ++;
                index = 0;
                while (c != '\n') {
                    while (!available()) {}//wait for data to come.
                    c = read();
                    if (c == '\n') {
                        readData[index] = 0;//end of line
                    }else{
                        readData[index] = c;
                    }
                    index++;
                }
            }
            delay(1);
        }while (millis()-last_time < 10);
        return readData;
    }
    return "";
}

void gpsClass::parser(float &latitude,float &longitude,char *readData){
    int length = strlen(readData);
    for (int k=0; k<length; k++) {
        Serial.print(readData[k]);
    }
}
void gpsClass::RMCParser(char *readData,RMCData &rmc){
    
}

void gpsClass::send_pmtk_packet(char *p)
{
    uint8_t checksum = 0;
    print('$');
    do {
        char c = *p++;
        if(c){
            checksum ^= (uint8_t)c;
            print(c);
        }
        else{
            break;
        }
    }
    while(1);
    print('*');
    println(checksum,HEX);
}
