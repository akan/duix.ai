#include <malloc.h>
#include "JniHelper.h"
#include "Log.h"

#define TAG "JniHelper"

using namespace std;

JavaVM *JniHelper::sJavaVM = nullptr;

JNIEnv *JniHelper::getJNIEnv() {
    if (sJavaVM == nullptr) {
        LOGE(TAG, "sJavaVM is nullptr");
        return nullptr;
    }

    JNIEnv *env = nullptr;
    bool attached = false;
    switch (sJavaVM->GetEnv((void **) &env, JNI_VERSION_1_4)) {
        case JNI_OK:
            break;
        case JNI_EDETACHED:
            if (sJavaVM->AttachCurrentThread(&env, nullptr) != 0) {
                LOGE(TAG, "Could not attach current thread");
            }
            attached = true;
            break;
        case JNI_EVERSION:
            LOGE(TAG, "Invalid java version");
            break;
        default:
            break;
    }

    if (attached) {
        sJavaVM->DetachCurrentThread();
    }

    return env;
}

bool JniHelper::attachCurrentThread() {
    if (sJavaVM == nullptr) {
        LOGE(TAG, "sJavaVM is nullptr");
        return false;
    }

    JNIEnv *env = nullptr;
    bool attached = false;
    switch (sJavaVM->GetEnv((void **) &env, JNI_VERSION_1_4)) {
        case JNI_OK:
            break;
        case JNI_EDETACHED:
            if (sJavaVM->AttachCurrentThread(&env, nullptr) != 0) {
                LOGE(TAG, "Could not attach current thread");
            } else {
                attached = true;
            }
            break;
        case JNI_EVERSION:
            LOGE(TAG, "Invalid java version");
            break;
        default:
            break;
    }

    return attached;
}

void JniHelper::detachCurrentThread() {
    sJavaVM->DetachCurrentThread();
}

void JniHelper::throwException(JNIEnv *env, const char *className, const char *msg) {
    jclass exception = env->FindClass(className);
    env->ThrowNew(exception, msg);
}

jstring JniHelper::newStringUTF(JNIEnv *env, const char *data) {
    if (!data) return nullptr;
    jstring str = nullptr;
    int size = strlen(data);
    jbyteArray array = env->NewByteArray(size);
    if (!array) {  // OutOfMemoryError exception has already been thrown.
        LOGE(TAG, "convertString: OutOfMemoryError is thrown.");
    } else {
        env->SetByteArrayRegion(array, 0, size, (jbyte *) data);
        jclass string_Clazz = env->FindClass("java/lang/String");
        jmethodID string_initMethodID = env->GetMethodID(string_Clazz, "<init>",
                                                         "([BLjava/lang/String;)V");
        jstring utf = env->NewStringUTF("UTF-8");
        str = (jstring) env->NewObject(string_Clazz, string_initMethodID, array, utf);
        env->DeleteLocalRef(utf);
        env->DeleteLocalRef(array);
    }
    return str;
};

jobject JniHelper::createByteBuffer(JNIEnv *env, unsigned char *buffer, int size) {
    if (env == nullptr || buffer == nullptr) {
        return nullptr;
    }

    jobject byteBuffer = env->NewDirectByteBuffer(buffer, size);
    //byteBuffer = env->NewGlobalRef(byteBuffer);

    return byteBuffer;
}

jobject JniHelper::createByteBuffer(JNIEnv *env, int size) {
    if (env == nullptr) {
        return nullptr;
    }

    auto buffer = static_cast<uint8_t *>(malloc(static_cast<size_t>(size)));
    jobject byteBuffer = env->NewDirectByteBuffer(buffer, size);
    free(buffer);
    return byteBuffer;
}

void JniHelper::deleteLocalRef(jobject jobj) {
    JNIEnv *env = JniHelper::getJNIEnv();
    if (env == nullptr || jobj == nullptr) {
        return;
    }

    env->DeleteLocalRef(jobj);
}

string JniHelper::getStringUTF(JNIEnv *env, jstring obj) {
    char *c_str = (char *) env->GetStringUTFChars(obj, nullptr);
    string tmpString = std::string(c_str);
    env->ReleaseStringUTFChars(obj, c_str);
    return tmpString;
}

char *JniHelper::getCharArrayUTF(JNIEnv *env, jstring obj) {
    char *c_str = (char *) env->GetStringUTFChars(obj, nullptr);
    env->ReleaseStringUTFChars(obj, c_str);
    return c_str;
}

void JniHelper::callVoidMethod(jobject obj, jmethodID methodId) {
    if (sJavaVM == nullptr) {
        LOGE(TAG, "sJavaVM is nullptr");
        return;
    }

    JNIEnv *env = nullptr;
    bool attached = false;
    switch (sJavaVM->GetEnv((void **) &env, JNI_VERSION_1_4)) {
        case JNI_OK:
            break;
        case JNI_EDETACHED:
            if (sJavaVM->AttachCurrentThread(&env, nullptr) != 0) {
                LOGE(TAG, "Could not attach current thread");
            }
            attached = true;
            break;
        case JNI_EVERSION:
            LOGE(TAG, "Invalid java version");
            break;
        default:
            break;
    }

    if (env != nullptr) {
        env->CallVoidMethod(obj, methodId);
    }

    if (attached) {
        sJavaVM->DetachCurrentThread();
    }
}

