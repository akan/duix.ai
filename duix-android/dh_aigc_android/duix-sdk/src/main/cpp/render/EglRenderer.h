#ifndef EGL_RENDERER_H
#define EGL_RENDERER_H

#include <EGL/egl.h>
#include <GLES2/gl2.h>
#include <Log.h>

class EglRenderer {

public:
    EglRenderer();

    ~EglRenderer();
    //void setWindow(ANativeWindow *window);
    void setWindow(NativeWindowType window);

    void setVideoSize(int width, int height);

    bool initialize();

    void destroy();

    void swapBuffers();

private:
    NativeWindowType _window{};
    //ANativeWindow *_window{};
    int videoWidth = 0;
    int videoHeight = 0;

    EGLDisplay _display{};
    EGLSurface _surface{};
    EGLContext _context{};
};

#endif // EGL_RENDERER_H
