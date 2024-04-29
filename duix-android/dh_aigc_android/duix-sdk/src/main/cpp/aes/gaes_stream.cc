
#include "gaes_stream.h"

#include <cstring>
#include <iostream>
#include <fstream>
#include <vector>
#include <cstdio>
#include <cstdlib>
#include "gj_aes.h"


class GaesIStreamBuf final: public std::streambuf
{
private:
	char *m_inbuf;
	size_t m_inbufsize;
	bool m_owns_inbuf;
	char *m_leftbuf;

    FILE *file;
    uint64_t cur_size;
    uint64_t file_size;
    gj_aesc_t* aesc ;
protected:
	virtual std::streambuf* setbuf(char *s, std::streamsize n){
	    setg(0, 0, 0);
	    if (m_owns_inbuf) {
	        delete [] m_inbuf;
	    }
	    m_inbufsize = n;
	    if (s) {
	        m_inbuf = s;
	        m_owns_inbuf = false;
	    } else {
	        m_inbuf = new char[m_inbufsize];
	        m_leftbuf = new char[m_inbufsize];
	        m_owns_inbuf = true;
        }
	    return this;
    }

	virtual int sync(){
	    int result = 0;
	    return result;
    }

    virtual int underflow() override{
	    int __c = traits_type::eof();
        if (!file) return __c;
        if(cur_size>=file_size){
            printf("===eof %ld ===%ld\n",cur_size,file_size);
            return __c;
        }
	    bool initial = false;
	    if (eback() == 0) {
	        setg(m_inbuf, m_inbuf + m_inbufsize, m_inbuf + m_inbufsize);
	        initial = true;
        }
	    const size_t unget_sz = initial ? 0 : std::min<size_t>((egptr() - eback()) / 2, 4);
	    if (gptr() == egptr()) {
	        memmove(eback(), egptr() - unget_sz, unget_sz);
	        size_t nmemb = static_cast<size_t>(egptr() - eback() - unget_sz);
            char* pdst = eback() + unget_sz;
            int modb = nmemb % 16;
            size_t leftb = nmemb - modb;
            char* pbuf = m_leftbuf;
            size_t leftf = file_size - cur_size;
            if(leftb>leftf)leftb=leftf;
            memset(pbuf,0,m_inbufsize);
            size_t rd = fread(pbuf, 1, leftb, file);
            //printf("%d-%ld-%ld----------------%ld--%ld#\n",cur_size,file_size,modb,nmemb,rd);
	        //ssize_t readed = read(m_fd, eback() + unget_sz, nmemb);
            if(rd>0){
                cur_size += rd;
                int cnt = leftb /16;
                int k;
                for(k=0;k<cnt;k++){
                    int outlen = 0;
                    do_aesc(aesc,pbuf,16,pdst,&outlen);
                    pbuf += 16;
                    pdst += 16;
                }
		        setg(eback(), eback() + unget_sz, eback() + unget_sz + rd);
		        __c = traits_type::to_int_type(*gptr());
            }
	    } else {
	        __c = traits_type::to_int_type(*gptr());
        }
	    return __c;
    }
public:
    GaesIStreamBuf(std::string& filename) :m_inbuf(0), m_inbufsize(0), m_owns_inbuf(false){
	    setbuf(0, 1024);
        cur_size = 0;
        file = fopen(filename.c_str(), "rb");
        fseek(file, 0, SEEK_END);
        file_size = ftell(file); //获取音频文件大小
        fseek(file, 0, SEEK_SET);
        char* key = "yymrjzbwyrbjszrk";
        char* aiv = "yymrjzbwyrbjszrk";
        init_aesc(key,aiv,0,&this->aesc);
        char head[50];
        memset(head,0,50);
        uint64_t rst = fread(head,1,8,file);
        rst = fread(&cur_size,1,8,file);
        printf("===head %s size %ld\n",head,cur_size);
        rst = fread(head,1,16,file);
        cur_size = 32;
    }

    ~GaesIStreamBuf(){
        close();
	    if (m_owns_inbuf) {
	        delete[] m_inbuf;
	    }
    }

    void close(){
        if(aesc){
            free_aesc(&this->aesc);
        }
        if (file){
            fclose(file);
            file = NULL;
        }
    }
};



GaesIStream::GaesIStream(std::string filename):
    std::istream(new GaesIStreamBuf(filename)){
}

GaesIStream::~GaesIStream()
{
    delete rdbuf();
}

#ifdef TEST
int maindec(int argc,char** argv){
    std::string filename(argv[1]);// = "test.enc";
    //std::string filename = "final.mdlenc";
    GaesIStream fin(filename);
    //std::string fn2 = "final.mdldec";
    std::string fn2(argv[2]);// = "test.dec";
    std::ofstream fout(fn2,std::ios::binary);

    char buf[1024];
    int rd = 0;
    while(!fin.eof()){
    //while((rd = fin.read(buf,16))>0){
        //printf("===rd %ld\n",rd);
        fin.read(buf,16);
        fout.write(buf,16);

    }
    //char ch;
    //while (fin.get(ch)) {
        //printf("+");
        //fout << ch;
    //}
    return 0;
}



int mainenc(int argc,char** argv){
    char result[255] ;
    memset(result,0,255);
    char* key = "yymrjzbwyrbjszrk";
    char* aiv = "yymrjzbwyrbjszrk";
    int base64 = 1;
    int outlen = 0;
    gj_aesc_t* aesc = NULL;
    init_aesc(key,aiv,1,&aesc);
    char* fn1 = argv[1];
    char* fn2 = argv[2];
    FILE* fr = fopen(fn1,"rb");
    FILE* fw = fopen(fn2,"wb");
    fwrite("abcdefgh",1,8,fw);
    uint64_t size = 0;
    fwrite(&size,1,8,fw);
    fwrite(&size,1,8,fw);
    fwrite(&size,1,8,fw);
    while(!feof(fr)){
        char data[16];
        memset(data,0,16);
        uint64_t rst = fread(data,1,16,fr);
        printf("===rst %d\n",rst);
        if(rst){
            size +=rst;
            do_aesc(aesc,data,16,result,&outlen);
            printf("===out %d\n",outlen);
            fwrite(result,1,16,fw);
        }
    }
    fseek(fw,8,0);
    fwrite(&size,1,8,fw);
    fclose(fr);
    fclose(fw);
    return 0;
}

int main(int argc,char** argv){
    if(argc<4){
        return mainenc(argc,argv);
    }else{
        return maindec(argc,argv);
    }
}
#endif
