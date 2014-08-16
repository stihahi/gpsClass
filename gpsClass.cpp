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
            Serial.println("ok");
            return;
        }
        delay(100);
    }
    
    Serial.println("fail");
    while(1);//エラー終了。
}

void gpsClass::serialSetup(void){
    auto_detect_baud_rate();
    char sendPack[128];
    sprintf(sendPack,"PMTK220,%d",reloadSec * 1000);//秒数
    send_pmtk_packet(sendPack);
    //////////////////////////l,r,v,g,s,v,r,t,0,0,0,0,0,0,0,0,0,z,0
    send_pmtk_packet("PMTK314,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0");
    send_pmtk_packet("PMTK251,9600");
    auto_detect_baud_rate();
}
void gpsClass::readData(void){
    if(available()){
        Serial.print((char)read());
    }
}

bool gpsClass::gpsFetch(void){
    unsigned long last_time = millis();
    bool changed = false;
    do{
        char c = 0;
        while (c != '$') {//$が出てくるまでまつ。
            changed = true;//データ取り込み成功！
            while (!available()) {}
            c=read();
        }
        int index=0;
        char readData[256];
        while (1) {
            while (!available()){}
            c = read();
            if (c != '\n') {//'\n'がでてくるまでよみとる。
                readData[index] = c;
            }else{
                readData[index] = 0;
                parser(readData);
                break;
            }
            index++;
        }
    }while (millis()-last_time < 100);//これで、タイムアウト20msで、できる限りデータを取り込んでParseすることができる。
    //    Serial.println(readData);
    return changed;
}

bool gpsClass::parser(char *readData){
    int length = strlen(readData);
//    Serial.println(readData);
    if (length != 0) {
        int datType = headerParser(readData);//ヘッダで、タイプわけする。
        Serial.print("dat type:");
        Serial.println(datType);
        gpsData *gpsD;
        switch (datType) {
            case NMEA_RMC:
                gpsD = &rmcD;
                break;
            case NMEA_ZDA:
                gpsD = &zdaD;
                break;
            case NMEA_GGA:
                gpsD = &ggaD;
                break;
            default:
                break;
        }
        gpsD->parser(readData);
        return true;
    }
    return false;
}

int gpsClass::headerParser(char *rawData){
    char *c = rawData;
    char headPhrase[10];
    int index = 0;
    while (*c != ',') {
        headPhrase[index++] = *c;
        c++;
    }
    headPhrase[index] = 0;
    if (strcmp(headPhrase,"GPRMC") == 0) {
        return NMEA_RMC;
    }else if(strcmp(headPhrase,"GPGGA") == 0){
        return NMEA_GGA;
    }else if(strcmp(headPhrase,"GPZDA") == 0){
        return NMEA_ZDA;
    }
    return NMEA_OTHER;
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

char* gpsClass::getLCD(int mode,int line){
    gpsData *gpsD;
    switch (mode) {
        case NMEA_RMC:
            gpsD = &rmcD;
            break;
        case NMEA_ZDA:
            gpsD = &zdaD;
            break;
        case NMEA_GGA:
            gpsD = &ggaD;
            break;
        default:
            return "";
            break;
    }
    return gpsD->lcdShow(line);
}

