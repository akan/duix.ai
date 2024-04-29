#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#include "blendgram.h"


  void  exColorBlend_Normal(uint8* T,uint8* A,uint8* B){ ColorBlend_Buffer(T,A,B,Normal); }
  void  exColorBlend_Lighten(uint8* T,uint8* A,uint8* B)       { ColorBlend_Buffer(T,A,B,Lighten);}
  void  exColorBlend_Darken(uint8* T,uint8* A,uint8* B)        { ColorBlend_Buffer(T,A,B,Darken);}
  void  exColorBlend_Multiply(uint8* T,uint8* A,uint8* B)      { ColorBlend_Buffer(T,A,B,Multiply);}
  void  exColorBlend_Average(uint8* T,uint8* A,uint8* B)       { ColorBlend_Buffer(T,A,B,Average);}
  void  exColorBlend_Add(uint8* T,uint8* A,uint8* B)           { ColorBlend_Buffer(T,A,B,Add);}

  void  exColorBlend_Subtract(uint8* T,uint8* A,uint8* B)      { ColorBlend_Buffer(T,A,B,Subtract);}
  void  exColorBlend_Difference(uint8* T,uint8* A,uint8* B)    { ColorBlend_Buffer(T,A,B,Difference);}
  void  exColorBlend_Negation(uint8* T,uint8* A,uint8* B)      { ColorBlend_Buffer(T,A,B,Negation);}
  void  exColorBlend_Screen(uint8* T,uint8* A,uint8* B)        { ColorBlend_Buffer(T,A,B,Screen);}
  void  exColorBlend_Exclusion(uint8* T,uint8* A,uint8* B)     { ColorBlend_Buffer(T,A,B,Exclusion);}

  void  exColorBlend_Overlay(uint8* T,uint8* A,uint8* B)       { ColorBlend_Buffer(T,A,B,Overlay);}
  void  exColorBlend_SoftLight(uint8* T,uint8* A,uint8* B)     { ColorBlend_Buffer(T,A,B,SoftLight);}
  void  exColorBlend_HardLight(uint8* T,uint8* A,uint8* B)     { ColorBlend_Buffer(T,A,B,HardLight);}
  void  exColorBlend_ColorDodge(uint8* T,uint8* A,uint8* B)    { ColorBlend_Buffer(T,A,B,ColorDodge);}
  void  exColorBlend_ColorBurn(uint8* T,uint8* A,uint8* B)     { ColorBlend_Buffer(T,A,B,ColorBurn);}

  void  exColorBlend_LinearDodge(uint8* T,uint8* A,uint8* B)   { ColorBlend_Buffer(T,A,B,LinearDodge);}
  void  exColorBlend_LinearBurn(uint8* T,uint8* A,uint8* B)    { ColorBlend_Buffer(T,A,B,LinearBurn);}
  void  exColorBlend_LinearLight(uint8* T,uint8* A,uint8* B)   { ColorBlend_Buffer(T,A,B,LinearLight);}
  void  exColorBlend_VividLight(uint8* T,uint8* A,uint8* B)    { ColorBlend_Buffer(T,A,B,VividLight);}
  void  exColorBlend_PinLight(uint8* T,uint8* A,uint8* B)      { ColorBlend_Buffer(T,A,B,PinLight);}

  void  exColorBlend_HardMix(uint8* T,uint8* A,uint8* B)       { ColorBlend_Buffer(T,A,B,HardMix);}
  void  exColorBlend_Reflect(uint8* T,uint8* A,uint8* B)       { ColorBlend_Buffer(T,A,B,Reflect);}
  void  exColorBlend_Glow(uint8* T,uint8* A,uint8* B)          { ColorBlend_Buffer(T,A,B,Glow);}
  void  exColorBlend_Phoenix(uint8* T,uint8* A,uint8* B)       { ColorBlend_Buffer(T,A,B,Phoenix);}

