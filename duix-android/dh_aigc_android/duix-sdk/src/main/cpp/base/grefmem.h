#ifndef GUIJI_REFCOUNT_H
#define GUIJI_REFCOUNT_H

#include <stdio.h>
#include <stdint.h>
#include <pthread.h>
#include <string.h>

#if defined(_WIN32)
#include <windows.h>
#else
#include "dh_mem.h"
#endif
#ifdef __cplusplus
extern "C" {
#endif

#if defined(_WIN32)

#define BILLION  (1E9)
static BOOL g_first_time = 1;
static LARGE_INTEGER g_counts_per_sec;

static inline uint64_t timer_msstamp() {
    struct timespec ts;
    struct timespec *ct = &ts;
    LARGE_INTEGER count;
    if (g_first_time) {
        g_first_time = 0;
        if (0 == QueryPerformanceFrequency(&g_counts_per_sec)) {
            g_counts_per_sec.QuadPart = 0;
        }
    }
    if ((NULL == ct) || (g_counts_per_sec.QuadPart <= 0) || (0 == QueryPerformanceCounter(&count))) {
        return -1;
    }
    ct->tv_sec = count.QuadPart / g_counts_per_sec.QuadPart;
    ct->tv_nsec = ((count.QuadPart % g_counts_per_sec.QuadPart) * BILLION) / g_counts_per_sec.QuadPart;
    return (ts.tv_sec*1000l) + (ts.tv_nsec/CLOCKS_PER_SEC);
}
#else
static inline uint64_t timer_msstamp() {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (ts.tv_sec*1000l) + (ts.tv_nsec/CLOCKS_PER_SEC);
}
#endif

#define gjrefobj_alloc(memrefp, memsize , free_fn) { \
    *((char**)&memrefp) = (char*)mem_alloc(memsize,free_fn);\
}

#define gjrefobj_ref(memrefp) { \
    mem_ref(memrefp);\
}

#define gjrefobj_unref(memrefp) { \
    mem_deref(memrefp);\
}

#ifdef __cplusplus
}
#endif
#endif
