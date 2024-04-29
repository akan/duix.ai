#include "netcurl.h"
#include <unistd.h>
#include <stdlib.h>
#include "face_utils.h"
#include "jlog.h"


static int curl_inited = 0;
static char curl_certpath[255];
int guiji_curl_final(){
    if(curl_inited){
        curl_inited = 0;
        curl_global_cleanup();
    }
	return 0;
}

int guiji_curl_init(const char* certpath){
    if(!curl_inited){
        curl_inited = 1;
        curl_global_init(CURL_GLOBAL_ALL);
        //sprintf(curl_certpath,"%s/cacert.pem",certpath);
        sprintf(curl_certpath,"%s",certpath);
        //curl_global_init(0);
    }
    return 0;
}

static size_t write_data(char *buffer, size_t size, size_t nmemb, void *data) {
    NetCurl* pcurl = (NetCurl*)data;
    if(pcurl->status())return -1;
    int realsize = size * nmemb;
    int rst = pcurl->push(buffer,realsize);
    return rst;
}


NetCurl::NetCurl(const char* url,float duration,KWav* wavmat,looper* loop, int timeoutms){
    m_loop = loop;
    //guiji_curl_init();
    m_timeoutms = timeoutms;
    m_url = std::string(url);
    m_duration = duration;
    m_wavmat = wavmat;//new KWav(m_duration);
    //post(0,this);

    //int rst = docurl();
    LOGD("====post init\n");
}

int NetCurl::docurl(){
    int rst = 0;
    CURLcode res = CURLE_OK;
    CURL    *curl = curl_easy_init();
    if(!curl)return -997;
    //
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 1L); // 跳过证书检查
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 2L);
    //curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L); // 跳过证书检查
    //curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
    curl_easy_setopt(curl, CURLOPT_CAINFO, curl_certpath);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, this);

    curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT_MS, m_timeoutms);
    curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT_MS, m_timeoutms);
    curl_easy_setopt(curl, CURLOPT_URL, m_url.c_str());
    curl_easy_setopt(curl, CURLOPT_POST, 0);
        //curl_easy_setopt(curl, CURLOPT_POSTFIELDS, params);
    res = curl_easy_perform(curl);
    if(curl) curl_easy_cleanup(curl);
    if (res == CURLE_OK) {
        //
        rst = 0;
        if(m_loop) m_loop->post(9999,m_wavmat);
    }else{
        m_status = -1;
        rst= 0 - res;
    }
    return rst;
}

int NetCurl::push(char* data,int len){
    if(m_first){
        //check header
        if(len<44)return -1;
        wave_pcm_hdr *hdr = (wave_pcm_hdr*)data;
        char* riff = hdr->riff;
        if((riff[0]!='R')||
            (riff[1]!='I')||
            (riff[2]!='F')||
            (riff[3]!='F'))return -2;
        int alllen = hdr->size_8 + 8;
        int datasize = hdr->data_size;
        char* chck = hdr->data;
        int list = 0;
        if((chck[0]=='L')||
            (chck[1]=='I')||
            (chck[2]=='S')||
            (chck[3]=='T'))list=1;
        if(list){
            m_skiplen = 44+datasize;
            int* ddd = (int*)((uint8_t*)data+m_skiplen);
            chck = (char*)ddd;
            if((chck[0]=='d')||
            (chck[1]=='a')||
            (chck[2]=='t')||
            (chck[3]=='a')){
                datasize = ddd[1];
                m_skiplen += 8;
            }else{
                LOGE("none data after LIST %c %c %c %c\n",chck[0],chck[1],chck[2],chck[3]);
                return -5;
            }
        }else{
            m_skiplen = 44;
        }
        LOGD("===data %d \n",datasize);
        LOGD("===len %d skip %d\n",len,m_skiplen);
        m_wavlen = alllen - m_skiplen;
        if(len<m_skiplen)return -3;
        LOGD("===alllen %d skip %d\n",m_wavlen,m_skiplen);

        m_wavmat->pushpcm((uint8_t*)data+m_skiplen,len-m_skiplen);
        m_first = 0;
        m_checked = 1;
        checkwav();
        return len;
    }
    if(!m_checked)return -999;
    m_wavmat->pushpcm((uint8_t*)data,len);
    checkwav();
    return len;
}

