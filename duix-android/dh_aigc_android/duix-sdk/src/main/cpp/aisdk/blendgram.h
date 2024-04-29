#ifndef __BLENDGRAM_H__
#define __BLENDGRAM_H__

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

typedef unsigned char uchar;
#define CLAMPCOLOR(x) (uchar)((x)<(0)?(0):((x)>(255)?(255):(x)))

#define MMAX(A,B)     ((A)>(B)?(A):(B))
#define MMIN(A,B)     ((A)<(B)?(A):(B))

static int ConstBlend_Buffer = 0;
static int ConstBlend_Normal=ConstBlend_Buffer+1;
static int ConstBlend_Lighten=ConstBlend_Buffer+2;
static int  ConstBlend_Darken=ConstBlend_Buffer+3;
static int  ConstBlend_Multiply=ConstBlend_Buffer+4;
static int  ConstBlend_Average=ConstBlend_Buffer+5;

static int  ConstBlend_Add=ConstBlend_Buffer+6;
static int  ConstBlend_Subtract=ConstBlend_Buffer+7;
static int  ConstBlend_Difference=ConstBlend_Buffer+8;
static int  ConstBlend_Negation=ConstBlend_Buffer+9;
static int  ConstBlend_Screen=ConstBlend_Buffer+10;
static int  ConstBlend_Exclusion=ConstBlend_Buffer+11;
static int  ConstBlend_Overlay=ConstBlend_Buffer+12;
static int  ConstBlend_SoftLight=ConstBlend_Buffer+13;
static int  ConstBlend_HardLight=ConstBlend_Buffer+14;
static int  ConstBlend_ColorDodge=ConstBlend_Buffer+15;
static int  ConstBlend_ColorBurn=ConstBlend_Buffer+16;
static int  ConstBlend_LinearDodge=ConstBlend_Buffer+17;
static int  ConstBlend_LinearBurn=ConstBlend_Buffer+18;
static int  ConstBlend_LinearLight=ConstBlend_Buffer+19;
static int  ConstBlend_VividLight=ConstBlend_Buffer+20;
static int  ConstBlend_PinLight=ConstBlend_Buffer+21;
static int  ConstBlend_HardMix=ConstBlend_Buffer+22;
static int  ConstBlend_Reflect=ConstBlend_Buffer+23;
static int  ConstBlend_Glow=ConstBlend_Buffer+24;
static int  ConstBlend_Phoenix=ConstBlend_Buffer+25;

//void BlendGram(CBitmap* immage,CBitmap* mask,int mode);

//#typedef unsigned char uint8
#define uint8 unsigned char
#define float64 double
#define TRUE 1
#define FALSE 0

inline uint8 mmin(uint8 A,uint8 B){
	return A<B?A:B;
}
inline uint8 mmax(uint8 A,uint8 B){
	return A>B?A:B;
}

#define ChannelBlend_Normal(A,B)     ((uint8)(A))
#define ChannelBlend_Lighten(A,B)    ((uint8)((B > A) ? B:A))
#define ChannelBlend_Darken(A,B)     ((uint8)((B > A) ? A:B))
#define ChannelBlend_Multiply(A,B)   ((uint8)((A * B) / 255))
#define ChannelBlend_Average(A,B)    ((uint8)((A + B) / 2))
#define ChannelBlend_Add(A,B)        ((uint8)(mmin(255, (A + B))))
#define ChannelBlend_Subtract(A,B)   ((uint8)((A + B < 255) ? 0:(A + B - 255)))
#define ChannelBlend_Difference(A,B) ((uint8)(abs(A - B)))
#define ChannelBlend_Negation(A,B)   ((uint8)(255 - abs(255 - A - B)))
#define ChannelBlend_Screen(A,B)     ((uint8)(255 - (((255 - A) * (255 - B)) >> 8)))
#define ChannelBlend_Exclusion(A,B)  ((uint8)(A + B - 2 * A * B / 255))
#define ChannelBlend_Overlay(A,B)    ((uint8)((B < 128) ? (2 * A * B / 255):(255 - 2 * (255 - A) * (255 - B) / 255)))
#define ChannelBlend_SoftLight(A,B)  ((uint8)((B < 128)?(2*((A>>1)+64))*((float)B/255):(255-(2*(255-((A>>1)+64))*(float)(255-B)/255))))
#define ChannelBlend_HardLight(A,B)  (ChannelBlend_Overlay(B,A))
#define ChannelBlend_ColorDodge(A,B) ((uint8)((B == 255) ? B:mmin(255, ((A << 8 ) / (255 - B)))))
#define ChannelBlend_ColorBurn(A,B)  ((uint8)((B == 0) ? B:mmax(0, (255 - ((255 - A) << 8 ) / B))))
#define ChannelBlend_LinearDodge(A,B)(ChannelBlend_Add(A,B))
#define ChannelBlend_LinearBurn(A,B) (ChannelBlend_Subtract(A,B))
#define ChannelBlend_LinearLight(A,B)((uint8)(B < 128)?ChannelBlend_LinearBurn(A,(2 * B)):ChannelBlend_LinearDodge(A,(2 * (B - 128))))
#define ChannelBlend_VividLight(A,B) ((uint8)(B < 128)?ChannelBlend_ColorBurn(A,(2 * B)):ChannelBlend_ColorDodge(A,(2 * (B - 128))))
#define ChannelBlend_PinLight(A,B)   ((uint8)(B < 128)?ChannelBlend_Darken(A,(2 * B)):ChannelBlend_Lighten(A,(2 * (B - 128))))
#define ChannelBlend_HardMix(A,B)    ((uint8)((ChannelBlend_VividLight(A,B) < 128) ? 0:255))
#define ChannelBlend_Reflect(A,B)    ((uint8)((B == 255) ? B:mmin(255, (A * A / (255 - B)))))
#define ChannelBlend_Glow(A,B)       (ChannelBlend_Reflect(B,A))
#define ChannelBlend_Phoenix(A,B)    ((uint8)(mmin(A,B) - mmax(A,B) + 255))
#define ChannelBlend_SoftEx(A,B)    (A*B/255+A*(255-((255-A)*(255-B)/255)-A*B/255)/255)

