#ifndef GPLAYER_CONCURRENTQUEUE_H
#define GPLAYER_CONCURRENTQUEUE_H

#include <deque>
#include <mutex>
#include <Log.h>
#include <condition_variable>


template<typename T>
class ConcurrentQueue {
public:
    ConcurrentQueue(int maxSize, std::string,int simp=0);

    ~ConcurrentQueue();

    unsigned long push(T data);

    unsigned long front(T *data);

    void pop();

    void flush();

    unsigned long size();

    void reset();

    virtual void deleteItem(T data);

    void log(const char *msg);

private:
    unsigned long maxSize;
    int m_simp;
    std::string tag;
    std::deque<T> queue;

    volatile bool isWaitingIn;
    std::mutex inConLock;
    std::condition_variable inConVar;
    volatile bool isWaitingOut;
    std::mutex outConLock;
    std::condition_variable outConVar;
    std::mutex queueLock;
};

template<typename T>
ConcurrentQueue<T>::ConcurrentQueue(int maxSize, std::string tag,int simp) {
    this->maxSize = maxSize > 1 ? maxSize : 1;
    this->tag = tag;
    this->m_simp = simp;
}

template<typename T>
ConcurrentQueue<T>::~ConcurrentQueue() {

}

template<typename T>
unsigned long ConcurrentQueue<T>::push(T data) {
    if(m_simp){
        if (size() >= maxSize) {
            deleteItem(data);
            return 0;
        }
        queueLock.lock();
        queue.push_back(data);
        queueLock.unlock();
        return size();
    }
    queue.push_back(data);
    if (isWaitingOut) {
        log("output notify all");
        outConVar.notify_all();
    }
    if (size() >= maxSize) {
        isWaitingIn = true;
        log("input waiting start");
        std::unique_lock<std::mutex> lck(inConLock);
        inConVar.wait_for(lck, std::chrono::milliseconds(200));
        log("input waiting end");
        isWaitingIn = false;
    }
    return size();
}

template<typename T>
unsigned long ConcurrentQueue<T>::front(T *data) {
    if(m_simp){
        if (size() <= 0) return 0;
        *data = queue.front();
        if (!(*data)) {
            pop();
            return 0;
        }
        return size();
    }
    if (size() <= 0) {
        isWaitingOut = true;
        log("output waiting start");
        std::unique_lock<std::mutex> lck(outConLock);
        outConVar.wait_for(lck, std::chrono::milliseconds(200));
        log("output waiting end");
        isWaitingOut = false;
        return 0;
    }
    *data = queue.front();
    if (!(*data)) {
        pop();
        return 0;
    }
    return size();
}

template<typename T>
void ConcurrentQueue<T>::pop() {
    queueLock.lock();
    if (size() > 0) {
        T data = queue.front();
        if(!m_simp)deleteItem(data);
        queue.pop_front();
    }
    queueLock.unlock();
    if(m_simp)return;
    if (isWaitingIn) {
        log("input notify all");
        inConVar.notify_all();
    }
}

template<typename T>
void ConcurrentQueue<T>::flush() {
    queueLock.lock();
    while (size() > 0) {
        T data = queue.front();
        deleteItem(data);
        queue.pop_front();
    }
    queueLock.unlock();
}

template<typename T>
unsigned long ConcurrentQueue<T>::size() {
    return queue.size();
}

template<typename T>
void ConcurrentQueue<T>::reset() {
    inConVar.notify_all();
    outConVar.notify_all();
}

template<typename T>
void ConcurrentQueue<T>::deleteItem(T data) {
    delete data;
}

template<typename T>
void ConcurrentQueue<T>::log(const char *msg) {
    LOGD(tag.c_str(), msg);
}


#endif //GPLAYER_CONCURRENTQUEUE_H
