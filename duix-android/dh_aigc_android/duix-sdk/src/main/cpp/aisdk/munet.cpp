#include "munet.h"
#include "cpu.h"
#include "face_utils.h"
#include "blendgram.h"

Mobunet::Mobunet(const char* fnbin,const char* fnparam,const char* fnmsk){
    initModel(fnbin,fnparam,fnmsk);
}

Mobunet::Mobunet(const char* modeldir,const char* modelid){
    char fnbin[1024];
    char fnparam[1024];
    char fnmsk[1024];
    sprintf(fnbin,"%s/%s.bin",modeldir,modelid);
    sprintf(fnparam,"%s/%s.param",modeldir,modelid);
    sprintf(fnmsk,"%s/weight_168u.bin",modeldir);
    initModel(fnbin,fnparam,fnmsk);
}

int Mobunet::initModel(const char* binfn,const char* paramfn,const char* mskfn){
    unet.clear();
    //ncnn::set_cpu_powersave(2);
    //ncnn::set_omp_num_threads(ncnn::get_big_cpu_count());
    //unet.opt = ncnn::Option();
    //unet.opt.use_vulkan_compute = true;
    unet.opt.num_threads = ncnn::get_big_cpu_count();
    //unet.load_param("model/mobileunet_v5_wenet_sim.param");
    //unet.load_model("model/mobileunet_v5_wenet_sim.bin");
    unet.load_param(paramfn);
    unet.load_model(binfn);
    char* wbuf = NULL;
    dumpfile((char*)mskfn,&wbuf);
    mat_weights = new JMat(160,160,(uint8_t*)wbuf,1);
    mat_weights->forceref(0);
    //mat_weights->show("weight");
    //cv::waitKey(0);
    return 0;
}

Mobunet::~Mobunet(){
    unet.clear();
    if(mat_weights){
        delete mat_weights;
        mat_weights = nullptr;
    }
}

int Mobunet::domodelold(JMat* pic,JMat* msk,JMat* feat){
    JMat  picall(160*160,2,3,0,1);
    uint8_t* buf = picall.udata();
    int width = pic->width();
    int height = pic->height();
    cv::Mat c1(height,width,CV_8UC3,buf);
    cv::Mat c2(height,width,CV_8UC3,buf+width*height*3);
    cv::cvtColor(pic->cvmat(),c1,cv::COLOR_RGB2BGR);
    cv::cvtColor(msk->cvmat(),c2,cv::COLOR_RGB2BGR);
    ncnn::Mat inall = ncnn::Mat::from_pixels(buf, ncnn::Mat::PIXEL_BGR, 160*160, 2);
    inall.substract_mean_normalize(mean_vals, norm_vals);
    //inall.reshape(160,160,6);
    ncnn::Mat inwenet(256,20,1,feat->data());
    ncnn::Mat outpic;
    ncnn::Extractor ex = unet.create_extractor();
    ex.input("face", inall);
    ex.input("audio", inwenet);
    ex.extract("output", outpic);
    float outmean_vals[3] = {-1.0f, -1.0f, -1.0f};
    float outnorm_vals[3] = { 0.5f,  0.5f,  0.5f};
    outpic.substract_mean_normalize(outmean_vals, outnorm_vals);
    ncnn::Mat pakpic;
    ncnn::convert_packing(outpic,pakpic,3);
    cv::Mat cvadj(160,160,CV_32FC3,pakpic.data);
    //dumpfloat((float*)cvadj.data,160*160*3);
    cv::Mat cvreal;
    float scale = 255.0f;
    cvadj.convertTo(cvreal,CV_8UC3,scale);
    cv::Mat cvmask;
    cv::cvtColor(cvreal,cvmask,cv::COLOR_RGB2BGR);
    cv::imshow("cvreal",cvreal);
    cv::imshow("cvmask",cvmask);
    //cv::waitKey(0);
    //getchar();
    BlendGramAlpha((uchar*)cvmask.data,(uchar*)mat_weights->data(),(uchar*)pic->data(),160,160);
    return 0;
}

