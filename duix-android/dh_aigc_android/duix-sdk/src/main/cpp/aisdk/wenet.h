#pragma once
#include "jmat.h"
#include "aimodel.h"
#include <vector>
#include "wavcache.h"

//#include <onnx/onnxruntime_cxx_api.h>
class Wenet{
    private:
        OnnxModel   *m_model = nullptr;
        void initModel(const char* modelfn);
    public:
        int calcmfcc(JMat* mwav,JMat* mmel);
        int calcmfcc(float* fwav,float* mel2);
        int calcbnf(float* melbin,int melnum,float* bnfbin,int bnfnum);
        //int nextwav(const char* wavfile,JMat** pmat);
        int nextwav(const char* wavfile,MBnfCache* bnfcache);
        float* nextbnf(JMat* bnfmat,int index);
        Wenet(const char* modeldir,const char* modelid);
        Wenet(const char* modelfn);
        ~Wenet();
};
