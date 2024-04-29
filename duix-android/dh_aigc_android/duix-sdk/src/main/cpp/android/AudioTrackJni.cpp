#include <Log.h>
#include "AudioTrackJni.h"
#include "JniHelper.h"

#define TAG "AudioTrackJni"

AudioTrackJni::AudioTrackJni(void* obj):AudioTrack(obj) {
    JNIEnv *env = JniHelper::getJNIEnv();
    audioTrackObj = env->NewGlobalRef(obj);
    audioTrackClass = env->GetObjectClass(obj);
    audioTrackStart = env->GetMethodID(audioTrackClass, "open", "(IIII)V");
    audioTrackWrite = env->GetMethodID(audioTrackClass, "write", "(Ljava/nio/ByteBuffer;I)I");
    audioTrackStop = env->GetMethodID(audioTrackClass, "close", "()V");
}

AudioTrackJni::~AudioTrackJni() = default;

void AudioTrackJni::start(int sampleRate, int sampleFormat, int channels, int bytesPerSample) {
    LOGI(TAG, "start %d %d %d %d", sampleRate, sampleFormat, channels, bytesPerSample);
    bool attach = JniHelper::attachCurrentThread();
    if (audioTrackObj != nullptr && audioTrackStart != nullptr) {
        JniHelper::callVoidMethod(audioTrackObj, audioTrackStart, sampleRate, sampleFormat,
                                  channels, bytesPerSample);
    }
    if (attach) {
        JniHelper::detachCurrentThread();
    }
}

int AudioTrackJni::write(uint8_t *buffer, int size) {
    bool attach = JniHelper::attachCurrentThread();
    if (audioTrackObj != nullptr && audioTrackWrite != nullptr) {
        JNIEnv *env = JniHelper::getJNIEnv();
        jobject byteBuffer = JniHelper::createByteBuffer(env, buffer, size);
        int result = JniHelper::callIntMethod(audioTrackObj, audioTrackWrite, byteBuffer, size);
        env->DeleteLocalRef(byteBuffer);
        return result;
    }
    if (attach) {
        JniHelper::detachCurrentThread();
    }
    return -1;
}

void AudioTrackJni::stop() {
    bool attach = JniHelper::attachCurrentThread();
    if (audioTrackObj != nullptr && audioTrackStop != nullptr) {
        JniHelper::callVoidMethod(audioTrackObj, audioTrackStop);
    }
    if (attach) {
        JniHelper::detachCurrentThread();
    }
}
