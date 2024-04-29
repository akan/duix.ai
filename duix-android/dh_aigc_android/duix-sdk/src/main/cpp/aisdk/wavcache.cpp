#include "wavcache.h"
#include "aicommon.h"


JMat* MBufCache::secBuf(int sec){
    JMat* mat = NULL;
    m_lock->lock();
    if(sec<vec_buf.size()){
        mat = vec_buf[sec];
    }else{
        mat = new JMat(m_secw,m_sech+1,1);
        vec_buf.push_back(mat);
    }
    m_lock->unlock();
    return mat;
}
void MBufCache::debug(){

}

JMat* MBufCache::inxBuf(int inx){
    int seca = inx/m_sech;
    int secb = inx%m_sech;
    JMat* mat = NULL;
    if(secb>=m_lineh){
        mat= new JMat(m_secw,m_blockh,1);
        JMat* sa = secBuf(seca);
        int la = m_sech-secb;
        int parta = la*m_secw;
        float* pa = mat->fdata();
        memcpy(pa,sa->frow(secb),parta*sizeof(float));
        JMat* sb = secBuf(seca+1);
        int lb = m_blockh - la;
        float* pb = pa+parta;
        int partb = lb*m_secw;
        memcpy(pb,sa->frow(0),partb*sizeof(float));
    }else{
        JMat* src = secBuf(seca);
        float* buf = src->frow(secb);
        //printf("==dist %d\n",(buf-src->fdata())*4);
        mat = new JMat(m_secw,m_blockh,buf,1);
        //printf("==size %d %d =  %d\n",m_secw,m_blockh,m_secw*m_blockh*4);
    }
    return mat;
}

int*    MBufCache::tagarr(){
    return m_tagarr;
}

MBufCache::MBufCache(int initsec,int secw,int sech,int blockh){
    m_lock = new std::mutex();
    m_secw = secw;
    m_sech = sech;
    m_blockh = blockh;
    m_lineh = sech-blockh;
    for(int k=0;k<initsec;k++){
        JMat* mat = new JMat(m_secw,m_sech+1,1);
        vec_buf.push_back(mat);
    }
    memset(m_tagarr,0,512*sizeof(int));
}

MBufCache::~MBufCache(){
    m_lock->lock();
    for(int k=0;k<vec_buf.size();k++){
        JMat* mat = vec_buf[k];
        delete mat;
    }
    vec_buf.clear();
    m_lock->unlock();
    delete m_lock;
}

#include "aicommon.h"

MBnfCache::MBnfCache():MBufCache(3,MFCC_BNFCHUNK,MFCC_BNFBASE,20){
}

MBnfCache::~MBnfCache(){
}

#ifdef _WAVTEST_
int main(int argc,char** argv){
    MBnfCache cache;
    for(int k=0;k<100;k++){
        JMat* mat = cache.secBuf(k);
    }
    for(int k=816;k<817;k++){
        printf("#%d# \n",k);
        JMat* mat = cache.inxBuf(k);
        JMat cm = mat->clone();
        delete mat;
    }
    return 0;
}
#endif
