#include "malpha.h"
#include "blendgram.h"
#include "face_utils.h"

MWorkMat::MWorkMat(JMat* pic,JMat* msk,const int* boxs){
    m_boxx = boxs[0];
    m_boxy=boxs[1];
    m_boxwidth=boxs[2]-m_boxx;
    m_boxheight=boxs[3]-m_boxy;
    //printf("x %d y %d w %d h %d \n",m_boxx,m_boxy,m_boxwidth,m_boxheight);
    m_pic = pic;
    m_msk = msk;

    pic_real160 = new JMat(160,160,3,0,1);
    pic_mask160 = new JMat(160,160,3,0,1);
    //pic_crop160 = new JMat(160,160,3,0,1);

    msk_real160 = new JMat(160,160,1,0,1);

    //msk_mask160 = new JMat(160,160,3,0,1);

}

MWorkMat::~MWorkMat(){
    matpic_org168.release();
    matpic_roirst.release();
    delete pic_real160;
    delete pic_mask160;
    delete msk_real160;
    if(pic_clone160) delete pic_clone160;
}

int MWorkMat::munet(JMat** ppic,JMat** pmsk){
    *ppic = pic_real160;
    *pmsk = pic_mask160;
    return 0;
}

int MWorkMat::premunet(){
    matpic_roisrc = cv::Mat(m_pic->cvmat(),cv::Rect(m_boxx,m_boxy,m_boxwidth,m_boxheight));
    cv::resize(matpic_roisrc , matpic_org168, cv::Size(168, 168), cv::INTER_AREA);
    //vtacc
    matpic_roi160 = cv::Mat(matpic_org168,cv::Rect(4,4,160,160));
    cv::Mat cvmask = pic_mask160->cvmat();
    cv::Mat cvreal = pic_real160->cvmat();
    matpic_roi160.copyTo(cvmask);
    matpic_roi160.copyTo(cvreal);
    //cv::rectangle(cvmask,cv::Rect(5,5,150,150),cv::Scalar(0,0,0),-1);//,cv::LineTypes::FILLED);
    cv::rectangle(cvmask,cv::Rect(5,5,150,145),cv::Scalar(0,0,0),-1);//,cv::LineTypes::FILLED);
    //cv::rectangle(cvmask,cv::Rect(4,4,152,152),cv::Scalar(0,0,0),-1);//,cv::LineTypes::FILLED);
    //cv::imwrite("cvmask.bmp",cvmask);
    //cv::waitKey(0);
    pic_clone160 = pic_real160->refclone(0);
    return 0;
}

int MWorkMat::finmunet(JMat* fgpic){
    cv::Mat cvreal = pic_real160->cvmat();
    cvreal.copyTo(matpic_roi160);
    //cv::imwrite("accpre.bmp",matpic_org168);
    if(m_msk) vtacc((uint8_t*)matpic_org168.data,168*168);
    //cv::imwrite("accend.bmp",matpic_org168);
    cv::resize(matpic_org168, matpic_roirst, cv::Size(m_boxwidth, m_boxheight), cv::INTER_AREA);
    if(fgpic){
        matpic_roisrc = cv::Mat(fgpic->cvmat(),cv::Rect(m_boxx,m_boxy,m_boxwidth,m_boxheight));
        matpic_roirst.copyTo(matpic_roisrc);
    }else{
        matpic_roirst.copyTo(matpic_roisrc);
    }
    return 0;
}

int MWorkMat::alpha(JMat** preal,JMat** pimg,JMat** pmsk){
    *preal = pic_clone160;
    *pimg =  pic_real160;
    *pmsk =  msk_real160;
    return 0;
}

int MWorkMat::prealpha(){
    printf("x %d y %d w %d h %d \n",m_boxx,m_boxy,m_boxwidth,m_boxheight);
    //m_msk->show("cba");
    //cv::waitKey(0);
    matmsk_roisrc = cv::Mat(m_msk->cvmat(),cv::Rect(m_boxx,m_boxy,m_boxwidth,m_boxheight));
    cv::resize(matmsk_roisrc , matmsk_org168, cv::Size(168, 168), cv::INTER_AREA);

    matmsk_roi160 = cv::Mat(matmsk_org168,cv::Rect(4,4,160,160));
    cv::Mat cvmask = msk_real160->cvmat();
    cv::cvtColor(matmsk_roi160,cvmask,cv::COLOR_RGB2GRAY);

    //BlendGramAlphaRev(pic_clone160->udata(),msk_real160->udata(),pic_crop160->udata(),160,160);
    //pic_crop160->show("aaa");
    //cv::waitKey(0);
    //pic_crop160
    //
    return 0;
}

int MWorkMat::finalpha(){
    cv::Mat cvmask = msk_real160->cvmat();
    cv::cvtColor(cvmask,matmsk_roi160,cv::COLOR_GRAY2RGB);
    //
    cv::resize(matmsk_org168, matmsk_roirst, cv::Size(m_boxwidth, m_boxheight), cv::INTER_AREA);
    matmsk_roirst.copyTo(matmsk_roisrc);
    return 0;
}

