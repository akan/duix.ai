#include "turbojpeg.h"
#include <stdio.h>
#include <stdint.h>
#include <string>
#include <string.h>

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

//#include "opencv2/core.hpp"
//#include "opencv2/imgproc.hpp"
//#include "opencv2/highgui.hpp"



int cvtmain(int jpg,std::string picfile,std::string gpgfile){
        bool        m_ref = 0;
        uint32_t    m_size = 0;
        void*       m_buf = NULL;
        int     m_bit = 0;
        int     m_width = 0;
        int     m_height = 0;
        int     m_channel = 0;
        int     m_stride = 0;

if(1){
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
        m_ref = 0;
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
    if(rst)return rst;
}else{
    /*
    cv::Mat image = cv::imread(picfile);
    m_bit = 1;
    m_width = image.cols;
    m_height = image.rows;
    if(m_width<1)return -999;
    m_channel = 3;//image.channels();
    m_stride = m_width*m_channel;
    m_size = m_bit*m_stride*m_height;
    m_buf = malloc(m_size+m_bit*m_stride);
    m_ref = 0;
    memcpy(m_buf,image.data,m_size);
    image.release();
    */
}

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


int main(int argc,char** argv){
    if(argc<3){
        printf("ggpgmain filein fileout\n");
        return 0;
    }
    std::string picfile(argv[1]);
    std::string gpgfile(argv[2]);

    const char* fn = picfile.c_str();
    int len = strlen(fn);
    if(len<4)return -1;
    fn+= len-3;
    int jpg = (fn[0]=='j')&&(fn[1]=='p')&&(fn[2]=='g');

    int rst = cvtmain(jpg,picfile,gpgfile);
    return 0;
}
