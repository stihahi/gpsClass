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
    send_pmtk_packet("PMTK314,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0");
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
    }while (millis()-last_time < 20);//これで、タイムアウト20msで、できる限りデータを取り込んでParseすることができる。
//    Serial.println(readData);
    return changed;
}

bool gpsClass::parser(char *readData){
    int length = strlen(readData);
/*    for (int k=0; k<length; k++) {
        Serial.print(readData[k]);
    }*/
    if (length != 0) {
        int datType = headerParser(readData);//ヘッダで、タイプわけする。
        Serial.print("dat type:");
        Serial.println(datType);
        switch (datType) {
            case NMEA_RMC:
                RMCParser(readData,rmcD);
                break;
            case NMEA_ZDA:
                ZDAParser(readData,zdaD);
                break;
            default:
                break;
        }
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

char* gpsClass::readNextData(char *rawData){
    char retPhrase[32];
    int index = 0;
    while(*rawData != ',' && *rawData != 0){
        retPhrase[index++] = *rawData++;
    }
    retPhrase[index] = 0;
    return retPhrase;
}

void gpsClass::ZDAParser(char *readData,ZDAData &zda){
    char *nowPhrase;
    int index = 0;
    int dataNum = 0;
    do{
        nowPhrase = readNextData(readData);
        switch (dataNum) {
            case 0:
                if (strcmp(nowPhrase,"GPZDA") != 0) {
                    return;
                }
                break;
            case 1:
                zda.hour = CTOI(nowPhrase[0]) * 10 + CTOI(nowPhrase[1]);
                zda.min  = CTOI(nowPhrase[2]) * 10 + CTOI(nowPhrase[3]);
                zda.sec  = CTOI(nowPhrase[4]) * 10 + CTOI(nowPhrase[5]);
                break;
            case 2:
                zda.day = atoi(nowPhrase);
                break;
            case 3:
                zda.month = atoi(nowPhrase);
                break;
            case 4:
                zda.year = atoi(nowPhrase);
                break;
            default:
                break;
        }
        dataNum ++;
        readData += strlen(nowPhrase)+1;
    }while (strlen(nowPhrase) != 0);
}


char* ZDAData::lcdShow(int line){
    char ret[128];
    if (line == 0) {
        sprintf(ret,"%d/%02d/%02d",year,month,day);
    }else{
        sprintf(ret,"%2d:%02d:%02d",hour,min,sec);
    }
    return ret;
}

void gpsClass::RMCParser(char *readData,RMCData &rmc){
    char *nowPhrase;
    int index = 0;
    int dataNum = 0;
    do{
        nowPhrase = readNextData(readData);
/*        Serial.print("dataNum:");Serial.print(dataNum);
        Serial.print("[");Serial.print(nowPhrase);Serial.println("]");*/
        switch (dataNum) {
            case 0:
                if (strcmp(nowPhrase,"GPRMC") != 0) {
                    return;//データの種類が違う~!
                }
                break;
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
        readData += strlen(nowPhrase)+1;
    }while (strlen(nowPhrase) != 0);
    
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
    switch (mode) {
        case NMEA_RMC:
            return rmcD.lcdShow(line);
            break;
        case NMEA_ZDA:
            return zdaD.lcdShow(line);
            break;
        default:
            return "";
            break;
    }
}

char* RMCData::serialShow(void){
    char ret[256];
    char NS;
    char WE;
    char temp1[12],temp2[12],temp3[12];
    if (latitude >0) {NS = 'N';}else{NS='S';}
    if (longitude > 0){WE = 'E';}else{WE='W';}
    sprintf(ret,"%d:%d:%d status:%d %c%s %c%s speed:%s heading:%d",hour,min,sec,status,NS,dtostrf(latitude,9,4,temp1),WE,dtostrf(longitude,10,4,temp2),dtostrf(knot,7,4,temp3),heading);
    return ret;
}

char* RMCData::lcdShow(int line){
    if (!status) {
        return "00000000";
    }
    char ret[128];
    char NSWE;
    char temp1[12];
    if (line == 0) {
        if (latitude >0) {NSWE = 'N';}else{NSWE='S';}
        sprintf(ret,"%c%s",NSWE,dtostrf(latitude/100,8,6,temp1));
    }else{
        if (longitude > 0){NSWE = 'E';}else{NSWE='W';}
        sprintf(ret,"%c%s",NSWE,dtostrf(longitude/100,8,5,temp1));
    }
    return ret;
}

