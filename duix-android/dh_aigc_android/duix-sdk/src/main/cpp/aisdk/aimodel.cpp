#include "aimodel.h"
#include <stdlib.h>
#include <string.h>

void AiCfg::dump(){
    int incnt = size_inputs.size();
    int outcnt = size_outputs.size();
    std::cout<<"======in onnx:"<<incnt<<std::endl;
    //std::cout<<"======in onnx:"<<name_inputs.size()<<std::endl;
    for(int k=0;k<incnt;k++){
        //std::string sname(name_inputs[k]);
        //std::cout<<"in name:"<<sname<<std::endl;
        std::cout<<"size input:"<<size_inputs[k]<<std::endl;
        for(int m=0;m<shape_inputs[k].size();m++)
            std::cout<<"shape :"<<shape_inputs[k][m]<<std::endl;
        std::cout<<"kind input:"<<kind_inputs[k]<<std::endl;
    }
    std::cout<<"=========out onnx:"<<outcnt<<std::endl;
    //std::cout<<"======in onnx:"<<name_outputs.size()<<std::endl;
    for(int k=0;k<outcnt;k++){
        //std::string sname(name_outputs[k]);
        //std::cout<<"out name:"<<sname<<std::endl;
        std::cout<<"size output:"<<size_outputs[k]<<std::endl;
        for(int m=0;m<shape_outputs[k].size();m++)
            std::cout<<"shape :"<<shape_outputs[k][m]<<std::endl;
        std::cout<<"kind outintpu:"<<kind_outputs[k]<<std::endl;
    }
}

int AiCfg::inShape(int inx,int dim,int val){
    if(inx>=shape_inputs.size())return -1;
    if(dim>=shape_inputs[inx].size())return -2;
    shape_inputs[inx][dim] = val;
    int size = 1;
    for(int k=0;k<shape_inputs[inx].size();k++){
        size *= shape_inputs[inx][dim];
    }
    if(size>0){
        size_inputs[inx] = size;
    }
    return 0;
}

int AiCfg::outShape(int inx,int dim,int val){
    if(inx>=shape_outputs.size())return -1;
    if(dim>=shape_outputs[inx].size())return -2;
    shape_outputs[inx][dim] = val;
    int size = 1;
    for(int k=0;k<shape_outputs[inx].size();k++){
        size *= shape_outputs[inx][dim];
    }
    if(size>0){
        size_outputs[inx] = size;
    }
    return 0;
}

AiCfg AiCfg::clone(){
    AiCfg onecfg;
    AiCfg* cfg = &onecfg;//new AiCfg();
                         //
    cfg->names.assign(names.begin(),names.end());
    cfg->kind_inputs.assign(kind_inputs.begin(),kind_inputs.end());
    cfg->name_inputs.assign(name_inputs.begin(),name_inputs.end());
    cfg->size_inputs.assign(size_inputs.begin(),size_inputs.end());
    //cfg->m_sizeinput = m_sizeinput;
    for(int k=0;k<shape_inputs.size();k++){
        std::vector<int64_t> shape(shape_inputs[k]);
        cfg->shape_inputs.push_back(shape);
    }

    cfg->kind_outputs.assign(kind_outputs.begin(),kind_outputs.end());
    cfg->name_outputs.assign(name_outputs.begin(),name_outputs.end());
    cfg->size_outputs.assign(size_outputs.begin(),size_outputs.end());
    //cfg->m_sizeoutput = m_sizeoutput;
    for(int k=0;k<shape_outputs.size();k++){
        std::vector<int64_t> shape(shape_outputs[k]);
        cfg->shape_outputs.push_back(shape);
    }
    return onecfg;//cfg;
}

AiModel::AiModel(){
    m_cfg = new AiCfg();
}

AiModel::~AiModel(){
    delete m_cfg;
}

int AiModel::doInitModel(){
    return -1;
}

AiCfg AiModel::config(){
    return m_cfg->clone();
}

void AiModel::dump(){
    m_cfg->dump();
}

int AiModel::pushName(const char* name,int input){
    std::string sname(name);
    m_cfg->names.push_back(sname);
    if(input){
        m_cfg->name_inputs.push_back(m_cfg->names[m_cfg->names.size()-1].c_str());
    }else{
        m_cfg->name_outputs.push_back(m_cfg->names[m_cfg->names.size()-1].c_str());
    }
    return 0;
}

int AiModel::initModel(std::string& modelpath){
    m_modelPath = modelpath;
    m_inited = doInitModel();
    printf("===init %d\n",m_inited);

    return m_inited;
}

