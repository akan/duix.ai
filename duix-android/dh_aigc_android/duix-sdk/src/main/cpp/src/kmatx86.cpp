#include "kmat.h"


#include "ppl/cv/x86/resize.h" 
#include "ppl/cv/x86/convertto.h" 
#include "ppl/cv/x86/split.h" 
#include "ppl/cv/x86/merge.h" 
#include "ppl/cv/x86/convertto.h"

int  KMat::vtacc(KMat* mat){
    float* pt0 = (float*)m_buf;
    const int stride = 532*532;
    uint8_t pct[stride];
    float* p0 = pt0;
    float* p1 = p0+stride;
    float* p2 = p1+stride;
    float* psum = mat->fdata();
    float* pd = psum;
    for(int k=0;k<stride;k++){
        *pd++ = *p0+++*p1+++*p2++;
    }
    float* pmean = psum + stride;
    float* pt = pmean;
    pd = psum;
    p1 = pt0+stride;
    for(int k=0;k<stride;k++){
        *pt++ = (*pd++-*p1++)*0.5f;
    }
    uint8_t* pc = pct;
    p1 = pt0+stride;
    pt = pmean;
    for(int k=0;k<stride;k++){
        *pc++ = *p1++>*pt++; 
    }
    memcpy(psum,pt0,stride*sizeof(float));
    memcpy(psum+2*stride,pt0+2*stride,stride*sizeof(float));
    //float* ped = pt0+stride;
    pc = pct;
    p1 = pt0+stride;
    pt = pmean;
    //pd = ped;
    for(int k=0;k<stride;k++){
        if(*pc++){
            pt++;p1++;
        }else{
            *pt++ = *p1++;
        }
        //*pd++ = *pc?*pt++:*p1++;
    }
    return 0;
}

int  KMat::resize(KMat* mat){
    if(m_dev!=mat->m_dev)return -1;
    ppl::cv::x86::ResizeLinear<float, 3>(m_height,m_width,m_stride,(float*)m_buf, mat->m_height, mat->m_width, mat->m_stride, (float*)mat->m_buf);
    return 0;
}

int  KMat::img2hwc(KMat* mat){
    if(m_dev!=mat->m_dev)return -1;
    if(m_channel!=3)return -2;
    float* gpu_split = (float*)mat->m_buf;
    float* gpu_s0 = gpu_split ;
    int line = mat->m_stride*mat->m_height;
    float* gpu_s1 = gpu_split + line;
    float* gpu_s2 = gpu_split + line*2;
    ppl::cv::x86::Split3Channels<float>(m_height, m_width,m_stride, (float*)m_buf, mat->m_width,gpu_s2,gpu_s1,gpu_s0);
    return 0;
}

int  KMat::hwc2img(KMat* mat){
    if(m_dev!=mat->m_dev)return -1;
    if(m_channel!=3)return -2;
    return 0;
}

int  KMat::downzero(KMat* mat){
    if(m_dev!=mat->m_dev)return -1;
    if((m_height!=mat->m_height)||(m_width!=mat->m_width))return -2;
    if(!mat){
        float* dst = (float*)m_buf;
        for(int k=0;k< m_height;k++){
            float* line = dst;
            for(int m=0;m<m_width*m_channel;m++){
                *line = *line *2.0f-1.0f;
                line++;
            }
            dst+=m_stride;
        }
    }else{
        float* src = (float*)m_buf;
        float* dst = (float*)mat->m_buf;
        for(int k=0;k< m_height;k++){
            float* ls = src;
            float* ld = dst;
            for(int m=0;m<m_width*m_channel;m++){
                *ld++ = *ls++ *2.0f-1.0f;
            }
            src+= m_stride;
            dst+= mat->m_stride;
        }
    }
    return 0;
}

int  KMat::upzero(KMat* mat ){
    if(m_dev!=mat->m_dev)return -1;
    if((m_height!=mat->m_height)||(m_width!=mat->m_width))return -2;
    if(!mat){
        float* dst = (float*)m_buf;
        for(int k=0;k< m_height;k++){
            float* line = dst;
            for(int m=0;m<m_width*m_channel;m++){
                *line = (*line+1.0f) /2.0f;
                line++;
            }
            dst+=m_stride;
        }
    }else{
        float* src = (float*)m_buf;
        float* dst = (float*)mat->m_buf;
        for(int k=0;k< m_height;k++){
            float* ls = src;
            float* ld = dst;
            for(int m=0;m<m_width*m_channel;m++){
                *ld++ = (*ls++ +1.0f) /2.0f;
            }
            src+= m_stride;
            dst+= mat->m_stride;
        }
    }
    return 0;
}

