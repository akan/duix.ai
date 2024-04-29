#ifndef DH_H_MEM_
#define DH_H_MEM_

#include "dh_types.h"

#ifdef __cplusplus
extern "C" {
#endif
    typedef void (mem_destroy_h)(void *data);

    void    *mem_alloc(size_t size, mem_destroy_h *dh);
    void    *mem_zalloc(size_t size, mem_destroy_h *dh);
    void    *mem_realloc(void *data, size_t size);
    void    *mem_reallocarray(void *ptr, size_t nmemb,
            size_t membsize, mem_destroy_h *dh);
    void     mem_destructor(void *data, mem_destroy_h *dh);
    void    *mem_ref(void *data);
    void    *mem_deref(void *data);
    uint32_t mem_nrefs(const void *data);

    void     mem_debug(void);
    void     mem_threshold_set(ssize_t n);
    struct re_printf;
    int      mem_status(struct re_printf *pf, void *unused);
    //int      mem_get_stat(struct memstat *mstat);


    /* Secure memory functions */
    int mem_seccmp(const uint8_t *s1, const uint8_t *s2, size_t n);
    void mem_secclean(void *data, size_t size);

#ifdef __cplusplus
}
#endif

#endif
