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

enum NMEA0183{NMEA_OTHER,NMEA_RMC,NMEA_ZDA,NMEA_GGA};

class gpsData{
public:
    char* lcdShow(int line);//データを整形して返すメソッド。画面表示用。
    char* serialShow(void);
};

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
    RMCData(){status = false;};
};

struct ZDAData : public gpsData{
    int year;
    char month,day,hour,min,sec;
    char *lcdShow(int line);
    char *serialShow(void);
};

struct GGAData : public gpsData{
    
};

class gpsClass : public SoftwareSerial{
    int reloadSec;
    void auto_detect_baud_rate(void);
    void send_pmtk_packet(char *p);
    int headerParser(char *rawData);//データの種類だけparseする。rawDataのポインタ位置変更はしない。
    char* readNextData(char *rawData);//次のデータを見つけて、返す。最後のdataPhraseだったら、""を返す.ポインタ位置を変更するよ。
    RMCData rmcD;
    ZDAData zdaD;
public:
    gpsClass(int a,int b) : SoftwareSerial(a,b){
        reloadSec = 1;//default 1 sec
    };
    void serialSetup(void);
    void readData(void);
    bool gpsFetch(void);
    bool parser(char *readData);
    void RMCParser(char *readData,RMCData &rmc);
    void ZDAParser(char *readData,ZDAData &rmc);
    char* getLCD(int mode,int line);
};




#endif /* defined(____gpsClass__) */
