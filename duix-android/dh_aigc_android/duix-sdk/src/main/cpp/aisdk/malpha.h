#pragma once
#include "jmat.h"
#include "net.h"
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <stdio.h>
#include <vector>
#include "aimodel.h"

class MWorkMat{
    private:
        int     m_boxx;
        int     m_boxy;
        int     m_boxwidth;
        int     m_boxheight;
        JMat*   m_pic;
        JMat*   m_msk;

        JMat*   pic_real160;//blendimg
        JMat*   pic_mask160;

        cv::Mat matpic_roisrc;//box area
        cv::Mat matpic_org168;
        cv::Mat matpic_roi160;
        JMat*   pic_clone160;//blendimg
        cv::Mat matpic_roirst;

        //JMat*   pic_crop160;
        //
        JMat*   msk_real160;
        //JMat*   msk_mask160;

        cv::Mat matmsk_roisrc;//box area
        cv::Mat matmsk_org168;
        cv::Mat matmsk_roi160;

        cv::Mat matmsk_roirst;

        int vtacc(uint8_t* buf,int count);
    public:
        MWorkMat(JMat* pic,JMat* msk,const int* boxs);
        int premunet();
        int munet(JMat** ppic,JMat** pmsk);
        int finmunet(JMat* fgpic=NULL);
        int prealpha();
        int alpha(JMat** preal,JMat** pimg,JMat** pmsk);
        int finalpha();

        virtual ~MWorkMat();
};

class MAlpha:public NcnnModel{
    private:
    public:
        int doModel(JMat* real,JMat* img,JMat* pha);
        MAlpha(const char* fnbin,const char* fnparam);
        virtual ~MAlpha();
};
