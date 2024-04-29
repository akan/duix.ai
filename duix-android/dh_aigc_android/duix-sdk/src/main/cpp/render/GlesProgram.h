#ifndef GPLAYER_GLESPROGRAM_H
#define GPLAYER_GLESPROGRAM_H

#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include <Log.h>

class GTexture{
    private:
        GLuint  m_tid = -1;
        GLuint  m_format = 0;
        int     m_width = 0;
        int     m_height = 0;
        int     m_loaded = 0;
    public:
        GTexture(int width,int height,int format);
        int load(int width,int height,uint8_t* buf);
        ~GTexture();
        int loaded();
        GLuint  tid();
};

void printGLString(const char *name, GLenum s) ;
void checkGlError(const char *op) ;


class GlesProgram {
public:
    GlesProgram();
    virtual ~GlesProgram();
    virtual bool buildProgram();
    virtual void drawFrame(GTexture** arrtex,int arrcnt);
    virtual char* vertexSrc();
    virtual char* fragmentSrc();
protected:
    GLuint createProgram(char *vertexSource, char *fragmentSource);
    GLuint loadShader(GLenum shaderType, char *source);
protected:
    GLuint  m_arrTexture[10];
    GLuint  m_arrIndex[10];
    GLuint  m_arrHandle[10];
    int     m_arrcnt = 0;
    GLfloat *SQUARE_VERTICES;
    GLfloat *COORD_VERTICES;
    GLfloat *MVP_MATRIX;

    // program id
    GLuint _program;
    //GLuint _vertex;
    //GLuint _fragment;

    GLuint _mvpHandle;
    GLuint _positionHandle, _coordHandle;
};


class RgbGlesProgram:public GlesProgram {
    public:
        RgbGlesProgram():GlesProgram(){};
        virtual ~RgbGlesProgram(){};
        virtual char* vertexSrc();
        virtual char* fragmentSrc();
};

class AlphaGlesProgram:public GlesProgram {
    public:
        AlphaGlesProgram():GlesProgram(){};
        virtual ~AlphaGlesProgram(){};
        virtual char* vertexSrc();
        virtual char* fragmentSrc();
};

#endif //GPLAYER_YUVGLESPROGRAM_H
