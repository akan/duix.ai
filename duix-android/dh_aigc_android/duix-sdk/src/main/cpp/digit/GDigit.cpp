#include "GDigit.h"
#include "MediaData.h"
#include "LoopThreadHelper.h"
#include <Log.h>
#include <cstdint>
#include <unistd.h>
#include "grtcfg.h"
#include "benchmark.h"

#ifdef __ANDROID__
#include "coffeecatch.h"
#endif


#define TAG "tooken"

int LoopWenet::calcinx(KWav* wavmat,int index){
    float* pwav = NULL;
    float* pmfcc = NULL;
    float* pbnf = NULL;
    int melcnt = 0;
    int bnfcnt = 0;
    int rst = wavmat->calcbuf(index, &pwav,&pmfcc,&pbnf,&melcnt,&bnfcnt);
    LOGE(TAG,"===tooken calcinx %d index %d \n",index,rst);
    if(rst == index){
        m_wenet->calcmfcc(pwav,pmfcc);
    //double t0 = ncnn::get_current_time();
        m_wenet->calcbnf(pmfcc,melcnt,pbnf,bnfcnt);
    //double t1 = ncnn::get_current_time();
    //float dist = t1-t0;
        //dumpfloat(pbnf,10);
        wavmat->finishone(index);
    }
    return 0;
}

int LoopWenet::calcall(KWav* wavmat){
    int rst =wavmat->readyall();
    int cnt = 0;
    while(cnt<1000){
        rst = wavmat->isready();
        if(!rst)break;
        calcinx(wavmat,rst);
    }
    return 0;
}

void LoopWenet::handle(int what, void *obj){
    LOGD(TAG,"===tooken loopwenet %d\n",what);
#ifdef __ANDROID__
COFFEE_TRY() {
#else
try{
#endif
    if(what==9999){
        if(m_wenet){
            KWav* matwav = (KWav*)obj;
            calcall(matwav);
        }
    }else if(what>=0){
        if(m_wenet){
            KWav* matwav = (KWav*)obj;
            calcinx(matwav,what);
        }
    }else if(what==-1){
        Wenet* wenet = (Wenet*)obj;
        m_wenet = wenet;
    }else if(what==-2){
        m_wenet = nullptr;
        Wenet* wenet = (Wenet*)obj;
        delete wenet;
    }else if(what==-11){
        KWav* matwav = (KWav*)obj;
        delete matwav;
    }
#ifdef __ANDROID__
}COFFEE_CATCH() {
    LOGE(TAG,"loopcurl err %d",what);
} COFFEE_END();
#else
}catch(...){
    LOGE(TAG,"loopwenet err %d",what);
}
#endif
}

LoopWenet::LoopWenet():looper(){
    //
}

LoopWenet::~LoopWenet(){
    m_wenet = nullptr;
}

void LoopCurl::handle(int what, void *obj){
#ifdef __ANDROID__
COFFEE_TRY() {
#else
try{
#endif
    if(what==1){
        LOGD(TAG,"===docurl \n");
        NetCurl* curl = (NetCurl*)obj;
        int rst = curl->docurl();
        LOGD(TAG, " docurl %d  ",rst);
    }else if(what==-1){
        NetCurl* curl = (NetCurl*)obj;
        delete curl;
    }
#ifdef __ANDROID__
}COFFEE_CATCH() {
    LOGE(TAG,"loopcurl err %d",what);
} COFFEE_END();
#else
}catch(...){
    LOGE(TAG,"loopcurl err %d",what);
}
#endif
}

LoopCurl::LoopCurl():looper(){
}

LoopCurl::~LoopCurl(){
}

void GDigit::asyncWenet(int act,Wenet* wenet){
    if(act){
        wenetThread->post(-2,wenet);
    }else{
        wenetThread->post(-1,wenet);
    }
}

void GDigit::asyncNetwav(int act,KWav* netwav){
    if(act){
        wenetThread->post(-11,netwav);
    }
}

