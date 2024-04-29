#ifndef __GJ_DLL_H__
#define __GJ_DLL_H__

#ifdef __cplusplus
extern "C" {
#endif
#define GJLIB_EXPORT 1
#if defined(GJLIB_EXPORT)
    #if defined _WIN32 || defined __CYGWIN__
        #define GJLIBAPI __declspec(dllexport)
    #else
        #define GJLIBAPI __attribute__((visibility("default")))
    #endif
#else
    #define GJLIBAPI
#endif

#ifdef __cplusplus
}
#endif
#endif
