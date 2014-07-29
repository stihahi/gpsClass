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
    send_pmtk_packet("PMTK314,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0");
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
    if (length != 0) {
        RMCData rmcD;
        RMCParser(readData,rmcD);
//        Serial.println(rmcD.show());
    }
}


void gpsClass::RMCParser(char *readData,RMCData &rmc){
    char *c = readData;
    char nowPhrase[50];
    int index = 0;
    int dataNum = 0;
    while (*c != 0) {
        if (*c != ',') {
            nowPhrase[index++] = *c;
        }else{
            nowPhrase[index++] = 0;
/*            Serial.print("dataNum:");Serial.print(dataNum);
            Serial.print("[");Serial.print(nowPhrase);Serial.println("]");*/
            switch (dataNum) {
                case 1://225446.00	＝　測位時刻（UTC）　22:54:46.00
                    rmc.hour = CTOI(nowPhrase[0])*10 + CTOI(nowPhrase[1]);
                    rmc.min = CTOI(nowPhrase[2])*10 + CTOI(nowPhrase[3]);
                    rmc.sec = CTOI(nowPhrase[4])*10 + CTOI(nowPhrase[5]);
                    break;
                case 2://A	＝　ステータス；A = 有効，V = 無効
                    if (nowPhrase[0] == 'A') {
                        rmc.status = true;
                    }else{
                        rmc.status = false;
                    }
                    break;
                case 3://4916.452653,N	＝　緯度　49度16.452653分（北緯）
                    rmc.latitude = atof(nowPhrase);
                    break;
                case 4:
                    if (nowPhrase[0] == 'S') {
                        rmc.latitude = -rmc.latitude;
                    }
                    break;
                case 5://12311.123747,W	＝　経度　123度11.123747分（西経）
                    rmc.longitude = atof(nowPhrase);
                    Serial.println(rmc.longitude);
                    break;
                case 6:
                    if (nowPhrase[0] == 'W') {
                        rmc.longitude = -rmc.longitude;
                    }
                    break;
                case 7://000.5	＝　対地速度（ノット）　0.5ノット
                    rmc.knot = atof(nowPhrase);
                    break;
                case 8://054.7	＝　進行方向（度，真北）　54.7度
                    rmc.heading = atoi(nowPhrase);
                    break;
                default:
                    break;
            }
            dataNum ++;
            index = 0;
        }
        c++;
    }
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


char* RMCData::show(void){
    char ret[256];
    char NS;
    char WE;
    char temp1[12],temp2[12],temp3[12];
    if (latitude >0) {NS = 'N';}else{NS='S';}
    if (longitude > 0){WE = 'E';}else{WE='W';}
    sprintf(ret,"%d:%d:%d status:%d %c%s %c%s speed:%s heading:%d",hour,min,sec,status,NS,dtostrf(latitude,9,4,temp1),WE,dtostrf(longitude,10,4,temp2),dtostrf(knot,7,4,temp3),heading);
    return ret;
}
