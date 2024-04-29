#include "GRender.h"
#include "MediaData.h"
#include "LoopThreadHelper.h"
#include <Log.h>
#include <cstdint>
#include <unistd.h>


#define TAG "tooken"

GRender::GRender(int width,int height, MessageCb *obj) {
    m_width = width;
    m_height = height;
    messageSource = new MessageSource();
    frameSource = new FrameSource(MAX_BUFFER_FRAME_SIZE,  MAX_BUFFER_FRAME_SIZE);
    renderHelper = nullptr;
    messageHelper = new MessageHelper(messageSource, obj);
    audioRenderThread = nullptr;
    videoRenderThread = nullptr;
    messageThread = nullptr;
}

GRender::~GRender() {
    delete audioRenderThread;
    delete videoRenderThread;
    delete messageThread;

    delete renderHelper;
    delete messageHelper;
    delete frameSource;
    delete messageSource;
    LOGI(TAG, "CoreFlow : GRenderImp destroyed");
}

void GRender::setSurface(NativeWindowType window) {
    nativeWindow = window;
    LOGI(TAG, "setSurface %p", nativeWindow);
}

void GRender::setAudioTrack(AudioTrack *track) {
    audioTrack = track;
    LOGI(TAG, "setAudioTrack %p", audioTrack);
}

void GRender::prepare(){
    playState = -1;
    bufferState = -1;
    isEof = false;
    onPlayStateChanged(STATE_PREPARING, 0);
    startMessageLoop();
}

void GRender::start() {
    LOGI(TAG, "CoreFlow : start playing");
    if (!nativeWindow ) {
        messageHelper->notifyObj(MSG_DOMAIN_ERROR, MSG_ERROR_RENDERING, 0, "invalid surface", nullptr);
        return;
    }
    startRendering();
    //上报开始缓冲消息，等缓冲满了再进行播放
    onBufferStateChanged(0);
}

void GRender::pause() {
    onPlayStateChanged(STATE_PAUSED, 0);
    pauseThreads( true, true);
}

void GRender::resume() {
    onPlayStateChanged(STATE_STARTED, 0);
    resumeThreads( true, true);
}


void GRender::stop() {
    LOGI(TAG, "CoreFlow : stop playing");
    stopRendering();
    stopMessageLoop();

    frameSource->flush();
    messageSource->flush();

    LOGI(TAG, "CoreFlow : player stopped %d %d %d",
            frameSource->audioSize(), frameSource->videoSize(), messageSource->size());
    onPlayStateChanged(STATE_STOPPED, 0);
}

int GRender::setVideoParam(int width,int height){
    m_width = width;
    m_height = height;
    return 0;
}

int GRender::getVideoWidth() {
    return m_width;
}

int GRender::getVideoHeight() {
    return m_height;
}


void GRender::onRenderingChanged(int state) {
    LOGI(TAG, "onRenderingChanged %d", state);
    if (state == NOTIFY_END) {
        if (isEof) {
            messageSource->pushMessage(MSG_DOMAIN_COMPLETE, 0, 0);
        }
    }
}

void GRender::onBufferStateChanged(int state) {
    playerLock.lock();
    if (state == bufferState || playState != STATE_STARTED) {
        playerLock.unlock();
        return;
    }
    LOGI(TAG, "onBufferStateChanged %d-->%d", bufferState, state);
    bufferState = state;
    messageHelper->notifyObj(MSG_DOMAIN_BUFFER, bufferState, bufferState, nullptr, nullptr);
    playerLock.unlock();
}

void GRender::onPlayStateChanged(int state, long extra) {
    playerLock.lock();
    if (state == playState) {
        playerLock.unlock();
        return;
    }
    LOGI(TAG, "onPlayStateChanged %d-->%d", playState, state);
    playState = state;
    /*
    if (playState == STATE_PREPARED) {
        hasAudio = (extra & HAS_AUDIO) == HAS_AUDIO;
        hasVideo = (extra & HAS_VIDEO) == HAS_VIDEO;
        LOGI(TAG, "hasAudio %d, hasVideo %d", hasAudio, hasVideo);
    }
    */
    messageHelper->notifyObj(MSG_DOMAIN_STATE, playState, 0, nullptr, nullptr);
    playerLock.unlock();
}