typedef void (*BlendFunc) (uint8* T,uint8* A,uint8* B);
static int MAX_FUNC = 25;
static BlendFunc blendfuncs[25]={
  &exColorBlend_Normal,
  &exColorBlend_Lighten,
  &exColorBlend_Darken,
  &exColorBlend_Multiply,
  &exColorBlend_Average,
  &exColorBlend_Add,

  &exColorBlend_Subtract,
  &exColorBlend_Difference,
  &exColorBlend_Negation,
  &exColorBlend_Screen,
  &exColorBlend_Exclusion,

  &exColorBlend_Overlay,
  &exColorBlend_SoftLight,
  &exColorBlend_HardLight,
  &exColorBlend_ColorDodge,
  &exColorBlend_ColorBurn,

  &exColorBlend_LinearDodge,
  &exColorBlend_LinearBurn,
  &exColorBlend_LinearLight,
  &exColorBlend_VividLight,
  &exColorBlend_PinLight,

  &exColorBlend_HardMix,
  &exColorBlend_Reflect,
  &exColorBlend_Glow,
  &exColorBlend_Phoenix
};

void BlendGramSimp(unsigned char *Src,unsigned char* Mask, unsigned char *Dest, int Width, int Height, int Mode)
{
	if(Mode<1)return;
	if(Mode>=MAX_FUNC)return;
	BlendFunc func=blendfuncs[Mode];
	unsigned char *LinePS, *LinePD,*LinePM;
	for (int Y = 0; Y < Height; Y += 1)
	{
		LinePS = Src + Y * Width * 4;
		LinePM = Mask + Y * Width * 4;
		LinePD = Dest + Y * Width * 4;
		for (int X = 0; X < Width; X += 1)
		{
			func(LinePD,LinePS,LinePM);
			LinePS += 4;
			LinePM += 4;
			LinePD += 4;
		}
	}
}

void BlendGramAlpha3(unsigned char *Src,unsigned char* Mask, unsigned char *Dest, int Width, int Height)
{
    printf("w %d h %d\n",Width,Height);
	unsigned char *LinePS, *LinePD,*LinePM;
	for (int Y = 0; Y < Height; Y += 1)
	{
		LinePS = Src + Y * Width * 3;
		LinePM = Mask + Y * Width * 3;
		LinePD = Dest + Y * Width * 3;
		for (int X = 0; X < Width; X += 1)
		{
			//func(LinePD,LinePS,LinePM);
            //ColorBlend_Alpha(LinePD,LinePD,LinePS,*LinePM);
            float alpha = *LinePM/255.0f;
            float beta = 1.0f-alpha;
            //if(beta<0.5f) printf("==alpha %f beta %f\n",alpha,beta);
            //if(beta<0.5f) printf("od %u ps %u\n",LinePD[0],LinePS[0]);
            LinePD[0] =  CLAMPCOLOR(     LinePD[0]*alpha+LinePS[0]*beta);
            //if(beta<0.5f) printf("new %u ps%u \n",LinePD[0],LinePS[0]);
            //if(beta<0.5f) getchar();
            LinePD[1] = CLAMPCOLOR(LinePD[1]*alpha+LinePS[1]*beta);
            LinePD[2] = CLAMPCOLOR( LinePD[2]*alpha+LinePS[2]*beta);
			LinePS += 3;
			LinePM += 3;
			LinePD += 3;
		}
	}
}

