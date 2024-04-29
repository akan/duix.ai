#include "jmat.h"

extern "C"{
#pragma pack(push)
#pragma pack(4)

    typedef struct _gpg_hdr {
        char        head[4];
        int         box[4];
        int         size[4];
        int         width[4];
        int         height[4];
        uint8_t     channel[4];
        uint8_t     bit[4];
    }gpg_hdr;
#pragma pack(pop)
}


int JBuf::zeros(){
    memset(m_buf,0,m_size);
    return m_size;
}

int JBuf::forceref(int bref){
    if(m_ref!=bref){
        m_ref = bref;
    }
    return 0;
}

JBuf::JBuf(uint32_t size,void* buf ){
    if(buf){
        m_ref = true;
        m_buf = buf;
        m_size = size;
    }else{
        m_ref = false;
        m_size = size;
        m_buf = malloc(size+1024);
    }
}

JBuf::~JBuf(){
    if(!m_ref){
        free(m_buf);
        m_buf = nullptr;
    }
}

JBuf::JBuf(){
    m_size = 0;
    m_buf = nullptr;
}

JMat::JMat(){
    init_tagarr();
}

void JMat::init_tagarr(){
    memset(m_tagarr,0,512*sizeof(int));
}

int* JMat::tagarr(){
    return m_tagarr;
}

int JMat::savegpg(std::string gpgfile){
    gpg_hdr ghead;
    memset(&ghead,0,sizeof(gpg_hdr));
    ghead.head[0]='g';
    ghead.head[1]='p';
    ghead.head[2]='g';
    ghead.head[3]='1';
    ghead.size[0]=m_size;
    ghead.width[0]=m_width;
    ghead.height[0]=m_height;
    ghead.channel[0]=m_channel;
    ghead.bit[0]=m_bit;

    FILE *gpgFile = NULL;
    const char* fn = gpgfile.c_str();
    if ((gpgFile = fopen(fn, "wb")) == NULL)return -1;
    fwrite(&ghead,sizeof(gpg_hdr),1,gpgFile);
    fwrite(m_buf, m_size, 1, gpgFile);
    fclose(gpgFile);
    return 0;
}

int JMat::load(std::string picfile){
    const char* fn = picfile.c_str();
    int len = strlen(fn);
    if(len<4)return -1;
    fn+= len-3;
    int gpg = (fn[0]=='g')&&(fn[1]=='p')&&(fn[2]=='g');
    if(gpg){
        return loadgpg(picfile);
    }else{
        return loadjpg(picfile);
    }

}

int JMat::loadgpg(std::string gpgfile){
    FILE *gpgFile = NULL;
    const char* fn = gpgfile.c_str();
    if ((gpgFile = fopen(fn, "rb")) == NULL)return -1;
    int rst = 0;
    while(1){
        gpg_hdr ghead;
        memset(&ghead,0,sizeof(gpg_hdr));
        fread(&ghead,sizeof(gpg_hdr),1,gpgFile);
        char* arr=ghead.head;
        if((arr[0]=='g')&&
            (arr[1]=='p')&&
            (arr[2]=='g')){

            int imgSize  = ghead.size[0];
            if(m_size<imgSize){
                if((!m_ref)&&m_buf)free(m_buf);
                m_buf = malloc(imgSize);
            }
            m_size = imgSize;
            m_width = ghead.width[0];
            m_height = ghead.height[0];
            m_channel = ghead.channel[0];
            m_bit = ghead.bit[0];
            fread(m_buf, m_size, 1, gpgFile);
        }else{
            rst = -11;
        }
        break;
    }
    fclose(gpgFile);
    return rst;
}

