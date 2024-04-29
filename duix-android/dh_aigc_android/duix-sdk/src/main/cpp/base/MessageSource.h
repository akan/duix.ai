#ifndef GPLAYER_MESSAGESOURCE_H
#define GPLAYER_MESSAGESOURCE_H

#include "ConcurrentQueue.h"

#define MAX_BUFFER_PACKET_SIZE 40
#define MAX_BUFFER_FRAME_SIZE  10

#define MSG_DOMAIN_ERROR      0
#define MSG_DOMAIN_STATE      1
#define MSG_DOMAIN_TIME       2
#define MSG_DOMAIN_BUFFER     3
#define MSG_DOMAIN_DEMUXING   4
#define MSG_DOMAIN_DECODING   5
#define MSG_DOMAIN_COMPLETE   6
#define MSG_DOMAIN_SEEK       7

#define MSG_ERROR_DEMUXING    0
#define MSG_ERROR_DECODING    1
#define MSG_ERROR_RENDERING   2
#define MSG_ERROR_SEEK        3

#define MSG_DEMUXING_INIT     0
#define MSG_DEMUXING_DESTROY  1
#define MSG_DEMUXING_EOF      2

#define MSG_SEEK_START        0
#define MSG_SEEK_END          1

#define STATE_IDLE            0
#define STATE_INITIALIZED     1
#define STATE_PREPARING       2
#define STATE_PREPARED        3
#define STATE_STARTED         4
#define STATE_PAUSED          5
#define STATE_STOPPED         6
#define STATE_COMPLETED       7
#define STATE_END             8
#define STATE_ERROR           9


class Message {
public:
    Message(int from, int type, long extra) {
        this->from = from;
        this->type = type;
        this->extra = extra;
    }

    int from;
    int type;
    long extra;
};

class MessageSource {
public:
    MessageSource();

    ~MessageSource();

    void pushMessage(int from, int type, long extra);

    unsigned long readMessage(Message **message);

    void popMessage();

    void flush();

    void reset();

    unsigned long size();

private:
    ConcurrentQueue<Message *> *msgQueue;
    int currentTime = 0;
};


#endif //GPLAYER_MESSAGESOURCE_H
