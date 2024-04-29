#pragma once

#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <algorithm>
#include "jmat.h"

struct AiCfg{
    std::vector<int64_t>    kind_inputs;
    std::vector<int64_t>    size_inputs;
    std::vector<std::string>    names;
    std::vector<const char*>    name_inputs;
    //int                 m_sizeinput;
    std::vector<std::vector<int64_t>> shape_inputs;

    std::vector<int64_t>    kind_outputs;
    std::vector<int64_t>    size_outputs;
    std::vector<const char*>    name_outputs;
    //int                 m_sizeoutput;
    std::vector<std::vector<int64_t>> shape_outputs;

    const char** names_in;
    const char** names_out;
    void dump();
    int inShape(int inx,int dim,int val);
    int outShape(int inx,int dim,int val);
    AiCfg clone();
};

class AiModel{
    protected:
        int                 m_inited;
        std::string         m_modelPath;
        std::string         m_modelbin;
        std::string         m_modelparam;

        AiCfg               *m_cfg;

        virtual int doInitModel();
        virtual int doRunModel(void** arrin,void** arrout,void* stream,AiCfg* pcfg=nullptr);
    public:
        void dump();
        AiCfg config();
        int pushName(const char* name,int input);
        int initModel(std::string& modelpath);
        int initModel(std::string& binfn,std::string& paramfn);
        int runModel(void** arrin,void** arrout,void* stream,AiCfg* pcfg = nullptr);
        AiModel();
        virtual ~AiModel();
};

#define _ONNX_
#ifdef _ONNX_
#include "onnx/onnxruntime_cxx_api.h"
class OnnxModel:public AiModel{
    protected:
        int m_batch = 0;
        int m_width = 640;
        int m_height = 960;
        Ort::Env env{nullptr};
        Ort::SessionOptions sessionOptions{nullptr};
        Ort::Session session{nullptr};
        int doInitModel()override;
        int doRunModel(void** arrin,void** arrout,void* stream,AiCfg* pcfg=nullptr)override;
    public:
        OnnxModel(int b,int w,int h);
        OnnxModel();
        virtual ~OnnxModel();
};
#endif

#define _NCNN_
#ifdef _NCNN_
#include "net.h"
class NcnnModel:public AiModel{
    protected:
        int m_width = 160;
        int m_height = 160;
        ncnn::Net net;
        int doInitModel()override;
        int doRunModel(void** arrin,void** arrout,void* stream,AiCfg* pcfg=nullptr)override;
    public:
        NcnnModel(int w,int h);
        NcnnModel();
        virtual ~NcnnModel();
};

#endif

#ifdef _TENSORRT_
#include "NvInfer.h"
#include "NvInferLegacyDims.h"
#include "NvInferRuntime.h"
class Logger:public nvinfer1::ILogger {
    public:
        void log(nvinfer1::ILogger::Severity severity, const char *msg) noexcept override {
            // suppress info-level messages
            if (severity == Severity::kINFO)
                return;
            switch (severity) {
                case Severity::kINTERNAL_ERROR:
                    std::cerr << "INTERNAL_ERROR: ";
                    break;
                case Severity::kERROR:
                    std::cerr << "ERROR: ";
                    break;
                case Severity::kWARNING:
                    std::cerr << "WARNING: ";
                    break;
                case Severity::kINFO:
                    std::cerr << "INFO: ";
                    break;
                default:
                    std::cerr << "UNKNOWN: ";
                    break;
            }
            std::cerr << msg << std::endl;
        }
};

class TrtModel:public AiModel{
    protected:
        Logger m_logger;
        nvinfer1::IExecutionContext *m_context;
        nvinfer1::ICudaEngine *m_engine;

        int doInitModel()override;
        int doRunModel(void** arrin,void** arrout,void* stream,AiCfg* pcfg=nullptr)override;
    public:
        TrtModel(int b,int w,int h);
        TrtModel();
        virtual ~TrtModel();
};
#endif
