#include <android/asset_manager_jni.h>
#include <android/native_window_jni.h>
#include <android/native_window.h>
#include <android/log.h>
#include <jni.h>
#include <string>
#include <vector>
#include <unistd.h>
#include "GDigit.h"
#include "Log.h"
#include "MsgcbJni.h"
#include "JniHelper.h"
#include "aesmain.h"

#if __ARM_NEON
#include <arm_neon.h>
#endif // __ARM_NEON
       //
       //
static GDigit* g_digit = 0;
static JMat*    g_gpgmat = NULL;
static MessageCb* g_msgcb = new MessageCb();
static int  g_width = 540;
static int  g_height = 960;
static int  g_taskid = -1;
#define TAG  "tooken"
extern "C" {

    JNIEXPORT jint JNI_OnLoad(JavaVM *vm, void *reserved) {
        LOGD(TAG, "JNI_OnLoad");
        g_digit = new GDigit(g_width,g_height,g_msgcb);
        JniHelper::sJavaVM = vm;
        return JNI_VERSION_1_4;
    }

    JNIEXPORT void JNI_OnUnload(JavaVM *vm, void *reserved) {
        LOGI(TAG, "unload");
        if(g_digit){
            delete g_digit;
            g_digit = nullptr;
        }
        if(g_msgcb){
            delete g_msgcb;
            g_msgcb = nullptr;
        }
    }

    static std::string getStringUTF(JNIEnv *env, jstring obj) {
        char *c_str = (char *) env->GetStringUTFChars(obj, nullptr);
        std::string tmpString = std::string(c_str);
        env->ReleaseStringUTFChars(obj, c_str);
        return tmpString;
    }

    JNIEXPORT jint JNICALL Java_com_btows_ncnntest_SCRFDNcnn_initModel(JNIEnv *env, jobject thiz, jstring cfgtxt){
        return -1;
    }

    JNIEXPORT jint JNICALL Java_com_btows_ncnntest_SCRFDNcnn_createdigit(JNIEnv *env, jobject thiz, jint taskid,jobject msgcbobj){
        LOGI(TAG, "create");
        g_taskid = taskid;
        return 0;
    }

    JNIEXPORT jint JNICALL Java_com_btows_ncnntest_SCRFDNcnn_releasedigit(JNIEnv *env, jobject thiz,jint taskid){
        if(g_taskid==taskid){
            g_digit->stop();
            g_digit->recyle();
        }
        return 0;
    }

    JNIEXPORT jint JNICALL
        Java_com_btows_ncnntest_SCRFDNcnn_config(JNIEnv *env, jobject thiz, jstring cfgtxt){
            std::string str = getStringUTF(env,cfgtxt);
            LOGI(TAG,"cfgstr %s",str.c_str());
            g_digit->config(str.c_str());
            LOGI(TAG,"cfgstr %s",str.c_str());
            g_digit->prepare();
            return 0;
        }

    JNIEXPORT jint JNICALL
        Java_com_btows_ncnntest_SCRFDNcnn_start(JNIEnv *env, jobject thiz){
            g_digit->start();
            return 0;
        }

    JNIEXPORT jint JNICALL
        Java_com_btows_ncnntest_SCRFDNcnn_stop(JNIEnv *env, jobject thiz){
            g_digit->stop();
            return 0;
        }

    JNIEXPORT jint JNICALL
        Java_com_btows_ncnntest_SCRFDNcnn_netwav(JNIEnv *env, jobject thiz, jstring wavurl,jfloat duration){
            std::string s_wav = getStringUTF(env,wavurl);
            return g_digit->netwav(s_wav.c_str(),duration);
        }

    JNIEXPORT jint JNICALL
        Java_com_btows_ncnntest_SCRFDNcnn_bgpic(JNIEnv *env, jobject thiz, jstring picfn){
            std::string s_pic = getStringUTF(env,picfn);
            return g_digit->bgpic(s_pic.c_str());
        }

    JNIEXPORT jint JNICALL
        Java_com_btows_ncnntest_SCRFDNcnn_drawmskpic(JNIEnv *env, jobject thiz, jstring picfn,jstring mskfn){
            std::string s_pic = getStringUTF(env,picfn);
            std::string s_dump = getStringUTF(env,mskfn);
            return g_digit->drawmskpic(s_pic.c_str(),s_dump.c_str());
        }

    JNIEXPORT jint JNICALL
        Java_com_btows_ncnntest_SCRFDNcnn_drawmskbuf(JNIEnv *env, jobject thiz, jstring picfn,jstring mskfn,jbyteArray arrbuf,jbyteArray mskbuf,jint bufsize){
            std::string s_pic = getStringUTF(env,picfn);
            std::string s_dump = getStringUTF(env,mskfn);
            jbyte *pixels = (jbyte *) env->GetPrimitiveArrayCritical(arrbuf, 0);
            jbyte *pmsk = (jbyte *) env->GetPrimitiveArrayCritical(mskbuf, 0);
            int size = bufsize;
            int rst =  g_digit->drawmskbuf(s_pic.c_str(),s_dump.c_str(),(char*)pixels,(char*)pmsk,size);
            env->ReleasePrimitiveArrayCritical(arrbuf, pixels, 0);
            env->ReleasePrimitiveArrayCritical(mskbuf, pmsk, 0);
            return rst;
        }

    JNIEXPORT jint JNICALL Java_com_btows_ncnntest_SCRFDNcnn_mskrstpic(JNIEnv* env, jobject thiz,jstring picfile, jstring mskfile,jintArray arrbox, jint index,jstring fgfile){
        std::string s_pic = getStringUTF(env,picfile);
        std::string s_msk = getStringUTF(env,mskfile);
        std::string s_fg = getStringUTF(env,fgfile);
        jint *boxData = (jint*) env->GetIntArrayElements( arrbox, NULL);
        int rst =  g_digit->mskrstpic(index,s_pic.c_str(),(int*)boxData,s_msk.c_str(),s_fg.c_str());
        env->ReleaseIntArrayElements( arrbox, boxData, 0);
        return rst;
    }

    JNIEXPORT jint JNICALL
        Java_com_btows_ncnntest_SCRFDNcnn_drawonebuf(JNIEnv *env, jobject thiz, jstring picfn,jbyteArray arrbuf,jint bufsize){
            std::string s_pic = getStringUTF(env,picfn);
            jbyte *pixels = (jbyte *) env->GetPrimitiveArrayCritical(arrbuf, 0);
            int size = bufsize;
            int rst =  g_digit->drawonebuf(s_pic.c_str(),(char*)pixels,size);
            env->ReleasePrimitiveArrayCritical(arrbuf, pixels, 0);
            return rst;
        }

    JNIEXPORT jint JNICALL Java_com_btows_ncnntest_SCRFDNcnn_onerstbuf(JNIEnv* env, jobject thiz,jstring picfile, jintArray arrbox, jint index,jbyteArray arrbuf,jint bufsize){
        std::string s_pic = getStringUTF(env,picfile);
        jint *boxData = (jint*) env->GetIntArrayElements( arrbox, NULL);
        jbyte *pixels = (jbyte *) env->GetPrimitiveArrayCritical(arrbuf, 0);
        int size = bufsize;
        int rst =  g_digit->onerstbuf(index,s_pic.c_str(),(int*)boxData,(char*) pixels,size);
        env->ReleasePrimitiveArrayCritical(arrbuf, pixels, 0);
        env->ReleaseIntArrayElements( arrbox, boxData, 0);
        return rst;
    }

    JNIEXPORT jint JNICALL Java_com_btows_ncnntest_SCRFDNcnn_mskrstbuf(JNIEnv* env, jobject thiz,jstring picfile, jstring mskfile,jintArray arrbox, jint index,jstring fgfile,jbyteArray arrbuf,jbyteArray mskbuf,jint bufsize){
        std::string s_pic = getStringUTF(env,picfile);
        std::string s_msk = getStringUTF(env,mskfile);
        std::string s_fg = getStringUTF(env,fgfile);
        jint *boxData = (jint*) env->GetIntArrayElements( arrbox, NULL);
        jbyte *pixels = (jbyte *) env->GetPrimitiveArrayCritical(arrbuf, 0);
        jbyte *pmsk = (jbyte *) env->GetPrimitiveArrayCritical(mskbuf, 0);
        int size = bufsize;
        int rst =  g_digit->mskrstbuf(index,s_pic.c_str(),(int*)boxData,s_msk.c_str(),s_fg.c_str(),(char*) pixels,(char*)pmsk,size);
        env->ReleasePrimitiveArrayCritical(arrbuf, pixels, 0);
        env->ReleasePrimitiveArrayCritical(mskbuf, pmsk, 0);
        env->ReleaseIntArrayElements( arrbox, boxData, 0);
        return rst;
    }


    JNIEXPORT jint JNICALL
        Java_com_btows_ncnntest_SCRFDNcnn_onewav(JNIEnv *env, jobject thiz, jstring wavfn,jstring dumpfn){
            std::string s_wav = getStringUTF(env,wavfn);
            std::string s_dump = getStringUTF(env,dumpfn);
            return g_digit->newwav(s_wav.c_str(),s_dump.c_str());
        }


    JNIEXPORT jint JNICALL
        Java_com_btows_ncnntest_SCRFDNcnn_onepic(JNIEnv *env, jobject thiz, jstring picfn,jstring dumpfn){
            std::string s_pic = getStringUTF(env,picfn);
            std::string s_dump = getStringUTF(env,dumpfn);
            return g_digit->newpic(s_pic.c_str(),s_dump.c_str());
        }

    JNIEXPORT jint JNICALL
        Java_com_btows_ncnntest_SCRFDNcnn_drawpic(JNIEnv *env, jobject thiz, jstring picfn){
            std::string s_pic = getStringUTF(env,picfn);
            return g_digit->drawpic(s_pic.c_str());
        }
    // public native boolean openCamera(int facing);
    JNIEXPORT jint JNICALL Java_com_btows_ncnntest_SCRFDNcnn_onerst(JNIEnv* env, jobject thiz, jint index,jstring dumpfn) {
        std::string s_dump = getStringUTF(env,dumpfn);
        return g_digit->newrst(index,s_dump.c_str());
    }

    JNIEXPORT jint JNICALL Java_com_btows_ncnntest_SCRFDNcnn_netrst(JNIEnv* env, jobject thiz, jint index,jstring dumpfn) {
        std::string s_dump = getStringUTF(env,dumpfn);
        return g_digit->netrst(index,s_dump.c_str());
    }

    JNIEXPORT jint JNICALL Java_com_btows_ncnntest_SCRFDNcnn_picrst(JNIEnv* env, jobject thiz,jstring picfile, jintArray arrbox, jint index,jstring dumpfn){
        std::string s_pic = getStringUTF(env,picfile);
        std::string s_dump = getStringUTF(env,dumpfn);
        jint *boxData = (jint*) env->GetIntArrayElements( arrbox, NULL);
        int rst =  g_digit->picrst(s_pic.c_str(),(int*)boxData,index,s_dump.c_str());
        __android_log_print(ANDROID_LOG_DEBUG, "tooken", "picrst %d",rst);
        env->ReleaseIntArrayElements( arrbox, boxData, 0);
        return rst;
    }

    JNIEXPORT jint JNICALL Java_com_btows_ncnntest_SCRFDNcnn_netrstpic(JNIEnv* env, jobject thiz,jstring picfile, jintArray arrbox, jint index,jstring dumpfn){
        std::string s_pic = getStringUTF(env,picfile);
        std::string s_dump = getStringUTF(env,dumpfn);
        jint *boxData = (jint*) env->GetIntArrayElements( arrbox, NULL);
        int rst =  g_digit->netrstpic(s_pic.c_str(),(int*)boxData,index,s_dump.c_str());
        __android_log_print(ANDROID_LOG_DEBUG, "tooken", "picrst %d",rst);
        env->ReleaseIntArrayElements( arrbox, boxData, 0);
        return rst;
    }


    JNIEXPORT jint JNICALL Java_com_btows_ncnntest_SCRFDNcnn_reset(JNIEnv* env, jobject thiz){
        //g_digit->reset_fps();
        //g_digit->reset_inx();
        g_digit->setSurface(NULL);
        return 0;
    }

    // public native boolean setOutputWindow(Surface surface);
    JNIEXPORT jint JNICALL Java_com_btows_ncnntest_SCRFDNcnn_setOutputWindow(JNIEnv* env, jobject thiz, jobject surface)
    {
        ANativeWindow* win = ANativeWindow_fromSurface(env, surface);

        __android_log_print(ANDROID_LOG_DEBUG, "ncnn", "setOutputWindow %p", win);
        g_digit->setSurface(win);
        //g_digit->start();

        return 0;
    }

    JNIEXPORT jint JNICALL
        Java_com_btows_ncnntest_SCRFDNcnn_startgpg(JNIEnv *env, jobject thiz, jstring picfn,jstring gpgfn){
            std::string s_pic = getStringUTF(env,picfn);
            std::string s_gpg = getStringUTF(env,gpgfn);
            if(!g_gpgmat)g_gpgmat = new JMat();
            int rst = g_gpgmat->loadjpg(s_pic);
            if(rst)return rst;
            rst = g_gpgmat->savegpg(s_gpg);
            return rst;
        }

    JNIEXPORT jint JNICALL
        Java_com_btows_ncnntest_SCRFDNcnn_processmd5(JNIEnv *env, jobject thiz, jint kind,jstring infn,jstring outfn){
            std::string s_in = getStringUTF(env,infn);
            std::string s_out = getStringUTF(env,outfn);
            int rst = mainenc(kind,(char*)s_in.c_str(),(char*)s_out.c_str());
            return rst;
        }

    JNIEXPORT jint JNICALL
        Java_com_btows_ncnntest_SCRFDNcnn_stopgpg(JNIEnv *env, jobject thiz){
            if(g_gpgmat){
                delete g_gpgmat;
                g_gpgmat = NULL;
            }
            return 0;
    }
}

