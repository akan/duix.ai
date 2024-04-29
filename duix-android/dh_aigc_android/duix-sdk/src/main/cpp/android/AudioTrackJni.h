#ifndef GPLAYER_AUDIOTRACKJNI_H
#define GPLAYER_AUDIOTRACKJNI_H


#include <jni.h>
#include "FrameSource.h"
#include "AudioTrack.h"

class AudioTrackJni:public AudioTrack {
public:
    AudioTrackJni(jobject obj);

    ~AudioTrackJni();

    virtual void start(int sampleRate, int sampleFormat, int channels, int bytesPerSample);

    virtual int write(uint8_t *buffer, int size);

    virtual void stop();

private:
    jclass audioTrackClass;
    jmethodID audioTrackStart;
    jmethodID audioTrackWrite;
    jmethodID audioTrackStop;

private:
    jobject audioTrackObj;
};


#endif //GPLAYER_AUDIOTRACKJNI_H
