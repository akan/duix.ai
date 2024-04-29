#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "gj_aes.h"
#include "aesmain.h"

int mainenc(int enc,char* infn,char* outfn){
    char result[255] ;
    memset(result,0,255);
    char* key = "yymrjzbwyrbjszrk";
    char* aiv = "yymrjzbwyrbjszrk";
    int base64 = 1;
    int outlen = 0;
    int encrst = 0;
    char* fn1 = infn;
    char* fn2 = outfn;
    FILE* fr = fopen(fn1,"rb");
    FILE* fw = fopen(fn2,"wb");
    while(1){
        if(!fr){
            encrst = -1001;
            break;
        }
        if(!fw){
            encrst = -1002;
            break;
        }
        gj_aesc_t* aesc = NULL;
        init_aesc(key,aiv,enc,&aesc);
        uint64_t size = 0;
        uint64_t realsize = 0;
        if(enc){
            fwrite("gjdigits",1,8,fw);
            fwrite(&size,1,8,fw);
            fwrite(&size,1,8,fw);
            fwrite(&size,1,8,fw);

            while(!feof(fr)){
                char data[16];
                memset(data,0,16);
                uint64_t rst = fread(data,1,16,fr);
                if(rst){
                    size +=rst;
                    do_aesc(aesc,data,16,result,&outlen);
                    fwrite(result,1,outlen,fw);
                }
            }
            fseek(fw,8,0);
            fwrite(&size,1,8,fw);

        }else{
            uint64_t rst = fread(result,1,32,fr);
            if(!rst){
                encrst = -1003;
                break;
            }
            if((result[0]!='g')||(result[1]!='j')){
                encrst = -1004;
                break;
            }
            uint64_t *psize = (uint64_t*)(result+8);
            realsize = *psize;
            if(realsize>1034*1024*1024){
                encrst = -1005;
                break;
            }
            while(!feof(fr)){
                char data[16];
                memset(data,0,16);
                uint64_t rst = fread(data,1,16,fr);
                if(rst){
                    size +=rst;
                    do_aesc(aesc,data,16,result,&outlen);
                    if(size>realsize){
                        outlen -= (size-realsize);
                        //printf("===%lu > %lu rst %lu %d outlen \n",size,realsize,rst,outlen);
                    }
                    fwrite(result,1,outlen,fw);
                }
            }
        }
        break;
    }
    if(fr) fclose(fr);
    if(fw) fclose(fw);
    return encrst;
}


#ifdef TEST
int main(int argc,char** argv){
    if(argc<4){
        printf("gaes enc|dec filein fileout\n");
        return 0;
    }
    char k = argv[1][0];
    if(k=='e'){
        int rst =  mainenc(1,argv[2],argv[3]);
        printf("====enc %s to %s rst %d\n",argv[2],argv[3],rst);
        return rst;
    }else if(k=='d'){
        int rst =  mainenc(0,argv[2],argv[3]);
        printf("====dec %s to %s rst %d\n",argv[2],argv[3],rst);
        return rst;
    }else{
        printf("gaes enc|dec filein fileout\n");
        return 0;
    }
}
#endif
