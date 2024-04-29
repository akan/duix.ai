#include <thread>
#include "RgbVideoRenderer.h"
#include "MediaData.h"

RgbVideoRenderer::RgbVideoRenderer(FrameSource *source) {
    LOGI("VideoRenderer", "CoreFlow : new RgbVideoRenderer");
    eglRenderer = new EglRenderer();
    progRgb = new RgbGlesProgram();
    progAlpha = new AlphaGlesProgram();
    mediaSource = source;
}

RgbVideoRenderer::~RgbVideoRenderer() {
    if(tex_base){
        delete tex_base;
        tex_base = nullptr;
    }
    if(tex_msk){
        delete tex_msk;
        tex_msk = nullptr;
    }
    if(tex_bg){
        delete tex_bg;
        tex_bg = nullptr;
    }
    delete eglRenderer;
    eglRenderer = nullptr;
    delete progRgb;
    progRgb = nullptr;
    delete progAlpha;
    progAlpha = nullptr;
}

void RgbVideoRenderer::surfaceCreated(NativeWindowType window, int videoWidth, int videoHeight) {
    eglRenderer->setWindow(window);
    eglRenderer->setVideoSize(videoWidth, videoHeight);
    if (eglRenderer->initialize()) {
        isEglInit = progRgb->buildProgram();
        isEglInit = progAlpha->buildProgram();
    }

    LOGE("VideoRenderer", "surfaceCreated isEglInit %d", isEglInit);
}

void RgbVideoRenderer::surfaceChanged(int width, int height) {

}

void RgbVideoRenderer::surfaceDestroyed() {
    eglRenderer->destroy();
}

uint64_t RgbVideoRenderer::render(uint64_t nowMs) {
    MediaData *mediaData = NULL;
    if (mediaSource->readVidFrame(&mediaData) > 0) {
        //if (nowMs > mediaData->pts) {
        if (!mediaData->pts) {
            //glProgram->buildTextures(mediaData->data, mediaData->data1, mediaData->data2, mediaData->width, mediaData->height);
            if(!tex_base)tex_base = new GTexture(mediaData->width,mediaData->height,3);
            tex_base->load(mediaData->width,mediaData->height,mediaData->data);
            int alpha = 0;
            if(mediaData->data1){
                if(!tex_msk)tex_msk = new GTexture(mediaData->width,mediaData->height,3);
                tex_msk->load(mediaData->width,mediaData->height,mediaData->data1);
                if(!tex_bg)tex_bg = new GTexture(mediaData->width,mediaData->height,3);
                tex_bg->load(mediaData->width,mediaData->height,mediaData->data2);
                alpha = 1;
            }
            if(alpha){
                GTexture* arr[]={tex_base,tex_msk,tex_bg};
                progAlpha->drawFrame(arr,3);
            }else{
                GTexture* arr[]={tex_base};
                progRgb->drawFrame(arr,1);
            }
            eglRenderer->swapBuffers();
            uint64_t  rst = mediaData->pts;
            mediaSource->pushVidRecyle(mediaData->m_data);
            mediaSource->pushVidRecyle(mediaData->m_msk);
            mediaData->reset(1);
            mediaSource->popVidFrame(mediaData);
            return rst;
        }
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(5));
    return 0;
    }
