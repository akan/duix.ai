#pragma once
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
//#include <opencv2/dnn.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>

cv::Mat resize_image(cv::Mat srcimg, int height, int width, int* top, int* left);

void dumpchar(char* abuf,int len);
void dumphex(char* abuf,int len);
void dumpfloat(float* abuf,int len);
void dumpdouble(double* abuf,int len);
int dumpfile(char* file,char** pbuf);
int diffbuf(char* abuf,char* bbuf,int size);

uint64_t timer_msstamp();

