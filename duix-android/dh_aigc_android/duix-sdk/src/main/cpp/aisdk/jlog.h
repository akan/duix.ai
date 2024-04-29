#pragma once
#include <stdio.h>


#ifdef __cplusplus
extern "C" {
#endif


#ifdef LIB_JNI
#include <jni.h>
#include <android/log.h>

#define  LOG_TAG    "tooken"
#define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
#define  LOGE(...)  __android_log_print(ANDROID_LOG_DEBUG,LOG_TAG,__VA_ARGS__)
#define  LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG,LOG_TAG,__VA_ARGS__)


#else

#define  LOGE(...)  printf(__VA_ARGS__)
#define  LOGI(...)  printf(__VA_ARGS__)
#define  LOGD(...)  printf(__VA_ARGS__)

#endif

#ifdef __cplusplus
}
#endif

