#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "dh_mem.h"
#include "dh_atomic.h"


/** Defines a reference-counting memory object */
struct mem {
    DH_ATOMIC uint32_t nrefs; /**< Number of references  */
    uint32_t size;         /**< Size of memory object */
    mem_destroy_h *dh;     /**< Destroy handler       */
};


#define STAT_ALLOC(_m, _size) (_m)->size = (uint32_t)(_size);
#define STAT_REALLOC(_m, _size) (_m)->size = (uint32_t)(_size);
#define STAT_DEREF(_m)
#define MAGIC_CHECK(_m)


enum {
#if defined(__x86_64__)
    /* Use 16-byte alignment on x86-x32 as well */
    mem_alignment = 16u,
#else
    mem_alignment = sizeof(void*) >= 8u ? 16u : 8u,
#endif
    alignment_mask = mem_alignment - 1u,
    mem_header_size = (sizeof(struct mem) + alignment_mask) &
        (~(size_t)alignment_mask)
};

#define MEM_SIZE_MAX \
    (size_t)(sizeof(size_t) > sizeof(uint32_t) ? \
            (~(uint32_t)0u) : (~(size_t)0u) - mem_header_size)


static inline struct mem *get_mem(void *p)
{
    return (struct mem *)(void *)(((unsigned char *)p) - mem_header_size);
}


static inline void *get_mem_data(struct mem *m)
{
    return (void *)(((unsigned char *)m) + mem_header_size);
}


/**
 * Allocate a new reference-counted memory object
 *
 * @param size Size of memory object
 * @param dh   Optional destructor, called when destroyed
 *
 * @return Pointer to allocated object
 */
void *mem_alloc(size_t size, mem_destroy_h *dh)
{
    struct mem *m;

    if (size > MEM_SIZE_MAX)
        return NULL;


    m = (struct mem*)malloc(mem_header_size + size);
    if (!m) return NULL;

    re_atomic_rlx_set(&m->nrefs, 1u);
    m->dh    = dh;

    STAT_ALLOC(m, size);

    return get_mem_data(m);
}


/**
 * Allocate a new reference-counted memory object. Memory is zeroed.
 *
 * @param size Size of memory object
 * @param dh   Optional destructor, called when destroyed
 *
 * @return Pointer to allocated object
 */
void *mem_zalloc(size_t size, mem_destroy_h *dh)
{
    void *p;

    p = mem_alloc(size, dh);
    if (!p)
        return NULL;

    memset(p, 0, size);

    return p;
}


/**
 * Re-allocate a reference-counted memory object
 *
 * @param data Memory object
 * @param size New size of memory object
 *
 * @return New pointer to allocated object
 *
 * @note Realloc NULL pointer is not supported
 */
void *mem_realloc(void *data, size_t size)
{
    struct mem *m, *m2;

    if (!data)
        return NULL;

    if (size > MEM_SIZE_MAX)
        return NULL;

    m = get_mem(data);

    MAGIC_CHECK(m);

    if (re_atomic_acq(&m->nrefs) > 1u) {
        void* p = mem_alloc(size, m->dh);
        if (p) {
            memcpy(p, data, m->size);
            mem_deref(data);
        }
        return p;
    }


    m2 = (struct mem*)realloc(m, mem_header_size + size);


    if (!m2) {
        return NULL;
    }

    STAT_REALLOC(m2, size);

    return get_mem_data(m2);
}


/**
 * Re-allocate a reference-counted array
 *
 * @param ptr      Pointer to existing array, NULL to allocate a new array
 * @param nmemb    Number of members in array
 * @param membsize Number of bytes in each member
 * @param dh       Optional destructor, only used when ptr is NULL
 *
 * @return New pointer to allocated array
 */
void *mem_reallocarray(void *ptr, size_t nmemb, size_t membsize,
        mem_destroy_h *dh)
{
    size_t tsize;

    if (membsize && nmemb > MEM_SIZE_MAX / membsize) {
        return NULL;
    }

    tsize = nmemb * membsize;

    if (ptr) {
        return mem_realloc(ptr, tsize);
    }
    else {
        return mem_alloc(tsize, dh);
    }
}


/**
 * Set or unset a destructor for a memory object
 *
 * @param data Memory object
 * @param dh   called when destroyed, NULL for remove
 */
void mem_destructor(void *data, mem_destroy_h *dh)
{
    struct mem *m;

    if (!data)
        return;

    m = get_mem(data);

    MAGIC_CHECK(m);

    m->dh = dh;
}


/**
 * Reference a reference-counted memory object
 *
 * @param data Memory object
 *
 * @return Memory object (same as data)
 */
void *mem_ref(void *data)
{
    struct mem *m;

    if (!data)
        return NULL;

    m = get_mem(data);

    MAGIC_CHECK(m);

    re_atomic_rlx_add(&m->nrefs, 1u);

    return data;
}


/**
 * Dereference a reference-counted memory object. When the reference count
 * is zero, the destroy handler will be called (if present) and the memory
 * will be freed
 *
 * @param data Memory object
 *
 * @return Always NULL
 */
/* coverity[-tainted_data_sink: arg-0] */
void *mem_deref(void *data)
{
    struct mem *m;

    if (!data)
        return NULL;

    m = get_mem(data);

    MAGIC_CHECK(m);

    if (re_atomic_acq_sub(&m->nrefs, 1u) > 1u) {
        return NULL;
    }

    if (m->dh)
        m->dh(data);

    /* NOTE: check if the destructor called mem_ref() */
    if (re_atomic_rlx(&m->nrefs) > 0u)
        return NULL;


    STAT_DEREF(m);

    free(m);

    return NULL;
}


/**
 * Get number of references to a reference-counted memory object
 *
 * @param data Memory object
 *
 * @return Number of references
 */
uint32_t mem_nrefs(const void *data)
{
    struct mem *m;

    if (!data)
        return 0;

    m = get_mem((void*)data);

    MAGIC_CHECK(m);

    return (uint32_t)re_atomic_acq(&m->nrefs);
}




/**
 * Debug all allocated memory objects
 */
void mem_debug(void)
{
}


/**
 * Set the memory allocation threshold. This is only used for debugging
 * and out-of-memory simulation
 *
 * @param n Threshold value
 */
void mem_threshold_set(ssize_t n)
{
    (void)n;
}


/**
 * Print memory status
 *
 * @param pf     Print handler for debug output
 * @param unused Unused parameter
 *
 * @return 0 if success, otherwise errorcode
 */
int mem_status(struct re_printf *pf, void *unused)
{
    (void)pf;
    (void)unused;
    return 0;
}


/**
 * Get memory statistics
 *
 * @param mstat Returned memory statistics
 *
 * @return 0 if success, otherwise errorcode
int mem_get_stat(struct memstat *mstat)
{
    if (!mstat)
        return EINVAL;
    return ENOSYS;
}
 */

