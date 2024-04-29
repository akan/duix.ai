#ifndef GPLAYER_MSGCBERJNI_H
#define GPLAYER_MSGCBERJNI_H

#include <jni.h>
#include <string>
#include "CommObj.h"


class MsgcbJni:public MessageCb {
public:
    MsgcbJni(jobject obj);

    virtual ~MsgcbJni();

    virtual void onMessageCallback(int msgId, int arg1, long arg2, const char *msg1, const char *msg2);

    virtual void onMessageCallback(int msgId, int arg1, long arg2, const char *msg1, const char *msg2, jobject obj);

private:
    jobject msgcbJObj;
    jmethodID onMessageCallbackMethod;
};


#endif //GPLAYER_GPLAYERJNI_H
