/*
 * Created by Gibbs on 2021/1/1.
 * Copyright (c) 2021 Gibbs. All rights reserved.
 */

#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <Log.h>
#include "MediaData.h"

MediaData::MediaData() = default;

MediaData::MediaData(JMat* mat,bool ref ){
    m_data =  mat;
    data = mat->udata();
    size = mat->size();
    data1 = nullptr;
    size1 = 0;
    data2 = nullptr;
    size2 = 0;
    width = mat->width();;
    height = mat->height();
      m_ref = ref;
}

MediaData::MediaData(JMat* fmat,JMat* mmat,JMat* bmat,bool ref ){
    m_data =  fmat;
    m_msk =  mmat;
    m_bg =  bmat;
    data = fmat->udata();
    size = fmat->size();
    data1 = mmat->udata();
    size1 = mmat->size();
    data2 = bmat->udata();
    size2 = bmat->size();
    width = fmat->width();;
    height = fmat->height();
    m_ref = ref;
}

int MediaData::copy(MediaData* mediadata){
    //copy
    MediaData* mmm = mediadata;
    data = mmm->data;
    size = mmm->size;
    data1 = mmm->data1;
    size1 = mmm->size1;
    data2 = mmm->data2;
    size2 = mmm->size2;
    width = mmm->width;
    height = mmm->height;
    pts = mediadata->pts;
    dts = mediadata->dts;
    m_data =  mmm->m_data;
    m_msk =  mmm->m_msk;
    m_bg =  mmm->m_bg;
    m_ref = true;
    mmm->reset();
    return 0;
}

MediaData::~MediaData() {
    if(m_data){
        delete m_data;
        m_data = nullptr;
    }
    if(m_msk){
        delete m_msk;
        m_msk = nullptr;
    }
    if(m_bg){
        delete m_bg;
        m_bg = nullptr;
    }
}

void MediaData::reset(int flag){
        m_data = nullptr;
        m_msk = nullptr;
        if(!flag) m_bg = nullptr;
}

void MediaData::print() const {
    //LOGI("MediaData", "print MediaData %d %d %d", size, size1, size2);
}

