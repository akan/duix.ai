#if defined(_WIN32)
#define _CRT_SECURE_NO_WARNINGS
#endif

#include "Log.h"

#include <stdio.h>
#include <time.h>
#include <stdarg.h>

#ifdef __ANDROID__
#include <android/log.h>
android_LogPriority s_android_logprio[LOG_TRACE + 1] = {
        ANDROID_LOG_UNKNOWN,
        ANDROID_LOG_FATAL,
        ANDROID_LOG_ERROR,
        ANDROID_LOG_WARN,
        ANDROID_LOG_INFO,
        ANDROID_LOG_DEBUG,
        ANDROID_LOG_VERBOSE
};

#endif

#if defined(_WIN32)
#include <windows.h>
#endif

void __log_print(int lv, const char *tag, const char *funame, int line, const char *fmt, ...) {
    char log_info[2040];
    char *buf = log_info;
    int ret, len = sizeof(log_info);

//Android 不需要时间
#ifndef __ANDROID__
    /*
    if (lv <= LogLevel::LOG_INFO) {    // 日志级别不小于INFO则打印时带时间标记
        *buf++ = '[';
        _get_curtime_str(buf);
        //buf = buf + strlen(buf);
        buf += 23;  // 时间格式为：XXXX - XX - XX XX : XX : XX.XXX  共占23个字节
        *buf++ = ']';
        *buf++ = ' ';

        len -= buf - log_info;
    }
    */

    if (lv <= LogLevel::LOG_WARN) {    // 日志级别不小于WARN则打印时带代码行信息
        ret = sprintf(buf, "%s line:%-4d ", funame, line);
        buf += ret;
        len -= ret;
    }
#endif

    va_list arglist;
    va_start(arglist, fmt);

    int itemLen = buf - log_info;
#if defined( WIN32 )
    ret = _vsnprintf(buf, len - 1, fmt, arglist);
#else
    ret = vsnprintf(buf, len - 1, fmt, arglist);
#endif
    if (ret < 0) {
        buf[len - 1] = 0;
        buf[len - 2] = '\n';
        itemLen += len - 1;
    } else
        itemLen += ret;

    va_end(arglist);

#if defined(__ANDROID__)
    __android_log_print(s_android_logprio[lv], tag, log_info, "");
#else
    //本地输出
    //printf("Tag=%s %s\n", tag, log_info);
#endif
}
