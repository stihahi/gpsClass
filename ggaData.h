//
//  ggaData.h
//  
//
//  Created by 岩井聡 on 2014/08/16.
//
//

#ifndef ____ggaData__
#define ____ggaData__

#include "gpsData.h"

struct GGAData : public gpsData{
    char hour,min,sec;
    float latitude;
    float longitude;
    char quality;
    char satelliteCount;
    float height;
    char* lcdShow(int line);
    char* serialShow(void);
    void parser(char *readData);
};

#endif /* defined(____ggaData__) */
