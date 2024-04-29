#include <stdio.h>
#include <stdlib.h>
#include <stdio.h>
#include <memory>
#include <vector>
#include <string.h>
#include <mutex>

//#include "NumCpp.hpp"
#include "gjdigit.h"
#include "jmat.h"
#include "wenet.h"
#include "munet.h"
#include "malpha.h"
#include "blendgram.h"
#include  "wavcache.h"
#include "aesmain.h"

struct gjdigit_s{
    Wenet       *ai_wenet;
    Mobunet     *ai_munet;
    MAlpha      *ai_malpha;
    //JMat        *mat_wenet;
    std::mutex  *lock_munet;
    std::mutex  *lock_wenet;
    MBnfCache   *bnf_cache;
    int         cnt_wenet;
    int         inx_wenet;
    JMat*       mat_gpg;
};

int gjdigit_alloc(gjdigit_t** pdg){
    gjdigit_t* dg = (gjdigit_t*)malloc(sizeof(gjdigit_t));
    memset(dg,0,sizeof(gjdigit_t));
    dg->lock_munet = new std::mutex();
    dg->lock_wenet = new std::mutex();
    dg->bnf_cache = new MBnfCache();
    *pdg = dg;
    return 0;
}

int gjdigit_initWenet(gjdigit_t* dg,char* fnwenet){
    dg->lock_wenet->lock();
    if(dg->ai_wenet){
        delete dg->ai_wenet;
        dg->ai_wenet = NULL;
    }
    dg->ai_wenet = new Wenet(fnwenet);
    dg->lock_wenet->unlock();
    return 0;
}

int gjdigit_initMalpha(gjdigit_t* dg,char* fnparam,char* fnbin){
    if(1)return 0;
    if(dg->ai_malpha){
        delete dg->ai_malpha;
        dg->ai_malpha = NULL;
    }
    std::string fb(fnbin);
    std::string fp(fnparam);
    dg->ai_malpha = new MAlpha(fb.c_str(),fp.c_str());
    return 0;
}

int gjdigit_initMunet(gjdigit_t* dg,char* fnparam,char* fnbin,char* fnmsk){
    dg->lock_munet->lock();
    if(dg->ai_munet){
        delete dg->ai_munet;
        dg->ai_munet = NULL;
    }
    dg->ai_munet = new Mobunet(fnbin,fnparam,fnmsk);
    dg->lock_munet->unlock();
    return 0;
}

int gjdigit_onewav(gjdigit_t* dg,const char* wavfn){
    if(!dg->ai_wenet)return -999;
    /*
       if(dg->mat_wenet){
       delete dg->mat_wenet;
       dg->mat_wenet = NULL;
       }
       */
    int rst = dg->ai_wenet->nextwav(wavfn,dg->bnf_cache);
    dg->bnf_cache->debug();
    dg->cnt_wenet = rst;
    dg->inx_wenet = 0;
    return rst;
}

int gjdigit_picrst(gjdigit_t* dg,const char* picfn,int* box,int index){
    if(!dg->ai_munet)return -999;
    //if(!dg->mat_wenet)return -1;
    if(index<0)return -2;
    if(index>=dg->cnt_wenet)return -3;

    /*
       float* pwenet =  dg->ai_wenet->nextbnf(dg->mat_wenet,index);
       if(!pwenet){
       return -11;
       }
    //JMat feat(256, 20, pwenet, 1);
    */
    JMat* feat = dg->bnf_cache->inxBuf(index);

    std::string picfile(picfn);
    JMat* mat_pic = new JMat(picfile,1);
    int* arr = mat_pic->tagarr();
    arr[10] = box[0];
    arr[11] = box[1];
    arr[12] = box[2];
    arr[13] = box[3];
    dg->ai_munet->process(mat_pic, arr, feat);
    delete mat_pic;
    return 0;
}

int gjdigit_matrst(gjdigit_t* dg,uint8_t* buf,int width,int height,int* box,int index){
    if(!dg->ai_munet)return -999;
    //if(!dg->mat_wenet)return -1;
    if(index<0)return -2;
    if(index>=dg->cnt_wenet)return -3;
    /*
       float* pwenet = dg->ai_wenet->nextbnf(dg->mat_wenet,index);
       if(!pwenet){
       return -11;
       }
       JMat feat(256, 20, pwenet, 1);
       */
    JMat* feat = dg->bnf_cache->inxBuf(index);

    JMat* mat_pic = new JMat(width,height,buf);
    /*
       int* arr = mat_pic->tagarr();
       arr[10] = box[0];
       arr[11] = box[1];
       arr[12] = box[2];
       arr[13] = box[3];
       */
    dg->lock_munet->lock();
    dg->ai_munet->process(mat_pic, box, feat);
    dg->lock_munet->unlock();
    delete mat_pic;
    delete feat;
    return 0;
}

