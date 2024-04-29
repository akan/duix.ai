#include "jmat.h"
#include "scrfd.h"
#include "pfpld.h"
#include "munet.h"
#include "wenet.h"
#include "utils/face_utils.h"

int mainprocess(int argc,char** argv){
    const char* modeldir = "../model";
    std::string picfile("../mybin/r1.jpg");
    JMat* pic = new JMat(picfile,1);
    char* pwenet = NULL;
    dumpfile("../mybin/wenet.bin",&pwenet);
    Mobunet* munet = new Mobunet(modeldir,"mobileunet_v5_wenet_sim");
    int* arr = pic->tagarr();
    int* boxs = arr+10;
//    408, 140, 647, 379
    boxs[0]=140;boxs[1]=408;boxs[2]=379;boxs[3]=647;
    float* pw = (float*)pwenet;
    for(int k=0;k<250;k++) {
        JMat dfeat(256, 20, pw, 1);
        JMat onepic = pic->clone();
        munet->process(&onepic,arr,&dfeat);
        onepic.show("pic");
        printf("===one\n");
        cv::waitKey(10);
        pw+=20*256;
    }
    cv::waitKey(0);
    return 0;
}

int mainpre(int argc,char** argv){
    const char* modeldir = "../model";
    std::string picfile("../mybin/orig_b.jpg");
    JMat* pic = new JMat(picfile,1);
    char* pwenet = NULL;
    dumpfile("../mybin/lab.bin",&pwenet);
    JMat *dfeat = new JMat(256, 20, (float *) pwenet, 1);
    Mobunet* munet = new Mobunet(modeldir,"mobileunet_v5_wenet_sim");
    munet->preprocess(pic,dfeat);
    pic->show("pic");
    dumpchar((char*)pic->data(),10);
    pic->tojpg("outc.jpg");
    cv::waitKey(0);
    delete pic;
    free(pwenet);
    delete dfeat;
    delete munet;
    return 0;
}

int mainmodel(int argc,char** argv){
    const char* modeldir = "../model";
    std::string picfile("../mybin/real_b.jpg");
    JMat* dpic = new JMat(picfile,1);
    dumpchar((char*)dpic->data(),10);
    picfile = std::string("../mybin/mask_b.jpg");
    JMat* dmsk = new JMat(picfile,1);
    char* pwenet = NULL;
    dumpfile("../mybin/wenet.bin",&pwenet);
    Mobunet* munet = new Mobunet(modeldir,"mobileunet_v5_wenet_sim");
    float* pd = (float*)pwenet;
    for(int k=0;k<250;k++) {
        JMat onepic = dpic->clone();
        JMat dfeat(256, 20, pd, 1);
        munet->domodel(dpic,dmsk,&dfeat);
        onepic.show("dpic");
    }
    dumpchar((char*)dpic->data(),10);
    cv::waitKey(0);
    getchar();
    return 0;
}

int maintest(int argc,char** argv){

    std::string picfile("../mybin/r1.jpg");
    JMat* rpic = new JMat(picfile,1);
    picfile = std::string("../mybin/c1.jpg");
    JMat* cpic = new JMat(picfile,1);

    char* frpic = NULL;
    dumpfile("../mybin/image.bin",&frpic);
    JMat* rimg = new JMat(540,960,(uint8_t*)frpic);

    char* fcpic = NULL;
    dumpfile("../mybin/mask_b.bin",&fcpic);
    JMat* cimg = new JMat(168,168,(uint8_t*)fcpic);
    //getchar();

    cv::Mat cvsrc = rpic->cvmat();
    int boxx, boxy ,boxwidth, boxheight ;
    boxy = 408;
    boxx = 140;
    boxwidth = 379 - boxx;
    boxheight = 647 - boxy;
    cv::Mat roisrc(cvsrc,cv::Rect(boxx,boxy,boxwidth,boxheight));
    JMat* proisrc = new JMat(boxwidth,boxheight,3,0,1);
    /*
    roisrc.copyTo(proisrc->cvmat());
    dumphex(fcpic,10);
    dumphex((char*)proisrc->data(),10);
    dumphex(fcpic+boxwidth*3,10);
    getchar();
    */

    printf("roisrc %d %d \n",roisrc.cols,roisrc.rows);

    cv::Mat cvorig ;
    //cv::resize(roisrc , cvorig, cv::Size(168, 168), cv::INTER_AREA);
    cv::resize(roisrc , cvorig, cv::Size(168, 168), cv::INTER_LINEAR);
    printf("====gogog\n");
    dumphex((char*)cvorig.data,10);
    dumphex((char*)fcpic,10);


    picfile = std::string("../mybin/d1.jpg");
    JMat* dpic = new JMat(picfile,1);


    //cpic->show("hecrop");
    //cv::imshow("mycrop",cvorig);
    //p168->show("p168");

    //cv::waitKey(0);

    return 0;
}