void GDigit::asyncCurl(int act,NetCurl* curl){
    if(act){
        curlThread->post(-1,curl);
    }else{
        curlThread->post(1,curl);
    }
}

GDigit::GDigit(int width,int height, MessageCb *obj):GRender(width,height,obj) {
    dispThread = new DispatchQueue("Digit");
    curlThread = new LoopCurl();
    wenetThread = new LoopWenet();
    bnf_cache = new MBnfCache();
    lock_munet = new std::mutex();
}

GDigit::~GDigit() {
    if(m_status)stop();
    recyle();
    usleep(1000);
    if(1){
        curlThread->quit();
        delete curlThread;
        curlThread = nullptr;
    }
    if(1){
        wenetThread->quit();
        delete wenetThread;
        wenetThread = nullptr;
    }
    if(1){
        dispThread->removePending();
        delete dispThread;
        dispThread = nullptr;
    }
    if(1){
        if(bnf_cache ){
            delete bnf_cache ;
            bnf_cache = nullptr;
        }
    }
    if(1){
        lock_munet->lock();
        lock_munet->unlock();
        delete lock_munet;
    }
}

int GDigit::config(const char* cfgtxt){
    rtcfg_t* cfg = make_rtcfgjson((char*)cfgtxt);
    if(!cfg)return -1;
    LOGE(TAG,"cfg %p",cfg);
    if(cfg->videowidth && cfg->videoheight){
        m_width = cfg->videowidth;
        m_height = cfg->videoheight;
        LOGD(TAG,"w %d h %d",m_width,m_height);
    }
    LOGE(TAG,"aaa %s",cfg->cacertfn);
    if(cfg->timeoutms&&cfg->cacertfn){
        initCurl(cfg->cacertfn,cfg->timeoutms);
    }
    LOGE(TAG,"bbb %s",cfg->wenetfn);
    if(cfg->wenetfn){
        initWenet(cfg->wenetfn);
    }
    LOGE(TAG,"ccc %s",cfg->unetmsk);
    if(cfg->unetbin&&cfg->unetparam&&cfg->unetmsk){
        initMunet(cfg->unetparam,cfg->unetbin,cfg->unetmsk);
    }
    if(cfg->alphabin&&cfg->alphaparam){
        initMalpha(cfg->alphaparam,cfg->alphabin);
    }
    LOGE(TAG,"ddd");
    if(cfg->scrfdbin&&cfg->scrfdparam){
        initScrfd(cfg->scrfdparam,cfg->scrfdbin);
    }
    if(cfg->pfpldbin&&cfg->pfpldparam){
        initPfpld(cfg->pfpldparam,cfg->pfpldbin);
    }
    final_rtcfg(cfg);
    LOGE(TAG,"eee");
    return 0;
}

int GDigit::initScrfd(char* fnparam,char* fnbin){
    return 0;
}
int GDigit::initPfpld(char* fnparam,char* fnbin){
    return 0;
}

int GDigit::initWenet(char* fnwenet){
    if(ai_wenet){
        asyncWenet(1,ai_wenet);
        ai_wenet = nullptr;
    }
    ai_wenet = new Wenet(fnwenet);
    asyncWenet(0,ai_wenet);
    return 0;
}

int GDigit::initMunet(char* fnparam,char* fnbin,char* fnmsk){
    Mobunet* munet = ai_munet;
    lock_munet->lock();
    ai_munet = new Mobunet(fnbin,fnparam,fnmsk);
    lock_munet->unlock();
dispThread->dispatch([munet,this]() {
    if(munet){
        delete munet;
    }
});
    LOGE(TAG,"init munet");
    return 0;
}

int GDigit::initMalpha(char* fnparam,char* fnbin){
    if(1)return 0;
    MAlpha* malpha = ai_malpha;
dispThread->dispatch([malpha,this]() {
    if(malpha){
        delete malpha;
    }
});
    ai_malpha = new MAlpha(fnbin,fnparam);
    LOGE(TAG,"init alpha");
    return 0;
}

