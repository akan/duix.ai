/*
 * Created by Gibbs on 2021/1/1.
 * Copyright (c) 2021 Gibbs. All rights reserved.
 */

#include "LoopThreadHelper.h"

LoopThread *
LoopThreadHelper::createLoopThread(const std::function<int(int, long)>& updateFunc) {
    return LoopThreadHelper::createLoopThread(nullptr, updateFunc, nullptr);
}

LoopThread *LoopThreadHelper::createLoopThread(const std::function<int(int, long)>& updateFunc,
                                               const std::function<void(int)>& notifyFunc) {
    return LoopThreadHelper::createLoopThread(nullptr, updateFunc, nullptr, notifyFunc);
}

LoopThread *LoopThreadHelper::createLoopThread(const std::function<void(void)>& startFunc,
                                               const std::function<int(int, long)>& updateFunc,
                                               const std::function<void()>& endFunc) {
    return LoopThreadHelper::createLoopThread(startFunc, updateFunc, endFunc, nullptr);
}

LoopThread *LoopThreadHelper::createLoopThread(const std::function<void(void)>& startFunc,
                                               const std::function<int(int, long)>& updateFunc,
                                               const std::function<void()>& endFunc,
                                               const std::function<void(int)>& notifyFunc) {
    auto *thread = new LoopThread();
    if (startFunc != nullptr) {
        thread->setStartFunc(startFunc);
    }
    if (updateFunc != nullptr) {
        thread->setUpdateFunc(updateFunc);
    }
    if (endFunc != nullptr) {
        thread->setEndFunc(endFunc);
    }
    if (notifyFunc != nullptr) {
        thread->setNotifyFunc(notifyFunc);
    }
    thread->start();
    return thread;
}
