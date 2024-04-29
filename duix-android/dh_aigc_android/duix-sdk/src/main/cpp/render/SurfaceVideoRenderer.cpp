#include <Log.h>
#include <thread>
#include "SurfaceVideoRenderer.h"

SurfaceVideoRenderer::SurfaceVideoRenderer(FrameSource *source) {
    LOGI("VideoRenderer", "CoreFlow : new SurfaceVideoRenderer");
    mediaSource = source;
}

SurfaceVideoRenderer::~SurfaceVideoRenderer() = default;

void SurfaceVideoRenderer::surfaceCreated(NativeWindowType window, int videoWidth, int videoHeight) {

}

void SurfaceVideoRenderer::surfaceChanged(int width, int height) {

}

void SurfaceVideoRenderer::surfaceDestroyed() {

}

uint64_t SurfaceVideoRenderer::render(uint64_t nowMs) {
    MediaData *mediaData;
    if (mediaSource->readVidFrame(&mediaData) > 0) {
        if (nowMs > mediaData->pts) {
            mediaSource->popVidFrame(mediaData);
            return mediaData->pts;
        }
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(5));
    return 0;
}
