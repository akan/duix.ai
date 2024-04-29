#ifndef GPLAYER_RENDERHELPER_H
#define GPLAYER_RENDERHELPER_H

#include "BaseRenderHelper.h"
#include <AudioRenderer.h>
#include <RgbVideoRenderer.h>
#include <MessageSource.h>

#define FELLOW_AUDIO 0
#define FELLOW_CLOCK 1

#define MAX_PTS 9223372036854775807

class RenderHelper:BaseRenderHelper {
public:
    RenderHelper(FrameSource *source, MessageSource *messageSource, int mediaCodecFlag, bool hasAudio, bool hasVideo);
    virtual ~RenderHelper();

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

    void setStopWhenEmpty(bool enable) {
        stopWhenEmpty = enable;
    }

protected:
    AudioRenderer *audioRenderer = nullptr;
    VideoRenderer *videoRenderer = nullptr;
    uint64_t nowPts = 0;
    uint64_t startPts = 0;
    uint64_t firstFramePts = 0;
    bool hasNotifyFirstFrame = false;
    bool stopWhenEmpty = false;
    int avSyncMethod = FELLOW_AUDIO;
};


#endif //GPLAYER_RENDERHELPER_H
