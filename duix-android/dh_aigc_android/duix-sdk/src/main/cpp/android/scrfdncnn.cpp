#include <android/asset_manager_jni.h>
#include <android/native_window_jni.h>
#include <android/native_window.h>

#include <android/log.h>

#include <jni.h>

#include <string>
#include <vector>
#include <unistd.h>

#include <platform.h>
#include <benchmark.h>

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include "scrfdncnn.h"



#if __ARM_NEON
#include <arm_neon.h>
#endif // __ARM_NEON


int MyDigit::checkfile(char* fn){
    return 0;
}

int MyDigit::reset_inx(){
    inx_wenet  = 0;
    return 0;
}

int MyDigit::picrst(const char* picfn,int* box,int index,const char* dumpfn){
    if(!inited)return -999;
    if(!mat_wenet)return -1;
    if(index<0)return -2;
    if(index>=cnt_wenet)return -3;
    std::string picfile(picfn);
    JMat onepic(picfile,1);
    int* arr = onepic.tagarr();
    arr[10] = box[0];
    arr[11] = box[1];
    arr[12] = box[2];
    arr[13] = box[3];
    //
    float* pwenet = ai_wenet->nextbnf(mat_wenet,index);
    if(!pwenet){
        __android_log_print(ANDROID_LOG_DEBUG, "tooken", "%d index error",index);
        return -1;
    }
    JMat feat(256, 20, pwenet, 1);
    //int* arr = onepic.tagarr();
    double t0 = ncnn::get_current_time();
    ai_munet->process(&onepic, arr, &feat);
    double t1 = ncnn::get_current_time();
    char text[1024];
    float dist = t1-t0;
    sprintf(text,"%d unet %f",index,dist);
    __android_log_print(ANDROID_LOG_DEBUG, "tooken", "%s dump %s",text,dumpfn);
    drawMat(&onepic,text);
    if(strlen(dumpfn)){
        onepic.tojpg(dumpfn);
    }
    return 0;
}

int MyDigit::netrstpic(const char* picfn,int* box,int index,const char* dumpfn){
    if(!inited)return -999;
    if(!net_wavmat)return -1;
    if(index<0)return -2;
    if(index>net_wavmat->bnfblocks())return -3;
    std::string picfile(picfn);
    JMat onepic(picfile,1);
    int* arr = onepic.tagarr();
    arr[10] = box[0];
    arr[11] = box[1];
    arr[12] = box[2];
    arr[13] = box[3];
    //
    JMat* mw = net_wavmat->bnfmat();
    int* marr = mw->tagarr();
        __android_log_print(ANDROID_LOG_DEBUG, "tooken", "arr %d %d %d %d %d %d %d",marr[0],marr[1],marr[2],marr[3],marr[4],marr[5],marr[6]);
    float* pwenet = ai_wenet->nextbnf(mw,index);
    if(!pwenet){
        __android_log_print(ANDROID_LOG_DEBUG, "tooken", "%d index error",index);
        return -1;
    }
    JMat feat(256, 20, pwenet, 1);
    //int* arr = onepic.tagarr();
    double t0 = ncnn::get_current_time();
    ai_munet->process(&onepic, arr, &feat);
    double t1 = ncnn::get_current_time();
    char text[1024];
    float dist = t1-t0;
    sprintf(text,"%d unet %f",index,dist);
    __android_log_print(ANDROID_LOG_DEBUG, "tooken", "%s dump %s",text,dumpfn);
    drawMat(&onepic,text);
    if(strlen(dumpfn)){
        onepic.tojpg(dumpfn);
    }
    return 0;
}

int MyDigit::netrst(int index,const char* dumpfn){
    if(!inited)return -999;
    if(!net_wavmat)return -1;
    if(index<0)return -2;
    if(index>net_wavmat->bnfblocks())return -3;
    JMat onepic = mat_pic->clone();
    JMat* mw = net_wavmat->bnfmat();
    float* pwenet = ai_wenet->nextbnf(mw,index);
    if(!pwenet){
        __android_log_print(ANDROID_LOG_DEBUG, "tooken", "%d index error",index);
        return -1;
    }
    //float* pd = pwenet+256*20*index;
    JMat feat(256, 20, pwenet, 1);
    int* arr = mat_pic->tagarr();
    double t0 = ncnn::get_current_time();
    ai_munet->process(&onepic, arr, &feat);
    double t1 = ncnn::get_current_time();
    char text[1024];
    float dist = t1-t0;
    sprintf(text,"%d unet %f",index,dist);
    __android_log_print(ANDROID_LOG_DEBUG, "tooken", "%s dump %s",text,dumpfn);
    drawMat(&onepic,text);
    if(strlen(dumpfn)){
        __android_log_print(ANDROID_LOG_DEBUG, "tooken", "%s dump %s",text,dumpfn);
        onepic.tojpg(dumpfn);
    }
    return 0;
}

