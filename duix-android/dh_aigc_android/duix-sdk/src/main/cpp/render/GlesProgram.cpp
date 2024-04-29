#include <cstdlib>
#include "GlesProgram.h"
#include <stdio.h>

#define TAG "GlesProgram"

void printGLString(const char *name, GLenum s) {
    const char *v = (const char *) glGetString(s);
    LOGI(TAG, "GL %s = %s\n", name, v);
}

void checkGlError(const char *op) {
    for (GLint error = glGetError(); error; error = glGetError()) {
        LOGE(TAG, "after %s() glError (0x%x)\n", op, error);
    }
}

GTexture::GTexture(int width,int height,int format){
    m_width = width;
    m_height = height;
    m_tid = -1;
    if(format==3){
        m_format = GL_RGB;
    }else if(format==4){
        m_format = GL_RGBA;
    }else{
        m_format = GL_LUMINANCE;
    }
}

GTexture::~GTexture(){
    if(m_tid>=0){
        glDeleteTextures(1, &m_tid);
        checkGlError("glDeleteTextures");
    }
}

int GTexture::load(int width,int height,uint8_t* buf){
    if((width!=m_width)||(height!=m_height))return -1;
    if(!m_loaded){
        glGenTextures(1, &m_tid);
        checkGlError("glGenTextures");
    }else{
    }
    //printf("==m_tid %d\n",m_tid);
    glBindTexture(GL_TEXTURE_2D, m_tid);
    checkGlError("glBindTexture");
    glTexImage2D(GL_TEXTURE_2D, 0, m_format, m_width, m_height, 0, m_format, GL_UNSIGNED_BYTE, buf);
    //aaa
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    checkGlError("glTexImage2D");
    //glBindTexture(GL_TEXTURE_2D, 0);
    m_loaded = 1;
    return 0;
}
GLuint  GTexture::tid(){
    return m_tid;
}

int GTexture::loaded(){
    return m_loaded;
}

static const char* g_vertexstr =
        "attribute vec4 a_position;\n" \
        "attribute vec2 a_texCoord;\n" \
        "varying vec2 v_color;\n" \
        "uniform mat4 u_mvp;\n" \
        "void main() {\n" \
            "gl_Position = u_mvp * a_position;\n" \
            "v_color = a_texCoord;\n" \
        "}\n";

static const char* g_fragmentstr =
        "precision mediump float;\n" \
        "uniform sampler2D tex_0;\n" \
        "varying vec2 v_color;\n" \
        "void main() {\n" \
            "vec4 c = vec4(texture2D(tex_0, v_color).r);\n" \
            "c.g = c.r;\n" \
            "c.b = c.r;\n" \
            "c.a = 1.0;\n" \
            "gl_FragColor = c;\n" \
        "}\n";

GlesProgram::GlesProgram() {
    m_arrTexture[0] =GL_TEXTURE0;
    m_arrTexture[1] =GL_TEXTURE1;
    m_arrTexture[2] =GL_TEXTURE2;
    m_arrTexture[3] =GL_TEXTURE3;
    m_arrTexture[4] =GL_TEXTURE4;
    m_arrTexture[5] =GL_TEXTURE5;
    m_arrTexture[6] =GL_TEXTURE6;
    m_arrTexture[7] =GL_TEXTURE7;
    m_arrTexture[8] =GL_TEXTURE8;
    m_arrTexture[9] =GL_TEXTURE9;
    for(int k=0;k<10;k++){
        m_arrIndex[k]=k;
        m_arrHandle[k]=-1;
    }
    SQUARE_VERTICES = new GLfloat[8]{-1.0f, -1.0f, 1.0f, -1.0f, -1.0f, 1.0f, 1.0f, 1.0f};// fullscreen
    COORD_VERTICES = new GLfloat[8]{0.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f};// whole-texture
    MVP_MATRIX = new GLfloat[16]{1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1};
}

GlesProgram::~GlesProgram() {
    delete[] SQUARE_VERTICES;
    delete[] COORD_VERTICES;
    delete[] MVP_MATRIX;
}

char* GlesProgram::vertexSrc(){
    return (char*)g_vertexstr;
}

char* GlesProgram::fragmentSrc(){
    return (char*)g_fragmentstr;
}

