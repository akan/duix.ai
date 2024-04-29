#if defined(_WIN32)
#define _CRT_SECURE_NO_WARNINGS
#endif

#include "XTick.h"
#include <sys/time.h>

#if defined( _WIN32 )

int64_t getTickCount64()
{
    LARGE_INTEGER TicksPerSecond = { 0 };
    LARGE_INTEGER Tick;
    if (!TicksPerSecond.QuadPart)
        QueryPerformanceFrequency(&TicksPerSecond);
    QueryPerformanceCounter(&Tick);
    __int64 Seconds = Tick.QuadPart / TicksPerSecond.QuadPart;
    __int64 LeftPart = Tick.QuadPart - (TicksPerSecond.QuadPart*Seconds);
    __int64 MillSeconds = LeftPart * 1000 / TicksPerSecond.QuadPart;
    __int64 Ret = Seconds * 1000 + MillSeconds;
    return Ret;
};

int32_t get_tick_sec()
{
    LARGE_INTEGER TicksPerSecond = { 0 };
    LARGE_INTEGER Tick;
    if (!TicksPerSecond.QuadPart)
        QueryPerformanceFrequency(&TicksPerSecond);
    QueryPerformanceCounter(&Tick);
    return  Tick.QuadPart / TicksPerSecond.QuadPart;
}

#define getTickCount()	GetTickCount()

/*!
    \brief	WIN32平台获取当前时间，输出格式为：XXXX-XX-XX XX:XX:XX.XXX 一共占23个字节。
*/
const char*  _get_curtime_str(char* pbuf)
{
    SYSTEMTIME	stm;
    GetLocalTime(&stm);
    sprintf(pbuf, "%04d-%02d-%02d %02d:%02d:%02d.%03d",
        stm.wYear, stm.wMonth, stm.wDay,
        stm.wHour, stm.wMinute, stm.wSecond,
        stm.wMilliseconds);
    return pbuf;
}

#elif defined(__APPLE__)

int64_t getTickCount64()
{
    struct timeval tv;
    gettimeofday(&tv,NULL);

    return ((int64_t)(tv.tv_usec/1000) + (int64_t)(tv.tv_sec*1000));
}

int32_t get_tick_sec()
{
    struct timeval tv;
    gettimeofday(&tv,NULL);
    return tv.tv_sec;
}

#define getTickCount()	(uint32_t)getTickCount64()

/*!
    \brief  非WIN32平台获取当前时间，输出格式为：XXXX-XX-XX XX:XX:XX.XXX 一共占23个字节。
*/
const char*  _get_curtime_str(char* pbuf)
{
    struct timeval	tv;
    struct tm		tmv;
    time_t tmt;

    gettimeofday(&tv, NULL);
#ifdef APP_ENVIRONMENT
    tmt = tv.tv_sec + 8 * 60 * 60;
#else
    tmt = tv.tv_sec;
#endif
    gmtime_r(&tmt, &tmv);

    sprintf(pbuf, "%04d-%02d-%02d %02d:%02d:%02d.%03d",
        tmv.tm_year + 1900, tmv.tm_mon + 1, tmv.tm_mday,
        tmv.tm_hour, tmv.tm_min, tmv.tm_sec,
        tv.tv_usec / 1000);
    return pbuf;
}

#else

int64_t getTickCount64() {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);    //此处可以判断一下返回值
    return ((int64_t) ts.tv_sec * 1000 + ts.tv_nsec / (1000 * 1000));
}

int32_t get_tick_sec() {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);    //此处可以判断一下返回值
    return ts.tv_sec;
}

#define getTickCount()    (uint32_t)getTickCount64()

/*!
	\brief  非WIN32平台获取当前时间，输出格式为：XXXX-XX-XX XX:XX:XX.XXX 一共占23个字节。
*/
const char *_get_curtime_str(char *pbuf) {
    struct timeval tv;
    struct tm tmv;
    time_t tmt;

    gettimeofday(&tv, NULL);
#ifdef APP_ENVIRONMENT
    tmt = tv.tv_sec + 8 * 60 * 60;
#else
    tmt = tv.tv_sec;
#endif
    gmtime_r(&tmt, &tmv);

    sprintf(pbuf, "%04d-%02d-%02d %02d:%02d:%02d.%03ld",
            tmv.tm_year + 1900, tmv.tm_mon + 1, tmv.tm_mday,
            tmv.tm_hour, tmv.tm_min, tmv.tm_sec,
            tv.tv_usec / 1000);
    return pbuf;
}

#endif
