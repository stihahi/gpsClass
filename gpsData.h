//
//  gpsData.h
//  
//
//  Created by 岩井聡 on 2014/08/16.
//
//

#ifndef _gpsData_h
#define _gpsData_h
#define CTOI(A) (A-'0')
#include <Arduino.h>

enum NMEA0183{NMEA_OTHER,NMEA_RMC,NMEA_ZDA,NMEA_GGA};

class gpsData{
public:
    virtual char* lcdShow(int line)=0;//データを整形して返すメソッド。画面表示用。
    virtual char* serialShow(void)=0;
    virtual void parser(char *readData)=0;
    char* readNextData(char *rawData){
        char retPhrase[32];
        int index = 0;
        while(*rawData != ',' && *rawData != 0){
            retPhrase[index++] = *rawData++;
        }
        retPhrase[index] = 0;
        return retPhrase;
    }//次のデータを見つけて、返す。最後のdataPhraseだったら、""を返す.ポインタ位置を変更するよ。
};


#endif
