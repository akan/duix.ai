#pragma once
#include "jmat.h"
#include "aicommon.h"
#include "wavcache.h"


class KWav{
    private:
        float m_duration = 0;
        int m_pcmsample = 0;;
        int m_wavsample = 0;;
        int m_seca = 0;
        int m_secb = 0;
        int m_mellast = 0;
        int m_bnflast = 0;
        int m_wavsize = 0;
        int m_melsize = 0;
        int m_bnfsize = 0;
        int m_calcsize = 0;
        int m_bnfblock = 0;
		uint8_t	m_alonearr[2] ;
		int		m_alonecnt = 0;
		int		m_leftsample = 0;
        float 	*m_curwav = nullptr;
		int		m_waitsample = 0;

		int		m_waitcnt = 0;
		int		m_calccnt = 0;
		int		m_resultcnt = 0;

		int		incsample(int sample);

        JMat    *m_wavmat = nullptr;
        JMat    *m_melmat = nullptr;
        //JMat    *m_bnfmat = nullptr;
        MBnfCache   *m_bnfcache = nullptr;
        int     initbuf(int pcmsample);
		int		initinx();
    public:
        KWav(float duration,MBnfCache* bnfcache);
        KWav(const char* filename,MBnfCache* bnfcache);
        //KWav(const char* wavfn);
        ~KWav();
		int pushpcm(uint8_t* pcm,int size);
        int isready();
        int readyall();
        int isfinish();
        int finishone(int index);
        int bnfblocks();
        float duration();
        int resultcnt();
        //JMat* bnfmat();
        int calcbuf(int calcinx,float** ppwav,float** ppmfcc,float** ppbnf,int* pmel,int* pbnf);
        int  debug();
};