#define ChannelBlend_Alpha(A,B,O)    ((uint8)(O * A + (1 - O) * B))
#define ChannelBlend_AlphaEx(A,B,O)    ((uint8)((O * A + (255 - O) * B)/255))
#define ChannelBlend_AlphaF(A,B,F,O) (ChannelBlend_AlphaEx(F(A,B),A,O))

#define ColorBlend_Alpha(T,A,B,O)      (T)[0] = ChannelBlend_AlphaEx((A)[0], (B)[0],O), (T)[1] = ChannelBlend_AlphaEx((A)[1], (B)[1],O), (T)[2] = ChannelBlend_AlphaEx((A)[2], (B)[2],O)
//, (T)[3] = ChannelBlend_AlphaEx((A)[3], (B)[3],O)
#define ColorBlend_AlphaF(T,A,B,F,O)      (T)[0] = ChannelBlend_AlphaF((A)[0], (B)[0],F,O), (T)[1] = ChannelBlend_AlphaF((A)[1], (B)[1],F,O), (T)[2] = ChannelBlend_AlphaF((A)[2], (B    )[2],F,O) , (T)[3] = ChannelBlend_AlphaEx((A)[3], (B)[3],O)


#define ColorBlend_Buffer(T,A,B,M)      (T)[0] = ChannelBlend_##M((A)[0], (B)[0]), (T)[1] = ChannelBlend_##M((A)[1], (B)[1]), (T)[2] = ChannelBlend_##M((A)[2], (B)[2])

#define ColorBlend_Normal(T,A,B)        (ColorBlend_Buffer(T,A,B,Normal))
#define ColorBlend_Lighten(T,A,B)       (ColorBlend_Buffer(T,A,B,Lighten))
#define ColorBlend_Darken(T,A,B)        (ColorBlend_Buffer(T,A,B,Darken))
#define ColorBlend_Multiply(T,A,B)      (ColorBlend_Buffer(T,A,B,Multiply))
#define ColorBlend_Average(T,A,B)       (ColorBlend_Buffer(T,A,B,Average))
#define ColorBlend_Add(T,A,B)           (ColorBlend_Buffer(T,A,B,Add))
#define ColorBlend_Subtract(T,A,B)      (ColorBlend_Buffer(T,A,B,Subtract))
#define ColorBlend_Difference(T,A,B)    (ColorBlend_Buffer(T,A,B,Difference))
#define ColorBlend_Negation(T,A,B)      (ColorBlend_Buffer(T,A,B,Negation))
#define ColorBlend_Screen(T,A,B)        (ColorBlend_Buffer(T,A,B,Screen))
#define ColorBlend_Exclusion(T,A,B)     (ColorBlend_Buffer(T,A,B,Exclusion))
#define ColorBlend_Overlay(T,A,B)       (ColorBlend_Buffer(T,A,B,Overlay))
#define ColorBlend_SoftLight(T,A,B)     (ColorBlend_Buffer(T,A,B,SoftLight))
#define ColorBlend_HardLight(T,A,B)     (ColorBlend_Buffer(T,A,B,HardLight))
#define ColorBlend_ColorDodge(T,A,B)    (ColorBlend_Buffer(T,A,B,ColorDodge))
#define ColorBlend_ColorBurn(T,A,B)     (ColorBlend_Buffer(T,A,B,ColorBurn))
#define ColorBlend_LinearDodge(T,A,B)   (ColorBlend_Buffer(T,A,B,LinearDodge))
#define ColorBlend_LinearBurn(T,A,B)    (ColorBlend_Buffer(T,A,B,LinearBurn))
#define ColorBlend_LinearLight(T,A,B)   (ColorBlend_Buffer(T,A,B,LinearLight))
#define ColorBlend_VividLight(T,A,B)    (ColorBlend_Buffer(T,A,B,VividLight))
#define ColorBlend_PinLight(T,A,B)      (ColorBlend_Buffer(T,A,B,PinLight))
#define ColorBlend_HardMix(T,A,B)       (ColorBlend_Buffer(T,A,B,HardMix))
#define ColorBlend_Reflect(T,A,B)       (ColorBlend_Buffer(T,A,B,Reflect))
#define ColorBlend_Glow(T,A,B)          (ColorBlend_Buffer(T,A,B,Glow))
#define ColorBlend_Phoenix(T,A,B)       (ColorBlend_Buffer(T,A,B,Phoenix))


