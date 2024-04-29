#ifndef GPLAYER_SURFACEVIDEORENDERER_H
#define GPLAYER_SURFACEVIDEORENDERER_H

#include "EglRenderer.h"
#include "VideoRenderer.h"
#include "FrameSource.h"

class SurfaceVideoRenderer : public VideoRenderer {
public:
    SurfaceVideoRenderer(FrameSource *source);

    ~SurfaceVideoRenderer();

    void surfaceCreated(NativeWindowType window, int videoWidth, int videoHeight);

    void surfaceChanged(int width, int height);

    void surfaceDestroyed();

    uint64_t render(uint64_t nowMs);

    bool isRenderValid() {
        return true;
    }

private:
    FrameSource *mediaSource;
};


#endif //GPLAYER_SURFACEVIDEORENDERER_H
