#ifndef _UTCTIMER_H
#define _UTCTIMER_H
#include "typedefine.h"
#include <string.h>

extern struct tm *mygmtime(const sint64 *timep);
extern sint64 mymktime(struct tm *tm);

#endif
