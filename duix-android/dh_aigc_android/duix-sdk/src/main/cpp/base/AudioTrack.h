#ifndef GPLAYER_AUDIOTRACK_H
#define GPLAYER_AUDIOTRACK_H

#include "CommObj.h"
#include <stdint.h>

class AudioTrack {
public:
    AudioTrack(CommObj* obj);

    virtual ~AudioTrack();

    void start(int sampleRate, int sampleFormat, int channels, int bytesPerSample);

    int write(uint8_t *buffer, int size);

    void stop();

private:
    CommObj* audioTrackObj;
};


#endif
