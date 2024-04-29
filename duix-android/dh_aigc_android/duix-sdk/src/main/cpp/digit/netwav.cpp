#include "netwav.h"
#include "face_utils.h"
#include "jlog.h"
#include "wavreader.h"


int KWav::initbuf(int pcmsample){
	m_pcmsample = pcmsample;
	m_wavsample = pcmsample + 2*MFCC_OFFSET;

	m_seca = m_wavsample / MFCC_WAVCHUNK;
	m_secb = m_wavsample % MFCC_WAVCHUNK;

	m_mellast = m_secb?(m_secb /160 +1):0;
	m_bnflast = m_secb?((m_mellast*0.25f)-0.75f):0;

	m_wavsize = m_seca*MFCC_WAVCHUNK + m_secb;

	m_melsize = m_seca*MFCC_MELBASE+m_mellast;
	m_bnfsize = m_seca*MFCC_BNFBASE+m_bnflast;

	//m_calcsize = m_seca+m_secb?1:0;
	m_calcsize = m_seca+(m_secb?1:0);

	m_wavmat = new KMat(MFCC_WAVCHUNK,m_calcsize,1);
	m_wavmat->zeros();
	m_melmat = new KMat(MFCC_MELCHUNK*MFCC_MELBASE,m_calcsize,1);
	m_melmat->zeros();
	//m_bnfmat = new KMat(MFCC_BNFCHUNK*MFCC_BNFBASE,m_calcsize,1);
	//m_bnfmat->zeros();
	m_bnfblock = m_duration*MFCC_FPS;
	if(m_bnfblock>(m_bnfsize-10))m_bnfblock = m_bnfsize-10;
    LOGD("==seca %d secb %d\n",m_seca,m_secb);
    LOGD("==melsize %d bnfsize %d\n",m_melsize,m_bnfsize);

	return 0;
}

int KWav::initinx(){
	m_curwav = m_wavmat->fdata() + MFCC_OFFSET;
	m_leftsample = m_pcmsample;
	m_waitsample  = MFCC_OFFSET;

    /*
    int* arr = m_bnfmat->tagarr();
    arr[0] = m_pcmsample*2;
    arr[1] = m_pcmsample;
    arr[2] = m_seca;
    arr[3] = m_secb;
    arr[4] = m_melsize;
    arr[5] = m_bnfsize;
    */
    float secs = m_pcmsample*1.0f/ MFCC_RATE;
    int bnfblock = secs*MFCC_FPS;
    if(bnfblock>(m_bnfsize-10))bnfblock = m_bnfsize-10;
    //arr[6] = bnfblock;//m_bnfblock;
                       //
    //LOGD("my==bnfblock %d arr6 %d",bnfblock,arr[6]);
    //LOGD("myarr %d %d %d %d %d %d %d",arr[0],arr[1],arr[2],arr[3],arr[4],arr[5],arr[6]);
	return 0;
}

int	KWav::incsample(int sample){
	if(sample<1)return 0;
	m_leftsample -= sample;
	m_waitsample += sample;
    //LOGD("===incsample %d left %d wait %d\n",sample,m_leftsample,m_waitsample);
	while(m_waitsample>MFCC_WAVCHUNK){
		m_waitsample -= MFCC_WAVCHUNK;
		m_waitcnt += 1;
        LOGD("===tooken calc %d waitcnt %d calc %d\n",m_calcsize,m_waitcnt,m_calccnt);
        LOGD("===tooken m_ldftsample %d\n",m_leftsample);
	}
	if(m_leftsample<=0){
		m_waitcnt = m_calcsize;
        LOGD("===tooken m_ldftsample %d\n",m_leftsample);
        LOGD("===tooken calc %d waitcnt %d\n",m_calcsize,m_waitcnt);
	}
	return 0;
}

int KWav::pushpcm(uint8_t* pcm,int size){
	uint8_t* pstart = pcm;
	int		psize = size;
	if(m_alonecnt){
		pstart++;
		psize--;
		m_alonearr[1]=*pcm;
		float* ps = (float*)m_alonearr;
		*m_curwav++ = (float)(*ps++/32767.f);
		incsample(1);
		m_alonecnt = 0;
	}
	int sample = psize / 2;
    //LOGD("push pcm %d left_sample %d\n",sample,m_leftsample);
	int left = psize % 2;
	if(sample>m_leftsample){
		sample = m_leftsample;
		left = 0;
	}

	short* ps = (short*)pstart;
    float* pf = m_curwav;
	for(int k=0;k<sample;k++){
		*pf++ = (float)(*ps++/32767.f);
	}
	m_curwav = pf;
    //LOGD("push pcm %d\n",sample);
	incsample(sample);
	if(left){
		uint8_t* pc = (uint8_t*)ps;
		m_alonearr[0] = *pc;
		m_alonecnt = 1;
	}
	return sample;
}
/*
int KWav::loadfn(const char* wavfile){

	int format, channels, sr, bits_per_sample;
	unsigned int data_length;
	void* fhnd = wav_read_open(wavfile);
	if(!fhnd)return -1;
	int res = wav_get_header(fhnd, &format, &channels, &sr, &bits_per_sample, &data_length);
	if(data_length<1) return -2;
	//init
	initbuf(data_length/2);
	//
	wav_read_close(fhnd);
	return 0;
}
*/

