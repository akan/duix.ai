#pragma once
#include "jmat.h"


class KMat:public JMat{
    private:
        int m_dev = 0;
        virtual void initgh();
    public:
        virtual int  vtacc(KMat* mat);

        virtual int  resize(KMat* mat);
        virtual int  img2hwc(KMat* mat);
        virtual int  hwc2img(KMat* mat);

        virtual int cvtfloat(KMat* dst,float scale = 1/255.0f,float delta = 0.0f);
        virtual int cvtuint8(KMat* dst,float scale = 255.f,float delta = 0.0f);

        virtual int  downzero(KMat* mat = nullptr);
        virtual int  upzero(KMat* mat = nullptr);
        virtual int  filtermask(KMat* msk,KMat* mat = nullptr);
        virtual int clamp(float minval,float maxval,KMat* mat = nullptr);
        virtual int blend(KMat* msk,KMat* bgmat,KMat* dstmat = nullptr);
        virtual int wherezero(KMat* msk,KMat* bgmat,KMat* dstmat = nullptr);

    public:
        KMat(int w,int h,float *buf ,int c = 3 ,int d = 0);
        KMat(int w,int h,uint8_t *buf ,int c = 3 ,int d = 0);
        KMat(int w,int h,int c = 3,int d = 0,int b=0);
        virtual ~KMat();
};

