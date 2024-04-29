#include "face_utils.h"
//#include <sys/timeb.h>


cv::Mat resize_image(cv::Mat srcimg, int height, int width, int* top, int* left){
    cv::Mat dstimg;
    int srch = srcimg.rows, srcw = srcimg.cols;
    int neww = width;
    int newh = height;
    if (srch != srcw) {
        float hw_scale = (float)srch / srcw;
        if (hw_scale > 1) {
            newh = height;
            neww = int(width / hw_scale);
            cv::resize(srcimg, dstimg, cv::Size(neww, newh), cv::INTER_AREA);
            *left = int((width - neww) * 0.5);
            cv::copyMakeBorder(dstimg, dstimg, 0, 0, *left, width - neww - *left, cv::BORDER_CONSTANT, 0);
        }
        else
        {
            newh = (int)height * hw_scale;
            neww = width;
            cv::resize(srcimg, dstimg,cv::Size(neww, newh), cv::INTER_AREA);
            *top = (int)(height - newh) * 0.5;
            cv::copyMakeBorder(dstimg, dstimg, *top, height - newh - *top, 0, 0, cv::BORDER_CONSTANT, 0);

        }
    } else {
        cv::resize(srcimg, dstimg, cv::Size(neww, newh), cv::INTER_AREA);
    }
    return dstimg;
}


int dumpfile(char* file,char** pbuf){
    std::string fname(file);
    std::ifstream cache(fname,std::ios::binary);
    cache.seekg(0,std::ios::end);
    const int engSize = cache.tellg();
    cache.seekg(0,std::ios::beg);
    char *modelMem = (char*)malloc(engSize+8000);
    cache.read(modelMem,engSize);
    cache.close();
    *pbuf = modelMem;
    return engSize;
}

void dumpchar(char* abuf,int len){
    uint8_t* buf = (uint8_t*)abuf;
    printf("\n----------------------chardump------------------------\n");
    int i;
    for(i = 0; i < len; i++) {
        printf("=%u=", buf[i]);
        if( (i+1) % 16 == 0) {
            printf("\n");
        }
    }
    if(i%16 != 0) {
        printf("\n");
    }
    printf("\n----------------------chardump------------------------\n");
}


void dumpfloat(float* abuf,int len){
    printf("\n----------------------floatdump------------------------\n");
    int i;
    for(i = 0; i < len; i++) {
        printf("=%f=", abuf[i]);
        if( (i+1) % 16 == 0) {
            printf("\n");
        }
    }
    if(i%16 != 0) {
        printf("\n");
    }
    printf("\n----------------------floatdump------------------------\n");
}

void dumphex(char* abuf,int len){
    unsigned char* buf = (unsigned char*)abuf;
    int i = 0;
    printf("\n----------------------hexdump------------------------\n");
    for(i = 0; i < len; i++) {
        printf("=%02x=", buf[i]);
        if( (i+1) % 16 == 0) {
            printf("\n");
        }
    }
    if(i%16 != 0) {
        printf("\n");
    }
    printf("---------------------hexdump-------------------------\n\n");
}

int diffbuf(char* abuf,char* bbuf,int size){
    char* pa = abuf;
    char* pb = bbuf;
    int diff = 0;
    for(int k= 0;k<size;k++){
        if(*pa++==*pb++){
        }else{
            diff++;
        }
    }
    return diff;
}

uint64_t timer_msstamp() {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (ts.tv_sec*1000l) + (ts.tv_nsec/CLOCKS_PER_SEC);
}