int AiModel::initModel(std::string& binfn,std::string& paramfn){
    m_modelbin = binfn;
    m_modelparam = paramfn;
    m_inited = doInitModel();
    //printf("===init %d\n",m_inited);

    return m_inited;
}

int AiModel::doRunModel(void** arrin,void** arrout,void* stream,AiCfg* pcfg){
    return 0;
}


int AiModel::runModel(void** arrin,void** arrout,void* stream,AiCfg* pcfg){
    if(m_inited){
        //std::cout<<"model not inited:"<<m_inited<<std::endl;
        return -999;
    }
    return doRunModel(arrin,arrout,stream,pcfg);
}

OnnxModel::OnnxModel(){
}

OnnxModel::OnnxModel(int b,int w,int h):AiModel(){
    m_batch = b;
    m_width = w;
    m_height = h;
}

OnnxModel::~OnnxModel(){
}

int OnnxModel::doInitModel(){
    env = Ort::Env(OrtLoggingLevel::ORT_LOGGING_LEVEL_WARNING, "ONNX");
    sessionOptions = Ort::SessionOptions();
    //sessionOptions.AddConfigEntry("session.load_model_format","ORT");
    //std::vector<std::string> availableProviders = Ort::GetAvailableProviders();
    //auto cudaAvailable = std::find(availableProviders.begin(), availableProviders.end(), "CUDAExecutionProvider");
    //OrtCUDAProviderOptions cudaOption;

    //if(cudaAvailable != availableProviders.end()){
        //std::cout << "Inference device: GPU" << std::endl;
        //sessionOptions.AppendExecutionProvider_CUDA(cudaOption);
    //}else{
        //std::cout << "Inference device: CPU" << std::endl;
    //}

    session = Ort::Session(env, m_modelPath.c_str(), sessionOptions);
    //Ort::AllocatorWithDefaultOptions allocator;
    size_t numInputNodes = session.GetInputCount();
    size_t numOutputNodes = session.GetOutputCount();
    //std::cout << "input NUM: " << numInputNodes << std::endl;
    //std::cout << "Output NUM: " << numOutputNodes << std::endl;
    for(int k=0;k<numInputNodes;k++){
        //m_cfg->name_inputs.push_back(session.GetInputName(k));//, allocator));
        Ort::TypeInfo inputTypeInfo = session.GetInputTypeInfo(k);
        auto tensorInfo = inputTypeInfo.GetTensorTypeAndShapeInfo();
        auto elemType = tensorInfo.GetElementType();
        m_cfg->kind_inputs.push_back((int)elemType);
        std::vector<int64_t> inputTensorShape = tensorInfo.GetShape();

        if(m_batch&&inputTensorShape.size()){
            if(inputTensorShape[0]==-1)inputTensorShape[0]=m_batch;
        }
        if(m_batch&&(inputTensorShape.size()==4)){
            if(inputTensorShape[0]==-1)inputTensorShape[0]=m_batch;
            if(inputTensorShape[2]==-1)inputTensorShape[2]=m_height;
            if(inputTensorShape[3]==-1)inputTensorShape[3]=m_width;
        }
        int size = 1;
        for (auto shape : inputTensorShape){
            size *= shape;
        }
        m_cfg->shape_inputs.push_back(inputTensorShape);
        m_cfg->size_inputs.push_back(size);
    }

    for(int k=0;k<numOutputNodes;k++){
        //m_cfg->name_outputs.push_back(session.GetOutputName(k));//, allocator));
        Ort::TypeInfo outputTypeInfo = session.GetOutputTypeInfo(k);
        auto tensorInfo = outputTypeInfo.GetTensorTypeAndShapeInfo();
        auto elemType = tensorInfo.GetElementType();
        m_cfg->kind_outputs.push_back((int)elemType);
        std::vector<int64_t> outputTensorShape = tensorInfo.GetShape();
        if(m_batch&&outputTensorShape.size()){
            if(outputTensorShape[0]==-1)outputTensorShape[0]=m_batch;
        }
        if(m_batch&&(outputTensorShape.size()==4)){
            if(outputTensorShape[0]==-1)outputTensorShape[0]=m_batch;
            if(outputTensorShape[2]==-1)outputTensorShape[2]=m_height;
            if(outputTensorShape[3]==-1)outputTensorShape[3]=m_width;
        }
        int size = 1;
        for (auto shape : outputTensorShape){
            size *= shape;
        }
        m_cfg->shape_outputs.push_back(outputTensorShape);
        m_cfg->size_outputs.push_back(size);
    }

    return 0;
}


