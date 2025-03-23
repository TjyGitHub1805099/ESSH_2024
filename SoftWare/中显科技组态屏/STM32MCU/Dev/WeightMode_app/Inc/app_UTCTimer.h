#ifndef _UTCTIMER_H
#define _UTCTIMER_H
#include "typedefine.h"
#include <string.h>

extern struct tm gUTCDecodeTime;
extern sint64 gS64UTCTime;
extern struct tm localtm;

extern struct tm *mygmtime(const sint64 *timep);
extern sint64 mymktime(struct tm *tm);
extern void RTC_MainFunction(uint32 ticks);

#endif
