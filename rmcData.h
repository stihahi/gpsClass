//
//  rmcData.h
//  
//
//  Created by 岩井聡 on 2014/08/16.
//
//

#ifndef ____rmcData__
#define ____rmcData__

#include <iostream>
#include "gpsData.h"

struct RMCData : public gpsData{
    char hour,min,sec;
    char day,month,year;
    bool status;
    float latitude;
    float longitude;
    float knot;
    int heading;
    char* lcdShow(int line);//データを整形して返すメソッド。画面表示用。
    char* serialShow(void);//データを整形してシリアルポートに返す。
    void parser(char *readData);
    RMCData(){status = false;}
};


#endif /* defined(____rmcData__) */
