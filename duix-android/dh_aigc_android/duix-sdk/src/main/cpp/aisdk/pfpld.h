#pragma once
#include "jmat.h"
#include "net.h"
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <stdio.h>
#include <vector>

class Pfpld{
    private:
        ncnn::Net pfpld;
        JMat* m_mat112;
        int scale_w = 112;
        int scale_h = 112;
        float mean_vals[3] = {0.f, 0.f, 0.f};
        float norm_vals[3] = {1 / 255.f, 1 / 255.f, 1 / 255.f};
        int m_width;
        int m_height;
        float m_wh;
        int m_cnt;
        void recal(int w,int h);
    public:
        int detect(JMat* pic,int* arrboxs,int* arrlands);
        Pfpld(const char* modeldir,const char* modelid,int w,int h);
        ~Pfpld();
};
