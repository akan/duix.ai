#ifndef GPLAYER_BASERENDERHELPER_H
#define GPLAYER_BASERENDERHELPER_H


#include "MessageSource.h"
#include "AudioTrack.h"
#include "FrameSource.h"

#define USE_RENDER
#ifdef USE_RENDER
#include <EGL/egl.h>
#else
typedef void* NativeWindowType;
#endif


class BaseRenderHelper {
public:
    BaseRenderHelper(FrameSource *source, MessageSource *messageSource, int mediaCodecFlag, bool hasAudio, bool hasVideo);

    virtual ~BaseRenderHelper();

    virtual void setNativeWindow(NativeWindowType window);

    virtual void setAudioTrack(AudioTrack *track);

    virtual void setVideoParams(int width, int height);

    virtual void setAudioParams(int sampleRate, int channels, int format, int bytesPerSample);

    virtual void initAudioRenderer();

    virtual void initVideoRenderer();

    virtual int renderAudio(int arg1, long arg2);

    virtual int renderVideo(int arg1, long arg2);

    virtual void releaseAudioRenderer();

    virtual void releaseVideoRenderer();

protected:
    MessageSource *messageSource = nullptr;
    NativeWindowType nativeWindow = 0;
    int videoWidth = 0;
    int videoHeight = 0;

    bool hasAudio = false;
    bool hasVideo = false;
};


#endif //GPLAYER_RENDERHELPER_H