int GDigit::initCurl(char* cacertfn,int timeoutms){
    guiji_curl_init(cacertfn);
    m_timeoutms = timeoutms;
    return 0;
}

int GDigit::netwav(const char* url,float duration){
    if(!m_status)return -1000;
    if(!ai_wenet)return -999;
    if(duration>1000.0f)return -990;
    if(duration<1.0f)return -991;
    int len = strlen(url);
    if(len<10)return -101;
    char* fn = (char*)(url+0);
    if((fn[0]!='h')||(fn[1]!='t')||(fn[2]!='t'))return -102;
    fn =(char*)(url+len-3);
    if((fn[0]!='w')||(fn[1]!='a')||(fn[2]!='v'))return -103;
    if(net_curl){
        net_curl->cancel();
        asyncCurl(1,net_curl);
        net_curl = nullptr;
    }
    if(net_wavmat){
        KWav* wm = net_wavmat;
        asyncNetwav(1,wm);
        //net_wavmat = nullptr;
    }
    net_wavmat = new KWav(duration,bnf_cache);
    net_curl = new NetCurl((char*)url,duration,net_wavmat,wenetThread,m_timeoutms);
    asyncCurl(0,net_curl);
    //
    int finished = 0;
    for(int k=0;k<100;k++){
        if(net_curl->status()){
            finished = 1;
            break;
        }
        if(net_wavmat->resultcnt())break;
        usleep(100000);
    }
    int rst = 0;
    if(!finished){
        rst =  net_wavmat->bnfblocks();//net_curl->checked();
        cnt_wenet = rst;
        inx_wenet = 0;
    }
    return rst;
}

int GDigit::bgpic(const char* bgfn){
    if(!m_status)return -1000;
    std::string picbg(bgfn);
    lock_munet->lock();
    if(!mat_bg)mat_bg = new JMat();//picbg,1);
    mat_bg->load(picbg);
    lock_munet->unlock();
    /*
    JMat* bg = mat_bg;
dispThread->dispatch([picbg,bg,this]() {
    delete bg;
});
    mat_bg = new JMat(picbg,1);
    */
    return 0;
}

int GDigit::drawmskbuf(const char* picfn,const char* mskfn,char* dstbuf,char* mskbuf,int size){
    //if(!m_status)return -1000;
    std::string picfile(picfn);
    std::string mskfile(mskfn);

    JMat* mat_pic = NULL;//new JMat(picfile,1);
    JMat* mat_msk = NULL;//new JMat(mskfile,1);
    frameSource->popVidRecyle(&mat_pic);
    frameSource->popVidRecyle(&mat_msk);
    if(!mat_pic)mat_pic = new JMat();
    if(!mat_msk)mat_msk = new JMat();
    int rst = 0;
    while(1){
        rst = mat_pic->load(picfile);
        if(rst)break;
        rst = mat_msk->load(mskfile);
        break;
    }
    if(rst){
        delete mat_pic;
        delete mat_msk;
        return rst*1000;
    }
    memcpy(dstbuf,mat_pic->data(),size);
    memcpy(mskbuf,mat_msk->data(),size);
    frameSource->pushVidRecyle(mat_pic);
    frameSource->pushVidRecyle(mat_msk);
    return 0;//});
}

int GDigit::drawonebuf(const char* picfn,char* dstbuf,int size){
    //if(!m_status)return -1000;
    std::string picfile(picfn);

    JMat* mat_pic = NULL;//new JMat(picfile,1);
    frameSource->popVidRecyle(&mat_pic);
    if(!mat_pic)mat_pic = new JMat();
    int rst = 0;
    while(1){
        rst = mat_pic->load(picfile);
        if(rst)break;
        break;
    }
    if(rst){
        delete mat_pic;
        return rst*1000;
    }
    memcpy(dstbuf,mat_pic->data(),size);
    frameSource->pushVidRecyle(mat_pic);
    return 0;//});
}


