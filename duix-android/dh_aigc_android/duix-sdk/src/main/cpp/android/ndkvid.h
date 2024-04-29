#pragma once
#include <android/log.h>
#include <android/native_window.h>
#include "jmat.h"
#include <android/native_window_jni.h>
#include <android/surface_texture.h>
#include <media/NdkMediaCodec.h>
#include <media/NdkMediaExtractor.h>
#define FRAME_DIST 40000

class MyVideo {
    private:

        char    filename[1024];

        int32_t     frameHeight = 0;
        int32_t     frameWidth = 0;
        int32_t     frameRate = 0;
        int32_t     duration  = 0;
        int32_t     position  = 0;

        int64_t renderstart;
        bool sawInputEOS = false;
        bool sawOutputEOS = false;
        bool isPlaying = false;
        bool renderonce = true;

        AMediaExtractor *ex = nullptr;
        AMediaCodec *codec = nullptr;

        void indexsample(long time);
        void     reset();
        int pre();
    public:
        int32_t getFrameHeight() const{return frameHeight;};
        int32_t getFrameWidth() const{return frameWidth;};
        int32_t getDuration() const{return duration;};
        int32_t getPosition() const{return position;};
        bool    isEof(){return sawOutputEOS||sawInputEOS ;}; 
        bool    getPlaying(){return isPlaying ;}; 

        int open();
        int close();

        int next(JMat* mat);

        MyVideo(const char* videofile);
        ~MyVideo();
};