int MyDigit::newrst(int index,const char* dumpfn){
    if(!inited)return -999;
    if(!mat_wenet)return -1;
    if(index<0)return -2;
    if(index>=cnt_wenet)return -3;
    if(!mat_pic)return -4;
    JMat onepic = mat_pic->clone();
    float* pwenet = ai_wenet->nextbnf(mat_wenet,index);
    if(!pwenet){
        __android_log_print(ANDROID_LOG_DEBUG, "tooken", "%d index error",index);
        return -1;
    }
    //float* pd = pwenet+256*20*index;
    JMat feat(256, 20, pwenet, 1);
    int* arr = mat_pic->tagarr();
    double t0 = ncnn::get_current_time();
    ai_munet->process(&onepic, arr, &feat);
    double t1 = ncnn::get_current_time();
    char text[1024];
    float dist = t1-t0;
    sprintf(text,"%d unet %f",index,dist);
    __android_log_print(ANDROID_LOG_DEBUG, "tooken", "%s dump %s",text,dumpfn);
    drawMat(&onepic,text);
    if(strlen(dumpfn)){
        __android_log_print(ANDROID_LOG_DEBUG, "tooken", "%s dump %s",text,dumpfn);
        onepic.tojpg(dumpfn);
    }
    return 0;
}

int MyDigit::drawpic(const char* picfn){
    if(!inited)return -999;
    std::string picfile(picfn);
    JMat mat(picfile,1);
    //
    drawMat(&mat,NULL);
    return 0;
}

int MyDigit::newpic(const char* picfn,const char* dumpbox){
    if(!inited)return -999;
    if(mat_pic){
        delete mat_pic;
        mat_pic = nullptr;
    }
    std::string picfile(picfn);
    mat_pic = new JMat(picfile,1);
    int* arr = mat_pic->tagarr();
    double t0 = ncnn::get_current_time();
    ai_scrfd->detect(mat_pic,arr);
    double t1 = ncnn::get_current_time();
    if((arr[0]<=0)||(arr[1]<=0))return -1;
    int* apts = arr+64;
    ai_pfpld->detect(mat_pic,arr,apts);
    double t2 = ncnn::get_current_time();
    char text[1024];
    float dist1 = t1-t0;
    float dist2 = t2-t1;
    sprintf(text,"%s scrfd %f pfpld %f",picfn,dist1,dist2);
    __android_log_print(ANDROID_LOG_DEBUG, "tooken", "%s",text);
    drawMat(mat_pic,text);
    if(apts[0]<=0)return -2;
    if(strlen(dumpbox)){
        FILE* file = fopen(dumpbox, "w");
        if(file){
            fprintf(file,"face w %d h %d\n",arr[0],arr[1]);
            int* box = arr+2;
            fprintf(file,"orig box x1y1 %d %d x2y2 %d %d\n",box[0],box[1],box[2],box[3]);
            box = arr+6;
            fprintf(file,"scrfd box x1y1 %d %d x2y2 %d %d\n",box[0],box[1],box[2],box[3]);
            box = arr+10;
            fprintf(file,"pfpld box x1y1 %d %d x2y2 %d %d\n",box[0],box[1],box[2],box[3]);
            fclose(file);
        }
    }
    return 0;
}

int MyDigit::netwav(const char* url,float duration){
    if(!inited)return -999;
    __android_log_print(ANDROID_LOG_DEBUG, "tooken", "netwav %s",url);
    if(net_curl){
        net_curl->cancel();
        delete net_curl;
        net_curl = nullptr;
    }
    __android_log_print(ANDROID_LOG_DEBUG, "tooken", "netwav gogogo");
    if(net_wavmat){
        delete net_wavmat;
        net_wavmat = nullptr;
    }
    __android_log_print(ANDROID_LOG_DEBUG, "tooken", "netwav %f",duration);
    net_wavmat = new KWav(duration);
    net_curl = new NetCurl((char*)url,duration,net_wavmat);
    for(int k=0;k<20;k++){
        if(net_curl->checked())break;
        usleep(100000);
    }
    int rst =  net_wavmat->bnfblocks();//net_curl->checked();
    __android_log_print(ANDROID_LOG_DEBUG, "tooken", "netwav %d",rst);
    return rst;
}

int MyDigit::newwav(const char* wavfn,const char* dumpfn){
    if(!inited)return -999;
    if(mat_wenet){
        delete mat_wenet;
        mat_wenet = nullptr;
    }
    double t0 = ncnn::get_current_time();
    int rst = ai_wenet->nextwav(wavfn,&mat_wenet);
    cnt_wenet = rst;
    double t1 = ncnn::get_current_time();
    JMat mat(640,480,CV_8UC3);
    char text[1024];
    float dist = t1-t0;
    sprintf(text,"%s wenet %f",wavfn,dist);
    __android_log_print(ANDROID_LOG_DEBUG, "tooken", "%s",text);
    //drawMat(&mat,text);
    if(strlen(dumpfn)){
        mat_wenet->tobin(dumpfn);
    }
    if(!mat_wenet)return -1;
    inx_wenet = 0;
    return rst;
}

