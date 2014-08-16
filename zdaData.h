//
//  zdaData.h
//  
//
//  Created by 岩井聡 on 2014/08/16.
//
//

#ifndef ____zdaData__
#define ____zdaData__

#include "gpsData.h"

struct ZDAData : public gpsData{
    int year;
    char month,day,hour,min,sec;
    char *lcdShow(int line);
    char *serialShow(void);
    void parser(char *readData);
};

#endif /* defined(____zdaData__) */