int gjdigit_maskrst(gjdigit_t* dg,uint8_t* bpic,int width,int height,int* box,uint8_t* bmsk,uint8_t* bfg,uint8_t* bbg,int index){
    if(!dg->ai_munet)return -999;
    //if(!dg->mat_wenet)return -1;
    if(index<0)return -2;
    if(index>=dg->cnt_wenet)return -3;
    /*
       float* pwenet = dg->ai_wenet->nextbnf(dg->mat_wenet,index);
       if(!pwenet){
       return -11;
       }
       JMat feat(256, 20, pwenet, 1);
       */
    dg->bnf_cache->debug();
    JMat* feat = dg->bnf_cache->inxBuf(index);
    dg->bnf_cache->debug();
    //JMat* feat = dg->bnf_cache->inxBuf(0);

    JMat* mat_pic = new JMat(width,height,bpic);
    JMat* mat_msk = new JMat(width,height,bmsk);
    JMat* mat_fg = new JMat(width,height,bfg);
    //int* arr = mat_pic->tagarr();
    //arr[10] = box[0];
    //arr[11] = box[1];
    //arr[12] = box[2];
    //arr[13] = box[3];
    MWorkMat wmat(mat_pic,mat_msk,box);
    wmat.premunet();
    JMat* mpic;
    JMat* mmsk;
    wmat.munet(&mpic,&mmsk);
    //JMat realb("real_B.bmp",1);
    //JMat maskb("mask_B.bmp",1);
    dg->lock_munet->lock();
    dg->ai_munet->domodel(mpic, mmsk, feat);
    dg->lock_munet->unlock();
    //dg->ai_munet->domodel(&realb, &maskb, &feat);
    wmat.finmunet(mat_fg);
    //printf("===aaa\n");
    //mat_fg->show("aaa");
    //printf("===bfg %p mat %p\n",bfg,mat_fg->data());
    //cv::waitKey(0);
    /*
       wmat.prealpha();
       JMat* mreal;
       JMat* mimg;
       JMat* mpha;
       wmat.alpha(&mreal,&mimg,&mpha);
       dg->ai_malpha->doModel(mreal,mimg,mpha);
       wmat.finalpha();
       */
    //mat_msk->show("msk");
    //mat_pic->show("picaaa");
    if(bbg) BlendGramAlpha3(bbg,bmsk,bfg,width,height);
    //mat_pic->show("picbbb");
    //cv::waitKey(0);
    delete mat_pic;
    delete mat_msk;
    delete mat_fg;
    delete feat;
    return 0;
}

int gjdigit_free(gjdigit_t** pdg){
    if(!pdg)return -1;
    gjdigit_t* dg = *pdg;
    if(dg->lock_munet){
        dg->lock_munet->lock();
        dg->lock_munet->unlock();
        delete dg->lock_munet;
    }
    if(dg->ai_wenet){
        delete dg->ai_wenet;
        dg->ai_wenet = NULL;
    }
    if(dg->ai_munet){
        delete dg->ai_munet;
        dg->ai_munet = NULL;
    }
    if(dg->bnf_cache){
        delete dg->bnf_cache;
        dg->bnf_cache = NULL;
    }
    if(dg->lock_wenet){
        delete dg->lock_wenet;
        dg->lock_wenet = NULL;
    }
    if(dg->mat_gpg){
        delete dg->mat_gpg;
        dg->mat_gpg = NULL;
    }
    free(dg);
    *pdg = NULL;
    return 0;
}

int gjdigit_test(gjdigit_t* dg){
    dg->cnt_wenet = 10;
    dg->inx_wenet = 0;
    return 2;
}

int gjdigit_processmd5(gjdigit_t* dg,int enc,const char* infn,const char* outfn){
    //
    std::string s_in(infn);
    std::string s_out(outfn);
    int rst = mainenc(enc,(char*)s_in.c_str(),(char*)s_out.c_str());
    return rst;
}

int gjdigit_startgpg(gjdigit_t* dg,const char* infn,const char* outfn){
    std::string s_in(infn);
    std::string s_out(outfn);
    if(!dg->mat_gpg)dg->mat_gpg = new JMat();
    int rst = dg->mat_gpg->loadjpg(s_in);
    if(rst)return rst;
    rst = dg->mat_gpg->savegpg(s_out);
    return rst;
}

int gjdigit_stopgpg(gjdigit_t* dg){
    if(dg->mat_gpg){
        delete dg->mat_gpg;
        dg->mat_gpg = NULL;
    }
    return 0;
}