void JniHelper::callVoidMethod(jobject obj, jmethodID methodId, jint arg1, jint arg2, jint arg3, jint arg4) {
    if (sJavaVM == nullptr) {
        LOGE(TAG, "sJavaVM is nullptr");
        return;
    }

    JNIEnv *env = nullptr;
    bool attached = false;
    switch (sJavaVM->GetEnv((void **) &env, JNI_VERSION_1_4)) {
        case JNI_OK:
            break;
        case JNI_EDETACHED:
            if (sJavaVM->AttachCurrentThread(&env, nullptr) != 0) {
                LOGE(TAG, "Could not attach current thread");
            }
            attached = true;
            break;
        case JNI_EVERSION:
            LOGE(TAG, "Invalid java version");
            break;
        default:
            break;
    }

    if (env != nullptr) {
        env->CallVoidMethod(obj, methodId, arg1, arg2, arg3, arg4);
    }

    if (attached) {
        sJavaVM->DetachCurrentThread();
    }
}

void
JniHelper::callVoidMethod(jobject obj, jmethodID methodId, jint arg1, jint arg2, jint arg3,
                          jstring arg4, jstring arg5, jobject arg6) {
    if (sJavaVM == nullptr) {
        LOGE(TAG, "sJavaVM is nullptr");
        return;
    }

    JNIEnv *env = nullptr;
    bool attached = false;
    switch (sJavaVM->GetEnv((void **) &env, JNI_VERSION_1_4)) {
        case JNI_OK:
            break;
        case JNI_EDETACHED:
            if (sJavaVM->AttachCurrentThread(&env, nullptr) != 0) {
                LOGE(TAG, "Could not attach current thread");
            }
            attached = true;
            break;
        case JNI_EVERSION:
            LOGE(TAG, "Invalid java version");
            break;
        default:
            break;
    }

    if (env != nullptr) {
        env->CallVoidMethod(obj, methodId, arg1, arg2, arg3, arg4, arg5, arg6);
    }

    if (attached) {
        sJavaVM->DetachCurrentThread();
    }
}

int JniHelper::callIntMethod(jobject obj, jmethodID methodId, jobject arg1, jint arg2) {
    if (sJavaVM == nullptr) {
        LOGE(TAG, "sJavaVM is nullptr");
        return -1;
    }

    JNIEnv *env = nullptr;
    bool attached = false;
    switch (sJavaVM->GetEnv((void **) &env, JNI_VERSION_1_4)) {
        case JNI_OK:
            break;
        case JNI_EDETACHED:
            if (sJavaVM->AttachCurrentThread(&env, nullptr) != 0) {
                LOGE(TAG, "Could not attach current thread");
            }
            attached = true;
            break;
        case JNI_EVERSION:
            LOGE(TAG, "Invalid java version");
            break;
        default:
            break;
    }

    int ret = -1;
    if (env != nullptr) {
        ret = env->CallIntMethod(obj, methodId, arg1, arg2);
    }

    if (attached) {
        sJavaVM->DetachCurrentThread();
    }

    return ret;
}


void JniHelper::callStaticVoidMethod(jclass cls, jmethodID methodId, jint arg1) {
    if (sJavaVM == nullptr) {
        LOGE(TAG, "sJavaVM is nullptr");
        return;
    }

    JNIEnv *env = nullptr;
    bool attached = false;
    switch (sJavaVM->GetEnv((void **) &env, JNI_VERSION_1_4)) {
        case JNI_OK:
            break;
        case JNI_EDETACHED:
            if (sJavaVM->AttachCurrentThread(&env, nullptr) != 0) {
                LOGE(TAG, "Could not attach current thread");
            }
            attached = true;
            break;
        case JNI_EVERSION:
            LOGE(TAG, "Invalid java version");
            break;
        default:
            break;
    }

    if (env != nullptr) {
        env->CallStaticVoidMethod(cls, methodId, arg1);
    }

    if (attached) {
        sJavaVM->DetachCurrentThread();
    }
}

jobject JniHelper::callObjectMethod(jobject obj, jmethodID methodId) {
    if (sJavaVM == nullptr) {
        LOGE(TAG, "sJavaVM is nullptr");
        return nullptr;
    }

    JNIEnv *env = nullptr;
    bool attached = false;
    switch (sJavaVM->GetEnv((void **) &env, JNI_VERSION_1_4)) {
        case JNI_OK:
            break;
        case JNI_EDETACHED:
            if (sJavaVM->AttachCurrentThread(&env, nullptr) != 0) {
                LOGE(TAG, "Could not attach current thread");
            }
            attached = true;
            break;
        case JNI_EVERSION:
            LOGE(TAG, "Invalid java version");
            break;
        default:
            break;
    }

    jobject ret = nullptr;
    if (env != nullptr) {
        ret = env->CallObjectMethod(obj, methodId);
    }

    if (attached) {
        sJavaVM->DetachCurrentThread();
    }

    return ret;
}

jboolean JniHelper::callBooleanMethod(jobject obj, jmethodID methodId) {
    if (sJavaVM == nullptr) {
        LOGE(TAG, "sJavaVM is nullptr");
        return false;
    }

    JNIEnv *env = nullptr;
    bool attached = false;
    switch (sJavaVM->GetEnv((void **) &env, JNI_VERSION_1_4)) {
        case JNI_OK:
            break;
        case JNI_EDETACHED:
            if (sJavaVM->AttachCurrentThread(&env, nullptr) != 0) {
                LOGE(TAG, "Could not attach current thread");
            }
            attached = true;
            break;
        case JNI_EVERSION:
            LOGE(TAG, "Invalid java version");
            break;
        default:
            break;
    }

    jboolean ret;
    if (env != nullptr) {
        ret = env->CallBooleanMethod(obj, methodId);
    }

    if (attached) {
        sJavaVM->DetachCurrentThread();
    }

    return ret;
}