int GDigit::drawmskpic(const char* picfn,const char* mskfn){
    if(!m_status)return -1000;
    if(!mat_bg)return -1;
    std::string picfile(picfn);
    std::string mskfile(mskfn);

    JMat* mat_pic = NULL;//new JMat(picfile,1);
    JMat* mat_msk = NULL;//new JMat(mskfile,1);
    frameSource->popVidRecyle(&mat_pic);
    frameSource->popVidRecyle(&mat_msk);
    if(!mat_pic)mat_pic = new JMat();
    if(!mat_msk)mat_msk = new JMat();
    int rst = 0;
    while(1){
        rst = mat_pic->load(picfile);
        if(rst)break;
        rst = mat_msk->load(mskfile);
        break;
    }
    if(rst){
        delete mat_pic;
        delete mat_msk;
        return rst*1000;
    }

//dispThread->dispatch([mat_pic,mat_msk,this]() {
    if(!mat_bg)return -11;
    JMat* bgm = mat_bg->refclone();
    MediaData md(mat_pic,mat_msk,bgm);
    frameSource->pushVidFrame(&md);
//});
    return 0;
}

int GDigit::mskrstbuf(int index,const char* picfn,int* box,const char* mskfn,const char* fgfn,char* dstbuf,char* mskbuf,int size){
    if(!m_status)return -1000;
    if(!ai_wenet)return -999;
    if(!ai_munet)return -998;
    //if(!mat_bg)return -3;
    if(!net_wavmat)return -1;
    if(index<0)return -2;
    if(index>=cnt_wenet)return -3;
    std::string picfile(picfn);
    std::string mskfile(mskfn);
    std::string fgfile(strlen(fgfn)?fgfn:"");

    JMat* mat_fg = NULL;//new JMat(fgfile,1);
    JMat* mat_pic = NULL;//new JMat(picfile,1);
    JMat* mat_msk = NULL;//new JMat(mskfile,1);
    int hasfg = fgfile.length();
    frameSource->popVidRecyle(&mat_pic);
    frameSource->popVidRecyle(&mat_msk);
    if(!mat_pic)mat_pic = new JMat();
    if(!mat_msk)mat_msk = new JMat();
    if(hasfg){
        frameSource->popVidRecyle(&mat_fg);
        if(!mat_fg)mat_fg = new JMat();
    }
    int rst = 0;
    while(1){
        rst = mat_pic->load(picfile);
        if(rst)break;
        rst = mat_msk->load(mskfile);
        if(rst)break;
        if(hasfg) rst = mat_fg->load(fgfile);
        break;
    }
    if(rst){
        if(mat_pic) delete mat_pic;
        if(mat_msk) delete mat_msk;
        if(mat_fg) delete mat_fg;
        return rst*10000;
    }
    if(size<mat_pic->size()){
        if(mat_pic) delete mat_pic;
        if(mat_msk) delete mat_msk;
        if(mat_fg) delete mat_fg;
        return -10000;
    }
    int arr[4]={box[0],box[1],box[2],box[3]};
    if((!ai_wenet)|| (!ai_munet) ||(!net_wavmat))return -13;
    JMat* mat_feat = bnf_cache->inxBuf(index);
    if(!mat_feat)return -14;
    MWorkMat wmat(mat_pic,mat_msk,arr);
    wmat.premunet();
    JMat *mpic, *mmsk;
    wmat.munet(&mpic,&mmsk);
    lock_munet->lock();
    if(ai_munet) ai_munet->domodel(mpic, mmsk, mat_feat);
    lock_munet->unlock();
    wmat.finmunet(mat_fg);
    //memcpy(mat_fg->data(),dstbuf,size);
    memcpy(dstbuf,mat_fg->data(),size);
    memcpy(mskbuf,mat_msk->data(),size);
    //todo
    frameSource->pushVidRecyle(mat_pic);
    frameSource->pushVidRecyle(mat_msk);
    frameSource->pushVidRecyle(mat_fg);
    return 0;
}

