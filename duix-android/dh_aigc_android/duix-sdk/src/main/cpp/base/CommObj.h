#ifndef GPLAYER_COMMOBJ_H_
#define GPLAYER_COMMOBJ_H_

#ifdef  ANDROID
#include <jni.h>
typedef jobject CommObj;
#else
class CommObj{};
#endif

class MessageCb{
    public:
        MessageCb(){};
        virtual ~MessageCb(){};
        virtual void onMessageCallback(int msgId, int arg1, long arg2, const char *msg1, const char *msg2){};
        virtual void onMessageCallback(int msgId, int arg1, long arg2, const char *msg1, const char *msg2, CommObj obj){};
};

#endif
