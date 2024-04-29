#ifndef __GPLAYER_LOG_H__
#define __GPLAYER_LOG_H__

#ifdef __cplusplus
extern "C" {
#endif

//调试日志开关，1为开，其它为关
#define LOG_OPEN 0

enum LogLevel
{
    LOG_OFF    = 0,		        //!< 不打印日志
    LOG_FATAL  = 1,	 	        //!< 严重
    LOG_ERROR  = 2,				//!< 错误
    LOG_WARN   = 3,				//!< 警告
    LOG_INFO   = 4,				//!< 信息
    LOG_DEBUG  = 5,				//!< 调试
    LOG_TRACE  = 6,				//!< 跟踪
};

void __log_print(int lv, const char* tag, const char* funame, int line, const char *fmt, ...);

#define LOGI(TAG, ...)  __log_print(LogLevel::LOG_INFO,  TAG, __FUNCTION__, __LINE__, __VA_ARGS__)
#define LOGW(TAG, ...)  __log_print(LogLevel::LOG_WARN,  TAG, __FUNCTION__, __LINE__, __VA_ARGS__)
#define LOGE(TAG, ...)  __log_print(LogLevel::LOG_ERROR, TAG, __FUNCTION__, __LINE__, __VA_ARGS__)
#define LOGF(TAG, ...)  __log_print(LogLevel::LOG_FATAL, TAG, __FUNCTION__, __LINE__, __VA_ARGS__)

#if defined(__ANDROID__)
#if(LOG_OPEN == 1)
#define LOGD(TAG,...)  __log_print(LogLevel::LOG_DEBUG, TAG, __FUNCTION__, __LINE__, __VA_ARGS__)
#else
#define LOGD(TAG, ...)  NULL
#endif
#else
#define LOGD(TAG, ...)  __log_print(LogLevel::LOG_DEBUG, TAG, __FUNCTION__, __LINE__, __VA_ARGS__)
#endif

#ifdef __cplusplus
};
#endif

#endif // !__GPLAYER_LOG_H__
