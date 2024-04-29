#include "wenet.h"
#include <stdio.h>
#include <memory>
#include <vector>
#include "wavreader.h"
#include "face_utils.h"
#include "mfcc/mfcc.hpp"
#include "jlog.h"
#include "aicommon.h"

void Wenet::initModel(const char* modelfn){
    m_model = new OnnxModel();
    string modelpath(modelfn);
    m_model->initModel(modelpath);
    //m_model->pushName("speech",1);
    //m_model->pushName("speech_lengths",1);
    //m_model->pushName("encoder_out",0);
}

int Wenet::calcbnf(float* melbin,int melnum,float* bnfbin,int bnfnum){
    int rst = 0;
    int chkmfcc = melnum;
    int chkbnf = bnfnum;
    auto onecfg = m_model->config();
    auto cfg = &onecfg;
    //cfg->dump();
    cfg->shape_inputs[0][0] = 1;
    cfg->shape_inputs[0][1] = chkmfcc;
    cfg->size_inputs[0] = chkmfcc*MFCC_MELCHUNK;
    cfg->shape_inputs[1][0] = 1;
    cfg->size_inputs[1] = 1;

    cfg->shape_outputs[0][0] = 1;
    cfg->shape_outputs[0][1] = chkbnf;
    cfg->shape_outputs[0][2] = MFCC_BNFCHUNK;
    cfg->size_outputs[0] = chkbnf*MFCC_BNFCHUNK;
    cfg->dump();
    void* arrin[] = { melbin,&chkmfcc,NULL };
    void* arrout[] = { bnfbin,NULL };
    const char* namein[] = {"speech","speech_lengths",NULL};
    const char* nameout[] = {"encoder_out",NULL};
    cfg->names_in = namein;
    cfg->names_out = nameout;
    rst = m_model->runModel(arrin,arrout,NULL,cfg);
    return rst;
}

Wenet::Wenet(const char* modeldir,const char* modelid){
    char path[1024];
    sprintf(path,"%s/%s.onnx",modeldir,modelid);
    initModel((const char*)path);
}

Wenet::Wenet(const char* modelfn){
    initModel(modelfn);
}

Wenet::~Wenet(){
    delete m_model;
}

