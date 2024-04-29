#ifndef __GPLAYER_THREAD_H__
#define __GPLAYER_THREAD_H__

#include <thread>
#include <functional>

class XThread {
public:
    XThread();

    virtual ~XThread();

    virtual bool start();

    virtual void pause();

    virtual void resume();

    virtual bool stop();

    void join();

    bool isAlive() { return mRunning; }

    bool isPausing() { return mPausing; }

    void setFunction(std::function<void(void)> func) { mFunc = func; }

protected:
    volatile bool mRunning = false;

    volatile bool mPausing = false;

    std::thread *mThread = nullptr;

    std::function<void(void)> mFunc;
};

#endif //__GPLAYER_THREAD_H__