bool GlesProgram::buildProgram(){
    checkGlError("before buildProgram");
    char* vs = vertexSrc();
    char* fs = fragmentSrc();
    _program = createProgram(vs,fs);

    LOGI(TAG, "_program = %d", _program);

    _mvpHandle = glGetUniformLocation(_program, "u_mvp");
    LOGI(TAG, "_mvpHandle = %d", _mvpHandle);
    checkGlError("glGetUniformLocation u_mvp");
    if (_mvpHandle == -1) {
        LOGE(TAG, "Could not get uniform location for u_mvp");
        return false;
    }

    /*
     * get handle for "a_position" and "a_texCoord"
     */
    _positionHandle = glGetAttribLocation(_program, "a_position");
    LOGI(TAG, "_positionHandle = %d", _positionHandle);
    checkGlError("glGetAttribLocation a_position");
    if (_positionHandle == -1) {
        LOGE(TAG, "Could not get attribute location for a_position");
        return false;
    }

    _coordHandle = glGetAttribLocation(_program, "a_texCoord");
    LOGI(TAG, "_coordHandle = %d", _coordHandle);
    checkGlError("glGetAttribLocation a_texCoord");
    if (_coordHandle == -1) {
        LOGE(TAG, "Could not get attribute location for a_texCoord");
        return false;
    }

    char texname[]="tex_0\0";
    char arrdigit[]="0123456789\0";
    int cnt = 0;
    for(int k=0;k<10;k++){
        texname[4] = arrdigit[k];
        GLuint  handle = glGetUniformLocation(_program,texname);
        LOGE(TAG, "handle %s %d = %d", texname,k,handle);
        checkGlError("glGetUniformLocation tex_y\n");
        if (handle == -1) {
            //LOGE(TAG, "Could not get uniform location for tex_y\n");
            break;
        }
        m_arrHandle[k] = handle;
        m_arrcnt ++;
    }
    return true;
}

GLuint GlesProgram::createProgram(char *vertexSource, char *fragmentSource) {
    LOGI(TAG, "createProgram vertexSource =\n%s", vertexSource);
    LOGI(TAG, "createProgram fragmentSource =\n%s", fragmentSource);
    // create shaders
    GLuint vertexShader = loadShader(GL_VERTEX_SHADER, vertexSource);
    GLuint pixelShader = loadShader(GL_FRAGMENT_SHADER, fragmentSource);
    // just check
    LOGI(TAG, "vertexShader = %d", vertexShader);
    LOGI(TAG, "pixelShader = %d", pixelShader);

    GLuint program = glCreateProgram();
    if (program != 0) {
        glAttachShader(program, vertexShader);
        checkGlError("glAttachShader");
        glAttachShader(program, pixelShader);
        checkGlError("glAttachShader");
        glLinkProgram(program);
        checkGlError("glLinkProgram");
        GLint linkStatus = 0;
        glGetProgramiv(program, GL_LINK_STATUS, &linkStatus);
        if (linkStatus != GL_TRUE) {
            GLint bufLength = 0;
            glGetProgramiv(program, GL_INFO_LOG_LENGTH, &bufLength);
            if (bufLength) {
                char *buf = (char *) malloc(static_cast<size_t>(bufLength));
                if (buf) {
                    glGetProgramInfoLog(program, bufLength, nullptr, buf);
                    LOGE("Could not link program:\n%s\n", buf);
                    free(buf);
                }
            }
            glDeleteProgram(program);
            program = 0;
        }
    }
    return program;
}

GLuint GlesProgram::loadShader(GLenum shaderType, char *source) {
    GLuint shader = glCreateShader(shaderType);
    if (shader != 0) {
        glShaderSource(shader, 1, &source, nullptr);
        checkGlError("glShaderSource");
        glCompileShader(shader);
        checkGlError("glCompileShader");
        GLint compiled = 0;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
        if (compiled != GL_TRUE) {
            GLint infoLen = 0;
            glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLen);
            if (infoLen) {
                char *buf = (char *) malloc(static_cast<size_t>(infoLen));
                if (buf) {
                    glGetShaderInfoLog(shader, infoLen, nullptr, buf);
                    LOGE("Could not compile shader :\n%s\n", buf);
                    free(buf);
                }
                glDeleteShader(shader);
                shader = 0;
            }
        }
    } else {
        checkGlError("glCreateShader");
    }
    return shader;
}


