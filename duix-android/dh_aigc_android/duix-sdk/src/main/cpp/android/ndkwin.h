#pragma once
#include <android/looper.h>
#include <android/native_window.h>
#include <android/sensor.h>
#include <media/NdkImageReader.h>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <string>
#include <android/log.h>
#include "mat.h"
#include "jmat.h"
#include "benchmark.h"

class MyDisp {
    private:
        ANativeWindow *win = nullptr;
        ASensorManager* sensor_manager;
        mutable ASensorEventQueue* sensor_event_queue;
        const ASensor* accelerometer_sensor;
        int draw_text(cv::Mat& rgb,const char* text);
        int draw_fps(cv::Mat& rgb);
    public:
        mutable int accelerometer_orientation;
        int drawMat(JMat* mat,const char* text);
        MyDisp();
        virtual ~MyDisp();
        int set_window( ANativeWindow* newwin);
        int reset_fps();
};