//int Wenet::nextwav(const char* wavfile,JMat** pmat){
int Wenet::nextwav(const char* wavfile,MBnfCache* bnfcache){

    int     m_pcmsample = 0;
    JBuf   *m_pcmbuf = nullptr;
    JMat   *m_wavmat = nullptr;
    JMat   *m_melmat = nullptr;
    //JMat   *m_bnfmat = nullptr;

    int format, channels, sr, bits_per_sample;
    unsigned int data_length;
    void* fhnd = wav_read_open(wavfile);
    if(!fhnd)return -1;
    int res = wav_get_header(fhnd, &format, &channels, &sr, &bits_per_sample, &data_length);
    if(data_length<1) {
        wav_read_close(fhnd);
        return -2;
    }
    LOGE("data len %d\n",data_length);
    m_pcmbuf = new JBuf(data_length);
    int rst = wav_read_data(fhnd,(unsigned char*)m_pcmbuf->data(),data_length);
    wav_read_close(fhnd);
    int wavsample = data_length/2;
    m_pcmsample = wavsample + 2*MFCC_OFFSET;
    int seca = m_pcmsample / MFCC_WAVCHUNK;
    int secb = m_pcmsample % MFCC_WAVCHUNK;
    if(secb>0){
        //m_pcmsample = wavsample + 2*MFCC_OFFSET + MFCC_WAVCHUNK - secb;
        //seca++;
    }
    int mellast = secb?(secb /160 +1):0;
    int bnflast = secb?((mellast*0.25f)-0.75f):0;

    int wavsize = seca*MFCC_WAVCHUNK + secb;
    int melsize = seca*MFCC_MELBASE+mellast;
    int bnfsize = seca*MFCC_BNFBASE+bnflast;

    int calcsize = seca+1;

    m_wavmat = new JMat(MFCC_WAVCHUNK,calcsize,1);
    m_wavmat->zeros();
    short* ps = (short*)m_pcmbuf->data();
    float* pd = (float*)m_wavmat->data();
    float* pf = pd+MFCC_OFFSET;
    for(int k=0;k<wavsample;k++){
        *pf++ = (float)(*ps++/ 32767.f);
    }
    m_melmat = new JMat(MFCC_MELCHUNK,MFCC_MELBASE*calcsize,1);
    m_melmat->zeros();
    //m_bnfmat = new JMat(MFCC_BNFCHUNK,MFCC_BNFBASE*calcsize,1);
    //m_bnfmat->zeros();
    //
    //printf("===seca %d secb %d mellast %d\n",seca,secb,mellast);
    //m_bnfmat = new JMat(
    calcmfcc(m_wavmat,m_melmat);
    float* mel = m_melmat->fdata();
    for(int k=0;k<seca;k++){
        float* bnf = bnfcache->secBuf(k)->fdata();
        calcbnf(mel,MFCC_MELBASE,bnf,MFCC_BNFBASE);
        //dumpfloat(bnf,10);
        mel+=MFCC_MELBASE*MFCC_MELCHUNK;
        //bnf+=MFCC_BNFBASE*MFCC_BNFCHUNK;
    }
    if(mellast){
        //fix last
        int inxsec = seca ;//seca?(seca+1):0;
        printf("===indexsec %d\n",inxsec);
        float* bnf = bnfcache->secBuf(inxsec)->fdata();
        calcbnf(mel,mellast,bnf,bnflast);
        //dumpfloat(bnf,10);
        //calcbnf(mel,MFCC_MELBASE,bnf,MFCC_BNFBASE);
    }
    int* arr = bnfcache->tagarr();
    //
    arr[0] = wavsize;
    arr[1] = m_pcmsample;
    arr[2] = seca;
    arr[3] = secb;
    float secs = wavsample *1.0f/ MFCC_RATE;
    int bnfblock = secs*MFCC_FPS;
    if(bnfblock>(bnfsize-10))bnfblock = bnfsize-10;
    arr[4] = melsize;
    arr[5] = bnfsize;
    arr[6] = bnfblock;
    /*
    for(int k=0;k<10;k++){
        float* bnf = m_bnfmat->frow(k);
        printf("==%d =bnf %f\n",k,*bnf);
    }
    */
    //*pmat = m_bnfmat;
    delete m_pcmbuf;
    delete m_wavmat;
    delete m_melmat ;
    return bnfblock;
}

float* Wenet::nextbnf(JMat* bnfmat,int index){
    int* arr = bnfmat->tagarr();
    int bnfsize = arr[5] ;
    int bnfblock = arr[6] ;
    LOGD("===index %d bnfsize %d bnfblock %d\n",index,bnfsize,bnfblock);
    if(bnfblock>bnfsize)return NULL;
    if(index>=bnfblock)return NULL;
    float* buf = bnfmat->fdata();
    buf += index*MFCC_BNFCHUNK+MFCC_BNFCHUNK;
    return buf;
}

int Wenet::calcmfcc(float* fwav,float* mel2){
    int rst = 0;
    int melcnt = MFCC_WAVCHUNK/160+1;
    rst = log_mel(fwav,MFCC_WAVCHUNK, 16000,mel2);
    return rst;
}

int Wenet::calcmfcc(JMat* mwav,JMat* mmel){
    int rst = 0;
    int melcnt = MFCC_WAVCHUNK/160+1;
    for(size_t k=0;k<mwav->height();k++){
        float* fwav = mwav->frow(k);
        float* mel2 = mmel->frow(k);
        rst = log_mel(fwav,MFCC_WAVCHUNK, 16000,mel2);
    }
    return rst;
}

#ifdef WENET_MAIN

int main(int argc,char** argv){
    Wenet net("../model","wenet");
    net->nextwav("../mybin/a.wav");
    return 0;
}
#endif
