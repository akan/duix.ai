
#include <Log.h>
#include <JniHelper.h>
#include "MsgcbJni.h"

#define TAG "MediaSourceJni"

MsgcbJni::MsgcbJni(jobject obj) {
    LOGI(TAG, "create MsgcbJni");
    JNIEnv *env = JniHelper::getJNIEnv();
    msgcbJObj = env->NewGlobalRef(obj);
    jclass sourceClass = env->GetObjectClass(obj);
    onMessageCallbackMethod = env->GetMethodID(sourceClass, "onMessageCallback",
                                               "(IIJLjava/lang/String;Ljava/lang/String;Ljava/lang/Object;)V");
}

MsgcbJni::~MsgcbJni() {
    bool attach = JniHelper::attachCurrentThread();
    JNIEnv *env = JniHelper::getJNIEnv();
    if (msgcbJObj != nullptr && env) {
        env->DeleteGlobalRef(msgcbJObj);
        msgcbJObj = nullptr;
        onMessageCallbackMethod = nullptr;
    }
    if (attach) {
        JniHelper::detachCurrentThread();
    }
    LOGE(TAG, "MsgcbJni destroyed");
}

void
MsgcbJni::onMessageCallback(int msgId, int arg1, long arg2, const char *msg1, const char *msg2) {
    onMessageCallback(msgId, arg1, arg2, msg1, msg2, (jobject)nullptr);
}

void
MsgcbJni::onMessageCallback(int msgId, int arg1, long arg2, const char *msg1, const char *msg2, jobject obj) {
    if (msgcbJObj != nullptr && onMessageCallbackMethod != nullptr) {
        bool attach = JniHelper::attachCurrentThread();

        JNIEnv *env = JniHelper::getJNIEnv();
        jstring jMsg1 = msg1 ? JniHelper::newStringUTF(env, msg1) : nullptr;
        jstring jMsg2 = msg2 ? JniHelper::newStringUTF(env, msg2) : nullptr;
        JniHelper::callVoidMethod(msgcbJObj, onMessageCallbackMethod, msgId, arg1, arg2, jMsg1,
                                  jMsg2, obj);
        if (jMsg1) {
            env->DeleteLocalRef(jMsg1);
        }
        if (jMsg2) {
            env->DeleteLocalRef(jMsg2);
        }
        if (attach) {
            JniHelper::detachCurrentThread();
        }
    }
}
