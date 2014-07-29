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
#define CTOI(A) (A-'0')


struct RMCData{
    char hour,min,sec;
    char day,month,year;
    bool status;
    float latitude;
    float longitude;
    float knot;
    int heading;
    char* show(void);
};

class gpsClass : public SoftwareSerial{
    int reloadSec;
    void auto_detect_baud_rate(void);
    void send_pmtk_packet(char *p);
public:
    gpsClass(int a,int b) : SoftwareSerial(a,b){
        reloadSec = 1;//default 5 sec
    };
    void serialSetup(void);
    void readData(void);
    char* gpsFetch(void);
    void parser(float &latitude,float &longitude,char *readData);
    void RMCParser(char *readData,RMCData &rmc);
};




#endif /* defined(____gpsClass__) */