void GlesProgram::drawFrame(GTexture** arrtex,int arrcnt){
    if(arrcnt>m_arrcnt){
        LOGE(TAG,"drawframe %d but %d\n",m_arrcnt,arrcnt);
        return ;
    }
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(_program);
    checkGlError("glUseProgram");

    glVertexAttribPointer(_positionHandle, 2, GL_FLOAT, GL_FALSE, 8, SQUARE_VERTICES);
    checkGlError("glVertexAttribPointer mPositionHandle");
    glEnableVertexAttribArray(_positionHandle);

    glVertexAttribPointer(_coordHandle, 2, GL_FLOAT, GL_FALSE, 8, COORD_VERTICES);
    checkGlError("glVertexAttribPointer maTextureHandle");
    glEnableVertexAttribArray(_coordHandle);

    glUniformMatrix4fv(_mvpHandle, 1, GL_FALSE, MVP_MATRIX);

    for(int k=0;k<arrcnt;k++){
        GTexture* ptex = arrtex[k];
        glActiveTexture(m_arrTexture[k]);
        glBindTexture(GL_TEXTURE_2D, ptex->tid());
        glUniform1i(m_arrHandle[k], m_arrIndex[k]);
    }

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glFinish();

    glDisableVertexAttribArray(_positionHandle);
    glDisableVertexAttribArray(_coordHandle);
}

static const char* VERTEX_SHADER =
        "attribute vec4 a_position;\n" \
        "attribute vec2 a_texCoord;\n" \
        "varying vec2 v_color;\n" \
        "uniform mat4 u_mvp;\n" \
        "void main() {\n" \
            "gl_Position = u_mvp * a_position;\n" \
            "v_color = a_texCoord;\n" \
        "}\n";

static const char* FRAGMENT_SHADERRGB =
        "precision mediump float;\n" \
        "uniform sampler2D tex_0;\n" \
        "varying vec2 v_color;\n" \
        "void main() {\n" \
            "vec4 c = texture2D(tex_0, v_color);\n" \
            "c.a = c.r;\n" \
            "c.r = c.b;\n" \
            "c.b = c.a;\n" \
            "c.a = 1.0;\n" \
            "gl_FragColor = c;\n" \
        "}\n";


char* RgbGlesProgram::vertexSrc(){
    return (char*)VERTEX_SHADER;
    //return (char*)g_vertexstr;
}
char* RgbGlesProgram::fragmentSrc(){
    return (char*)FRAGMENT_SHADERRGB;
    //return (char*)g_fragmentstr;
}

char* AlphaGlesProgram::vertexSrc(){
    return (char*)VERTEX_SHADER;
    //return (char*)g_vertexstr;
}

static const char* fsglsl ="uniform sampler2D uTexture0; \n uniform sampler2D uTexture1; \n varying vec2 vUV; \n vec3 alpha(vec3 a, vec3 b,float opacity){ \n return a*opacity+(1.0-opacity)*b; \n } \n void main() { \n vec4 color0 = texture2D(uTexture0, vUV); \n vec4 color1 = texture2D(uTexture1, vUV); \n gl_FragColor = vec4(alpha(color0.rgb, color1.rgb, color0.a), 1.0); \n } ";
static const char* FRAGMENT_SHADERALPHA =
        "precision mediump float;\n" \
        "uniform sampler2D tex_0;\n" \
        "uniform sampler2D tex_1;\n" \
        "uniform sampler2D tex_2;\n" \
        "varying vec2 v_color;\n" \
        "void main() {\n" \
            "vec4 fg = texture2D(tex_0, v_color);\n" \
            "vec4 mg = texture2D(tex_1, v_color);\n" \
            "vec4 bg = texture2D(tex_2, v_color);\n" \
            "float alpha = mg.r;\n" \
            "float beta = 1.0 - alpha;\n" \
            "mg.b = fg.r*alpha + bg.r*beta;\n" \
            "mg.g = fg.g*alpha + bg.g*beta;\n" \
            "mg.r = fg.b*alpha + bg.b*beta;\n" \
            "gl_FragColor = mg;\n" \
        "}\n";


char* AlphaGlesProgram::fragmentSrc(){
    return (char*)FRAGMENT_SHADERALPHA;
    //return (char*)g_fragmentstr;
}