int Mobunet::domodel(JMat* pic,JMat* msk,JMat* feat){
    //convert to bgr
    //pic->tojpg("eee.bmp");

    //JMat  picmask(160,160,3,0,1);
    //JMat  picreal(160,160,3,0,1);
    //cv::cvtColor(pic->cvmat(),picreal.cvmat(),cv::COLOR_RGB2BGR);
    //cv::cvtColor(msk->cvmat(),picmask.cvmat(),cv::COLOR_RGB2BGR);
    ncnn::Mat inmask = ncnn::Mat::from_pixels(msk->udata(), ncnn::Mat::PIXEL_BGR2RGB, 160, 160);
    inmask.substract_mean_normalize(mean_vals, norm_vals);
    ncnn::Mat inreal = ncnn::Mat::from_pixels(pic->udata(), ncnn::Mat::PIXEL_BGR2RGB, 160, 160);
    inreal.substract_mean_normalize(mean_vals, norm_vals);
    //
    //JMat  picin(160*160,6);
    //char*  pd = (char*)picin.data();
    //memcpy(pd,inreal.data,160*160*3*4);
    //memcpy(pd+ 160*160*3*4,inmask.data,160*160*3*4);
    //
    //ncnn::Mat inpack(160,160,1,pd,(size_t)4u*6,6);
    //ncnn::Mat inpic;
    //ncnn::convert_packing(inpack,inpic,1);
    ncnn::Mat inpic(160,160,6);
    //printf("===in %d %d all %d %d\n",inreal.cstep,inreal.elempack,inpic.cstep,inpic.elempack);
    float* buf = (float*)inpic.data;
    float* pr = (float*)inreal.data;
    //printf("==%d==%f\n",pic->udata()[2],*pr);
    memcpy(buf,pr,inreal.cstep*sizeof(float)*inreal.c);
    /*
    for(int k=0;k<3;k++){
        memcpy(buf,pr,inreal.cstep*sizeof(float));
        buf += inpic.cstep;
        pr += inreal.cstep;
    }
    */
    buf+= inpic.cstep*inreal.c;
    float* pm = (float*)inmask.data;
    //printf("=%d===%f\n",msk->udata()[2],*pm);
    memcpy(buf,pm,inmask.cstep*sizeof(float)*inmask.c);
    /*
    for(int k=0;k<3;k++){
        memcpy(buf,pm,inreal.cstep*sizeof(float));
        buf += inpic.cstep;
        pm += inmask.cstep;
    }
    */

    ncnn::Mat inwenet(256,20,1,feat->data());
    //ncnn::Mat inwenet(20,256,1,feat->data());
    ncnn::Mat outpic;
    ncnn::Extractor ex = unet.create_extractor();
    ex.input("face", inpic);
    ex.input("audio", inwenet);
    ex.extract("output", outpic);
    float outmean_vals[3] = {-1.0f, -1.0f, -1.0f};
    float outnorm_vals[3] = { 127.5f,  127.5f,  127.5f};
    outpic.substract_mean_normalize(outmean_vals, outnorm_vals);
    cv::Mat cvout(160,160,CV_8UC3);
    outpic.to_pixels(cvout.data,ncnn::Mat::PIXEL_RGB2BGR);
    BlendGramAlpha((uchar*)cvout.data,(uchar*)mat_weights->data(),(uchar*)pic->data(),160,160);
    //pic->tojpg("fff.bmp");
    //getchar();
    /*
    ncnn::Mat pakpic;
    ncnn::convert_packing(outpic,pakpic,3);
    cv::Mat cvadj(160,160,CV_32FC3,pakpic.data);
    //dumpfloat((float*)cvadj.data,160*160*3);
    float scale = 255.0f;
    cvadj.convertTo(picreal.cvmat(),CV_8UC3,scale);
    cv::cvtColor(picreal.cvmat(),picmask.cvmat(),cv::COLOR_RGB2BGR);
    */
    //getchar();
    //getchar();
    //cv::Mat cout;
    //cv::cvtColor(picreal.cvmat(),cout,cv::COLOR_RGB2BGR);
    //BlendGramAlpha((uchar*)cout.data,(uchar*)mat_weights->data(),(uchar*)pic->data(),160,160);

    /*
    float outmean_vals[3] = {-1.0f, -1.0f, -1.0f};
//    float outnorm_vals[3] = { 2.0f,  2.0f,  2.0f};
    float outnorm_vals[3] = { 127.5f,  127.5f,  127.5f};
    outpic.substract_mean_normalize(outmean_vals, outnorm_vals);

    ncnn::Mat pakpic;
    ncnn::convert_packing(outpic,pakpic,3);
    //
    cv::Mat cvadj(160,160,CV_32FC3,pakpic.data);
    //
    float scale = 1.0f;
    cvadj.convertTo(picreal.cvmat(),CV_8UC3,scale);
    cv::Mat cout;
    cv::cvtColor(picreal.cvmat(),cout,cv::COLOR_RGB2BGR);
    BlendGramAlpha((uchar*)cout.data,(uchar*)mat_weights->data(),(uchar*)pic->data(),160,160);
    //BlendGramAlpha((uchar*)picreal.udata(),(uchar*)mat_weights->data(),(uchar*)pic->data(),160,160);
    //cv::cvtColor(picreal.cvmat(),pic->cvmat(),cv::COLOR_RGB2BGR);
    */
    return 0;
}