KWav::KWav(const char* filename,MBnfCache* bnfcache){
    m_bnfcache = bnfcache;
    std::string wavfile(filename);
    int format, channels, sr, bits_per_sample;
    unsigned int data_length;
    void* fhnd = wav_read_open(wavfile.c_str());
    if(!fhnd){
        m_duration = 0;
        return;
    }
    int res = wav_get_header(fhnd, &format, &channels, &sr, &bits_per_sample, &data_length);
    if(data_length<1) {
        m_duration = 0;
        return;
    }
    int sample = data_length/2;
    m_duration = sample*1.0f/16000.0f;
	initbuf(sample);
    initinx();
    JBuf* pcmbuf = new JBuf(data_length);
    int rst = wav_read_data(fhnd,(unsigned char*)pcmbuf->data(),data_length);
    //
    short* ps = (short*)pcmbuf->data();
    float* pd = (float*)m_wavmat->data();
    float* pf = pd+MFCC_OFFSET;
    for(int k=0;k<sample;k++){
        *pf++ = (float)(*ps++/ 32767.f);
    }
    incsample(sample);
    readyall();
    delete pcmbuf;
}

KWav::KWav(float duration,MBnfCache* bnfcache){
    m_bnfcache = bnfcache;
    m_duration = duration;
	int sample = duration*16000;
	initbuf(sample);
    initinx();
}

KWav::~KWav(){
	if(m_wavmat){
		delete m_wavmat;
		m_wavmat = nullptr;
	}
	if(m_melmat){
		delete m_melmat;
		m_melmat = nullptr;
	}
	//if(m_bnfmat){
		//delete m_bnfmat;
		//m_bnfmat = nullptr;
	//}
}

int KWav::bnfblocks(){
    return m_bnfblock;
}
/*
JMat* KWav::bnfmat(){
    return m_bnfmat;
}
*/

int KWav::finishone(int index){
    m_resultcnt = index;

    //int* arr = m_bnfmat->tagarr();

    float secs = index*MFCC_WAVCHUNK *1.0f/ MFCC_RATE;
    if(m_resultcnt==m_calccnt){
        secs = m_pcmsample*1.0f/ MFCC_RATE;
    }else{
        secs = index*MFCC_WAVCHUNK *1.0f/ MFCC_RATE;
    }
    int bnfblock = secs*MFCC_FPS;
    if(bnfblock>(m_bnfsize-10))bnfblock = m_bnfsize-10;
    //arr[7] = bnfblock;

    return 0;
}

int KWav::isfinish(){
    if((m_waitcnt == m_calcsize)&&(m_calcsize== m_calccnt)){
        return m_resultcnt==m_calccnt;
    }else{
        return 0;
    }
}

int KWav::readyall(){
    m_waitcnt=m_calcsize;
    return 0;
}

int KWav::isready(){
    if(m_waitcnt>m_calccnt){
        return ++m_calccnt;
    }else{
        return 0;
    }
}


int KWav::calcbuf(int calcinx,float** ppwav,float** ppmfcc,float** ppbnf,int* pmel,int* pbnf){
    if(calcinx>m_calcsize)return -1;
    if(calcinx<1)return -2;
    int index = calcinx -1;
    //LOGD("===tooken calcbuf %d\n",index);
    *ppwav = m_wavmat->frow(index);
    *ppmfcc = m_melmat->frow(index);
    //*ppbnf = m_bnfmat->frow(index);
    *ppbnf = m_bnfcache->secBuf(index)->fdata();
    if(calcinx==m_calcsize){
        *pmel = m_mellast;
        *pbnf = m_bnflast;
    }else{
        *pmel = MFCC_MELBASE;
        *pbnf = MFCC_BNFBASE;
    }
    return calcinx;
}

float KWav::duration(){
    return m_duration;
}

int KWav::resultcnt(){
    return m_resultcnt;
}

int  KWav::debug(){
    //dumpfloat(m_bnfmat->fdata(),10);
    return 0;
}


