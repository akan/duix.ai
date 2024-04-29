#ifndef GPLAYER_MEDIADATA_H
#define GPLAYER_MEDIADATA_H

#include <string>
#include "jmat.h"

class MediaData {
    private:
        bool m_ref = false;
    public:
        MediaData();

        MediaData(JMat* mat,bool ref = true);
        MediaData(JMat* fmat,JMat* mmat,JMat* bmat,bool ref = true);

        ~MediaData();

        void print() const;
        int copy(MediaData* mediadata);
        void reset(int flag = 0);

    public:
        JMat* m_data = nullptr;
        JMat* m_msk = nullptr;
        JMat* m_bg = nullptr;

        uint8_t *data = nullptr;
        uint32_t size = 0;
        uint8_t *data1 = nullptr;
        uint32_t size1 = 0;
        uint8_t *data2 = nullptr;
        uint32_t size2 = 0;


        uint64_t pts = 0;
        uint64_t dts = 0;
        uint32_t width = 0;
        uint32_t height = 0;
        uint8_t flag = 0;//与FLAG_KEY_相关
};


#endif //GPLAYER_MEDIADATA_H