int KMat::filtermask(KMat* msk, KMat* mat){
    if(!msk)return -1;
    if((msk->width()!=m_width) ||(msk->height()!=m_height))return -2;
    if(mat==nullptr){
        float* dst = (float*)m_buf;
        float* mk = (float*)msk->m_buf;
        if(msk->m_channel==3) {
            for (int k = 0; k < m_height; k++) {
                float *line = dst;
                float *mm = mk;
                for (int m = 0; m < m_width; m++) {
                    *line = *line * (*mm);
                    line++;
                    mm++;
                }
                dst += m_stride;
                mk += msk->m_stride;
            }
        }else {
            for (int k = 0; k < m_height; k++) {
                float *line = dst;
                float *mm = mk;
                for (int m = 0; m < m_width; m++) {
                    for(int n=0;n<m_channel;n++){
                        *line = *line * (*mm);
                        line++;
                    }
                    mm++;
                }
                dst += m_stride;
                mk += msk->m_stride;
            }
        }
    }else{

        if(mat->width()!=msk->width()||mat->height()!=msk->height()) return -3;

        float* src = (float*)m_buf;
        float* dst = (float*)mat->m_buf;
        float* mk = (float*)msk->m_buf;
        if(msk->m_channel==3) {
            for (int k = 0; k < m_height; k++) {
                float *line = dst;
                float *mm = mk;
                float *ms = src;
                for (int m = 0; m < m_width; m++) {
                    *line++ = *ms++ * (*mm++);
                }
                dst += mat->m_stride;
                src += m_stride;
                mk += msk->m_stride;
            }
        }else{
            for (int k = 0; k < m_height; k++) {
                float *line = dst;
                float *mm = mk;
                float *ms = src;
                for (int m = 0; m < m_width; m++) {
                    for(int n=0;n<m_channel;n++){
                        *line++ = *ms++ * (*mm);
                    }
                    mm++;
                }
                dst += mat->m_stride;
                src += m_stride;
                mk += msk->m_stride;
            }
        }
    }
    return 0;
}

int KMat::clamp(float minval,float maxval,KMat* mat){
    if(!mat){
        float* dst = (float*)m_buf;
        for(int k=0;k< m_height;k++){
            float* line = dst;
            for(int m=0;m<m_width*m_channel;m++){
                if(*line<minval){
                    *line = minval;
                }else if(*line>maxval){
                    *line = maxval;
                }
                line++;
            }
            dst+=m_stride;
        }
    }else{
        float* src = (float*)m_buf;
        float* dst = (float*)mat->m_buf;
        for(int k=0;k< m_height;k++){
            float* ls = src;
            float* ld = dst;
            for(int m=0;m<m_width*m_channel;m++){
                if(*ls<minval){
                    *ls = minval;
                }else if(*ls>maxval){
                    *ls = maxval;
                }else{
                    *ld = *ls;
                }
                *ld++ = *ls++;
            }
            src+= m_stride;
            dst+= mat->m_stride;
        }
    }
    return 0;
}

int KMat::blend(KMat* msk,KMat* bgmat,KMat* dstmat){
    if(!msk)return -1;
    if((msk->width()!=m_width) ||(msk->height()!=m_height))return -2;
    if(dstmat==nullptr){
        float* dst = (float*)m_buf;
        float* mk = (float*)msk->m_buf;
        float* bg = (float*)bgmat->m_buf;
        if(msk->m_channel==3) {
            for (int k = 0; k < m_height; k++) {
                float *dd = dst;
                float *mm = mk;
                float *bb = bg;
                for (int m = 0; m < m_width; m++) {
                    float ratio = *mm;
                    *dd = *dd * ratio + *bb*(1-ratio);
                    dd++; mm++; bb++;
                }
                dst += m_stride;
                mk += msk->m_stride;
                bg += bgmat->m_stride;
            }
        }else {
            for (int k = 0; k < m_height; k++) {
                float *dd = dst;
                float *mm = mk;
                float *bb = bg;
                for (int m = 0; m < m_width; m++) {
                    float ratio = *mm;
                    for(int n=0;n<m_channel;n++){
                        *dd = *dd * ratio + *bb*(1-ratio);
                        dd++;
                        bb++;
                    }
                    mm++;
                }
                dst += m_stride;
                mk += msk->m_stride;
                bg += bgmat->m_stride;
            }
        }
    }else{
        if(dstmat->width()!=msk->width()||dstmat->height()!=msk->height()) return -3;

        float* src = (float*)m_buf;
        float* dst = (float*)dstmat->m_buf;
        float* mk = (float*)msk->m_buf;
        float* bg = (float*)bgmat->m_buf;
        if(msk->m_channel==3) {
            for (int k = 0; k < m_height; k++) {
                float *dd = dst;
                float *mm = mk;
                //float *ss = src;
                float *bb = bg;
                for (int m = 0; m < m_width; m++) {
                    float ratio = *mm;
                    *dd = *dd * ratio + *bb*(1-ratio);
                    dd++; mm++; bb++;
                }
                dst += dstmat->m_stride;
                src += m_stride;
                mk += msk->m_stride;
                bg += bgmat->m_stride;
            }
        }else{
            for (int k = 0; k < m_height; k++) {
                float *dd = dst;
                float *mm = mk;
                //float *ss = src;
                float *bb = bg;
                for (int m = 0; m < m_width; m++) {
                    float ratio = *mm;
                    for(int n=0;n<m_channel;n++){
                        *dd = *dd * ratio + *bb*(1-ratio);
                        src += m_stride;
                        dd++; bb++;
                    }
                    mm++;
                }
                dst += dstmat->m_stride;
                src += m_stride;
                mk += msk->m_stride;
                bg += bgmat->m_stride;
            }
        }
    }
    return 0;
}

