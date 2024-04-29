#ifndef GPLAYER_LOOPTHREAD_H
#define GPLAYER_LOOPTHREAD_H

#include "XThread.h"
#include <functional>
#include <mutex>
#include <condition_variable>

#define ERROR_EXIST -100
#define ERROR_PAUSE -101

#define NOTIFY_START 0
#define NOTIFY_END   1

using namespace std;

class LoopThread : public XThread {
public:
    LoopThread();

	virtual ~LoopThread();

    void setStartFunc(std::function<void(void)> func);

    void setUpdateFunc(std::function<int(int, long)> func);

    void setEndFunc(std::function<void(void)> func);

    void setNotifyFunc(std::function<void(int)> func);

    void resume() override ;

    bool stop() override ;

    bool hasStarted() {
		return isStarted;
    }

    void setArgs(int a, long b) {
		this->arg1 = a;
		this->arg2 = b;
    }

protected:
    void handleRunning();

private:
	std::function<void(void)> startFunc;
	std::function<int(int, long)> updateFunc;
	std::function<void(void)> endFunc;
	std::function<void(int)> notifyFunc;
	bool isStarted = false;
	std::mutex threadLock;
	std::condition_variable conVar;

	int arg1 = -1;
	long arg2 = -1;
};

#endif //GPLAYER_LOOPTHREAD_H
