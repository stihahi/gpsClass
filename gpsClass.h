//
//  gpsClass.h
//
//
//  Created by 岩井聡 on 2014/07/28.
//
//

#ifndef ____gpsClass__
#define ____gpsClass__

#include <SoftwareSerial.h>
#include "rmcData.h"
#include "ggaData.h"
#include "zdaData.h"
#include "gpsData.h"




class gpsClass : public SoftwareSerial{
    int reloadSec;
    void auto_detect_baud_rate(void);
    void send_pmtk_packet(char *p);
    int headerParser(char *rawData);//データの種類だけparseする。rawDataのポインタ位置変更はしない。
    RMCData rmcD;
    ZDAData zdaD;
    GGAData ggaD;
public:
    gpsClass(int a,int b) : SoftwareSerial(a,b){
        reloadSec = 1;//default 1 sec
    };
    void serialSetup(void);
    void readData(void);
    bool gpsFetch(void);
    bool parser(char *readData);
    void RMCParser(char *readData,RMCData &rmc);
    void ZDAParser(char *readData,ZDAData &zda);
    void GGAParser(char *readData,GGAData &gga);
    char* getLCD(int mode,int line);
};




#endif /* defined(____gpsClass__) */