int GDigit::onerstbuf(int index,const char* picfn,int* box,char* dstbuf,int size){
    if(!m_status)return -1000;
    if(!ai_wenet)return -999;
    if(!ai_munet)return -998;
    //if(!mat_bg)return -3;
    if(!net_wavmat)return -1;
    if(index<0)return -2;
    if(index>=cnt_wenet)return -3;
    std::string picfile(picfn);

    JMat* mat_pic = NULL;//new JMat(picfile,1);
    frameSource->popVidRecyle(&mat_pic);
    if(!mat_pic)mat_pic = new JMat();
    int rst = 0;
    while(1){
        rst = mat_pic->load(picfile);
        if(rst)break;
        break;
    }
    if(rst){
        if(mat_pic) delete mat_pic;
        return rst*10000;
    }
    if(size<mat_pic->size()){
        if(mat_pic) delete mat_pic;
        return -10000;
    }
    int arr[4]={box[0],box[1],box[2],box[3]};
    if((!ai_wenet)|| (!ai_munet) ||(!net_wavmat))return -13;
    JMat* mat_feat = bnf_cache->inxBuf(index);
    if(!mat_feat)return -14;
    MWorkMat wmat(mat_pic,NULL,arr);
    wmat.premunet();
    JMat *mpic, *mmsk;
    wmat.munet(&mpic,&mmsk);
    lock_munet->lock();
    if(ai_munet) ai_munet->domodel(mpic, mmsk, mat_feat);
    lock_munet->unlock();
    //todo
    wmat.finmunet(mat_pic);
    //memcpy(mat_fg->data(),dstbuf,size);
    memcpy(dstbuf,mat_pic->data(),size);
    //todo
    frameSource->pushVidRecyle(mat_pic);
    return 0;
}

int GDigit::mskrstpic(int index,const char* picfn,int* box,const char* mskfn,const char* fgfn){
    if(!m_status)return -1000;
    if(!ai_wenet)return -999;
    if(!ai_munet)return -998;
    if(!mat_bg)return -3;
    if(!net_wavmat)return -1;
    if(index<0)return -2;
    //if(!net_wavmat)return -1;
    //if(index>net_wavmat->bnfblocks())return -3;
    if(index>=cnt_wenet)return -3;
    std::string picfile(picfn);
    std::string mskfile(mskfn);
    std::string fgfile(strlen(fgfn)?fgfn:"");

    JMat* mat_fg = NULL;//new JMat(fgfile,1);
    JMat* mat_pic = NULL;//new JMat(picfile,1);
    JMat* mat_msk = NULL;//new JMat(mskfile,1);
    int hasfg = fgfile.length();
    frameSource->popVidRecyle(&mat_pic);
    frameSource->popVidRecyle(&mat_msk);
    if(!mat_pic)mat_pic = new JMat();
    if(!mat_msk)mat_msk = new JMat();
    if(hasfg){
        frameSource->popVidRecyle(&mat_fg);
        if(!mat_fg)mat_fg = new JMat();
    }
    int rst = 0;
    while(1){
        rst = mat_pic->load(picfile);
        //printf("===matpic %d\n",rst);
        if(rst)break;
        rst = mat_msk->load(mskfile);
        //printf("===mat msk %d\n",rst);
        if(rst)break;
        if(hasfg) rst = mat_fg->load(fgfile);
        //printf("===mat fg %d\n",rst);
        break;
    }
    if(rst){
        if(mat_pic) delete mat_pic;
        if(mat_msk) delete mat_msk;
        if(mat_fg) delete mat_fg;
        return rst*10000;
    }
    //mat_msk->show("abc");
    //cv::waitKey(0);
    int arr[4]={box[0],box[1],box[2],box[3]};

//dispThread->dispatch([mat_pic,mat_msk,mat_fg,arr,index,this]() {
    if((!ai_wenet)|| (!ai_munet) ||(!mat_bg)||(!net_wavmat))return -13;

    //
    //JMat* mw = net_wavmat->bnfmat();
    //float* pwenet = ai_wenet->nextbnf(mw,index);
    //if(pwenet){
    //  JMat afeat(256, 20, pwenet, 1);
    //JMat feat = afeat.clone();
    JMat* mat_feat = bnf_cache->inxBuf(index);
    if(mat_feat){
        JMat* bgm = mat_bg->refclone();
        if(mat_fg){
            //if(ai_malpha){
            if(1){
                MWorkMat wmat(mat_pic,mat_msk,arr);
                wmat.premunet();
                JMat *mpic, *mmsk;
                wmat.munet(&mpic,&mmsk);
                lock_munet->lock();
                if(ai_munet) ai_munet->domodel(mpic, mmsk, mat_feat);
                lock_munet->unlock();
                wmat.finmunet(mat_fg);

                /*
                wmat.prealpha();
                JMat *mreal, *mimg, *mpha;
                wmat.alpha(&mreal,&mimg,&mpha);
                ai_malpha->doModel(mreal,mimg,mpha);
                wmat.finalpha();
                */
            }else{
                lock_munet->lock();
                if(ai_munet) ai_munet->fgprocess(mat_pic, arr, mat_feat,mat_fg);
                lock_munet->unlock();
            }
            MediaData md(mat_fg,mat_msk,bgm);
            frameSource->pushVidFrame(&md);
            frameSource->pushVidRecyle(mat_pic);
        }else{
            //if(ai_malpha){
            if(1){
                MWorkMat wmat(mat_pic,mat_msk,arr);
                wmat.premunet();
                JMat *mpic, *mmsk;
                wmat.munet(&mpic,&mmsk);
                lock_munet->lock();
                if(ai_munet) ai_munet->domodel(mpic, mmsk, mat_feat);
                lock_munet->unlock();
                wmat.finmunet();
                /*
                wmat.prealpha();
                JMat *mreal, *mimg, *mpha;
                wmat.alpha(&mreal,&mimg,&mpha);
                ai_malpha->doModel(mreal,mimg,mpha);
                wmat.finalpha();
                */
            }else{
                lock_munet->lock();
                if(ai_munet) ai_munet->process(mat_pic, arr, mat_feat);
                lock_munet->unlock();
            }
            MediaData md(mat_pic,mat_msk,bgm);
            frameSource->pushVidFrame(&md);
        }
        delete mat_feat;
    }
//});
    return 0;
}

