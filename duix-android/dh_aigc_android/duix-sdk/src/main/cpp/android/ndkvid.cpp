#include "ndkvid.h"

void MyVideo::reset(){
      renderstart = -1;
      sawInputEOS = false;
      sawOutputEOS = false;
      isPlaying = false;
      renderonce = true;
}

MyVideo::MyVideo(const char* videofile){
    int len = strlen(videofile);
    memcpy(filename,videofile,len+1);
    reset();
}

MyVideo::~MyVideo(){
}

int MyVideo::open(){
    ex = AMediaExtractor_new();
    media_status_t err = AMediaExtractor_setDataSource( ex, filename);
    if (err != AMEDIA_OK) return -2;
    int numtracks = AMediaExtractor_getTrackCount(ex);
    for (int i = 0; i < numtracks; i++) {
        AMediaFormat *format = AMediaExtractor_getTrackFormat(ex, i);
        const char *s = AMediaFormat_toString(format);
        const char *mime;
        if (!AMediaFormat_getString(format, AMEDIAFORMAT_KEY_MIME, &mime)) {
            return -3;
        } else if (!strncmp(mime, "video/", 6)) {
            AMediaExtractor_selectTrack(ex, i);

            AMediaFormat_getInt32(format, AMEDIAFORMAT_KEY_FRAME_RATE, &frameRate) ;
            AMediaFormat_getInt32(format, AMEDIAFORMAT_KEY_WIDTH, &frameWidth) ;
            AMediaFormat_getInt32(format, AMEDIAFORMAT_KEY_HEIGHT, &frameHeight) ;
            int64_t dur = 0;
            AMediaFormat_getInt64(format, AMEDIAFORMAT_KEY_DURATION, &dur) ;
            duration = dur;
            codec = AMediaCodec_createDecoderByType(mime);
            isPlaying = true;
            //AMediaCodec_configure(codec, format, texture->getRenderWindow(), NULL, 0);
            AMediaCodec_start(codec);
        }
        AMediaFormat_delete(format);
    }
    return 0;
}

int MyVideo::close(){
    AMediaCodec_stop(codec);
    AMediaCodec_delete(codec);
    AMediaExtractor_delete(ex);
    sawInputEOS = true;
    sawOutputEOS = true;
    return 0;
}

int MyVideo::next(JMat* mat){
    size_t  outsize = 0;
    ssize_t bufidx = -1;
    int64_t presentationTimeUs = -1;
    while(!sawInputEOS || !sawOutputEOS){
        if(!sawInputEOS){
            bufidx = AMediaCodec_dequeueInputBuffer(codec, 0);
            if (bufidx >= 0) {
                size_t bufsize;
                auto buf = AMediaCodec_getInputBuffer(codec, bufidx, &bufsize);
                auto sampleSize = AMediaExtractor_readSampleData(ex, buf, bufsize);
                if (sampleSize < 0) {
                    sampleSize = 0;
                    sawInputEOS = true;
                    isPlaying = false;
                }
                presentationTimeUs = AMediaExtractor_getSampleTime(ex);
                AMediaCodec_queueInputBuffer( codec, bufidx, 0, sampleSize, presentationTimeUs, sawInputEOS ? AMEDIACODEC_BUFFER_FLAG_END_OF_STREAM : 0);
                AMediaExtractor_advance(ex);
            }
        }
        if (!sawOutputEOS) {
            AMediaCodecBufferInfo info;
            bufidx = AMediaCodec_dequeueOutputBuffer(codec, &info, 0);
            if (bufidx >= 0) {
                if (info.flags & AMEDIACODEC_BUFFER_FLAG_END_OF_STREAM) {
                    sawOutputEOS = true;
                    isPlaying = false;
                }
                position = presentationTimeUs;
                if(mat){
                    uint8_t* output = AMediaCodec_getOutputBuffer(codec, bufidx, &outsize);
                    if(outsize>0){
                        memcpy(mat->data(),output,outsize);
                    }
                }
                //
                AMediaCodec_releaseOutputBuffer(codec, bufidx, false);
                break;
            } else if (bufidx == AMEDIACODEC_INFO_OUTPUT_FORMAT_CHANGED) {
                auto format = AMediaCodec_getOutputFormat(codec);
                AMediaFormat_delete(format);
            }
        }
    }
    return outsize;
}
