#include "pfpld.h"
#include "cpu.h"


static int pts68_pfpld(float* arr_pts98,float* arr_pts68){
    float* arr = arr_pts98;
    float* dst = arr_pts68;
    for(int j=0;j<17;j++){
        *dst++ = arr[j*4];
        *dst++ = arr[j*4 + 1];
    }
    for(int j=33;j<38;j++){
        *dst++ = arr[j*2];
        *dst++ = arr[j*2 + 1];
    }
    for(int j=42;j<47;j++){
        *dst++ = arr[j*2];
        *dst++ = arr[j*2 + 1];
    }
    for(int j=51;j<61;j++){
        *dst++ = arr[j*2];
        *dst++ = arr[j*2 + 1];
    }
    float* points = arr;
    float point_38_x = (float(points[60 * 2 + 0]) + float(points[62 * 2 + 0])) / 2.0;
    float point_38_y = (float(points[60 * 2 + 1]) + float(points[62 * 2 + 1])) / 2.0;
    float point_39_x = (float(points[62 * 2 + 0]) + float(points[64 * 2 + 0])) / 2.0;
    float point_39_y = (float(points[62 * 2 + 1]) + float(points[64 * 2 + 1])) / 2.0;
    float point_41_x = (float(points[64 * 2 + 0]) + float(points[66 * 2 + 0])) / 2.0;
    float point_41_y = (float(points[64 * 2 + 1]) + float(points[66 * 2 + 1])) / 2.0;
    float point_42_x = (float(points[60 * 2 + 0]) + float(points[66 * 2 + 0])) / 2.0;
    float point_42_y = (float(points[60 * 2 + 1]) + float(points[66 * 2 + 1])) / 2.0;
    float point_44_x = (float(points[68 * 2 + 0]) + float(points[70 * 2 + 0])) / 2.0;
    float point_44_y = (float(points[68 * 2 + 1]) + float(points[70 * 2 + 1])) / 2.0;
    float point_45_x = (float(points[70 * 2 + 0]) + float(points[72 * 2 + 0])) / 2.0;
    float point_45_y = (float(points[70 * 2 + 1]) + float(points[72 * 2 + 1])) / 2.0;
    float point_47_x = (float(points[72 * 2 + 0]) + float(points[74 * 2 + 0])) / 2.0;
    float point_47_y = (float(points[72 * 2 + 1]) + float(points[74 * 2 + 1])) / 2.0;
    float point_48_x = (float(points[68 * 2 + 0]) + float(points[74 * 2 + 0])) / 2.0;
    float point_48_y = (float(points[68 * 2 + 1]) + float(points[74 * 2 + 1])) / 2.0;

    *dst++ = point_38_x;
    *dst++ = point_38_y;
    *dst++ = point_39_x;
    *dst++ = point_39_y;
    *dst++ = points[64 * 2 + 0];
    *dst++ = points[64 * 2 + 1];
    *dst++ = point_41_x;
    *dst++ = point_41_y;
    *dst++ = point_42_x;
    *dst++ = point_42_y;
    *dst++ = points[68 * 2 + 0];
    *dst++ = points[68 * 2 + 1];
    *dst++ = point_44_x;
    *dst++ = point_44_y;
    *dst++ = point_45_x;
    *dst++ = point_45_y;
    *dst++ = points[72 * 2 + 0];
    *dst++ = points[72 * 2 + 1];
    *dst++ = point_47_x;
    *dst++ = point_47_y;
    *dst++ = point_48_x;
    *dst++ = point_48_y;
    for(int j = 76; j<96;j++){
        *dst++ = points[j * 2 + 0];
        *dst++ = points[j * 2 + 1];
    }
    int len = dst-arr_pts68;
    printf("====%d\n",len);
    return len;
}