#ifdef USE_TURBOJPG
#include "turbojpeg.h"
int JMat::loadjpg(std::string picfile,int flag){
    tjhandle tjInstance = NULL;
    int rst = 0;
    size_t jpegSize = 0;
    size_t imgSize = 0;
    int newbuf = 0;
    unsigned char *jpegBuf = NULL;
    if(1){
        long size;
        FILE *jpegFile = NULL;
        const char* fn = picfile.c_str();
        if ((jpegFile = fopen(fn, "rb")) == NULL)return -1;
        if (fseek(jpegFile, 0, SEEK_END) < 0 || ((size = ftell(jpegFile)) < 0) || (fseek(jpegFile, 0, SEEK_SET) < 0)){
            fclose(jpegFile);
            return -2;
        }
        if (size == 0){
            fclose(jpegFile);
            return -3;
        }
        jpegSize = size;
        jpegBuf = (unsigned char*)tj3Alloc(jpegSize);
        fread(jpegBuf, jpegSize, 1, jpegFile);
        fclose(jpegFile);
    }
    if ((tjInstance = tj3Init(TJINIT_DECOMPRESS)) == NULL)return -11;
    while(1){
        unsigned char *imgBuf = NULL;
        int w, h;
        int inSubsamp, inColorspace;
        int pixelFormat = TJPF_BGR;
        rst = tj3DecompressHeader(tjInstance, jpegBuf, jpegSize);
        if(rst<0){
            rst = -12;
            break;
        }
        w = tj3Get(tjInstance, TJPARAM_JPEGWIDTH);
        h = tj3Get(tjInstance, TJPARAM_JPEGHEIGHT);
        inSubsamp = tj3Get(tjInstance, TJPARAM_SUBSAMP);
        inColorspace = tj3Get(tjInstance, TJPARAM_COLORSPACE);
        imgSize = w * h * tjPixelSize[pixelFormat];
        if(imgSize <0){
            rst = -13;
            break;
        }
        //printf("===imgSize %d m_size %d\n",imgSize,m_size);
        if(m_size<imgSize){
            if((!m_ref)&&m_buf)free(m_buf);
            m_buf = malloc(imgSize);
            m_ref = 0;
        }
        m_size = imgSize;
        imgBuf = (unsigned char *)m_buf;
        if(tj3Decompress8(tjInstance, jpegBuf, jpegSize, imgBuf, 0, pixelFormat) < 0){
            rst = -15;
            break;
        }
        //m_ref = 0;
        m_bit = 1;
        m_channel = 3;
        m_stride = w*3;
        m_width = w;
        m_height = h;
        break;
    }
    if(jpegBuf)tj3Free(jpegBuf);
    jpegBuf = NULL;
    tj3Destroy(tjInstance);
    tjInstance = NULL;
    return rst;
}

#else
int JMat::loadjpg(std::string picfile,int flag){
    return -1;
}
#endif

JMat::JMat(int w,int h,float *buf ,int c  ,int d ):JBuf(){
    m_bit = sizeof(float);
    m_width = w;
    m_height = h;
    m_channel = c;
    m_stride = d?d:w*c;
    m_size = m_bit*m_stride*m_height;
    m_buf = buf;
    m_ref = 1;
    init_tagarr();
}

JMat::JMat(int w,int h,uint8_t *buf ,int c ,int d ):JBuf(){
    m_bit = 1;
    m_width = w;
    m_height = h;
    m_channel = c;
    m_stride = d?d:w*c;
    m_size = m_bit*m_stride*m_height;
    m_buf = buf;
    m_ref = 1;
    init_tagarr();
}

JMat::JMat(int w,int h,int c ,int d ,int b):JBuf(){
    m_bit = b==0?sizeof(float):b;
    m_width = w;
    m_height = h;
    m_channel = c;
    m_stride = d?d:w*c;
    m_size = m_bit*m_stride*m_height;
    //printf("===mat %d size %d\n",m_bit,m_size);
    m_buf = malloc(m_size+m_bit*m_stride);
    memset(m_buf,0,m_size+m_bit*m_stride);
    m_ref = 0;
    init_tagarr();
}

#ifdef USE_OPENCV

cv::Mat  JMat::cvmat(){
    if(m_channel == 3){
        cv::Mat rrr(m_height,m_width,m_bit==1?CV_8UC3:CV_32FC3,m_buf);
        return rrr;
    }else if(m_channel == 1){
        cv::Mat rrr(m_height,m_width,m_bit==1?CV_8UC1:CV_32FC1,m_buf);
        return rrr;
    }else{
        cv::Mat rrr(m_height,m_width*m_channel,m_bit==1?CV_8UC1:CV_32FC1,m_buf);
        return rrr;
    }
}

int JMat::show(const char* title){
    std::string name(title);
    cv::Mat mat(m_height,m_width,m_channel==3?CV_8UC3:CV_8UC1,m_buf);
    cv::imshow(name,mat);
    return 0;
}
int JMat::tojpg(const char* fn){
    cv::Mat mat(m_height,m_width,CV_8UC3,m_buf);
    std::string name(fn);
    return cv::imwrite(name,mat);
}
#else
int JMat::show(const char* title){
    return 0;
}
int JMat::tojpg(const char* fn){
    return 0;
}
#endif