int GDigit::netrstpic(const char* picfn,int* box,int index,const char* dumpfn){
    if(!m_status)return -1000;
    if(!ai_wenet)return -999;
    if(!net_wavmat)return -1;
    if(index<0)return -2;
    if(index>net_wavmat->bnfblocks())return -3;
    std::string picfile(picfn);
    JMat* mat_pic = NULL;//new JMat(picfile,1);
    frameSource->popVidRecyle(&mat_pic);
    if(!mat_pic)mat_pic = new JMat();
    int rst = mat_pic->load(picfn);
    if(rst){
        delete mat_pic;
        return rst*10000;
    }
    int arr[4]={box[0],box[1],box[2],box[3]};
dispThread->dispatch([mat_pic,index,arr,this]() {
    if(!ai_wenet)return -11;
    if(!net_wavmat)return -12;

    //
    //JMat* mw = net_wavmat->bnfmat();
    //float* pwenet = ai_wenet->nextbnf(mw,index);
    //if(pwenet){
        //JMat afeat(256, 20, pwenet, 1);
        //JMat feat = afeat.clone();
    JMat* mat_feat = bnf_cache->inxBuf(index);
    if(mat_feat){
        lock_munet->lock();
        if(ai_munet) ai_munet->process(mat_pic, arr, mat_feat);
        lock_munet->unlock();
        MediaData md(mat_pic);
        frameSource->pushVidFrame(&md);
        delete mat_feat;
    }
});
    return 0;
}

int GDigit::drawpic(const char* picfn){
    if(!m_status)return -1000;
    std::string picfile(picfn);
    JMat* mat_pic = NULL;//new JMat(picfile,1);
    frameSource->popVidRecyle(&mat_pic);
    if(!mat_pic)mat_pic = new JMat();
    int rst = mat_pic->load(picfn);
    if(rst){
        delete mat_pic;
        return rst*10000;
    }
//dispThread->dispatch([mat_pic,this]() {
    MediaData md(mat_pic);
    frameSource->pushVidFrame(&md);
//});
    return 0;
}

