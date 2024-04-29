#pragma once
#include  "curl/curl.h"
#include "looper.h"
#include "wenet.h"
#include "netwav.h"

extern "C"{
#pragma pack(push)
#pragma pack(4)

    typedef struct _wave_pcm_hdr {
        char            riff[4];
        int             size_8;
        char            wave[4];
        char            fmt[4];
        int             fmt_size;
        short int       format_tag;
        short int       channels;
        int             samples_per_sec;
        int             avg_bytes_per_sec;
        short int       block_align;
        short int       bits_per_sample;
        char            data[4];
        unsigned int    data_size;
    } wave_pcm_hdr;

    struct _chunk_t {
        char ID[4];
        unsigned long size;
    };

#pragma pack(pop)
}


class NetCurl{
    private:
        std::string 	m_url;
        int				m_timeoutms = 6000;
        float           m_duration = 0;
        int             m_wavlen = 0;
        int             m_skiplen = 0;

        volatile int    m_first = 1;
        volatile int    m_checked = 0;
        //volatile int    m_calccnt = 0;
        //volatile int    m_ready = 0;


        int             m_status = 0;
        KWav            *m_wavmat = nullptr;
        int             checkwav();
        looper          *m_loop = nullptr;
    public:
        int     docurl();
        int     push(char* data,int len);
        int     checked();
        //void    setready(int inx);
        //int     getready();
        int     calcinx(Wenet* pwenet,int index);
        int     calcall(Wenet* pwenet);
        int     cancel();
        int     status();
        NetCurl(const char* url,float duration,KWav* wavmat,looper* loop,int timeoutms = 10000);
        ~NetCurl();
        int     test(int index);
};

int guiji_curl_final();
int guiji_curl_init(const char* certpath);

