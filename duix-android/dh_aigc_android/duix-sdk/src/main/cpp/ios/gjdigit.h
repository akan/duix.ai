#ifndef GJDIGIT
#define GJDIGIT


#ifdef __cplusplus
extern "C"{
#endif

typedef struct gjdigit_s gjdigit_t;

int gjdigit_alloc(gjdigit_t** pdg);

int gjdigit_initWenet(gjdigit_t* dg,char* fnwenet);
int gjdigit_initMunet(gjdigit_t* dg,char* fnparam,char* fnbin,char* fnmsk);
int gjdigit_initMalpha(gjdigit_t* dg,char* fnparam,char* fnbin);
int gjdigit_onewav(gjdigit_t* dg,const char* wavfn);

//int gjdigit_picrst(gjdigit_t* dg,const char* picfn,int* box,int index);

int gjdigit_matrst(gjdigit_t* dg,uint8_t* buf,int width,int height,int* box,int index);

int gjdigit_maskrst(gjdigit_t* dg,uint8_t* bpic,int width,int height,int* box,uint8_t* bmsk,uint8_t* bfg,uint8_t* bbg,int index);

int gjdigit_processmd5(gjdigit_t* dg,int enc,const char* infn,const char* outfn);

int gjdigit_startgpg(gjdigit_t* dg,const char* infn,const char* outfn);
int gjdigit_stop(gjdigit_t* dg);

int gjdigit_free(gjdigit_t** pdg);
int gjdigit_test(gjdigit_t* dg);



#ifdef __cplusplus
}
#endif

#endif