#define ColorBlend_Hue(T,B,L)            ColorBlend_Hls(T,B,L,HueL,LuminationB,SaturationB)
#define ColorBlend_Saturation(T,B,L)     ColorBlend_Hls(T,B,L,HueB,LuminationB,SaturationL)
#define ColorBlend_Color(T,B,L)          ColorBlend_Hls(T,B,L,HueL,LuminationB,SaturationL)
#define ColorBlend_Luminosity(T,B,L)     ColorBlend_Hls(T,B,L,HueB,LuminationL,SaturationB)



#define ColorBlend_Hls(T,B,L,O1,O2,O3) {                                        \
    float64 HueB, LuminationB, SaturationB;                                     \
    float64 HueL, LuminationL, SaturationL;                                     \
    Color_RgbToHls((B)[2],(B)[1],(B)[0], &HueB, &LuminationB, &SaturationB);    \
    Color_RgbToHls((L)[2],(L)[1],(L)[0], &HueL, &LuminationL, &SaturationL);    \
    Color_HlsToRgb(O1,O2,O3,&(T)[2],&(T)[1],&(T)[0]);                           \
    }


/*********************************************************************/

#define COLOR_OPAQUE                (0)
#define COLOR_TRANSPARENT           (127)

#define RGB_SIZE                    (3)
#define RGB_BPP                     (24)
#define RGB_MAXRED                  (255)
#define RGB_MAXGREEN                (255)
#define RGB_MAXBLUE                 (255)

#define ARGB_SIZE                   (4)
#define ARGB_BPP                    (32)
#define ARGB_MAXALPHA               (127)
#define ARGB_MAXRED                 (RGB_MAXRED)
#define ARGB_MAXGREEN               (RGB_MAXGREEN)
#define ARGB_MAXBLUE                (RGB_MAXBLUE)

/*********************************************************************/

#define Color_GetChannel(c,shift)   ((uint8)((c) >> (shift)))
#define Color_Reverse(c,bpp)        ((((uint8)(c) << 24) | ((uint8)((c) >> 8 ) << 16) | ((uint8)((c) >> 16) << 8 ) | \ ((uint8)((c) >> 24))) >> (32 - (bpp)))

#define Rgb_ByteWidth(width)        ((width) * RGB_SIZE)
#define Rgb_PixelWidth(width)       ((width) / RGB_SIZE)

#define Rgb_GetRed(rgb)             (Color_GetChannel(rgb, 0))
#define Rgb_GetGreen(rgb)           (Color_GetChannel(rgb, 8))
#define Rgb_GetBlue(rgb)            (Color_GetChannel(rgb, 16))

#define Rgba_GetRed(rgba)           (Color_GetChannel(rgba, 24))
#define Rgba_GetGreen(rgba)         (Color_GetChannel(rgba, 16))
#define Rgba_GetBlue(rgba)          (Color_GetChannel(rgba, 8))
#define Rgba_GetAlpha(rgba)         (Color_GetChannel(rgba, 0))

#define Argb_GetAlpha(argb)         (Color_GetChannel(argb, 24))
#define Argb_GetRed(argb)           (Color_GetChannel(argb, 16))
#define Argb_GetGreen(argb)         (Color_GetChannel(argb, 8))
#define Argb_GetBlue(argb)          (Color_GetChannel(argb, 0))

#define MakeRgb(r,g,b)              (((uint32)(uint8)(b) << 16) | ((uint16)(uint8)(g) << 8 ) | (uint8)(r))
#define MakeRgba(r,g,b,a)           (((uint32)(uint8)(r) << 24) | ((uint16)(uint8)(g) << 16) | ((uint16)(uint8)(b) << 8 ) | (uint8)(a))
#define MakeArgb(a,r,g,b)           (((uint32)(uint8)(a) << 24) | ((uint32)(uint8)(r) << 16) | ((uint16)(uint8)(g) << 8 ) | (uint8)(b))
#define HexToRgb(hex)               (MakeRgb(((hex & 0xFF0000) >> 16), ((hex & 0x00FF00) >> 8 ), (hex & 0xFF)))

