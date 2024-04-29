#pragma once

//#define MFCC_OFFSET  6436
#define MFCC_OFFSET  6400
//##define MFCC_OFFSET  0
#define MFCC_DEFRMS  0.1f
#define MFCC_FPS    25
#define MFCC_RATE   16000
//#define MFCC_WAVCHUNK  960000
#define MFCC_WAVCHUNK  560000
//#define MFCC_WAVCHUNK  512

//#define MFCC_MELBASE  6001
#define MFCC_MELBASE  3501
#define MFCC_MELCHUNK  80
//#define MFCC_MELCHUNK  20

//#define MFCC_BNFBASE  1499
#define MFCC_BNFBASE  874
#define MFCC_BNFCHUNK  256
//input==== NodeArg(name='speech', type='tensor(float)', shape=['B', 'T', 80])
//input==== NodeArg(name='speech_lengths', type='tensor(int32)', shape=['B'])
//output==== NodeArg(name='encoder_out', type='tensor(float)', shape=['B', 'T_OUT', 'Addencoder_out_dim_2'])

