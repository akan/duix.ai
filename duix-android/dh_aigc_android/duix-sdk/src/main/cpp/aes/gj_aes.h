#ifndef __GJ_AES_H__
#define __GJ_AES_H__

#include "gj_dll.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef struct gj_aesc_s gj_aesc_t;

GJLIBAPI int free_aesc(gj_aesc_t** paesc);
GJLIBAPI int init_aesc(char* key,char* iv,int enc,gj_aesc_t** paesc);

GJLIBAPI int do_aesc(gj_aesc_t* aesc,char* in,int inlen,char* out,int* outlen);

GJLIBAPI int do_base64(int enc,char* in,int inlen,char* out,int* outlen);

#ifdef __cplusplus
}
#endif

#endif