void BlendGramAlpha(unsigned char *Src,unsigned char* Mask, unsigned char *Dest, int Width, int Height)
{
	unsigned char *LinePS, *LinePD,*LinePM;
	for (int Y = 0; Y < Height; Y += 1)
	{
		LinePS = Src + Y * Width * 3;
		LinePM = Mask + Y * Width * 1;
		LinePD = Dest + Y * Width * 3;
		for (int X = 0; X < Width; X += 1)
		{
			//func(LinePD,LinePS,LinePM);
            ColorBlend_Alpha(LinePD,LinePD,LinePS,*LinePM);
            /*
            float alpha = *LinePM/255.0f;
            float beta = 1.0f-alpha;
            //printf("==alpha %f beta %f\n",alpha,beta);
            LinePD[0] = LinePD[0]*alpha+LinePS[0]*beta;
            LinePD[1] = LinePD[1]*alpha+LinePS[1]*beta;
            LinePD[2] = LinePD[2]*alpha+LinePS[2]*beta;
            */
			LinePS += 3;
			LinePM += 1;
			LinePD += 3;
		}
	}
}

void BlendGramAlphaRev(unsigned char *Src,unsigned char* Mask, unsigned char *Dest, int Width, int Height)
{
	unsigned char *LinePS, *LinePD,*LinePM;
	for (int Y = 0; Y < Height; Y += 1)
	{
		LinePS = Src + Y * Width * 3;
		LinePM = Mask + Y * Width * 1;
		LinePD = Dest + Y * Width * 3;
		for (int X = 0; X < Width; X += 1)
		{
			//func(LinePD,LinePS,LinePM);
            ColorBlend_Alpha(LinePD,LinePS,LinePD,*LinePM);
			LinePS += 3;
			LinePM += 1;
			LinePD += 3;
		}
	}
}