int mainwenet(int argc,char** argv){
    Wenet net("../model","wenet");
    JMat* m_bnf = nullptr;
    int rst = net.nextwav("../mybin/1.wav",&m_bnf);
    printf("===one wav %d height %d\n",rst,m_bnf->height());
    int* arr = m_bnf->tagarr();
    printf("wavsize %d pcmsample %d melsize %d bnfsize %d bnfblock %d\n",arr[0],arr[1],arr[2],arr[5],arr[6]);
    dumpfloat(m_bnf->fdata(),10);
    float* buf = net.nextbnf(m_bnf,arr[6]-1);
    dumpfloat(buf,10);
    dumpfloat(buf+256,10);
    return 0;
}


int mainall(int argc,char** argv){
    const char* modeldir = "../model";
    Scrfd* scrfd = new Scrfd(modeldir,"scrfd_500m_kps-opt2",1080,1920);
    std::string picfile("../mybin/r1.jpg");
    JMat* pic = new JMat(picfile,1);
    int* arr = pic->tagarr();
    scrfd->detect(pic,arr);
    printf("===w h %d %d \n",arr[0],arr[1]);
    printf("===rect %d %d %d %d\n",arr[2],arr[3],arr[4],arr[5]);
    printf("===rect %d %d %d %d\n",arr[6],arr[7],arr[8],arr[9]);
    printf("precess to pfpld\n");
    getchar();
    Pfpld* pfpld = new Pfpld(modeldir,"pfpld",1080,1920);
    pfpld->detect(pic,arr,arr+64);
    printf("===rect %d %d %d %d\n",arr[10],arr[11],arr[12],arr[13]);
    printf("precess to munet\n");
    getchar();
    Wenet net("../model","wenet");
    JMat* m_bnf = nullptr;
    int rst = net.nextwav("../mybin/1.wav",&m_bnf);
    printf("===one wav %d height %d\n",rst,m_bnf->height());
    char* pwenet = (char*)m_bnf->data();
    //dumpfile("../mybin/wenet.bin",&pwenet);
    Mobunet* munet = new Mobunet(modeldir,"mobileunet_v5_wenet_sim");
    int* boxs = arr+10;
//    408, 140, 647, 379
    printf("===x1y1 %d %d x2y2 %d %d\n",boxs[0],boxs[1],boxs[2],boxs[3]);
    getchar();
    //boxs[0]=140;boxs[1]=408;boxs[2]=379;boxs[3]=647;
    //105 416 x2y2 412 723

    cv::Mat cvsrc = pic->cvmat();
    cv::imshow("one",cvsrc);
    cv::waitKey(0);
    printf("====gogogo\n");
    for(int k=0;k<40;k++) {
        JMat onepic = pic->clone();
        JMat *feat = new JMat(256, 20, (float *) pwenet, 1);
        munet->process(&onepic, arr, feat);
        cv::imshow("one",onepic.cvmat());
        cv::waitKey(0);
        pwenet+=20*256*4;
    }
    cv::waitKey(0);
    printf("precess to exit\n");
    getchar();
    delete pic;
    return 0;
}

int main(int argc,char** argv){
    return mainwenet(argc,argv);
}
