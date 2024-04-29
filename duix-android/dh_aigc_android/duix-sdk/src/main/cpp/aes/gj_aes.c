#include <stdlib.h>
#include <string.h>
#include "gj_aes.h"
#include "base64.h"

#include "aes.h"


struct gj_aesc_s{
    char key[16];
    char iv[16];
    int enc;
	AES_KEY *aeskey;
};

int free_aesc(gj_aesc_t** paesc){
    if(!paesc||!*paesc)return -1;
    if((*paesc)->aeskey)free((*paesc)->aeskey);
    free(*paesc);
    *paesc = NULL;
    return 0;
}


int init_aesc(char* key,char* iv,int enc,gj_aesc_t** paesc){
    if(strlen(key)!=16) return -1;
    if(strlen(iv)!=16) return -2;
    gj_aesc_t* aesc = (gj_aesc_t*)malloc(sizeof(gj_aesc_t));
    int k;
    for(k=0;k<16;k++){
        aesc->key[k]=key[k];
        aesc->iv[k]=iv[k];
    }
    aesc->aeskey = (AES_KEY*)malloc(sizeof(AES_KEY));
    aesc->enc = enc;
    if(enc){
	    AES_set_encrypt_key((const unsigned char*)aesc->key, 128, aesc->aeskey);
    }else{
	    AES_set_decrypt_key((const unsigned char*)aesc->key, 128, aesc->aeskey);
    }
    *paesc = aesc;
    return 0;
}

int do_aesc(gj_aesc_t* aesc,char* in,int inlen,char* out,int* outlen){
    char* psrc = in;
    char* pdest = out;
    int cnt = 0;
    int left=inlen;
    while(left>0){
	    AES_cbc_encrypt((const unsigned char*)psrc,(unsigned char*)pdest,16,aesc->aeskey,(unsigned char*)aesc->iv,aesc->enc);
        psrc += 16;
        pdest += 16;
        left -= 16;
        cnt += 16;
    }
    *outlen = cnt;
    return 0;
}

int do_base64(int enc,char* in,int inlen,char* out,int* outlen){
    if(enc){
        gjbase64_encode((unsigned char*)in,inlen,out);
        *outlen = strlen(out);
    }else{
        *outlen = gjbase64_decode(in,inlen,(unsigned char*)out);
    }
    return 0;
}
