#include <Log.h>
#include <thread>
#include "FrameSource.h"

#define TAG "FrameSource"

FrameSource::FrameSource(int audioMaxSize, int videoMaxSize) {
    LOGI(TAG, "CoreFlow : create FrameSource");
    audioPacketQueue = new ConcurrentQueue<MediaData*>(audioMaxSize, "AudioFrameQueue");
    videoPacketQueue = new ConcurrentQueue<MediaData*>(videoMaxSize, "VideoFrameQueue");
    videoRecyleQueue = new ConcurrentQueue<JMat*>(videoMaxSize, "VideoRecyleQueue",1);
}

FrameSource::~FrameSource() {
    LOGI(TAG, "CoreFlow : FrameSource destroyed %d %d",
         audioPacketQueue->size(), videoPacketQueue->size());
    delete audioPacketQueue;
    audioPacketQueue = nullptr;
    delete videoPacketQueue;
    videoPacketQueue = nullptr;
    delete videoRecyleQueue;
    videoRecyleQueue = nullptr;
}

unsigned long FrameSource::pushAudFrame(MediaData *frame) {
    LOGD(TAG, "pushAudFrame %lld", frame->pts);
    auto desBuffer = new MediaData();
    //MediaHelper::copy(frame, desBuffer);
    desBuffer->copy(frame);
    return audioPacketQueue->push(desBuffer);
}

unsigned long FrameSource::pushVidFrame(MediaData *frame) {
    LOGD(TAG, "pushVidFrame %lld", frame->pts);
    auto desBuffer = new MediaData();
    //MediaHelper::copy(frame, desBuffer);
    desBuffer->copy(frame);
    return videoPacketQueue->push(desBuffer);
}

unsigned long FrameSource::pushVidRecyle(JMat *frame) {
    if(!frame)return 0;
    return videoRecyleQueue->push(frame);
}

unsigned long FrameSource::readAudFrame(MediaData **frame) {
    unsigned long size = audioPacketQueue->front(frame);
    if (size > 0) {
        LOGD(TAG, "readAudFrame %lld", (*frame)->pts);
    }
    return size;
}

unsigned long FrameSource::readVidFrame(MediaData **frame) {
    unsigned long size = videoPacketQueue->front(frame);
    if (size > 0) {
        LOGD(TAG, "readVidFrame %lld", (*frame)->pts);
    }
    return size;
}

unsigned long FrameSource::popVidRecyle(JMat **frame) {
    unsigned long size = videoRecyleQueue->front(frame);
    if (size > 0) {
        videoRecyleQueue->pop();
    }
    return size;
}


void FrameSource::popAudFrame(MediaData *frame) {
    LOGD(TAG, "popAudFrame %lld", frame->pts);
    audioPacketQueue->pop();
}

void FrameSource::popVidFrame(MediaData *frame) {
    LOGD(TAG, "popVidFrame %lld", frame->pts);
    videoPacketQueue->pop();
}

void FrameSource::flush() {
    audioPacketQueue->flush();
    videoPacketQueue->flush();
    videoRecyleQueue->flush();
    LOGI(TAG, "flushBuffer");
}

void FrameSource::reset() {
    audioPacketQueue->reset();
    videoPacketQueue->reset();
    videoRecyleQueue->reset();
}

unsigned long FrameSource::audioSize() {
    return audioPacketQueue->size();
}

unsigned long FrameSource::videoSize() {
    return videoPacketQueue->size();
}

unsigned long FrameSource::recyleSize() {
    return videoRecyleQueue->size();
}