int Mobunet::preprocess(JMat* pic,JMat* feat){
    //pic 168
    cv::Mat roipic(pic->cvmat(),cv::Rect(4,4,160,160));
    JMat  picmask(160,160,3,0,1);
    JMat  picreal(160,160,3,0,1);
    cv::Mat cvmask = picmask.cvmat();
    cv::Mat cvreal = picreal.cvmat();
    roipic.copyTo(cvmask);
    roipic.copyTo(cvreal);
    cv::rectangle(cvmask,cv::Rect(5,5,150,145),cv::Scalar(0,0,0),-1);//,cv::LineTypes::FILLED);
    domodel(&picreal,&picmask,feat);
    cvreal.copyTo(roipic);
    return 0;
}

int Mobunet::fgprocess(JMat* pic,const int* boxs,JMat* feat,JMat* fg){
    int boxx, boxy ,boxwidth, boxheight ;
    boxx = boxs[0];boxy=boxs[1];boxwidth=boxs[2]-boxx;boxheight=boxs[3]-boxy;
    int stride = pic->stride();
    cv::Mat roisrc(pic->cvmat(),cv::Rect(boxx,boxy,boxwidth,boxheight));
    cv::Mat cvorig;
    cv::resize(roisrc , cvorig, cv::Size(168, 168), cv::INTER_AREA);
    JMat  pic168(168,168,(uint8_t*)cvorig.data);
    preprocess(&pic168,feat);
    cv::Mat cvrst;;
    cv::resize(cvorig , cvrst, cv::Size(boxwidth, boxheight), cv::INTER_AREA);
    cv::Mat roidst(fg->cvmat(),cv::Rect(boxx,boxy,boxwidth,boxheight));
    cvrst.copyTo(roidst);
    return 0;
}

int Mobunet::process(JMat* pic,const int* boxs,JMat* feat){
    int boxx, boxy ,boxwidth, boxheight ;
    boxx = boxs[0];boxy=boxs[1];boxwidth=boxs[2]-boxx;boxheight=boxs[3]-boxy;
    int stride = pic->stride();
    cv::Mat roisrc(pic->cvmat(),cv::Rect(boxx,boxy,boxwidth,boxheight));
    cv::Mat cvorig;
    cv::resize(roisrc , cvorig, cv::Size(168, 168), cv::INTER_AREA);
    JMat  pic168(168,168,(uint8_t*)cvorig.data);
    preprocess(&pic168,feat);
    cv::Mat cvrst;;
    cv::resize(cvorig , cvrst, cv::Size(boxwidth, boxheight), cv::INTER_AREA);
    cvrst.copyTo(roisrc);
    return 0;
}