int NetCurl::status(){
    return m_status;
}

int NetCurl::cancel(){
    m_status = -999;
    m_loop = nullptr;
    return 0;
}

int NetCurl::checkwav(){
    int rst = m_wavmat->isready();
    if(rst){
        LOGD("====tooken gogogo %d \n",rst);
        if(m_loop) m_loop->post(rst,m_wavmat);
    }
    return 0;
}
/*
void NetCurl::handle(int what, void *obj){
    //init curl
}

void NetCurl::setready(int inx){
    m_ready = inx;
}

int NetCurl::getready(){
    return m_ready;
}
*/

int NetCurl::checked(){
    return m_first&&m_checked;
}

NetCurl::~NetCurl(){
    m_status = -1000;
    //if(m_wavmat){
        //delete m_wavmat;
        //m_wavmat = nullptr;
    //}
}

/*
int   NetCurl::calcall(Wenet* pwenet){
    //printf("===calcall\n");
    int rst =m_wavmat->readyall();
    while(1){
        if(m_status)return -1;
        rst = m_wavmat->isready();
        if(!rst)break;
        calcinx(pwenet,rst);
    }
    return 0;
}

int   NetCurl::calcinx(Wenet* pwenet,int index){
    if(m_status)return -1;
    float* pwav = NULL;
    float* pmfcc = NULL;
    float* pbnf = NULL;
    int melcnt = 0;
    int bnfcnt = 0;
    int rst = m_wavmat->calcbuf(index, &pwav,&pmfcc,&pbnf,&melcnt,&bnfcnt);
    LOGD("===tooken calcinx %d index %d\n",index,rst);
    if(rst == index){
        pwenet->calcmfcc(pwav,pmfcc);
        if(m_status)return -1;
        pwenet->calcbnf(pmfcc,melcnt,pbnf,bnfcnt);
        //dumpfloat(pbnf,10);
        m_wavmat->finishone(index);
        m_calccnt = index;
    }
    return 0;
}
*/

int NetCurl::test(int index){
    return 0;
}


//#define _NETMAIN_
#ifdef _NETMAIN_

#include "utils/wavreader.h"
int main(int argc,char** argv){
    guiji_curl_init("../model");
    Wenet wenet("../model","wenet");
    init_wenetloop(&wenet);
if(argc>1){
    const char* wavfile = "http://127.0.0.1:8000/1.wav";
    //float duration = 25*60+19;
    //const char* wavfile = "https://digital-public.obs.cn-east-3.myhuaweicloud.com/yunwei_update/wsl/953883.wav";
    //float duration = 11.869;
    float duration = 11.869;
    KWav wavmat(duration);
    NetCurl* curl = new NetCurl(wavfile,duration,&wavmat);
    for(int k=0;k<10;k++){
        if(curl->checked())break;
        usleep(100000);
    }
    LOGD("===netwav %d\n",wavmat.bnfblocks());
    wavmat.debug();
    getchar();
    delete curl;
}else{
    const char* wavfile = "../mybin/1.wav";
    JMat* mat = nullptr;
    wenet.nextwav(wavfile,&mat);

    //mat->tobin("bbb.bin");
    dumpfloat(mat->fdata(),10);
}
    return 0;
    /*
    const char* wavfile = "../mybin/1.wav";
    FILE* fp = fopen(wavfile,"rb");    //
    wave_pcm_hdr* hdr = (wave_pcm_hdr*)malloc(sizeof(wave_pcm_hdr));
    memset(hdr,0,sizeof(wave_pcm_hdr));
    int size = fread(hdr,1,sizeof(wave_pcm_hdr),fp);
    char* riff = hdr->riff;
    printf("===sheader %d %c %c %c %c\n",size,riff[0],riff[1],riff[2],riff[3]);
    printf("===hdr size8 %d data size %d\n",hdr->size_8,hdr->data_size);
    fclose(fp);
    int format, channels, sr, bits_per_sample;
    unsigned int data_length;
    void* fhnd = wav_read_open(wavfile);
    if(!fhnd)return -1;
    int res = wav_get_header(fhnd, &format, &channels, &sr, &bits_per_sample, &data_length);
    if(data_length<1) return -2;
    printf("====data len %u\n",data_length);
    wav_read_close(fhnd);
    return 0;
    */
}
#endif
