#pragma once
#include "jmat.h"
#include <vector>
#include <mutex>

class MBufCache{
    protected:
        int     m_lineh;
        int     m_secw;
        int     m_sech;
        int     m_blockh;
        std::mutex  *m_lock;
        std::vector<JMat*>  vec_buf ;
        int     m_tagarr[512];
    public:
        JMat* secBuf(int sec);
        JMat* inxBuf(int inx);
        int*    tagarr();
        MBufCache(int initsec,int secw,int sech,int blockh);
        virtual ~MBufCache();
        void debug();
};

class MBnfCache:public MBufCache{
    public:
        MBnfCache();
        virtual ~MBnfCache();
};
