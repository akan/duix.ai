#include <Log.h>
#include <thread>
#include "MessageHelper.h"


MessageHelper::MessageHelper(MessageSource *messageSource, MessageCb* obj) {
    this->messageSource = messageSource;
    msgobj = obj;
}

MessageHelper::~MessageHelper() {
    notifyObj(MSG_DOMAIN_STATE, STATE_END, 0, nullptr, nullptr);
}

void MessageHelper::handleErrorMessage(Message *message) {
    const char *errorMsg = error2String(message->type, (int)message->extra);
    printf("===handle msg %s\n",errorMsg);
}

const char *MessageHelper::error2String(int errorCode, int errorExtra) {
    const char *errorMsg = nullptr;
    if (errorCode == MSG_ERROR_DEMUXING || errorCode == MSG_ERROR_DECODING) {
        errorMsg = "unknown error";
    }
    return errorMsg;
}

void MessageHelper::notifyObj(int msgId, int arg1, long arg2, const char *msg1, const char *msg2) {
    if(msgobj) msgobj->onMessageCallback(msgId, arg1, arg2, msg1, msg2); 
}
