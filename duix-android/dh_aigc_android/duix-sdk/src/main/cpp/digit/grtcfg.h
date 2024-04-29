#ifndef _GRTCFG_H__
#define _GRTCFG_H__

#ifdef __cplusplus
extern "C" {
#endif
/*
 *  {"width":1080,"height":1920,"fps":25,"samplerate":48000,"channels":1,"bitrate":0,"bframe":0,"gopsize":25,"cuda":0,"opus":0,"srcwidth":480,"srcheight":640,"srcformat":0,"srcsample":16000,"srcchannel":1,"maxqueue":200,"maxrecyle":50,"timeout":60000,"nb_samples":1024}
 *
 * */
    typedef struct rtcfg_s rtcfg_t;
    struct rtcfg_s{
        int     action;
        int     videowidth;
        int     videoheight;
        int     timeoutms;
        char*   defdir;
        char*   wenetfn;
        char*   unetbin;
        char*   unetparam;
        char*   unetmsk;
        char*   alphabin;
        char*   alphaparam;
        char*   cacertfn;
        char*   scrfdbin;
        char*   scrfdparam;
        char*   pfpldbin;
        char*   pfpldparam;
        void                *base_obj;
    };

    rtcfg_t* make_rtcfgjson(char* str);
    int nval_rtcfg(rtcfg_t* cfg,char* name);
    char* sval_rtcfg(rtcfg_t* cfg,char* name);
    char* dump_rtcfg(rtcfg_t* cfg);
    int final_rtcfg(rtcfg_t* cfg);

#ifdef __cplusplus
}
#endif
#endif

