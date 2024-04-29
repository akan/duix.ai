#ifndef GPLAYER_FRAMESOURCE_H
#define GPLAYER_FRAMESOURCE_H


#include "MediaData.h"
#include "ConcurrentQueue.h"


class FrameSource {

public:
    FrameSource(int audioMaxSize, int videoMaxSize);

    ~FrameSource();

public:
    unsigned long pushAudFrame(MediaData *frame);

    unsigned long pushVidFrame(MediaData *frame);

    unsigned long pushVidRecyle(JMat *frame);
    unsigned long popVidRecyle(JMat **frame);

    unsigned long readAudFrame(MediaData **frame);

    unsigned long readVidFrame(MediaData **frame);

    void popAudFrame(MediaData *frame);

    void popVidFrame(MediaData *frame);

    void flush();

    void reset();

    unsigned long audioSize();

    unsigned long videoSize();
    unsigned long recyleSize();

private:
    ConcurrentQueue<MediaData *> *audioPacketQueue;
    ConcurrentQueue<MediaData *> *videoPacketQueue;
    ConcurrentQueue<JMat *> *videoRecyleQueue;
};


#endif //GPLAYER_FRAMESOURCE_H
