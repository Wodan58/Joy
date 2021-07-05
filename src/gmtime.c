/*
    module  : gmtime.c
    version : 1.1
    date    : 05/21/21
*/
#ifndef GMTIME_C
#define GMTIME_C

#ifdef _MSC_VER
void gmtime_r(time_t *t, struct tm *tm)
{
    *tm = *gmtime(t);
}
#endif

/**
1710  gmtime  :  I  ->  T
Converts a time I into a list T representing universal time:
[year month day hour minute second isdst yearday weekday].
Month is 1 = January ... 12 = December;
isdst is false; weekday is 1 = Monday ... 7 = Sunday.
*/
UNMKTIME(gmtime_, "gmtime", gmtime_r)



#endif