int MyDigit::initModel(const char* modeldir){
    if(inited)return 0;
    guiji_curl_init(modeldir);
    double t0 = ncnn::get_current_time();
    ai_wenet = new Wenet(modeldir,"wenet");
    __android_log_print(ANDROID_LOG_DEBUG, "tooken", "init wenet");
    double t1 = ncnn::get_current_time();
    ai_scrfd = new Scrfd(modeldir,"scrfd_500m_kps-opt2",1080,1920);
    __android_log_print(ANDROID_LOG_DEBUG, "tooken", "init scrfd");
    double t2 = ncnn::get_current_time();
    ai_pfpld = new Pfpld(modeldir,"pfpld",1080,1920);
    __android_log_print(ANDROID_LOG_DEBUG, "tooken", "init pfpld");
    double t3 = ncnn::get_current_time();
    ai_munet = new Mobunet(modeldir,"mobileunet_v5_wenet_sim");
    double t4 = ncnn::get_current_time();
    __android_log_print(ANDROID_LOG_DEBUG, "tooken", "init munet");
    JMat mat(640,480,CV_8UC3);
    char text[1024];
    float da = t1-t0;
    float db = t2-t1;
    float dc = t3-t2;
    float dd = t4-t3;
    sprintf(text,"%s model wenet %f scrfd %f pfpld %f unet %f",modeldir,da,db,dc,dd);
    __android_log_print(ANDROID_LOG_DEBUG, "tooken", "%s",text);
    drawMat(&mat,text);
    //
    init_wenetloop(ai_wenet);
    inited = true;
    return 0;
}

MyDigit::MyDigit(){
}

MyDigit::~MyDigit(){
    if(inited){
        final_wenetloop();
        if(mat_wenet){
            delete mat_wenet;
            mat_wenet = nullptr;
        }
        if(net_curl){
            delete net_curl;
            net_curl = nullptr;
        }
        if(net_wavmat){
            delete net_wavmat;
            net_wavmat = nullptr;
        }
        delete ai_wenet;
        delete ai_scrfd;
        delete ai_pfpld;
        delete ai_munet;
    }
}

static MyDigit* g_digit = 0;

extern "C" {

    JNIEXPORT jint JNI_OnLoad(JavaVM *vm, void *reserved) {
        __android_log_print(ANDROID_LOG_DEBUG, "ncnn", "JNI_OnLoad");

        g_digit = new MyDigit;

        return JNI_VERSION_1_4;
    }

    JNIEXPORT void JNI_OnUnload(JavaVM *vm, void *reserved) {
        __android_log_print(ANDROID_LOG_DEBUG, "ncnn", "JNI_OnUnload");

        delete g_digit;
        g_digit = 0;
    }

    static std::string getStringUTF(JNIEnv *env, jstring obj) {
        char *c_str = (char *) env->GetStringUTFChars(obj, nullptr);
        std::string tmpString = std::string(c_str);
        env->ReleaseStringUTFChars(obj, c_str);
        return tmpString;
    }

    JNIEXPORT jint JNICALL
        Java_com_btows_ncnntest_SCRFDNcnn_initModel(JNIEnv *env, jobject thiz, jstring path){
            std::string str = getStringUTF(env,path);
            return g_digit->initModel(str.c_str());
        }

    JNIEXPORT jint JNICALL
        Java_com_btows_ncnntest_SCRFDNcnn_onewav(JNIEnv *env, jobject thiz, jstring wavfn,jstring dumpfn){
            std::string s_wav = getStringUTF(env,wavfn);
            std::string s_dump = getStringUTF(env,dumpfn);
            return g_digit->newwav(s_wav.c_str(),s_dump.c_str());
        }

    JNIEXPORT jint JNICALL
        Java_com_btows_ncnntest_SCRFDNcnn_netwav(JNIEnv *env, jobject thiz, jstring wavurl,jfloat duration){
            std::string s_wav = getStringUTF(env,wavurl);
            return g_digit->netwav(s_wav.c_str(),duration);
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
        g_digit->set_window(NULL);
        return 0;
    }

    // public native boolean setOutputWindow(Surface surface);
    JNIEXPORT jint JNICALL Java_com_btows_ncnntest_SCRFDNcnn_setOutputWindow(JNIEnv* env, jobject thiz, jobject surface)
    {
        ANativeWindow* win = ANativeWindow_fromSurface(env, surface);

        __android_log_print(ANDROID_LOG_DEBUG, "ncnn", "setOutputWindow %p", win);

        g_digit->set_window(win);

        return 0;
    }

}
