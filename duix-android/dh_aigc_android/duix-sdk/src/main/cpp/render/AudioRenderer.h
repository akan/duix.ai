#ifndef GPLAYER_AUDIORENDERER_H
#define GPLAYER_AUDIORENDERER_H


#include <FrameSource.h>
#include <AudioTrack.h>

class AudioRenderer {
public:
    AudioRenderer(FrameSource *source);

    ~AudioRenderer();

    void setAudioTrack(AudioTrack *track);

    void setAudioParams(int rate, int channels, int format, int bytesPerSample);

    void init();

    uint64_t render(uint64_t nowMs);

    void release();

private:
    FrameSource *frameSource;
    AudioTrack *audioTrack;
    int sampleRate = 0;
    int channels = 0;
    int format = 0;
    int bytesPerSample = 0;
};


#endif //GPLAYER_AUDIORENDERER_H
