/**
 *  Copyright (C) 2011-2012  Juho Vähä-Herttua
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 */

#ifndef COMPAT_H
#define COMPAT_H

#if defined(WIN32)
#include <ws2tcpip.h>
#include <windows.h>
#ifndef snprintf
#define snprintf _snprintf
#endif
#else
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>
#include <netdb.h>
#include <pthread.h>
#endif

#include "memalign.h"
#include "sockets.h"
#include "threads.h"


#ifdef WIN32
#define strdup(A)               _strdup(A)
extern const __int64 DELTA_EPOCH_IN_MICROSECS;

/* IN UNIX the use of the timezone struct is obsolete;
 I don't know why you use it. See http://linux.about.com/od/commands/l/blcmdl2_gettime.htm
 But if you want to use this structure to know about GMT(UTC) diffrence from your local time
 it will be next: tz_minuteswest is the real diffrence in minutes from GMT(UTC) and a tz_dsttime is a flag
 indicates whether daylight is now in use
*/
struct timezone
{
    __int32  tz_minuteswest; /* minutes W of Greenwich */
    BOOL  tz_dsttime;     /* type of dst correction */
};
//
//struct timeval2 {
//    __int32    tv_sec;         /* seconds */
//    __int32    tv_usec;        /* microseconds */
//};

inline int gettimeofday(struct timeval* tv/*in*/, struct timezone* tz/*in*/)
{
    FILETIME ft;
    __int64 tmpres = 0;
    TIME_ZONE_INFORMATION tz_winapi;
    int rez = 0;

    ZeroMemory(&ft, sizeof(ft));
    ZeroMemory(&tz_winapi, sizeof(tz_winapi));

    GetSystemTimeAsFileTime(&ft);

    tmpres = ft.dwHighDateTime;
    tmpres <<= 32;
    tmpres |= ft.dwLowDateTime;

    /*converting file time to unix epoch*/
    tmpres /= 10;  /*convert into microseconds*/
    tmpres -= DELTA_EPOCH_IN_MICROSECS;
    tv->tv_sec = (__int32)(tmpres * 0.000001);
    tv->tv_usec = (tmpres % 1000000);


    //_tzset(),don't work properly, so we use GetTimeZoneInformation
    rez = GetTimeZoneInformation(&tz_winapi);
    if (tz != NULL) {
        tz->tz_dsttime = (rez == 2) ? TRUE : FALSE;
        tz->tz_minuteswest = tz_winapi.Bias + ((rez == 2) ? tz_winapi.DaylightBias : 0);
    }

    return 0;
}
#endif // WIN32

#endif
