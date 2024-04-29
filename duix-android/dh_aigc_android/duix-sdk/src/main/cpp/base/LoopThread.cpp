/*
 * Created by Gibbs on 2021/1/1.
 * Copyright (c) 2021 Gibbs. All rights reserved.
 */

#include "LoopThread.h"

#include <utility>

LoopThread::LoopThread() {
    setFunction(std::bind(&LoopThread::handleRunning, this));
}

LoopThread::~LoopThread()= default;

void LoopThread::handleRunning() {
    if (startFunc) {
        startFunc();
    }
    if (notifyFunc) {
        notifyFunc(NOTIFY_START);
    }
    isStarted = true;

    while (mRunning) {
        if (mPausing) {
            std::unique_lock<std::mutex> lck(threadLock);
            conVar.wait(lck);
            continue;
        }
        if (!updateFunc) {
            break;
        }
        bool hasParams = arg1 >= 0 || arg2 >= 0;
        int updateResult = updateFunc(arg1, arg2);
        if (hasParams) {
            arg1 = -1;
            arg2 = -1;
        }
        if (updateResult == ERROR_EXIST) {
            mRunning = false;
            continue;
        } else if (updateResult == ERROR_PAUSE) {
            mPausing = true;
            continue;
        }
    }

    if (endFunc) {
        endFunc();
    }
    if (notifyFunc) {
        notifyFunc(NOTIFY_END);
    }
    isStarted = false;
}

void LoopThread::setStartFunc(std::function<void(void)> func) {
    startFunc = std::move(func);
}

void LoopThread::setUpdateFunc(std::function<int(int, long)> func) {
    updateFunc = std::move(func);
}

void LoopThread::setEndFunc(std::function<void(void)> func) {
    endFunc = std::move(func);
}

void LoopThread::setNotifyFunc(std::function<void(int)> func) {
    notifyFunc = std::move(func);
}

void LoopThread::resume() {
    XThread::resume();
    conVar.notify_all();
}

bool LoopThread::stop() {
    if (isPausing()) {
        resume();
    }
    return XThread::stop();
}
