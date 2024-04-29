#include "ghmat.h"
#ifdef _TENSORRT_
#include <cuda.h>
#include <cuda_runtime.h>
#include <cuda_runtime_api.h>
#include "ppl/cv/cuda/resize.h" 
#include "ppl/cv/cuda/convertto.h" 
#include "ppl/cv/cuda/split.h" 
#include "ppl/cv/cuda/merge.h" 
#include "ppl/cv/cuda/convertto.h"
#include "cudautil.h"

int  GhMat::vtacc(GpMat mat,void* stream ){
    if(m_dev!=mat->m_dev)return -1;
    cavtacc((cudaStream_t)stream,m_height,m_width,m_stride,(float*)m_buf,  mat->m_stride, (float*)mat->m_buf);
}

int  GhMat::resize(GpMat mat,void* stream){
    if(m_dev!=mat->m_dev)return -1;
    ppl::cv::cuda::Resize<float, 3>((cudaStream_t)stream,m_height,m_width,m_stride,(float*)m_buf, mat->m_height, mat->m_width, mat->m_stride, (float*)mat->m_buf);
    return 0;
}

int  GhMat::img2hwc(GpMat mat,void* stream){
    if(m_dev!=mat->m_dev)return -1;
    if(m_channel!=3)return -2;
    float* gpu_split = (float*)mat->m_buf;
    float* gpu_s0 = gpu_split ;
    int line = mat->m_stride*mat->m_height;
    float* gpu_s1 = gpu_split + line;
    float* gpu_s2 = gpu_split + line*2;
    ppl::cv::cuda::Split3Channels<float>((cudaStream_t)stream,m_height, m_width,m_stride, (float*)m_buf, mat->m_width,gpu_s2,gpu_s1,gpu_s0);
    return 0;
}

int  GhMat::hwc2img(GpMat mat,void* stream){
    if(m_dev!=mat->m_dev)return -1;
    if(m_channel!=3)return -2;
    return 0;
}

int  GhMat::downzero(GpMat mat,void* stream){
    if(m_dev!=mat->m_dev)return -1;
    if((m_height!=mat->m_height)||(m_width!=mat->m_width))return -2;
    ppl::cv::cuda::ConvertTo<float,float,3>((cudaStream_t)stream,m_height,m_width,m_stride,(const float*)m_buf, mat->m_stride,(float*)mat->m_buf, 2.0f,-1.0f);
    return 0;
}

int  GhMat::upzero(GpMat mat ,void* stream){
    if(m_dev!=mat->m_dev)return -1;
    if((m_height!=mat->m_height)||(m_width!=mat->m_width))return -2;
    ppl::cv::cuda::ConvertTo<float,float,3>((cudaStream_t)stream,m_height,m_width,m_stride,(const float*)m_buf, mat->m_stride,(float*)mat->m_buf, 0.5f,0.5f);
    return 0;
}


int GhMat::filtermask(GpMat msk, GpMat mat,void* stream) {
    if(!msk)return -1;
    if((msk->width()!=m_width) ||(msk->height()!=m_height))return -2;
    return  cafiltermsk((cudaStream_t)stream,m_height,m_width,m_stride,(float*)m_buf, msk->m_stride,(float*)msk->m_buf, mat->m_stride,  (float*)mat->m_buf);
}

int GhMat::clamp(float minval,float maxval,GpMat mat,void* stream ){
    if(!mat)return -1;
    return  caclamp((cudaStream_t)stream,m_height,m_width,m_stride,(float*)m_buf,  mat->m_stride,  (float*)mat->m_buf,minval,maxval);
}

int GhMat::blend(GpMat msk,GpMat bgmat,GpMat dstmat,void* stream ){
    if(!msk)return -1;
    if((msk->width()!=m_width) ||(msk->height()!=m_height))return -2;
    return  cablendmsk((cudaStream_t)stream,m_height,m_width,m_stride,(float*)m_buf, msk->m_stride,(float*)msk->m_buf, bgmat->m_stride,(float*)bgmat->m_buf,dstmat->m_stride,  (float*)dstmat->m_buf);
}