int GRender::processMessage(int arg1, long arg2) {
    Message *message;
    if (messageSource->readMessage(&message) > 0) {
        if (message->from == MSG_DOMAIN_ERROR) {
            messageHelper->handleErrorMessage(message);
        } else if (message->from == MSG_DOMAIN_STATE) {
            onPlayStateChanged(message->type, message->extra);
        } else if (message->from == MSG_DOMAIN_BUFFER) {
            onBufferStateChanged(message->type);
        } else if (message->from == MSG_DOMAIN_DEMUXING) {
            isEof = message->type == MSG_DEMUXING_EOF;
        } else if (message->from == MSG_DOMAIN_SEEK) {
            //onSeekStateChanged(message->type);
        } else {
            messageHelper->notifyObj(message->from, message->type, message->extra, nullptr, nullptr);
        }
        messageSource->popMessage();
    } else {
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }
    return 0;
}

void GRender::startMessageLoop() {
    LOGI(TAG, "startMessageLoop");
    messageThread = LoopThreadHelper::createLoopThread(
            std::bind(&GRender::processMessage, this, std::placeholders::_1, std::placeholders::_2));
}

void GRender::stopMessageLoop() {
    if (messageThread && messageThread->hasStarted()) {
        messageThread->stop();
        messageSource->reset();
        messageThread->join();
        delete messageThread;
        messageThread = nullptr;
    }
    LOGI(TAG, "message thread was stopped!");
}


void GRender::startRendering() {
    LOGI(TAG, "startRender");
    int mediaCodecFlag = 0;
    renderHelper = new RenderHelper(frameSource, messageSource, mediaCodecFlag, hasAudio, hasVideo);
    if (hasAudio) {
        int sampleRate = 16000;
        int channels = 1;
        int format = 1;
        int bytesPerSample = 16;
        renderHelper->setAudioTrack(audioTrack);
        renderHelper->setAudioParams(sampleRate, channels, format, bytesPerSample);
        audioRenderThread = LoopThreadHelper::createLoopThread(
                std::bind(&RenderHelper::initAudioRenderer, renderHelper),
                std::bind(&RenderHelper::renderAudio, renderHelper, std::placeholders::_1, std::placeholders::_2),
                std::bind(&RenderHelper::releaseAudioRenderer, renderHelper),
                std::bind(&GRender::onRenderingChanged, this, std::placeholders::_1));
    }

    if (hasVideo) {
        int width = m_width;
        int height = m_height;
        renderHelper->setNativeWindow(nativeWindow);
        renderHelper->setVideoParams(width, height);
        videoRenderThread = LoopThreadHelper::createLoopThread(
                std::bind(&RenderHelper::initVideoRenderer, renderHelper),
                std::bind(&RenderHelper::renderVideo, renderHelper, std::placeholders::_1, std::placeholders::_2),
                std::bind(&RenderHelper::releaseVideoRenderer, renderHelper),
                std::bind(&GRender::onRenderingChanged, this, std::placeholders::_1));
    }
}

void GRender::stopRendering() {
    if (audioRenderThread && audioRenderThread->hasStarted()) {
        audioRenderThread->stop();
        frameSource->reset();
        audioRenderThread->join();
        delete audioRenderThread;
        audioRenderThread = nullptr;
    }
    if (videoRenderThread && videoRenderThread->hasStarted()) {
        videoRenderThread->stop();
        frameSource->reset();
        videoRenderThread->join();
        delete videoRenderThread;
        videoRenderThread = nullptr;
    }
    delete renderHelper;
    renderHelper = nullptr;
    LOGI(TAG, "rendering threads were stopped!");
}

void GRender::pauseThreads( bool rendering, bool messaging) {
    if (rendering) {
        if (hasAudio) audioRenderThread->pause();
        if (hasVideo) videoRenderThread->pause();
    }
    if (messaging) {
        messageThread->pause();
    }
}

void GRender::resumeThreads( bool rendering, bool messaging) {
    if (rendering) {
        if (hasAudio) audioRenderThread->resume();
        if (hasVideo) videoRenderThread->resume();
    }
    if (messaging) {
        messageThread->resume();
    }
}