inline int Color_HueToRgb(float64 M1, float64 M2, float64 Hue, float64 *Channel)
{
    if (Hue < 0.0)
        Hue += 1.0;
    else if (Hue > 1.0)
        Hue -= 1.0;

    if ((6.0 * Hue) < 1.0)
        *Channel = (M1 + (M2 - M1) * Hue * 6.0);
    else if ((2.0 * Hue) < 1.0)
        *Channel = (M2);
    else if ((3.0 * Hue) < 2.0)
        *Channel = (M1 + (M2 - M1) * ((2.0F / 3.0F) - Hue) * 6.0);
    else
        *Channel = (M1);

    return TRUE;
}

inline void Color_RgbToHls(uint8 Red, uint8 Green, uint8 Blue, float64 *Hue, float64 *Lumination, float64 *Saturation)
{
    float64 Delta;
    float64 Max, Min;
    float64 Redf, Greenf, Bluef;

    Redf    = (float64)Red   / 255.0;
    Greenf  = (float64)Green / 255.0;
    Bluef   = (float64)Blue  / 255.0;

    //Max     = fmax(fmax(Redf, Greenf), Bluef);
    //Min     = fmin(fmin(Redf, Greenf), Bluef);
    Max     = MMAX(MMAX(Red, Green), Blue)/255.0;
    Min     = MMIN(MMIN(Red, Green), Blue)/255.0;

    *Hue        = 0;
    *Lumination = (Max + Min) / 2.0F;
    *Saturation = 0;

    if (Max == Min)
        return ;

    Delta = (Max - Min);

    if (*Lumination < 0.5)
        *Saturation = Delta / (Max + Min);
    else
        *Saturation = Delta / (2.0 - Max - Min);

    if (Redf == Max)
        *Hue = (Greenf - Bluef) / Delta;
    else if (Greenf == Max)
        *Hue = 2.0 + (Bluef - Redf) / Delta;
    else
        *Hue = 4.0 + (Redf - Greenf) / Delta;

    *Hue /= 6.0;

    if (*Hue < 0.0)
        *Hue += 1.0;

}

inline void Color_HlsToRgb(float64 Hue, float64 Lumination, float64 Saturation, uint8 *Red, uint8 *Green, uint8 *Blue)
{
    float64 M1, M2;
    float64 Redf, Greenf, Bluef;

    if (Saturation == 0) {
        Redf    = Lumination;
        Greenf  = Lumination;
        Bluef   = Lumination;
    } else {
        if (Lumination <= 0.5)
            M2 = Lumination * (1.0 + Saturation);
        else
            M2 = Lumination + Saturation - Lumination * Saturation;

        M1 = (2.0 * Lumination - M2);

        Color_HueToRgb(M1, M2, Hue + (1.0F / 3.0F), &Redf);
        Color_HueToRgb(M1, M2, Hue, &Greenf);
        Color_HueToRgb(M1, M2, Hue - (1.0F / 3.0F), &Bluef);
    }

    *Red    = (uint8)(Redf * 255);
    *Blue   = (uint8)(Bluef * 255);
    *Green  = (uint8)(Greenf * 255);

}

void BlendGramSimp(unsigned char *Src,unsigned char* Mask, unsigned char *Dest, int Width, int Height, int Mode);
void BlendGramAlpha(unsigned char *Src,unsigned char* Mask, unsigned char *Dest, int Width, int Height);
void BlendGramAlpha3(unsigned char *Src,unsigned char* Mask, unsigned char *Dest, int Width, int Height);
void BlendGramAlphaRev(unsigned char *Src,unsigned char* Mask, unsigned char *Dest, int Width, int Height);
/*
void BlendImageAdjustWithMask(CBitmap* bmp,CBitmap* adj,CBitmap* dst ,CBitmap* msk,int mode);
void BlendImageAdjustWithMaskEx(CBitmap* bmp,CBitmap* adj,CBitmap* dst ,CBitmap* msk,int mode);
void BlendImageAdjustWithAlpha(CBitmap* bmp,CBitmap* adj,CBitmap* dst ,int alpha,int mode);
void BlendImageAdjustWithAlphaMask(CBitmap* bmp,CBitmap* adj,CBitmap* dst ,CBitmap* msk,int alpha,int mode);

void CheckAlpha(CBitmap* bmp,CBitmap* alpha);
void ReadAlphaBySrc(CBitmap* src,CBitmap* alpha);
*/

#endif
