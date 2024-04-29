#ifndef GPLAYER_RGBVIDEORENDERER_H
#define GPLAYER_RGBVIDEORENDERER_H


#include "EglRenderer.h"
#include "FrameSource.h"
#include "VideoRenderer.h"
#include "GlesProgram.h"

class RgbVideoRenderer : public VideoRenderer {
public:
    RgbVideoRenderer(FrameSource *source);

    ~RgbVideoRenderer();

    void surfaceCreated(NativeWindowType window, int videoWidth, int videoHeight);

    void surfaceChanged(int width, int height);

    void surfaceDestroyed();

    uint64_t render(uint64_t nowMs);

    bool isRenderValid() {
        return isEglInit;
    }

private:
    EglRenderer *eglRenderer;
    RgbGlesProgram *progRgb;
    AlphaGlesProgram *progAlpha;
    FrameSource *mediaSource;
    bool isEglInit = false;
    GTexture    *tex_base = nullptr;
    GTexture    *tex_msk = nullptr;
    GTexture    *tex_bg = nullptr;
};


#endif //GPLAYER_YUVVIDEORENDERER_H
