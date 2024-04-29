#ifndef GPLAYER_GDigit_H
#define GPLAYER_GDigit_H

#include "scrfd.h"
#include "pfpld.h"
#include "munet.h"
#include "wenet.h"
#include "netcurl.h"
#include "netwav.h"
#include "GRender.h"
#include "looper.h"
#include "dispatchqueue.hpp"
#include "malpha.h"
#include "wavcache.h"

class LoopWenet:public looper{
    private:
        Wenet* m_wenet = nullptr;
        int calcinx(KWav* wavmat,int index);
        int calcall(KWav* wavmat);
    public:
        virtual void handle(int what, void *obj);
        LoopWenet();
        virtual ~LoopWenet();
};

class LoopCurl:public looper{
    public:
        virtual void handle(int what, void *obj);
        LoopCurl();
        virtual ~LoopCurl();
};

class GDigit :public GRender{
    public:
        GDigit(int width,int height, MessageCb *obj);
        virtual ~GDigit();
    public:
        int config(const char* cfgtxt);

        int initScrfd(char* fnparam,char* fnbin);
        int initPfpld(char* fnparam,char* fnbin);

        int initWenet(char* fnwenet);
        int initMunet(char* fnparam,char* fnbin,char* fnmsk);
        int initMalpha(char* fnparam,char* fnbin);
        int initCurl(char* cacertfn,int timeoutms);

        int netwav(const char* url,float duration);
        int bgpic(const char* bgfn);
        int drawmskpic(const char* picfn,const char* mskfn);
        int drawmskbuf(const char* picfn,const char* mskfn,char* dstbuf,char* mskbuf,int size);
        int mskrstpic(int index,const char* picfn,int* box,const char* mskfn,const char* fgfn);
        int mskrstbuf(int index,const char* picfn,int* box,const char* mskfn,const char* fgfn,char* dstbuf,char* mskbuf,int size);
        int drawonebuf(const char* picfn,char* dstbuf,int size);
        int onerstbuf(int index,const char* picfn,int* box,char* dstbuf,int size);

        int netrstpic(const char* picfn,int* box,int index,const char* dumpfn);
        int drawpic(const char* picfn);

        int newwav(const char* wavfn,const char* dumpfn);
        int picrst(const char* picfn,int* box,int index,const char* dumpfn);

        //int mskrst(int index,const char* dumpfn);
        int netrst(int index,const char* dumpfn){return -1;};
        //int initModel(const char* modeldir);
        int newpic(const char* picfn,const char* dumpbox){return -1;};
        int newrst(int index,const char* dumpfn){return -1;};
    private:
        int m_timeoutms = 0;
        Scrfd* ai_scrfd = nullptr;
        Pfpld* ai_pfpld = nullptr;

        Wenet* ai_wenet = nullptr;
        Mobunet* ai_munet = nullptr;
        MAlpha* ai_malpha = nullptr;
        std::mutex  *lock_munet;

        NetCurl* net_curl = nullptr;
        KWav*   net_wavmat = nullptr;
        MBnfCache   *bnf_cache = nullptr;

        //JMat*  mat_wenet = nullptr;
        volatile int     cnt_wenet = 0;
        volatile int     inx_wenet = 0;

        JMat* mat_bg = nullptr;
        LoopWenet       *wenetThread = nullptr;
        void            asyncWenet(int act,Wenet* wenet);
        void            asyncNetwav(int act,KWav* netwav);
        LoopCurl        *curlThread = nullptr;
        void            asyncCurl(int act,NetCurl* curl);
    	DispatchQueue   *dispThread = nullptr;
        volatile int    m_status = 0;
        volatile int    m_working = 0;

        void            clear();
    public:

        virtual void prepare();
        virtual void start();
        virtual void stop();
        virtual void recyle();

};


#endif //GPLAYER_GPLAYER_H

