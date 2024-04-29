#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <stdio.h>
#include <memory>
#include <string>
#include <vector>
#include <string.h>

//#include "NumCpp.hpp"
#define USE_OPENCV
#define USE_NCNN
#define USE_TURBOJPG

#ifdef USE_OPENCV
#include "opencv2/core.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"
#endif

#ifdef USE_NCNN
#include "mat.h"
#endif

#ifdef USE_EIGEN
#include "eigen3/Eigen/Core"
typedef Eigen::Matrix<float, 1, Eigen::Dynamic, Eigen::RowMajor> Vectorf;
typedef Eigen::Matrix<std::complex<float>, 1, Eigen::Dynamic, Eigen::RowMajor> Vectorcf;
typedef Eigen::Matrix<float, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> Matrixf;
typedef Eigen::Matrix<std::complex<float>, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> Matrixcf;
#endif

class JBuf{
    protected:
        bool        m_ref = 0;
        uint32_t    m_size = 0;
        void*       m_buf = NULL;
    public:
        uint32_t    size(){return m_size;} ;
        void*       data(){return m_buf;};
        bool        ref(){return m_ref;};
        int         zeros();
        int         forceref(int bref);
        JBuf();
        JBuf(uint32_t size,void* buf = nullptr);
        virtual ~JBuf();
};

class JMat:public JBuf{
    protected:
        int     m_bit = 0;
        int     m_width = 0;
        int     m_height = 0;
        int     m_channel = 0;
        int     m_stride = 0;
        int     m_tagarr[512];
        void    init_tagarr();
    public:
        int height(){return m_height;}
        int width(){return m_width;}
        int stride(){return m_stride;}
        int channel(){return m_channel;}
        JMat(int w,int h,float *buf ,int c = 3 ,int d = 0);
        JMat(int w,int h,uint8_t *buf ,int c = 3 ,int d = 0);
        JMat(int w,int h,int c = 3,int d = 0,int b=0);
        JMat(std::string picfile,int flag=0);
        JMat();
        int load(std::string picfile);
        int loadjpg(std::string picfile,int flag=0);
        int savegpg(std::string gpgfile);
        int loadgpg(std::string gpgfile);
        float* fdata();
        float* frow(int row);
        float* fitem(int row,int col);
        int tojpg(const char* fn);
        int tobin(const char* fn);
        int show(const char* title);
        JMat clone();
        JMat* refclone(int ref=1);
        uint8_t* udata();
        virtual ~JMat();
        int*    tagarr();
        //nc::NdArray<float> ncarray();
#ifdef USE_OPENCV
        cv::Mat             cvmat();
#endif
#ifdef USE_NCNN
        ncnn::Mat           ncnnmat();
        ncnn::Mat           packingmat();
#endif
        //Matrixf  tomatrix();
};
