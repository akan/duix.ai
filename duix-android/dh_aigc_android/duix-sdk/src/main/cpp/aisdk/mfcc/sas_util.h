#pragma once

#include <string>
#include <chrono>
#include <vector>
#include <assert.h>
#include <memory>
#include <fstream>
#include "opencv2/opencv.hpp"

using namespace std;
using namespace std::chrono;

class parambase
{
public:
    string name;
    string help;
    string strval;
    parambase(){}
    virtual  ~parambase(){}
    virtual bool set(const char* value) {return true;};
};

/**
 * 引擎参数
 */
class EnginePar
{
public:
    static int cs_timeout; //叫号服务完成的超时时间(默认情况下下一次叫号代表上一次完成,默认值5分钟)
    static int cs_detecthandsup_time; //叫号后持续检测举手的时间(默认10s)
    static int cs_detecthandsup_interval ; //叫号后持续检测举手的时间间隔(默认1秒1次)
    static int cs_detectsmile_interval; //叫号后微笑检测的时间间隔(默认1秒1次)
    static int cs_detectspeech_interval;//叫号后语音检测的时间间隔(默认20秒)
    static int cs_detectpose_interval;  //叫号后姿态检测的时间间隔(默认5秒1次)
    static int detectpose_interval;     //非叫号期间姿态检测的时间间隔(默认5秒1次)
    static int detectsmile_interval;    //非叫号期间微笑检测的时间间隔(默认1秒1次)
    static int detectappearance_interval; //着装检测间隔
    static float action_turnpen_thrd;   //转笔阈值
    static float action_turnchair_thrd; //转椅阈值
    static float action_record_time;    //动作录制时长
    static float sit_supporthead_thrd;  //撑头阈值
    static float sit_layondesk_thrd;    //趴桌阈值
    static float sit_relyingonchair_thrd;//靠椅阈值
    static string log_path;
    static string log_level;
    static string temp_path;
	static bool set(const char* key, const char* val);
	static bool haskey(const char* key);
	static const char* getvalue(const char* key);
};
/**
 * 摄像头分析场景
 */
enum VideoScene
{
    SCENE_counter,    // 柜台
    SCENE_financial,   // 理财
    SCENE_lobby,       // 大堂
    SCENE_hall             // 门厅
};
/**
 * 摄像头分析参数
 */
class VideoPar
{
private:
    vector<shared_ptr<parambase>> params;
public:
    VideoScene scene;            //场景: 1柜台, 2理财, 3大堂, 4进门(着装检测)
    bool audio_enable ;          //音频开关 1开,0关
    int audio_channels ;         //音频通道数 0,1,2,4,6
    int audio_sample_rate ;      // 采样率 44100, 48000, 96000, 192000
    bool video_enable ;          // 视频开关 1开,0关
    //int video_analyse_rate ;   //视频分析速率: 数字>0,每秒分析帧数
    bool video_sample_keyframe;  //只解码关键帧
    bool video_record;           //启用录制视频 1开,0关
    int video_record_duration;   //视频录制时长,默认10s
    int video_record_reviewtime; //视频录制回溯时长,默认5s
    int face_minsize;            //最小人脸大小
    VideoPar();
    //~VideoPar();
    bool set(const char* key, const char* val);
    static bool haskey(const char* key);
};

template<class T>
inline int64_t NowTime()
{
	return std::chrono::time_point_cast<T>(std::chrono::system_clock::now()).time_since_epoch().count();
}

/**--------------------------------- 以下是models各个模型所用到的方法 ---------------------------------**/

inline bool detectFileExist(char *file_path) {
    std::ifstream _ifstream;
    _ifstream.open(file_path, std::ios::in);
    if (!_ifstream) {
        return false;
    }
    _ifstream.close();
    return true;
}

// 矩阵变换，对向量xy进行旋转
inline cv::Mat_<double> rotate_point(cv::Mat_<double> xy, double angle) {
    cv::Mat rotate_matrix = (cv::Mat_<double>(2, 2) << cos(angle), -sin(angle), sin(angle), cos(angle));
    cv::transpose(rotate_matrix, rotate_matrix);
    auto rotate_xy = xy * rotate_matrix;
    return rotate_xy;
}

// 检查点是否在框内
inline bool check_point_in_rect(cv::Point point, cv::Rect rect) {
    if ((rect.x < point.x && point.x < rect.x + rect.width) &&
        (rect.y < point.y && point.y < rect.y + rect.height)) {
        return true;//在rect内部
    } else {
        return false;//在rect边上或外部
    }
}

