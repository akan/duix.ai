#ifndef __GPLAYER_XTICK_H__
#define __GPLAYER_XTICK_H__

#include <stdio.h>
#include <time.h>
#include <stdint.h>

#define ONE_HOUR      (3600 * 1000)
#define ONE_MINUTE    (60 * 1000)
#define ONE_SEC       (1000)

#if defined( _WIN32 )
#include <windows.h>

#ifdef __cplusplus
extern "C" {
#endif

int64_t getTickCount64();
int32_t get_tick_sec();

#define getTickCount()	GetTickCount()

const char*  _get_curtime_str(char* pbuf);

#ifdef __cplusplus
}
#endif

#else

#ifdef __cplusplus
extern "C" {
#endif

#if defined(linux) || defined(APP_IOS)

#include <sys/time.h>
#include <string.h>

#endif

/**
 * 获取当前系统时间：单位毫秒
 * @return
 */
int64_t getTickCount64();

/**
 * 获取当前系统时间： 单位秒
 * @return
 */
int32_t get_tick_sec();

/**
 * 获取当前系统时间：单位毫秒
 * @return
 */
#define getTickCount()    (uint32_t)getTickCount64()

/**
 * 获取当前时间字符串
 * 输出格式为：XXXX-XX-XX XX:XX:XX.XXX 一共占23个字节。
 * @param pbuf 字符串存储buffer
 * @return
 */
const char *_get_curtime_str(char *pbuf);

#ifdef __cplusplus
}
#endif

#endif //_WIN32

#endif //__GPLAYER_XTICK_H__
