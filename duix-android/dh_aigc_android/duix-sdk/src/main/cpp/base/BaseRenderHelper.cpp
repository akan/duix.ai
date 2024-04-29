#include <LoopThread.h>
#include "BaseRenderHelper.h"

BaseRenderHelper::BaseRenderHelper(FrameSource *source, MessageSource *messageSource, int mediaCodecFlag, bool hasAudio, bool hasVideo) {
    this->hasAudio = hasAudio;
    this->hasVideo = hasVideo;
    this->messageSource = messageSource;
}

BaseRenderHelper::~BaseRenderHelper() {
}

void BaseRenderHelper::setNativeWindow(NativeWindowType window) {
    nativeWindow = window;
}

void BaseRenderHelper::setAudioTrack(AudioTrack *track) {
}

void BaseRenderHelper::setVideoParams(int width, int height) {
    videoWidth = width;
    videoHeight = height;
}

void BaseRenderHelper::setAudioParams(int sampleRate, int channels, int format, int bytesPerSample) {
}

void BaseRenderHelper::initAudioRenderer() {
}

void BaseRenderHelper::initVideoRenderer() {
}

int BaseRenderHelper::renderAudio(int arg1, long arg2) {
    return 0;
}

int BaseRenderHelper::renderVideo(int arg1, long arg2) {
    return 0;
}

void BaseRenderHelper::releaseAudioRenderer() {
}

void BaseRenderHelper::releaseVideoRenderer() {
}