int Pfpld::detect(JMat* pic,int* arrboxs,int* arrlands){
    int w = arrboxs[0];
    int h = arrboxs[1];

    int boxx, boxy ,boxwidth, boxheight ;
    int* boxs = arrboxs+6;
    boxx = boxs[0];boxy=boxs[1];boxwidth=boxs[2]-boxx;boxheight=boxs[3]-boxy;

    printf("===pfpld %d %d %d %d\n",boxx,boxy,boxwidth,boxheight);
    //boxx=192; boxy =245 ; boxwidth=908 -boxx;boxheight=1203 - boxy;
    int stride = pic->stride();
    uint8_t* data =(uint8_t*)pic->data() + boxy*stride + boxx*pic->channel();
    ncnn::Mat in = ncnn::Mat::from_pixels_resize(data, ncnn::Mat::PIXEL_BGR2RGB, boxwidth, boxheight, stride,scale_w, scale_h);
    in.substract_mean_normalize(mean_vals, norm_vals);
    ncnn::Extractor ex = pfpld.create_extractor();
    ex.input("input", in);
    ncnn::Mat pose_blob, landms_blob;
    ex.extract("pose", pose_blob);
    ex.extract("landms", landms_blob);
    float* arr = (float*)landms_blob.data;
    float tmpdst[512];
    float* dst = tmpdst;
    pts68_pfpld(arr,dst);

    int len = 68*2;
    int* apts = arrlands;
    int axmin = 10000,aymin = 10000,axmax = 0,aymax = 0;
    for(int j=0;j<len/2;j++){
        float x = *dst++ * boxwidth + boxx;
        float y = *dst++ * boxheight + boxy;
        apts[0] = x;
        apts[1] = y;
        printf("==adj %d x %f y %f\n",j,x,y);
        if(axmin>x) axmin = x;
        if(axmax<x) axmax = x;
        if(aymin>y) aymin = y;
        if(aymax<y) aymax = y;
        apts += 2;
    }

    printf("===all xmin %d ymin %d xmax %d ymax %d\n",axmin,aymin,axmax,aymax);
    m_wh = (axmax-axmin)*1.0f/(aymax-aymin);
    if(m_wh>1.0f)m_wh = 1.0f;
    printf("====wh %f\n",m_wh);
    float wh = m_wh;//0.8878923766816144;
    int xmin = 10000,ymin = 10000,xmax = 0,ymax = 0;
    apts = arrlands+2;
    for(int j=1;j<16;j++){
        int x = *apts++;
        int y = *apts++;
        if(xmin>x) xmin = x;
        if(xmax<x) xmax = x;
        if(ymin>y) ymin = y;
        if(ymax<y) ymax = y;
        printf("==min one %d x %d y %d\n",j,x,y);
    }
    printf("===b1 xmin %d ymin %d xmax %d ymax %d\n",xmin,ymin,xmax,ymax);

    apts = arrlands+30*2;
    for(int j=31;j<68;j++){
        int x = *apts++;
        int y = *apts++;
        if(xmin>x) xmin = x;
        if(xmax<x) xmax = x;
        if(ymin>y) ymin = y;
        if(ymax<y) ymax = y;
        printf("==min two %d x %d y %d\n",j,x,y);
    }

    printf("===xmin %d ymin %d xmax %d ymax %d\n",xmin,ymin,xmax,ymax);
    int rw = xmax-xmin;
    int rh = ymax-ymin;
    int x_c = xmin + rw/2;
    float x1,x2,y1,y2;
    x1 = x_c - rw*0.5f/wh;
    x2 = x_c + rw*0.5f/wh;
    y1 = ymin + rw*0.11f/wh;
    y2 = ymin + rw*1.11f/wh;
    if(x1<0)x1=0;
    if(y1<0)y1=0;
    if(x2>=m_width)x2=m_width-1;
    if(y2>=m_height)y2=m_height-1;
    boxs = arrboxs+10;
    *boxs++ = x1;
    *boxs++ = y1;
    *boxs++ = x2;
    *boxs++ = y2;
    return 0;
}

void Pfpld::recal(int w,int h){
    m_width = w;
    m_height = h;
}

Pfpld::Pfpld(const char* modeldir,const char* modelid,int w,int h){
    pfpld.clear();
    ncnn::set_cpu_powersave(2);
    ncnn::set_omp_num_threads(ncnn::get_big_cpu_count());
    pfpld.opt = ncnn::Option();
    //pfpld.opt.use_vulkan_compute = true;
    pfpld.opt.num_threads = ncnn::get_big_cpu_count();
    //pfpld.load_param("model/pfpld.param");
    //pfpld.load_model("model/pfpld.bin");
    char filepath[1024];
    sprintf(filepath,"%s/%s.param",modeldir,modelid);
    pfpld.load_param(filepath);
    sprintf(filepath,"%s/%s.bin",modeldir,modelid);
    pfpld.load_model(filepath);
    recal(w,h);
}

Pfpld::~Pfpld(){
}

#ifdef _PFPLD_MAIN_
int main(int argc,char** argv){
    Pfpld* pfpld = new Pfpld(1080,1920);
    std::string picfile("1.jpg");
    JMat* pic = new JMat(picfile,1);
    int* arr = pic->tagarr();
    pfpld->detect(pic,arr+6,arr+64);
    printf("precess to exit\n");
    getchar();
    return 0;
}
#endif