int JMat::tobin(const char* fn){
    FILE* file = fopen(fn, "w");
    if(!file)return 0;
    fwrite(m_buf, m_size, 1, file);
    fclose(file);
    return 1;
}

JMat* JMat::refclone(int ref){
    if(ref){
        if(m_bit==1){
            return new JMat(m_width,m_height,(uint8_t*)m_buf,m_channel,m_stride);
        }else{
            return new JMat(m_width,m_height,(float*)m_buf,m_channel,m_stride);
        }
    }else{
        JMat* cm = new JMat(m_width,m_height,m_channel,m_stride,m_bit);
        memcpy(cm->m_buf,m_buf,m_size);
        memcpy(cm->m_tagarr,m_tagarr,512*sizeof(int));
        return cm;
    }
}

JMat JMat::clone(){
    JMat cm(m_width,m_height,m_channel,m_stride,m_bit);
    //printf("==clone %d\n",m_size);
    memcpy(cm.m_buf,m_buf,m_size);
    memcpy(cm.m_tagarr,m_tagarr,512*sizeof(int));
    return cm;
}

#ifdef USE_OPENCV
JMat::JMat(std::string picfile,int flag):JBuf(){
    cv::Mat image = cv::imread(picfile);
    m_bit = flag?1:sizeof(float);
    m_width = image.cols;
    m_height = image.rows;
    m_channel = 3;//image.channels();
                  //printf("===channels %d\n",m_channel);
    m_stride = m_width*m_channel;
    m_size = m_bit*m_stride*m_height;
    m_buf = malloc(m_size+m_bit*m_stride);
    m_ref = 0;
    if(flag){
        memcpy(m_buf,image.data,m_size);
        //printf("===w %d h %d\n",image.cols,image.rows);
        //cv::imshow("aaa",image);
        //cv::waitKey(0);
        //cv::Mat fmat(m_height,m_width,CV_8UC3,m_buf);
        //float scale = 1.0f/255.0f;
        //image.convertTo(fmat,CV_32F,scale);
    }else{
        cv::Mat fmat(m_height,m_width,CV_32FC3,m_buf);
        float scale = 1.0f/255.0f;
        image.convertTo(fmat,CV_32F,scale);
    }
    image.release();
    init_tagarr();
}
#else
JMat::JMat(std::string picfile,int flag):JBuf(){

}
#endif

JMat::~JMat(){
}

float* JMat::fdata(){
    return (float*)m_buf;
}

float* JMat::frow(int row){
    return ((float*)m_buf)+ row*m_stride;
}

float* JMat::fitem(int row,int col){
    return ((float*)m_buf)+ row*m_stride + col;

}


uint8_t* JMat::udata(){
    return (uint8_t*)m_buf;
}
/*
   nc::NdArray<float> JMat::ncarray(){
   bool own = false;
   nc::NdArray<float> arr = nc::NdArray<float>((float*)m_buf, m_height, m_width, own);
   return arr;
   }
   */


#ifdef USE_NCNN
ncnn::Mat JMat::packingmat(){
    ncnn::Mat in_pack(m_width,m_height,1,(void*)m_buf,(size_t)4u*3,3);
    ncnn::Mat in ;
    ncnn::convert_packing(in_pack,in,1);
    return in;
}

ncnn::Mat           JMat::ncnnmat(){
    unsigned char* data = (unsigned char*)m_buf;
    if(m_channel == 3){
        ncnn::Mat mat = ncnn::Mat::from_pixels(data, ncnn::Mat::PIXEL_BGR, m_width, m_height);
        return mat;
    }else if(m_channel == 4){
        ncnn::Mat mat = ncnn::Mat::from_pixels(data, ncnn::Mat::PIXEL_BGRA, m_width, m_height);
        return mat;
    }else if(m_channel == 1){
        ncnn::Mat mat = ncnn::Mat::from_pixels(data, ncnn::Mat::PIXEL_GRAY, m_width, m_height);
        return mat;
    }else {
        ncnn::Mat mat = ncnn::Mat::from_pixels(data, ncnn::Mat::PIXEL_GRAY, m_width*m_channel, m_height);
        return mat;
    }
}
#endif

