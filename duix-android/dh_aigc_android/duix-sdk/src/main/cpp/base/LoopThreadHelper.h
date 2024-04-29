#ifndef GPLAYER_LOOPTHREADHELPER_H
#define GPLAYER_LOOPTHREADHELPER_H


#include "LoopThread.h"
#include <functional>

class LoopThreadHelper {
public:
    static LoopThread *createLoopThread(const std::function<int(int, long)>& updateFunc);

    static LoopThread *createLoopThread(const std::function<int(int, long)>& updateFunc,
                                        const std::function<void(int)>& notifyFunc);

    static LoopThread *createLoopThread(const std::function<void(void)>& startFunc,
                                        const std::function<int(int, long)>& updateFunc,
                                        const std::function<void(void)>& endFunc);

    static LoopThread *createLoopThread(const std::function<void(void)>& startFunc,
                                        const std::function<int(int, long)>& updateFunc,
                                        const std::function<void(void)>& endFunc,
                                        const std::function<void(int)>& notifyFunc);
};


#endif //GPLAYER_LOOPTHREADHELPER_H