int GhMat::wherezero(GpMat msk, GpMat bgmat, GpMat dstmat,void* stream) {
    if(!msk)return -1;
    if((msk->width()!=m_width) ||(msk->height()!=m_height))return -2;
    return  cablendzero((cudaStream_t)stream,m_height,m_width,m_stride,(float*)m_buf, msk->m_stride,(float*)msk->m_buf, bgmat->m_stride,(float*)bgmat->m_buf,dstmat->m_stride,  (float*)dstmat->m_buf);
}

int GhMat::cvtfloat(GpMat dst,void* stream) {
    ppl::cv::cuda::ConvertTo<uint8_t,float,3>((cudaStream_t)stream,m_height,m_width,m_width*m_channel,(uint8_t*)m_buf, dst->m_stride,(float*)dst->m_buf, 1.0/255.0f,0.f);
    return 0;
}

int GhMat::cvtuint8(GpMat dst,void* stream) {
    ppl::cv::cuda::ConvertTo<float,uint8_t,3>((cudaStream_t)stream,m_height,m_width,m_stride,(float*)m_buf, dst->m_stride,(uint8_t*)dst->m_buf, 255.0f,0.f);
    return 0;
}

static void *cudaalloc(size_t sz){
    void* pmem = NULL;
    cudaMalloc(&pmem,sz);
    return pmem;
}

static void cudafree(void* ptr) {
    if(ptr)cudaFree(ptr);
}

static void *cudaset(void* s,int ch,size_t n){
    cudaMemset(s,ch,n);
    return s;
}

static void *cudacpy(void* dst,const void* src,size_t n){
    cudaMemcpy(dst,src,n,cudaMemcpyDeviceToDevice);
    return dst;
}

static int cudaext(void* dst,const void* src,size_t n,int h2d,void* sync){
    if(sync){
        if(h2d){
            printf("====host to dev async\n");
            cudaMemcpyAsync(dst,src,n,cudaMemcpyHostToDevice,(cudaStream_t)sync);
        }else{
            cudaMemcpyAsync(dst,src,n,cudaMemcpyDeviceToHost,(cudaStream_t)sync);
        }
    }else{
        if(h2d){
            cudaMemcpy(dst,src,n,cudaMemcpyHostToDevice);
        }else{
            printf("====dev to host\n");
            cudaMemcpy(dst,src,n,cudaMemcpyDeviceToHost);
        }
    }
    return 0;
}

void GhMat::initgh(){
    m_dev = 1;
    dhalloc = cudaalloc;
    dhfree = cudafree;
    dhset = cudaset;
    dhcpy = cudacpy;
    dhext = cudaext;
}


GhMat::GhMat(DpMat mat):DhMat(){
    initgh();
    m_width = mat->width();
    m_height = mat->height();
    m_stride = mat->stride(); 
    m_channel = mat->channel();;
    m_cnt = mat->count();
    m_size = mat->size();
    if(mat->dev() != m_dev){
        m_ref = 0;
        m_buf = (float*)dhalloc(m_size+m_stride*sizeof(float));
        m_devhost = mat;
    }else{
        m_buf = mat->buffer();
        m_ref = 1;
    }
}

GhMat::GhMat(GpMat mat):DhMat(){
    initgh();
    m_width = mat->width();
    m_height = mat->height();
    m_stride = mat->stride(); 
    m_channel = mat->channel();;
    m_cnt = mat->count();
    m_size = mat->size();
    if(mat->dev() != m_dev){
        m_ref = 0;
        m_buf = (float*)dhalloc(m_size+m_stride*sizeof(float));
        m_devhost = mat;
    }else{
        m_buf = mat->buffer();
        m_ref = 1;
    }
}

GhMat::GhMat(int w,int h,float *buf ,int c  ,int d ):DhMat(w,h,buf,c,d){
    initgh();
}

GhMat::GhMat(int w,int h,int c  ,int d):DhMat(){
    initgh();
    m_width = w;
    m_height = h;
    m_stride = d?d:w*c;
    m_channel = c;
    m_cnt = m_stride*m_height;
    m_size = m_cnt*sizeof(float);
    m_ref = 0;
    m_buf = (float*)dhalloc(m_size+d*sizeof(float));
}

GhMat::~GhMat(){
    if(!m_ref&&m_buf)dhfree(m_buf);
}
#endif
