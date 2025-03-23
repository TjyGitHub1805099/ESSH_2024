/**********************************************************************************************************************
 *  FILE DESCRIPTION
 *  -------------------------------------------------------------------------------------------------------------------
 *             File:  app_UTCTimer.c
 *        SW-C Type:  IIC
 *  Generation Time:  2024-09-02 11:58:15
 *
 *      Description:  C-Code implementation template for SW-C <IIC>
 *********************************************************************************************************************/


/**********************************************************************************************************************
 * DO NOT CHANGE THIS COMMENT!           << Start of version logging area >>                DO NOT CHANGE THIS COMMENT!
 *********************************************************************************************************************/

/* PRQA S 0777, 0779 EOF */ /* MD_MSR_Rule5.1, MD_MSR_Rule5.2 */

/**********************************************************************************************************************
 * DO NOT CHANGE THIS COMMENT!           << End of version logging area >>                  DO NOT CHANGE THIS COMMENT!
 *********************************************************************************************************************/


/**********************************************************************************************************************
 * DO NOT CHANGE THIS COMMENT!           << Start of include and declaration area >>        DO NOT CHANGE THIS COMMENT!
 *********************************************************************************************************************/
#include "app_UTCTimer.h"
#include "time.h"
/**********************************************************************************************************************
 * DO NOT CHANGE THIS COMMENT!           << End of include and declaration area >>          DO NOT CHANGE THIS COMMENT!
 *********************************************************************************************************************/
static const int days_in_month[2][12] = {
    {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31},
    {31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31}
};
struct tm gUTCDecodeTime;
sint64 gS64UTCTime = 0xFFFFFFFF;//0x66B40C68;

static int is_leap_year(int year)
{
    return ((year % 4 == 0) && (year % 100 != 0)) || (year % 400 == 0);
}

static int get_days_in_month(int year, int month)
{
    return days_in_month[is_leap_year(year)][month];
}

/***********************************************************************
 * @brief       : check Results, The flag is cleared after reading.
 * @param       : 
 * @retval      : 
 ***********************************************************************/
struct tm *mygmtime(const sint64 *timep)
{
    static struct tm tm;
    sint64 t = *timep;
    //int days;
	int year, month, time_ymd;

    tm.tm_sec = t % 60;
    t /= 60;
    tm.tm_min = t % 60;
    t /= 60;
    tm.tm_hour = t % 24;
    t /= 24;

    tm.tm_wday = (t + 4) % 7;
    if (tm.tm_wday < 0) {
        tm.tm_wday += 7;
    }
    time_ymd = (int)t;
    year = 1970;
    //days = 0;

    while(time_ymd >= (is_leap_year(year) ? 366 : 365)){
        time_ymd -=(is_leap_year(year) ? 366 : 365);
        year++;
    }

    tm.tm_year = year;
    tm.tm_yday = time_ymd;
    month = 0;
    while (time_ymd >= get_days_in_month(year, month)) {
        time_ymd -= get_days_in_month(year, month);
        month++;
    }

    tm.tm_mon = month + 1;
    tm.tm_mday = time_ymd + 1;
    tm.tm_isdst = -1;

    return &tm;
}
/***********************************************************************
 * @brief       : check Results, The flag is cleared after reading.
 * @param       : 
 * @retval      : 
 ***********************************************************************/
sint64 mymktime(struct tm *tm)
{
    int year = tm->tm_year;
    int month = tm->tm_mon;
    int day = tm->tm_mday;
    int hour = tm->tm_hour;
    int min = tm->tm_min;
    int sec = tm->tm_sec;
    int y = 0;
    int m = 0;
    sint64 result = 0;

    // Check for invalid input
    if (year < 1970 || month < 1 || month > 12 || day < 1 || day > 31 || hour < 0 || hour > 23 || min < 0 || min > 59 || sec < 0 || sec > 59) {
        return -1;
    }

    // Compute the number of seconds since the epoch

    for ( y = 1970; y < year; y++) {
        if(is_leap_year(y)){
            result +=366 * 24 * 60 * 60;
        }else{
            result +=365 * 24 * 60 * 60;
        }
    }

    for ( m = 1; m < month; m++) {
        result +=get_days_in_month(year,m-1) *24*60*60;
    }

    result += (day - 1) *24*60*60;
    result += hour * 3600;
    result += min * 60;
    result += sec;

    return result;
}


void RTC_MainFunction(uint32 ticks)
{
    if((ticks % 1000 == 0) && (0xFFFFFFFF != gS64UTCTime))
	{
		gS64UTCTime++;
		gUTCDecodeTime = *(mygmtime(&gS64UTCTime));
	} 
}




/**********************************************************************************************************************
 * DO NOT CHANGE THIS COMMENT!           << End of documentation area >>                    DO NOT CHANGE THIS COMMENT!
 *********************************************************************************************************************/


/**********************************************************************************************************************
 MISRA 2012 violations and justifications
 *********************************************************************************************************************/

/* module specific MISRA deviations:
   MD_Rte_0624:  MISRA rule: Rule8.3
     Reason:     This MISRA violation is a consequence from the RTE requirements [SWS_Rte_01007] [SWS_Rte_01150].
                 The typedefs are never used in the same context.
     Risk:       No functional risk. Only a cast to uint8* is performed.
     Prevention: Not required.

   MD_Rte_3206:  MISRA rule: Rule2.7
     Reason:     The parameter are not used by the code in all possible code variants.
     Risk:       No functional risk.
     Prevention: Not required.

*/