int MWorkMat::vtacc(uint8_t* buf,int count){
    /*
    int avgr = 0;
    int avgb = 0;
    int avgg = 0;
    if(1){
        uint8_t* pb = m_pic->udata();
        for(int k=0;k<10;k++){
            avgr += *pb++;
            avgg += *pb++;
            avgb += *pb++;
        }
        avgr =avgr/10 +10;
        avgg =avgg/10 -20;
        if(avgg<0)avgg=0;
        avgb =avgb/10 + 10;
    }
    */
    uint8_t* pb = buf;
    for(int k=0;k<count;k++){
        int sum  = (pb[0]+ pb[2])/2.0f;
        if(pb[1]>=sum){
            pb[1]=sum;
            //pb[0]=0;
            //pb[2]=0;
            // }else if((pb[0]<avgr)&&(pb[1]>avgg)&&(pb[2]<avgb)){
            //pb[1]=0;
            //pb[0]=0;
            //pb[2]=0;
        }
        pb+=3;
    }
    /*
    long sum = 0l;
    float  mean = sum*0.5f/count;
    uint8_t maxg = (mean>255.f)?255:mean;
    //printf("sum %ld mean %f maxg %d\n",sum,mean,maxg);
    //getchar();
    pb = buf +1;
    for(int k=0;k<count;k++){
        if(*pb>maxg){
            *pb = maxg;
        }
        pb+=3;
    }
    */
    return 0;
}

int MAlpha::doModel(JMat* real,JMat* img,JMat* pha){
    if(1)return 0;
    /*
    if(1)return 0;
    JMat  picimg(160,160,3,0,1);
    JMat  picreal(160,160,3,0,1);
    cv::cvtColor(real->cvmat(),picreal.cvmat(),cv::COLOR_RGB2BGR);
    cv::cvtColor(img->cvmat(),picimg.cvmat(),cv::COLOR_RGB2BGR);
    */
    float mean_vals[3] = {127.5f, 127.5f, 127.5f};
    float norm_vals[3] = {1 / 127.5f, 1 / 127.5f, 1 / 127.5f};
    ncnn::Mat inimg = ncnn::Mat::from_pixels(img->udata(), ncnn::Mat::PIXEL_BGR2RGB, 160, 160);
    inimg.substract_mean_normalize(mean_vals, norm_vals);
    ncnn::Mat inreal = ncnn::Mat::from_pixels(real->udata(), ncnn::Mat::PIXEL_BGR2RGB, 160, 160);
    inreal.substract_mean_normalize(mean_vals, norm_vals);
    ncnn::Mat inpha = ncnn::Mat::from_pixels(pha->udata(), ncnn::Mat::PIXEL_GRAY, 160, 160);
    float gmean_vals[3] = {0.0f, 0.0f, 0.0f};
    float gnorm_vals[3] = {1 / 255.0f, 1 / 255.0f, 1 / 255.0f};
    inpha.substract_mean_normalize(gmean_vals, gnorm_vals);

    ncnn::Mat inpic(160,160,7);
    //printf("===in %d %d all %d %d pha %d %d\n",inreal.cstep,inreal.elempack,inpic.cstep,inpic.elempack,inpha.cstep,inpha.elempack);
    //JMat  picin(160,160,7);
    float* buf = (float*)inpic.data;
    memcpy(buf,inreal.data,inreal.cstep*inreal.c*sizeof(float));
    buf += inpic.cstep*inreal.c;
    memcpy(buf,inimg.data,inimg.cstep*inimg.c*sizeof(float));
    buf += inimg.cstep*inimg.c;
    memcpy(buf, inpha.data,inpha.cstep*sizeof(float));
    //ncnn::Mat inpic(160,160,7,pd,4);
    //ncnn::Mat inpack(160,160,1,pd,(size_t)4u*7,7);
    //ncnn::Mat inpic;
    //ncnn::convert_packing(inpack,inpic,1);
    ncnn::Mat outpic;
    ncnn::Extractor ex = net.create_extractor();
    int rst = ex.input("input", inpic);
    //printf("input %d\n",rst);
    rst = ex.extract("output", outpic);
    //printf("output %d\n",rst);
    float outmean_vals[3] = {0.0f, 0.0f, 0.0f};
    float outnorm_vals[3] = { 255.0f,  255.0f,  255.0f};
    outpic.substract_mean_normalize(outmean_vals, outnorm_vals);
    outpic.to_pixels(pha->udata(),ncnn::Mat::PIXEL_GRAY);
    //pha->show("mmm");
    //cv::waitKey(0);
    //dumpfloat((float*)outpic.data+160*159,10);
    //ncnn::Mat pakpic;
    //ncnn::convert_packing(outpic,pakpic,1);
    //dumpfloat((float*)pakpic.data,160*160*1);
    //getchar();
    /*
    cv::Mat cvadj(160,160,CV_32FC1,outpic.data);
    cv::Mat cvout;//(160,160,CV_8UC1);
    float scale = 255.0f;//255.0f;
    cvadj.convertTo(cvout,CV_8UC1,scale);
    cvout.copyTo(pha->cvmat());
    */
    //cv::imshow("pha",cvout);
    //pha->show("pha");
    //cv::waitKey(0);
    return 0;
}

MAlpha::MAlpha(const char* fnbin,const char* fnparam):NcnnModel(160,160){
    std::string fb(fnbin);
    std::string fp(fnparam);
    initModel(fb,fp);
}

MAlpha::~MAlpha(){

}