int OnnxModel::doRunModel(void** arrin,void** arrout,void* stream,AiCfg* pcfg){
    AiCfg* cfg = pcfg==nullptr?m_cfg:pcfg;
    int incnt = cfg->size_inputs.size();
    int outcnt = cfg->size_outputs.size();
    std::cout<<"run onnx:"<<outcnt<<std::endl;
    if(!arrin || !arrout)return -1;
    std::vector<Ort::Value> inputTensors;
    Ort::MemoryInfo memoryInfo = Ort::MemoryInfo::CreateCpu( OrtAllocatorType::OrtArenaAllocator, OrtMemType::OrtMemTypeDefault);
    for(int k=0;k<incnt;k++){
        inputTensors.push_back(Ort::Value::CreateTensor( memoryInfo, arrin[k] ,cfg->size_inputs[k]*4 , cfg->shape_inputs[k].data(), cfg->shape_inputs[k].size(), (ONNXTensorElementDataType)cfg->kind_inputs[k] ));
    }
    std::vector<Ort::Value> outputTensors;
    for(int k=0;k<outcnt;k++){
        outputTensors.push_back(Ort::Value::CreateTensor( memoryInfo, arrout[k] ,cfg->size_outputs[k]*4 , cfg->shape_outputs[k].data(), cfg->shape_outputs[k].size(),(ONNXTensorElementDataType)cfg->kind_outputs[k] ));
    }
    this->session.Run(Ort::RunOptions{nullptr}, cfg->names_in, inputTensors.data(), incnt, cfg->names_out, outputTensors.data(),outcnt);
    if(1)return 0;
    /*
    //for(int k=0;k<9;k++)dumpfloat((float*)arrout[k],10);//size_outputs[0]);
    std::vector<Ort::Value> aoutputTensors = this->session.Run(Ort::RunOptions{nullptr}, name_inputs.data(), inputTensors.data(),     1, name_outputs.data(), 9);
    //bool* pmsk = (bool*)aoutputTensors[1].GetTensorData<bool>();
    for(int k=0;k<9;k++){
    float* pbnf = (float*)aoutputTensors[0].GetTensorData<float>();
    memcpy(arrout[k],pbnf,size_outputs[k]*4);
    }
    */
    return 0;
}

NcnnModel::NcnnModel():AiModel(){
}

NcnnModel::NcnnModel(int w,int h):AiModel(){
    m_width = w;
    m_height = h;
}

NcnnModel::~NcnnModel(){
    net.clear();
}

int NcnnModel::doInitModel(){
    net.clear();
    net.load_param(m_modelparam.c_str());
    net.load_model(m_modelbin.c_str());
    return 0;    //
}

int NcnnModel::doRunModel(void** arrin,void** arrout,void* stream,AiCfg* pcfg){
    ncnn::Extractor ex = net.create_extractor();
    AiCfg* cfg = pcfg==nullptr?m_cfg:pcfg;
    int incnt = cfg->size_inputs.size();
    int outcnt = cfg->size_outputs.size();
    ncnn::Mat inmat[incnt];
    for(int k=0;k<incnt;k++){
        std::string name = cfg->name_inputs[k];
        JMat* mat = (JMat*)arrin[k];
        ncnn::Mat in_pack =mat->packingmat();
        ex.input(name.c_str(), in_pack);
    }
    for(int k=0;k<outcnt;k++){
        std::string name = cfg->name_outputs[k];
        JMat* mat = (JMat*)arrout[k];
        ncnn::Mat output;
        ex.extract(name.c_str(), output);
        ncnn::Mat in_park;
        ncnn::convert_packing(output,in_park,3);
        int size =  mat->width()*mat->height()*3*sizeof(float);
        memcpy((uint8_t*)mat->data(),in_park,size);
    }
    return 0;
}

#ifdef _TENSORRT_
#include "NvInferPlugin.h"
#include "cuda_runtime_api.h"
#include "cuda.h"
#define FPW 4
using namespace nvinfer1;

