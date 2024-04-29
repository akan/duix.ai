#ifndef GPLAYER_GRender_H
#define GPLAYER_GRender_H

#include "CommObj.h"
#include "MessageHelper.h"
#include "LoopThread.h"
#include "FrameSource.h"
#include "MessageSource.h"
#include "RenderHelper.h"

class GRender {
public:
    GRender(int width,int height, MessageCb *obj);
    virtual ~GRender();

public:
    void setSurface(NativeWindowType window);
    void setAudioTrack(AudioTrack *track);

    virtual void prepare();
    virtual void start();
    virtual void pause();
    virtual void resume();
    virtual void stop();

    int getVideoWidth();
    int getVideoHeight();
    int setVideoParam(int width,int height);

    void onRenderingChanged(int state);
    void onBufferStateChanged(int state);
    void onPlayStateChanged(int state, long extra);
protected:
    int processMessage(int arg1, long arg2);

    void startMessageLoop();
    void stopMessageLoop();

    void startRendering();
    void stopRendering();

    void pauseThreads( bool rendering, bool messaging);
    void resumeThreads( bool rendering, bool messaging);
protected:
    NativeWindowType nativeWindow = 0;
    AudioTrack *audioTrack = nullptr;

    FrameSource *frameSource;
    MessageSource *messageSource;

    RenderHelper  *renderHelper;
    MessageHelper *messageHelper;

    LoopThread *audioRenderThread;
    LoopThread *videoRenderThread;
    LoopThread *messageThread;

    int m_width = 0;
    int m_height = 0;
    int playState = -1;
    int bufferState = -1;
    bool isEof = false;
    bool hasAudio = false;
    bool hasVideo = true;
    std::mutex playerLock;
};


#endif //GPLAYER_GPLAYER_H

