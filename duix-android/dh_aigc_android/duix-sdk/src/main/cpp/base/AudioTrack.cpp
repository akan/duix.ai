#include <Log.h>
#include "AudioTrack.h"

#define TAG "AudioTrack"

AudioTrack::AudioTrack(CommObj* obj) {
    audioTrackObj = obj;//
}

AudioTrack::~AudioTrack() = default;

void AudioTrack::start(int sampleRate, int sampleFormat, int channels, int bytesPerSample) {
    LOGI(TAG, "start %d %d %d %d", sampleRate, sampleFormat, channels, bytesPerSample);
}

int AudioTrack::write(uint8_t *buffer, int size) {
    return -1;
}

void AudioTrack::stop() {
}
