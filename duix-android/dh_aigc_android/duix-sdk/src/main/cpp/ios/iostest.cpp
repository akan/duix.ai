#include <stdio.h>
#include "jmat.h"
#include "gjdigit.h"
#include "blendgram.h"



int main(int argc,char** argv){
    gjdigit_t* gd = NULL;
    int rst = 0;
    rst = gjdigit_alloc(&gd);

    rst = gjdigit_initWenet(gd,"../../model/wenet.onnx");
    rst = gjdigit_initMunet(gd,"../../model/dh_model.param","../../model/dh_model.bin","../../model/weight_168u.bin");
    //rst = gjdigit_initMunet(gd,"../../model/zy_and_lw.param","../../model/zy_and_lw.bin","../../model/weight_168u.bin");
    rst = gjdigit_initMalpha(gd,"../../model/alpha_model.param","../../model/alpha_model.bin");
    //rst = gjdigit_initMunet(gd,"../../model/mobileunet_v5_wenet_sim.param","../../model/mobileunet_v5_wenet_sim.bin","../../model/weight_168u.bin");
    //int wavframe = gjdigit_onewav(gd,"../../mybin/1.wav");
    //int wavframe = gjdigit_onewav(gd,"../../mybin/1.wav");
    int wavframe = gjdigit_onewav(gd,"test.wav");
    //int wavframe = gjdigit_test(gd);//gjdigit_onewav(gd,"../../mybin/1.wav");
if(1){
    printf("====wavframe %d\n",wavframe);
    for(int k=0;k<wavframe;k++){
    //for(int k=0;k<10000;k++){
        //std::string fn = "../../mybin/t2.jpg";
        //std::string fn = "../../mybin/aaa1.jpg";
        std::string fn = "../../mybin/r1.jpg";
        JMat mat;
        mat.loadjpg(fn,1);
        //mat.show("pic");
        //cv::waitKey(0);
        fn = "../../mybin/m1.jpg";
        JMat msk;
        msk.loadjpg(fn,1);
        fn = "../../mybin/bg.jpg";
        JMat bg;
        bg.loadjpg(fn,1);
        JMat morg = msk.clone();

        int boxs[4];
        //148 221 242 315
        //boxs[1]=148;boxs[0]=221; boxs[3]=242;boxs[2]=315;
        boxs[0]=140;boxs[1]=408;boxs[2]=379;boxs[3]=647;

        //mat.show("pic");
        //cv::waitKey(0);
        //rst = gjdigit_matrst(gd,mat.udata(),mat.width(),mat. height(),boxs,k);
        rst = gjdigit_maskrst(gd,mat.udata(),mat.width(),mat.height(),boxs,msk.udata(),mat.udata(),mat.udata(),k);
        //rst = gjdigit_maskrst(gd,mat.udata(),mat.width(),mat.height(),boxs,msk.udata(),mat.udata(),NULL,k);
        //rst = gjdigit_maskrst(gd,mat.udata(),mat.width(),mat. height(),boxs,msk.udata(),mat.udata(),bg.udata(),0);
        //BlendGramAlpha3(bg.udata(),msk.udata(),mat.udata(),mat.width(),mat. height());
        //if(rst<0)break;
        //mat.show("pic");
        //cv::Mat diff = msk.cvmat()-morg.cvmat();
        //cv::imshow("msk",msk.cvmat());
        //cv::waitKey(0);
        printf("===one %d rst %d\n",k,rst);
        //cv::waitKey(40);
        //break;
    }
}
    //cv::waitKey(0);
    gjdigit_free(&gd);
    return 0;
}
