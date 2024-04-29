#include "EglRenderer.h"
#include <X11/Xatom.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include "FrameSource.h"
#include "RgbVideoRenderer.h"
#include "jmat.h"
#include "GDigit.h"
#include "CommObj.h"
#include "benchmark.h"

NativeWindowType createNativeWindow(int width,int height) {
    NativeWindowType native_window;

    Window root;
    XSetWindowAttributes swa;
    XSetWindowAttributes xattr;
    Atom wm_state;
    XWMHints hints;
    XEvent xev;
    EGLConfig ecfg;
    Display* x_display;

    // X11 native display initialization
    x_display = XOpenDisplay(NULL);
    if (x_display == NULL) {
        return EGL_FALSE;
    }

    root = DefaultRootWindow(x_display);

    swa.event_mask = ExposureMask | PointerMotionMask | KeyPressMask;
    native_window =
        XCreateWindow(x_display, root, 0, 0, width, height, 0, CopyFromParent,
                InputOutput, CopyFromParent, CWEventMask, &swa);

    xattr.override_redirect = false;
    XChangeWindowAttributes(x_display, native_window, CWOverrideRedirect, &xattr);

    hints.input = true;
    hints.flags = InputHint;
    XSetWMHints(x_display, native_window, &hints);

    // make the window visible on the screen
    XMapWindow(x_display, native_window);
    XStoreName(x_display, native_window, "Test");

    // get identifiers for the provided atom name strings
    wm_state = XInternAtom(x_display, "_NET_WM_STATE", false);

    memset(&xev, 0, sizeof(xev));
    xev.type = ClientMessage;
    xev.xclient.window = native_window;
    xev.xclient.message_type = wm_state;
    xev.xclient.format = 32;
    xev.xclient.data.l[0] = 1;
    xev.xclient.data.l[1] = false;
    XSendEvent(x_display, DefaultRootWindow(x_display), false,
            SubstructureNotifyMask, &xev);

    return native_window;
}



int testegl(){
    int width = 640;
    int height = 480;
    NativeWindowType window = createNativeWindow(width,height);
    EglRenderer* render = new EglRenderer();
    render->setWindow(window);
    render-> setVideoSize(width, height);
    render->initialize();

    glClearColor(1.0, 1.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);
    glFlush();
    render->swapBuffers();
    getchar();
    render->destroy();
    return 0;
}

int testrender(int argc,char** argv){
    FrameSource source(100,100);
    int width = 1080;
    int height = 1920;
    std::string fn = "../mybin/fg.jpg";
    JMat fmat(fn,1);
    fn = "../mybin/bg.jpg";
    JMat bmat(fn,1);
    fn = "../mybin/msk.jpg";
    JMat mmat(fn,1);
    MediaData frame(fmat.refclone(),mmat.refclone(),bmat.refclone());
    RgbVideoRenderer* render = new RgbVideoRenderer(&source);
    NativeWindowType window = createNativeWindow(width/2,height/2);
    render->surfaceCreated(window,width,height);
    source.pushVidFrame(&frame);
    //
    render->render(10);
    getchar();

    getchar();
    render->surfaceDestroyed();
    delete render;
    return 0;
}
static MessageCb* msgcb = new MessageCb();

int testdigit(NativeWindowType window){
    int rst = 0;
    int width = 530;
    int height = 960;
    GDigit* digit = new GDigit(width,height,msgcb);
    std::string cfgtxt = "{\"videowidth\":540," \
                    "\"videoheight\":960," \
                    "\"cacertfn\":\"../model/cacert.pem\"," \
                    "\"wenetfn\":\"../model/wenet.ort\"," \
                    "\"alphaparam\":\"../model/alpha_model.param\"," \
                    "\"alphabin\":\"../model/alpha_model.bin\"," \
                    "\"unetparam\":\"../model/dh_model.param\"," \
                    "\"unetbin\":\"../model/dh_model.bin\"," \
                    "\"unetmsk\":\"../model/weights_168u.bin\"," \
                    "\"timeoutms\":10000}"; \


    //"\"wenetfn\":\"../model/wenet.onnx\"," \

    digit->config(cfgtxt.c_str());
    //digit->initWenet("../model/wenet.onnx");
    //digit->initCurl("../model/cacert.pem",10000);
    //const char* wavfile = "https://digital-public.obs.cn-east-3.myhuaweicloud.com/yunwei_update/wsl/953883.wav";
    //float duration = 31.0;
    //digit->netwav(wavfile,duration);
    digit->prepare();
    digit->setSurface(window);
    digit->start();
    digit->drawpic("../mybin/t1.jpg");
    usleep(100000);
    digit->drawpic("../mybin/b1.jpg");
    usleep(100000);
    digit->bgpic("../mybin/b1.jpg");
    digit->drawmskpic("../mybin/t1.jpg","../mybin/m1.jpg");
    usleep(100000);
    double t0 = ncnn::get_current_time();
    int wavframe = digit->newwav("../mybin/1.wav",NULL);
    double t1 = ncnn::get_current_time();
    float dist = t1-t0;
    printf("unet %f",dist);
    getchar();
    //int wavframe = digit->netwav("abcdefg",10);
    usleep(100000);
    digit->bgpic("../mybin/b1.jpg");
    int boxs[4];
    boxs[1]=148;boxs[0]=221; boxs[3]=242;boxs[2]=315;
    std::string picfn = "../mybin/t1.jpg";
    std::string mskfn = "../mybin/m1.jpg";
    printf("===frame %d\n",wavframe);
    for(int k=0;k<wavframe;k++){
        //rst = digit->mskrstpic(k,picfn.c_str(),boxs,mskfn.c_str(),picfn.c_str());
        rst = digit->picrst(picfn.c_str(),boxs,k,NULL);
        usleep(40000);
        printf("====gogogo %d rst %d\n",k,rst);
    }
    printf("===frame %d\n",wavframe);

    getchar();
    digit->stop();
    delete digit;
    return 0;
}

int main(int argc,char** argv){
    if(1){
        JMat mat("../mybin/fg.jpg",1);
        //mat.savegpg("aaa.gpg");
        //JMat mb;
        //mb.loadgpg("aaa.gpg");
        //mb.show("mb");
        //cv::waitKey(0);
        double t0 = ncnn::get_current_time();
        for(int k=0;k<100;k++){
            JMat mat2("../mybin/fg.jpg",1);
        }
        double t1 = ncnn::get_current_time();
        float dist = t1-t0;
        printf("mat spend %f\n",dist);
        double a0 = ncnn::get_current_time();
        for(int k=0;k<100;k++){
            JMat* testmat = new JMat();
            testmat->loadjpg("../mybin/fg.jpg");
            delete testmat;
        }
        double a1 = ncnn::get_current_time();
        int rst = mat.loadjpg("../mybin/bg.jpg");
        printf("===rst %d\n",rst);
        dist = a1-a0;
        printf("load spend %f\n",dist);
        return 0;
    }
    int width = 1080;
    int height = 1920;
    NativeWindowType window = createNativeWindow(width/2,height/2);
    for(int k=0;k<1;k++){
        testdigit(window);
    }
    getchar();
    return 0;
}

