#include "EglRenderer.h"

#define TAG "EglRenderer"
#ifdef ANDROID
#include <android/native_window.h>
#endif

EglRenderer::EglRenderer() = default;

EglRenderer::~EglRenderer() = default;

void EglRenderer::setWindow(NativeWindowType window) {
    LOGI(TAG, "setWindow");
    _window = window;
}

void EglRenderer::setVideoSize(int width, int height) {
    LOGI(TAG, "setVideoSize %d %d", width, height);
    videoWidth = width;
    videoHeight = height;
}

bool EglRenderer::initialize() {
    const EGLint attrib[] = {
            EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
            EGL_BLUE_SIZE, 8,
            EGL_GREEN_SIZE, 8,
            EGL_RED_SIZE, 8,
            EGL_ALPHA_SIZE, 0,
            EGL_DEPTH_SIZE, 16,
            EGL_STENCIL_SIZE, 0,
            EGL_NONE
    };
    const EGLint version[] = {
            EGL_CONTEXT_CLIENT_VERSION, 2,
            EGL_NONE
    };

    EGLDisplay display;
    EGLConfig config;
    EGLint major;
    EGLint minor;
    EGLint numConfigs;
    EGLint format;
    EGLSurface surface;
    EGLContext context;
    EGLint width;
    EGLint height;

    if ((display = eglGetDisplay(EGL_DEFAULT_DISPLAY)) == EGL_NO_DISPLAY) {
        LOGE(TAG, "eglGetDisplay() returned error 0x%x", eglGetError());
        return false;
    }

    if (!eglInitialize(display, &major, &minor)) {
        LOGE(TAG, "eglInitialize() returned error 0x%x", eglGetError());
        return false;
    }
    LOGI(TAG, "initialize major %d minor %d", major, minor);

    if (!eglChooseConfig(display, attrib, &config, 1, &numConfigs)) {
        LOGE(TAG, "eglChooseConfig() returned error 0x%x", eglGetError());
        destroy();
        return false;
    }

    if (!eglGetConfigAttrib(display, config, EGL_NATIVE_VISUAL_ID, &format)) {
        LOGE(TAG, "eglGetConfigAttrib() returned error 0x%x", eglGetError());
        destroy();
        return false;
    }
#ifdef ANDROID
    ANativeWindow_setBuffersGeometry(_window, videoWidth, videoHeight, format);
#endif

    if (!(surface = eglCreateWindowSurface(display, config, _window, nullptr))) {
        LOGE(TAG, "eglCreateWindowSurface() returned error 0x%x", eglGetError());
        destroy();
        return false;
    }

    if (!(context = eglCreateContext(display, config, EGL_NO_CONTEXT, version))) {
        LOGE(TAG, "eglCreateContext() returned error 0x%x", eglGetError());
        destroy();
        return false;
    }

    if (!eglMakeCurrent(display, surface, surface, context)) {
        LOGE(TAG, "eglMakeCurrent() returned error 0x%x", eglGetError());
        destroy();
        return false;
    }

    if (!eglQuerySurface(display, surface, EGL_WIDTH, &width) ||
        !eglQuerySurface(display, surface, EGL_HEIGHT, &height)) {
        LOGE(TAG, "eglQuerySurface() returned error 0x%x", eglGetError());
        destroy();
        return false;
    }
    LOGI(TAG, "initialize width %d height %d", width, height);

    _display = display;
    _surface = surface;
    _context = context;

    glClearColor(0, 0, 0, 0);
    glViewport(0, 0, width, height);

    return true;
}

void EglRenderer::destroy() {
    LOGI(TAG, "Destroying context");

    eglMakeCurrent(_display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
    eglDestroyContext(_display, _context);
    eglDestroySurface(_display, _surface);
    eglTerminate(_display);

    _display = EGL_NO_DISPLAY;
    _surface = EGL_NO_SURFACE;
    _context = EGL_NO_CONTEXT;
}

void EglRenderer::swapBuffers() {
    if (_display) {
        if (!eglSwapBuffers(_display, _surface)) {
            LOGE(TAG, "eglSwapBuffers() returned error 0x%x", eglGetError());
        }
    }
}