int GDigit::newwav(const char* wavfn,const char* dumpfn){
    if(!m_status)return -1000;
    if(!ai_wenet)return -999;
    int len = strlen(wavfn);
    if(len<10)return -101;
    char* fn = (char*)(wavfn+len-3);
    if((fn[0]!='w')||(fn[1]!='a')||(fn[2]!='v'))return -103;
    if(net_curl){
        net_curl->cancel();
        asyncCurl(1,net_curl);
        net_curl = nullptr;
    }
    if(net_wavmat){
        KWav* wm = net_wavmat;
        asyncNetwav(1,wm);
        //net_wavmat = nullptr;
    }
    net_wavmat = new KWav(wavfn,bnf_cache);
    int rst = 0;
    if(net_wavmat->duration()>0){
        //
        //int rst = ai_wenet->nextwav(wavfn,&mat_wenet);
        wenetThread->post(9999,net_wavmat);
        for(int k=0;k<30;k++){
            if(net_wavmat->resultcnt()){
                break;
            }
            usleep(100000);
        }
        rst =  net_wavmat->bnfblocks();//net_curl->checked();
        cnt_wenet = rst;
        inx_wenet = 0;
    }else{
        rst = 0;
    }
    return rst;
}

int GDigit::picrst(const char* picfn,int* box,int index,const char* dumpfn){
    if(!m_status)return -1000;
    if(!ai_wenet)return -999;
    if(!net_wavmat)return -1;
    if(index<0)return -2;
    if(index>=cnt_wenet)return -3;
    std::string picfile(picfn);
    JMat* mat_pic = NULL;//new JMat(picfile,1);
    frameSource->popVidRecyle(&mat_pic);
    if(!mat_pic)mat_pic = new JMat();
    int rst = mat_pic->load(picfn);
    if(rst){
        delete mat_pic;
        return rst*10000;
    }
    int arr[4]={box[0],box[1],box[2],box[3]};
//dispThread->dispatch([mat_pic,arr,index,this]() {
    if(!ai_wenet)return -11;
    if(!net_wavmat)return -12;
            //
    /*
    JMat* mw = net_wavmat->bnfmat();
    float* pwenet = ai_wenet->nextbnf(mw,index);
    if(pwenet){
        JMat afeat(256, 20, pwenet, 1);
        JMat feat = afeat.clone();
        */
    JMat* mat_feat = bnf_cache->inxBuf(index);
    if(mat_feat){
        ai_munet->process(mat_pic, arr, mat_feat);
        MediaData md(mat_pic);
        frameSource->pushVidFrame(&md);
        delete mat_feat;
    }
//});
    return 0;
}


void GDigit::prepare(){
    GRender::prepare();
}

void GDigit::start(){
    m_status = 1;
    GRender::start();
}

void GDigit::recyle(){
dispThread->dispatch([this]() {
    if(ai_munet){
        lock_munet->lock();
        delete ai_munet;
        ai_munet = nullptr;
        lock_munet->unlock();
    }
    if(ai_malpha){
        delete ai_malpha;
        ai_malpha = nullptr;
    }
    if(mat_bg){
        delete mat_bg;
        mat_bg = nullptr;
    }
});
    if(ai_wenet){
        asyncWenet(1,ai_wenet);
        ai_wenet = nullptr;
    }
    if(net_wavmat){
        asyncNetwav(1,net_wavmat);
        net_wavmat = nullptr;
    }
}

void GDigit::clear(){
    if(net_curl){
        net_curl->cancel();
        asyncCurl(1,net_curl);
        net_curl = nullptr;
    }
}

void GDigit::stop(){
    m_status = 0;
    dispThread->removePending();
    GRender::stop();
    clear();
    usleep(1000);
}



