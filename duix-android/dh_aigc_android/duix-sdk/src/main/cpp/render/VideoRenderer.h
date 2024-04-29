#ifndef GPLAYER_VIDEORENDERER_H
#define GPLAYER_VIDEORENDERER_H

#include "EglRenderer.h"

class VideoRenderer {
public:
    //VideoRenderer() {};
    virtual ~VideoRenderer() {};

    virtual void surfaceCreated(NativeWindowType window, int videoWidth, int videoHeight) = 0;

    virtual void surfaceChanged(int width, int height) = 0;

    virtual void surfaceDestroyed() = 0;

    virtual uint64_t render(uint64_t nowMs) = 0;

    virtual bool isRenderValid() = 0;
};


#endif //GPLAYER_VIDEORENDERER_H
