/*
 * Created by Gibbs on 2021/1/1.
 * Copyright (c) 2021 Gibbs. All rights reserved.
 */

#include <Log.h>
#include <thread>
#include "MessageSource.h"

#define TAG "MessageSource"

MessageSource::MessageSource() {
    LOGI("MessageSource", "CoreFlow : create MessageSource");
    msgQueue = new ConcurrentQueue<Message *>(1000000, "MessageQueue");
}

MessageSource::~MessageSource() {
    LOGI("MessageSource", "CoreFlow : MessageSource destroyed %d", msgQueue->size());
    delete msgQueue;
    msgQueue = nullptr;
}

void MessageSource::pushMessage(int from, int type, long extra) {
    if (from == MSG_DOMAIN_TIME) {
        if (currentTime == type) {
            return;
        }
        currentTime = type;
        auto message = new Message(from, type, extra);
        msgQueue->push(message);
    } else {
        auto message = new Message(from, type, extra);
        msgQueue->push(message);
    }
    LOGI(TAG, "pushMessage size = %d", msgQueue->size());
}

unsigned long MessageSource::readMessage(Message **message) {
    unsigned long size = msgQueue->front(message);
    return size;
}

void MessageSource::popMessage() {
    msgQueue->pop();
}

void MessageSource::flush() {
    msgQueue->flush();
    LOGI(TAG, "flushBuffer");
}

void MessageSource::reset() {
    msgQueue->reset();
}

unsigned long MessageSource::size() {
    return msgQueue->size();
}
