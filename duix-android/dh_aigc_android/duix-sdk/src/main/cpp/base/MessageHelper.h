#ifndef GPLAYER_MESSAGEHELPER_H
#define GPLAYER_MESSAGEHELPER_H
#include "CommObj.h"

#include "MessageSource.h"

class MessageHelper {
public:
    MessageHelper(MessageSource *messageSource, MessageCb* obj);

    virtual ~MessageHelper();

    virtual void handleErrorMessage(Message *message);

    static const char *error2String(int errorCode, int errorExtra);

    virtual void notifyObj(int msgId, int arg1, long arg2, const char* msg1, const char* msg2);

private:
    MessageSource *messageSource;
    MessageCb *msgobj;
};


#endif //GPLAYER_MESSAGEHELPER_H