int Mobunet::process2(JMat* pic,const int* boxs,JMat* feat){
    int boxx, boxy ,boxwidth, boxheight ;
    boxx = boxs[0];boxy=boxs[1];boxwidth=boxs[2]-boxx;boxheight=boxs[3]-boxy;
    int stride = pic->stride();

    cv::Mat cvsrc = pic->cvmat();
    printf("cvsrc %d %d \n",cvsrc.cols,cvsrc.rows);
    cv::Mat roisrc(cvsrc,cv::Rect(boxx,boxy,boxwidth,boxheight));
    cv::Mat cvorig;
    cv::resize(roisrc , cvorig, cv::Size(168, 168), cv::INTER_AREA);
    /*
    uint8_t* data =(uint8_t*)pic->data() + boxy*stride + boxx*pic->channel();
    int scale_w = 168;
    int scale_h = 168;
    ncnn::Mat prepic = ncnn::Mat::from_pixels_resize(data, ncnn::Mat::PIXEL_BGR, boxwidth, boxheight, stride,scale_w, scale_h);
    //pic 168
    cv::Mat cvorig(168,168,CV_8UC3,prepic.data);
     */

    cv::Mat roimask(cvorig,cv::Rect(4,4,160,160));
    JMat  picmask(160,160,3,0,1);
    JMat  picreal(160,160,3,0,1);
    cv::Mat cvmask = picmask.cvmat();
    cv::Mat cvreal = picreal.cvmat();
    roimask.copyTo(cvmask);
    roimask.copyTo(cvreal);

    cv::rectangle(cvmask,cv::Rect(5,5,150,150),cv::Scalar(0,0,0),-1);//,cv::LineTypes::FILLED);
//    cv::imshow("000",cvorig);
//    cv::imshow("aaa",cvmask);
//    cv::imshow("bbb",cvreal);
//    cv::waitKey(0);

    ncnn::Mat inmask = ncnn::Mat::from_pixels(picmask.udata(), ncnn::Mat::PIXEL_BGR2RGB, 160, 160);
    inmask.substract_mean_normalize(mean_vals, norm_vals);
    ncnn::Mat inreal = ncnn::Mat::from_pixels(picreal.udata(), ncnn::Mat::PIXEL_BGR2RGB, 160, 160);
    inreal.substract_mean_normalize(mean_vals, norm_vals);

    JMat  picin(160*160,2,3);
    char*  pd = (char*)picin.data();
    memcpy(pd,inreal.data,160*160*3*4);
    memcpy(pd+ 160*160*3*4,inmask.data,160*160*3*4);

//    char* pinpic = NULL;
//    dumpfile("pic.bin",&pinpic);
//    dumpfloat((float*)pd,10);
//    dumpfloat((float*)pinpic,10);
    //ncnn::Mat inpic(160,160,6,pd,4);
    ncnn::Mat inpack(160,160,1,pd,(size_t)4u*6,6);
    ncnn::Mat inpic;
    ncnn::convert_packing(inpack,inpic,1);

//    char* pwenet = NULL;
//    dumpfile("wenet.bin",&pwenet);
    ncnn::Mat inwenet(256,20,1,feat->data(),4);
    ncnn::Mat outpic;
    ncnn::Extractor ex = unet.create_extractor();
    ex.input("face", inpic);
    ex.input("audio", inwenet);
    ex.extract("output", outpic);

    float outmean_vals[3] = {-1.0f, -1.0f, -1.0f};
//    float outnorm_vals[3] = { 2.0f,  2.0f,  2.0f};
    float outnorm_vals[3] = { 127.5f,  127.5f,  127.5f};
    outpic.substract_mean_normalize(outmean_vals, outnorm_vals);

    ncnn::Mat pakpic;
    ncnn::convert_packing(outpic,pakpic,3);

    cv::Mat cvadj(160,160,CV_32FC3,pakpic.data);
    cv::Mat cvout(160,160,CV_8UC3);
    float scale = 1.0f;
    cvadj.convertTo(cvout,CV_8UC3,scale);
    //cv::imwrite("cvout.jpg",cvout);
    cv::cvtColor(cvout,roimask,cv::COLOR_RGB2BGR);
//    cvout.copyTo(roimask);

    //cv::imwrite("roimask.jpg",roimask);
    //cv::imwrite("cvorig.jpg",cvorig);
    //cv::waitKey(0);
    cv::resize(cvorig , roisrc, cv::Size(boxwidth, boxheight), cv::INTER_AREA);
    //cv::imwrite("roisrc.jpg",roisrc);
        //cv::imshow("cvsrc",cvsrc);
//    cv::imshow("roisrc",roisrc);
//    cv::imshow("cvorig",cvorig);
//    cv::waitKey(20);
    /*
    {
        uint8_t *pr = (uint8_t *) cvoutc.data;
        printf("==%u %u %u\n", pr[0], pr[1], pr[2]);
    }
    //
    float* p = (float*)cvadj.data;
    printf("==%f %f %f\n",p[0],p[1],p[2]);
    p+=160*160;
    printf("==%f %f %f\n",p[0],p[1],p[2]);
    p+=160*160;
    printf("==%f %f %f\n",p[0],p[1],p[2]);
    */
    return 0;
}

