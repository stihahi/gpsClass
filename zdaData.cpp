//
//  zdaData.cpp
//  
//
//  Created by 岩井聡 on 2014/08/16.
//
//

#include "zdaData.h"

char* ZDAData::lcdShow(int line){
    char ret[32];
    if (line == 0) {
        sprintf(ret,"%d/%02d/%02d",year,month,day);
    }else{
        sprintf(ret,"%2d:%02d:%02d",hour,min,sec);
    }
    return ret;
}

char* ZDAData::serialShow(void){
    return "";
}

void ZDAData::parser(char *readData){
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
                hour = CTOI(nowPhrase[0]) * 10 + CTOI(nowPhrase[1]);
                min  = CTOI(nowPhrase[2]) * 10 + CTOI(nowPhrase[3]);
                sec  = CTOI(nowPhrase[4]) * 10 + CTOI(nowPhrase[5]);
                break;
            case 2:
                day = atoi(nowPhrase);
                break;
            case 3:
                month = atoi(nowPhrase);
                break;
            case 4:
                year = atoi(nowPhrase);
                break;
            default:
                break;
        }
        dataNum ++;
        readData += strlen(nowPhrase)+1;
    }while (strlen(nowPhrase) != 0);
}
