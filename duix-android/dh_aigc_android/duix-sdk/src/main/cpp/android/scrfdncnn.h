#pragma once

#include "jmat.h"
#include "kmat.h"
#include "scrfd.h"
#include "pfpld.h"
#include "munet.h"
#include "wenet.h"
#include "utils/face_utils.h"
#include "netcurl.h"
#include "looper.h"
#include "ndkwin.h"
#include "netwav.h"
#include "netcurl.h"

class MyDigit:public MyDisp {
    private:
        Scrfd* ai_scrfd = nullptr;
        Pfpld* ai_pfpld = nullptr;

        Wenet* ai_wenet = nullptr;
        Mobunet* ai_munet = nullptr;

        int inited = false;
        JMat*  mat_wenet = nullptr;
        int     cnt_wenet = 0;
        int     inx_wenet = 0;
        JMat* mat_pic = nullptr;
        JMat* mat_bg = nullptr;
        int checkfile(char* fn);
        NetCurl* net_curl = nullptr;
        KWav*   net_wavmat = nullptr;
    public:
        MyDigit();
        virtual ~MyDigit();
        int initModel(const char* modeldir);

        int netwav(const char* url,float duration);
        int netrst(int index,const char* dumpfn);
        int netrstpic(const char* picfn,int* box,int index,const char* dumpfn);

        int newwav(const char* wavfn,const char* dumpfn);
        int newpic(const char* picfn,const char* dumpbox);
        int drawpic(const char* picfn);

        int newrst(int index,const char* dumpfn);
        int picrst(const char* picfn,int* box,int index,const char* dumpfn);
        int reset_inx();
};