int TrtModel::doInitModel(){
    std::cout<<"deserialize eng !"<<m_modelPath<<std::endl;
    bool didInitPlugins = initLibNvInferPlugins(nullptr, "");
    std::ifstream cache(m_modelPath,std::ios::binary);
    cache.seekg(0,std::ios::end);
    int engSize = cache.tellg();
    if(!engSize)return -1;
    cache.seekg(0,std::ios::beg);
    void *modelMem = malloc(engSize);
    if(!modelMem)return -2;
    cache.read((char*)modelMem,engSize);
    cache.close();
    std::cout<<"deserialize size!"<<engSize<<std::endl;

    IRuntime *runtime = nvinfer1::createInferRuntime(m_logger);
    m_engine = runtime->deserializeCudaEngine(modelMem,engSize);
    runtime->destroy();
    free(modelMem);
    if(! m_engine){
        std::cout<<"deserialize eng error!"<<std::endl;
        return -10;
    }

    std::cout<<"aaa"<<std::endl;
    m_context = m_engine->createExecutionContext();
    int bindings = m_engine->getNbBindings();
    std::cout<<"bindings!"<<bindings<<std::endl;
    for(int k=0;k<bindings;k++){
        const char* name = m_engine->getBindingName(k);
        std::string sname(name    );
        bool input = m_engine->bindingIsInput(k);
        std::cout<<"name!"<<sname<<"===input"<<input<<std::endl;
        Dims dims = m_engine->getBindingDimensions(k);
        auto dt = m_engine->getBindingDataType(k);
        int tdt = dt==DataType::kFLOAT?ONNX_TENSOR_ELEMENT_DATA_TYPE_FLOAT:ONNX_TENSOR_ELEMENT_DATA_TYPE_INT32;
        std::vector<int64_t> tensorShape;
        int dsize = 1;
        for(int i = 0 ;i < dims.nbDims; i++){
            tensorShape.push_back(dims.d[i]);
            dsize *= dims.d[i];
            //cout<<"==dsize!"<<dims.d[i]<<endl;
        }
        //cout<<"==========dsize!"<<dsize<<endl;

        int size = dsize*FPW;//size_list[k];
        if(input){
            m_cfg->kind_inputs.push_back(tdt);
            m_cfg->shape_inputs.push_back(tensorShape);
            m_cfg->size_inputs.push_back(size);
            m_cfg->name_inputs.push_back(name);
        }else{
            m_cfg->kind_outputs.push_back(tdt);
            m_cfg->size_outputs.push_back(size);
            m_cfg->name_outputs.push_back(name);
            m_cfg->shape_outputs.push_back(tensorShape);
        }
    }
    //getchar();
    /*
       m_sizeinput = 0;
       for(int k=0;k<m_cfg->size_inputs.size();k++){
       int size = m_cfg->size_inputs[k];
       void* input ;
    //int flag = cudaMalloc(&input,size);
    //m_inputs.push_back(input);
    //m_bindings[k]=input;
    m_sizeinput += size;
    //printf("===input %d output %d \n",m_sizeinput,m_sizeoutput);
    }
    std::cout<<"bbb"<<std::endl;
    m_sizeoutput = 0;
    int offset = m_cfg->size_inputs.size();
    for(int k=0;k<m_cfg->size_outputs.size();k++){
    int size = m_cfg->size_outputs[k];
    void* output ;
    //int flag = cudaMalloc(&output,size);
    //m_outputs.push_back(output);
    //m_bindings[offset+k]=output;
    m_sizeoutput += size;
    //printf("===input %d output %d \n",m_sizeinput,m_sizeoutput);
    }
    std::cout<<"ccc"<<std::endl;
    */
    return 0;
}

int TrtModel::doRunModel(void** arrin,void** arrout,void* stream,AiCfg* pcfg){
    AiCfg* cfg = pcfg==nullptr?m_cfg:pcfg;
    int incnt = cfg->size_inputs.size();
    int outcnt = cfg->size_outputs.size();
    std::cout<<"incnt:"<<incnt<<"==outcnt:"<<outcnt<<std::endl;
    std::vector<void*> m_bindings(incnt+outcnt,NULL);
    for(int k=0;k<incnt;k++){
        std::cout<<"in:"<<cfg->size_inputs[k]<<std::endl;
        m_bindings[k] = arrin[k];
    }
    for(int k=0;k<outcnt;k++){
        std::cout<<"out:"<<cfg->size_outputs[k]<<std::endl;
        m_bindings[incnt+k] = arrout[k];
    }
    bool status = m_context->enqueue(1,m_bindings.data(),(cudaStream_t)stream,nullptr);
    return status?0:-1;
}

TrtModel::TrtModel(int b,int w,int h):AiModel(){
}

TrtModel::TrtModel():AiModel(){
}

TrtModel::~TrtModel(){
    if(m_context){
        m_context->destroy();
        m_context = nullptr;
    }
    if(m_engine){
        m_engine->destroy();
        m_engine = nullptr;
    }
}

#endif
/*
   int main(int argc,char** argv){
   OnnxModel *model=new OnnxModel();
   std::string fn = "onnx/mfcc.onnx";
   model->initModel(fn);
   delete model;
   return 0;
   }
   */
