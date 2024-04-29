
#include <LoopThread.h>
#include <SurfaceVideoRenderer.h>
#include <XTick.h>
#include "RenderHelper.h"

RenderHelper::RenderHelper(FrameSource *source, MessageSource *messageSource, int mediaCodecFlag, bool hasAudio, bool hasVideo):BaseRenderHelper(source,messageSource,mediaCodecFlag,hasAudio,hasVideo) {
    this->hasAudio = hasAudio;
    this->hasVideo = hasVideo;
    if (hasAudio) {
        audioRenderer = new AudioRenderer(source);
    }
    if (hasVideo) {
        if (mediaCodecFlag) {
            videoRenderer = new SurfaceVideoRenderer(source);
        } else {
            videoRenderer = new RgbVideoRenderer(source);
        }
    }
    avSyncMethod = hasAudio ? FELLOW_AUDIO : FELLOW_CLOCK;
    this->messageSource = messageSource;
}

RenderHelper::~RenderHelper() {
    delete audioRenderer;
    delete videoRenderer;
}

void RenderHelper::setNativeWindow(NativeWindowType window) {
    nativeWindow = window;
}

void RenderHelper::setAudioTrack(AudioTrack *track) {
    audioRenderer->setAudioTrack(track);
}

void RenderHelper::setVideoParams(int width, int height) {
    videoWidth = width;
    videoHeight = height;
}

void RenderHelper::setAudioParams(int sampleRate, int channels, int format, int bytesPerSample) {
    audioRenderer->setAudioParams(sampleRate, channels, format, bytesPerSample);
}

void RenderHelper::initAudioRenderer() {
    audioRenderer->init();
    hasNotifyFirstFrame = false;
}

void RenderHelper::initVideoRenderer() {
    videoRenderer->surfaceCreated(nativeWindow, videoWidth, videoHeight);
    hasNotifyFirstFrame = false;
    startPts = getTickCount64();
}

int RenderHelper::renderAudio(int arg1, long arg2) {
    uint64_t temp = audioRenderer->render(nowPts);
    if (temp > 0) {
        if (!hasNotifyFirstFrame) {
            hasNotifyFirstFrame = true;
            messageSource->pushMessage(MSG_DOMAIN_STATE, STATE_STARTED, 0);
        }
        nowPts = temp;
        int nowPtsSecond = nowPts / 1000 / 1000;
        messageSource->pushMessage(MSG_DOMAIN_TIME, nowPtsSecond, 0);
    } else {
        if (stopWhenEmpty) {
            stopWhenEmpty = false;
            return ERROR_EXIST;
        }
    }
    return 0;
}

int RenderHelper::renderVideo(int arg1, long arg2) {
    if (!videoRenderer->isRenderValid()) {
        return ERROR_EXIST;
    }
    uint64_t temp;
    if (avSyncMethod == FELLOW_CLOCK) {
        nowPts = (getTickCount64() - startPts) * 1000 + firstFramePts;
        int nowPtsSecond = nowPts / 1000 / 1000;
        messageSource->pushMessage(MSG_DOMAIN_TIME, nowPtsSecond, 0);
        //确保第一帧快速被渲染
        temp = videoRenderer->render(firstFramePts != 0 ? nowPts : MAX_PTS);
    } else {
        temp = videoRenderer->render(nowPts);
    }
    if (temp > 0) {
        if (!hasNotifyFirstFrame) {
            firstFramePts = temp;
            hasNotifyFirstFrame = true;
            messageSource->pushMessage(MSG_DOMAIN_STATE, STATE_STARTED, 0);
        }
    } else {
        if (stopWhenEmpty) {
            stopWhenEmpty = false;
            return ERROR_EXIST;
        }
    }
    return 0;
}

void RenderHelper::releaseAudioRenderer() {
    audioRenderer->release();
}

void RenderHelper::releaseVideoRenderer() {
    videoRenderer->surfaceDestroyed();
}
