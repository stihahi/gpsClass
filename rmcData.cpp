//
//  rmcData.cpp
//  
//
//  Created by 岩井聡 on 2014/08/16.
//
//

#include "rmcData.h"


char* RMCData::serialShow(void){
    char ret[128];
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
    char ret[32];
    char NSWE;
    char temp1[12];
    if (line == 0) {
        if (latitude >0) {NSWE = 'N';}else{NSWE='S';}
        sprintf(ret,"%c%s",NSWE,dtostrf(latitude,8,6,temp1));
    }else{
        if (longitude > 0){NSWE = 'E';}else{NSWE='W';}
        sprintf(ret,"%c%s",NSWE,dtostrf(longitude,9,6,temp1));
    }
    return ret;
}

void RMCData::parser(char *readData){
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
                hour = CTOI(nowPhrase[0])*10 + CTOI(nowPhrase[1]);
                min = CTOI(nowPhrase[2])*10 + CTOI(nowPhrase[3]);
                sec = CTOI(nowPhrase[4])*10 + CTOI(nowPhrase[5]);
                break;
            case 2://A	＝　ステータス；A = 有効，V = 無効
                if (nowPhrase[0] == 'A') {
                    status = true;
                }else{
                    status = false;
                }
                break;
            case 3://4916.452653,N	＝　緯度　49度16.452653分（北緯）
                latitude = atof(nowPhrase) / 100;
                break;
            case 4:
                if (nowPhrase[0] == 'S') {
                    latitude = -latitude;
                }
                break;
            case 5://12311.123747,W	＝　経度　123度11.123747分（西経）
                longitude = atof(nowPhrase) / 100;
                break;
            case 6:
                if (nowPhrase[0] == 'W') {
                    longitude = -longitude;
                }
                break;
            case 7://000.5	＝　対地速度（ノット）　0.5ノット
                knot = atof(nowPhrase);
                break;
            case 8://054.7	＝　進行方向（度，真北）　54.7度
                heading = atoi(nowPhrase);
                break;
            default:
                break;
        }
        dataNum ++;
        readData += strlen(nowPhrase)+1;
    }while (strlen(nowPhrase) != 0);

}

