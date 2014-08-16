//
//  ggaData.cpp
//  
//
//  Created by 岩井聡 on 2014/08/16.
//
//

#include "ggaData.h"
#include <Arduino.h>


void GGAData::parser(char *readData){
    char *nowPhrase;
    int index = 0;
    int dataNum = 0;
    do{
        nowPhrase = readNextData(readData);
        switch (dataNum) {
            case 0:
                if (strcmp(nowPhrase,"GPGGA") != 0) {
                    return;
                }
                break;
            case 1://123519.00	＝　測位時刻（UTC）　12:35:19.00
                hour = CTOI(nowPhrase[0]) * 10 + CTOI(nowPhrase[1]);
                min  = CTOI(nowPhrase[2]) * 10 + CTOI(nowPhrase[3]);
                sec  = CTOI(nowPhrase[4]) * 10 + CTOI(nowPhrase[5]);
                break;
            case 2://4807.038247,N	＝　緯度　48度07.038247分（北緯）
                latitude = atof(nowPhrase)/100;
                break;
            case 3:
                if (nowPhrase[0] == 'S') {latitude = -latitude;}
                break;
            case 4://01131.324523,E	＝　経度　11度31.324523分（東経）
                longitude = atof(nowPhrase)/100;
                break;
            case 5:
                if (nowPhrase[0] == 'W') {longitude = -longitude;}
                break;
            case 6://1	＝　GPSのクオリティ； 0 = 受信不能， 1 = 単独測位，2 = DGPS
                quality = CTOI(nowPhrase[0]);
                break;
            case 7://08	＝　受信衛星数
                satelliteCount = atoi(nowPhrase);
                break;
            case 11://46.93, M	＝　WGS-84楕円体から平均海水面の高度差（m）
                height = atof(nowPhrase);
                break;
            default:
                break;
        }
        dataNum ++;
        readData += strlen(nowPhrase)+1;
    }while (strlen(nowPhrase) != 0);
}

char* GGAData::serialShow(void){
    char ret[128];
    char NS;
    char WE;
    char temp1[12],temp2[12],temp3[12];
    if (latitude >0) {NS = 'N';}else{NS='S';}
    if (longitude > 0){WE = 'E';}else{WE='W';}
    sprintf(ret,"%02d:%02d:%02d  %c%s %c%s height:%s satellite:%d",hour,min,sec,NS,dtostrf(latitude,9,6,temp1),WE,dtostrf(longitude,10,6,temp2),dtostrf(height,8,2,temp3),satelliteCount);
    return ret;
}

char *GGAData::lcdShow(int line){
    return "Hello World";
}
