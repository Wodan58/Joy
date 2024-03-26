/*
    module  : gmtime.c
    version : 1.4
    date    : 03/22/24
*/
#ifndef GMTIME_C
#define GMTIME_C

/**
OK 1710  gmtime  :  I  ->  T
Converts a time I into a list T representing universal time:
[year month day hour minute second isdst yearday weekday].
Month is 1 = January ... 12 = December;
isdst is false; weekday is 1 = Monday ... 7 = Sunday.
*/
UNMKTIME(gmtime_, "gmtime", gmtime)
#endif
