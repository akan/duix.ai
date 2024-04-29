#pragma once
#include "jmat.h"
#include "net.h"
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <stdio.h>
#include <vector>

struct FaceObject
{
    cv::Rect_<float> rect;
    cv::Point2f landmark[5];
    float prob;
};

class Scrfd{
    private:
        ncnn::Net scrfd;

        int     target_size = 640;
        float   prob_threshold = 0.3f;
        float   nms_threshold = 0.45f;
        float mean_vals[3] = {127.5f, 127.5f, 127.5f};
        float norm_vals[3] = {1 / 128.f, 1 / 128.f, 1 / 128.f};
        float   scale;
        int     scale_w;
        int     scale_h;
        int     wpad;
        int     hpad;
        int m_width;
        int m_height;
        void recal(int nw ,int nh);
    public:
        int detect(JMat* pic,int* boxs);
        Scrfd(const char* modeldir,const char* modelid,int cols,int rows);
        ~Scrfd();
};
