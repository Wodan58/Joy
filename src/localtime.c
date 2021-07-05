/*
    module  : localtime.c
    version : 1.1
    date    : 05/21/21
*/
#ifndef LOCALTIME_C
#define LOCALTIME_C

#ifdef _MSC_VER
void localtime_r(time_t *t, struct tm *tm)
{
    *tm = *localtime(t);
}
#endif

/**
1700  localtime  :  I  ->  T
Converts a time I into a list T representing local time:
[year month day hour minute second isdst yearday weekday].
Month is 1 = January ... 12 = December;
isdst is a Boolean flagging daylight savings/summer time;
weekday is 1 = Monday ... 7 = Sunday.
*/
UNMKTIME(localtime_, "localtime", localtime_r)



#endif