/*
void BlendGram(CBitmap* image,CBitmap* mask,int mode)
{
	if(mode<1)return;
	if(mode>=MAX_FUNC)return;
	BlendFunc func=blendfuncs[mode];
	int Stride=image->width*4;
		unsigned char *LinePS, *LinePD,*LinePM;
	for (int Y = 0; Y < image->height; Y += 1)
	{
		LinePS = (unsigned char*)image->pixels +image->stride*Y;
		LinePM = (unsigned char*)mask->pixels + mask->stride*Y;
		LinePD = (unsigned char*)image->pixels +image->stride*Y;
		for (int X = 0; X < image->width; X += 1)
		{
			func(LinePD,LinePS,LinePM);
			LinePS += 4;
			LinePM += 4;
			LinePD += 4;
		}
	}
}

void BlendImageAdjustWithMask(CBitmap* bmp,CBitmap* adj,CBitmap* dst ,CBitmap* msk,int mode)
{
	unsigned char* bmppixels=(unsigned char*)bmp->pixels;
	unsigned char* mskpixels=(unsigned char*)msk->pixels;
	unsigned char* dstpixels=(unsigned char*)dst->pixels;
	unsigned char* adjpixels=(unsigned char*)adj->pixels;
	int stride=bmp->stride;
	int width=bmp->width;
	int height=bmp->height;
	int X,Y;
	unsigned char* LinePS , * LinePM , * LinePD , * LinePA ;
	#pragma omp parallel for private(LinePS,LinePM,LinePD,LinePA,X,Y)
	for (Y = 0; Y < height; Y ++)
	{
		int offset=stride*Y;
		LinePS = bmppixels +offset;
		LinePM = mskpixels +offset;
		LinePD = dstpixels +offset;
		LinePA = adjpixels +offset;
		for (X = 0; X < width; X ++)
		{
			unsigned char M=*LinePM;
			if(M==0xFF){
				LinePD[0]=LinePS[0];
				LinePD[1]=LinePS[1];
				LinePD[2]=LinePS[2];
			}else if(M==0x00){
				LinePD[0]=LinePA[0];
				LinePD[1]=LinePA[1];
				LinePD[2]=LinePA[2];
			}else{
				ColorBlend_Alpha(LinePD,LinePS,LinePA,M);
			}
			LinePD[3]=LinePS[3];
			LinePS += 4; LinePM += 4; LinePD += 4; LinePA += 4;
		}
	}
}


void BlendImageAdjustWithMaskEx(CBitmap* bmp,CBitmap* adj,CBitmap* dst ,CBitmap* msk,int mode)
{
	unsigned char* bmppixels=(unsigned char*)bmp->pixels;
	unsigned char* mskpixels=(unsigned char*)msk->pixels;
	unsigned char* dstpixels=(unsigned char*)dst->pixels;
	unsigned char* adjpixels=(unsigned char*)adj->pixels;
	int stride=bmp->stride;
	int width=bmp->width;
	int height=bmp->height;
	int X,Y;
	unsigned char* LinePS , * LinePM , * LinePD , * LinePA ;
	#pragma omp parallel for private(LinePS,LinePM,LinePD,LinePA,X,Y)
	for (Y = 0; Y < height; Y ++)
	{
		int offset=stride*Y;
		LinePS = bmppixels +offset;
		LinePM = mskpixels +offset;
		LinePD = dstpixels +offset;
		LinePA = adjpixels +offset;
		for (X = 0; X < width; X ++)
		{
			unsigned char M=*LinePM;
			if(M==0xFF){
				LinePD[0]=LinePS[0];
				LinePD[1]=LinePS[1];
				LinePD[2]=LinePS[2];
			}else if(M==0x00){
				LinePD[0]=LinePA[0];
				LinePD[1]=LinePA[1];
				LinePD[2]=LinePA[2];
			}else{
				//ColorBlend_Alpha(LinePD,LinePS,LinePA,M);
				LinePD[0]=LinePS[0]*M>>8;
				LinePD[1]=LinePS[1]*M>>8;
				LinePD[2]=LinePS[2]*M>>8;
			}
			LinePD[3]=M;
			LinePS += 4; LinePM += 4; LinePD += 4; LinePA += 4;
		}
	}
}




void BlendImageAdjustWithAlpha(CBitmap* bmp,CBitmap* adj,CBitmap* dst ,int alpha,int mode){
	unsigned char* bmppixels=(unsigned char*)bmp->pixels;
	unsigned char* dstpixels=(unsigned char*)dst->pixels;
	unsigned char* adjpixels=(unsigned char*)adj->pixels;
	int stride=bmp->stride;
	int width=bmp->width;
	int height=bmp->height;
	int X,Y;
	unsigned char M=CLAMPCOLOR(alpha);
	unsigned char *LinePS ,  *LinePD , *LinePA ;
	#pragma omp parallel for private(LinePS,LinePD,LinePA,X,Y)
	for (Y = 0; Y < height; Y ++)
	{
		int offset=stride*Y;
		LinePS = bmppixels +offset;
		LinePD = dstpixels +offset;
		LinePA = adjpixels +offset;
		for (X = 0; X < width; X ++)
		{
			if(M==0xFF){
				LinePD[0]=LinePS[0];
				LinePD[1]=LinePS[1];
				LinePD[2]=LinePS[2];
			}else if(M==0x00){
				LinePD[0]=LinePA[0];
				LinePD[1]=LinePA[1];
				LinePD[2]=LinePA[2];
			}else{
				ColorBlend_Alpha(LinePD,LinePS,LinePA,M);
			}
			LinePD[3]=LinePS[3];
			LinePS += 4;  LinePD += 4; LinePA += 4;
		}
	}
}

void BlendImageAdjustWithAlphaMask(CBitmap* bmp,CBitmap* adj,CBitmap* dst ,CBitmap* msk,int alpha,int mode){
	unsigned char* bmppixels=(unsigned char*)bmp->pixels;
	unsigned char* mskpixels=(unsigned char*)msk->pixels;
	unsigned char* dstpixels=(unsigned char*)dst->pixels;
	unsigned char* adjpixels=(unsigned char*)adj->pixels;
	int stride=bmp->stride;
	int width=bmp->width;
	int height=bmp->height;
	int X,Y;
	unsigned char NM=CLAMPCOLOR(alpha);
	unsigned char *LinePS , *LinePM , *LinePD , *LinePA ;
	#pragma omp parallel for private(LinePS,LinePM,LinePD,LinePA,X,Y)
	for (Y = 0; Y < height; Y ++)
	{
		int offset=stride*Y;
		LinePS = bmppixels +offset;
		LinePM = mskpixels +offset;
		LinePD = dstpixels +offset;
		LinePA = adjpixels +offset;
		for (X = 0; X < width; X ++)
		{
			unsigned char M=*LinePM;
			if(M==0xFF){
				LinePD[0]=LinePS[0];
				LinePD[1]=LinePS[1];
				LinePD[2]=LinePS[2];
			}else if(M==0x00){
				if(NM==0xFF){
					LinePD[0]=LinePS[0];
					LinePD[1]=LinePS[1];
					LinePD[2]=LinePS[2];
				}else {
					if(NM==0x00){
					//none
						LinePD[0]=LinePA[0];
						LinePD[1]=LinePA[1];
						LinePD[2]=LinePA[2];
					}else{
						ColorBlend_Alpha(LinePD,LinePS,LinePA,NM);
					}
				}
			}else{
				//
				if(NM==0xFF){
					LinePD[0]=LinePS[0];
					LinePD[1]=LinePS[1];
					LinePD[2]=LinePS[2];
				}else{
					if(NM==0x00){
						ColorBlend_Alpha(LinePD,LinePS,LinePA,M);
					}else{
						ColorBlend_Alpha(LinePA,LinePS,LinePA,NM);
						ColorBlend_Alpha(LinePD,LinePS,LinePA,M);
					}
				}
			}
			LinePD[3]=LinePS[3];
			LinePS += 4; LinePM += 4; LinePD += 4; LinePA += 4;
		}
	}
}

void ReadAlphaBySrc(CBitmap* src,CBitmap* alpha){
	memcpy(alpha,src,sizeof(CBitmap));
	alpha->stride=src->width;
	alpha->channel=1;
	alpha->pixels=(CPixel*)malloc(alpha->width*alpha->height*sizeof(unsigned char));
	unsigned char* bmppixels=(unsigned char*)src->pixels;
	unsigned char* alapixels=(unsigned char*)alpha->pixels;
	int stride=src->stride;
	int width=src->width;
	int height=src->height;
	int X,Y;
	unsigned char *LinePS ,  *LinePA;
	//#pragma omp parallel for private(LinePS,LinePA)
	for (Y = 0; Y < height; Y ++)
	{
		LinePS = bmppixels +stride*Y;
		LinePA = alapixels +width*Y;
		for (X = 0; X < width; X ++)
		{
			LinePA[0]=LinePS[3];
			LinePS += 4;  LinePA ++;
		}
	}
}


void CheckAlpha(CBitmap* bmp,CBitmap* alpha)
{
	unsigned char* bmppixels=(unsigned char*)bmp->pixels;
	unsigned char* alapixels=(unsigned char*)alpha->pixels;
	int stride=bmp->stride;
	int width=bmp->width;
	int height=bmp->height;
	int X,Y;
	unsigned char *LinePS ,  *LinePA;
	//#pragma omp parallel for private(LinePS,LinePA)
	for (Y = 0; Y < height; Y ++)
	{
		LinePS = bmppixels +stride*Y;
		LinePA = alapixels +width*Y;
		for (X = 0; X < width; X ++)
		{
			//unsigned char M=LinePA[0];
			if(*LinePA==0x00){
				LinePS[0]=0;
				LinePS[1]=0;
				LinePS[2]=0;
				LinePS[3]=0;
			//}else if(M<0xff){
				//if(LinePD[0]>M)LinePD[0]=M;
				//if(LinePD[1]>M)LinePD[1]=M;
				//if(LinePD[2]>M)LinePD[2]=M;
				//LinePD[3]=M;
			}else{
			}
			LinePS += 4;  LinePA++;
		}
	}
}
*/