int KMat::wherezero(KMat* msk, KMat* bgmat, KMat* dstmat){
    if(!msk)return -1;
    if((msk->width()!=m_width) ||(msk->height()!=m_height))return -2;
    if(dstmat==nullptr){
        float* dst = (float*)m_buf;
        float* mk = (float*)msk->m_buf;

        float* bg = (float*)bgmat->m_buf;
        if(msk->m_channel==3) {
            for (int k = 0; k < m_height; k++) {
                float *dd = dst;
                float *mm = mk;
                float *bb = bg;
                for (int m = 0; m < m_width; m++) {
                    float ratio = *mm;
                    *dd = ratio==0?*dd:*bb;
                    dd++; mm++; bb++;
                }
                dst += m_stride;
                mk += msk->m_stride;
                bg += bgmat->m_stride;
            }
        }else {
            for (int k = 0; k < m_height; k++) {
                float *dd = dst;
                float *mm = mk;
                float *bb = bg;
                for (int m = 0; m < m_width; m++) {
                    float ratio = *mm;
                    for(int n=0;n<m_channel;n++){
                        *dd = ratio==0?*dd:*bb;
                        dd++;
                        bb++;
                    }
                    mm++;
                }
                dst += m_stride;
                mk += msk->m_stride;
                bg += bgmat->m_stride;
            }
        }
    }else{
        if(dstmat->width()!=msk->width()||dstmat->height()!=msk->height()) return -3;

        float* src = (float*)m_buf;
        float* dst = (float*)dstmat->m_buf;
        float* mk = (float*)msk->m_buf;
        float* bg = (float*)bgmat->m_buf;
        if(msk->m_channel==3) {
            for (int k = 0; k < m_height; k++) {
                float *dd = dst;
                float *mm = mk;
                //float *ss = src;
                float *bb = bg;
                for (int m = 0; m < m_width; m++) {
                    float ratio = *mm;
                    *dd = ratio==0?*dd:*bb;
                    dd++; mm++; bb++;
                }
                dst += dstmat->m_stride;
                src += m_stride;
                mk += msk->m_stride;
                bg += bgmat->m_stride;
            }
        }else{
            for (int k = 0; k < m_height; k++) {
                float *dd = dst;
                float *mm = mk;
                //float *ss = src;
                float *bb = bg;
                for (int m = 0; m < m_width; m++) {
                    float ratio = *mm;
                    for(int n=0;n<m_channel;n++){
                        *dd = ratio==0?*dd:*bb;
                        src += m_stride;
                        dd++; bb++;
                    }
                    mm++;
                }
                dst += dstmat->m_stride;
                src += m_stride;
                mk += msk->m_stride;
                bg += bgmat->m_stride;
            }
        }
    }
    return 0;
}

int KMat::cvtfloat(KMat* dst,float scale,float delta){
    ppl::cv::x86::ConvertTo<uint8_t,3,float>(m_height,m_width,m_width*m_channel,(uint8_t*)m_buf
        ,scale,m_width*m_channel,(float*)m_buf);
    return 0;
}

int KMat::cvtuint8(KMat* dst,float scale,float delta){
    ppl::cv::x86::ConvertTo<float,3,uint8_t>(m_height,m_width,m_width*m_channel,(float*)m_buf ,scale,m_width*m_channel,(uint8_t*)m_buf);
    return 0;
}

void KMat::initgh(){
    m_dev = 0;
}

KMat::KMat(int w,int h,float *buf ,int c  ,int d ):JMat(w,h,buf,c,d){
    //
}

KMat::KMat(int w,int h,uint8_t *buf ,int c  ,int d ):JMat(w,h,buf,c,d){
    //
}

KMat::KMat(int w,int h,int c ,int d ,int b):JMat(w,h,c,d,b){
}


KMat::~KMat(){
    //
}

