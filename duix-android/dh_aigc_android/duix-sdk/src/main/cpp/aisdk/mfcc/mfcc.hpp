#pragma once

#include "AudioFFT.hpp"
//#include"../third/numcpp/NumCpp.hpp"
#include "opencv2/opencv.hpp"
#include "iir_filter.hpp"
#include "sas_util.h"

int nSamplesPerSec = 16000;                     //采样率(每秒样本数) //Sample rate.(keda, thchs30, aishell)
int length_DFT = 1024;//2048;                          //傅里叶点数 //fft points (samples)
int hop_length = 160;//int(0.05 * nSamplesPerSec);    //步长 //下一帧取数据相对于这一帧的右偏移量
int win_length = 800;// int(0.1 * nSamplesPerSec);     //帧长 //假设16000采样率，则取取0.1s时间的数据
int number_filterbanks = 80;                    //过滤器数量 //Number of Mel banks to generate
float preemphasis = 0.97;                       //预加重（高通滤波器比例值）
int max_db = 100;
int ref_db = 20;
int r = 1;                                      //librosa里的r=1，暂未深入分析其作用
double pi = 3.14159265358979323846;

cv::Mat_<double> mel_basis;
cv::Mat_<float> hannWindow;

std::shared_ptr<IIR_I> filter;

//"""Convert Hz to Mels"""
double hz_to_mel(double frequencies, bool htk = false) {
    if (htk) {
        return 2595.0 * log10(1.0 + frequencies / 700.0);
    }
    // Fill in the linear part
    double f_min = 0.0;
    double f_sp = 200.0 / 3;
    double mels = (frequencies - f_min) / f_sp;
    // Fill in the log-scale part
    double min_log_hz = 1000.0;                         // beginning of log region (Hz)
    double min_log_mel = (min_log_hz - f_min) / f_sp;   // same (Mels)
    double logstep = log(6.4) / 27.0;              // step size for log region

    // 对照Python平台的librosa库，移植
    //如果是多维数列
//    if (frequencies.ndim) {
//        // If we have array data, vectorize
//        log_t = (frequencies >= min_log_hz)
//        mels[log_t] = min_log_mel + np.log(frequencies[log_t] / min_log_hz) / logstep
//    } else
    if (frequencies >= min_log_hz) {
        // If we have scalar data, heck directly
        mels = min_log_mel + log(frequencies / min_log_hz) / logstep;
    }
    return mels;
}

//"""Convert mel bin numbers to frequencies"""
cv::Mat_<double> mel_to_hz(cv::Mat_<double> mels, bool htk = false) {
//    if (htk) {
//        return //python://700.0 * (10.0**(mels / 2595.0) - 1.0);
//    }
    // Fill in the linear scale
    double f_min = 0.0;
    double f_sp = 200.0 / 3;
    cv::Mat_<double> freqs = mels * f_sp + f_min;
    // And now the nonlinear scale
    double min_log_hz = 1000.0;                         // beginning of log region (Hz)
    double min_log_mel = (min_log_hz - f_min) / f_sp;   // same (Mels)
    double logstep = log(6.4) / 27.0;              // step size for log region
    // 对照Python平台的librosa库，移植
    //if (mels.ndim) {
    // If we have vector data, vectorize
    cv::Mat_<bool> log_t = (mels >= min_log_mel);
    for (int i = 0; i < log_t.cols; i++) {
        if (log_t(0, i)) {
            freqs(0, i) = cv::exp((mels(0, i) - min_log_mel) * logstep) * min_log_hz;
        }
    }
    //}
    return freqs;
}

// 生成等差数列，类似np.linspace
cv::Mat_<double> cvlinspace(double min_, double max_, int length) {
    auto cvmat = cv::Mat_<double>(1, length);
    for (int i = 0; i < length; i++) {
        cvmat(0, i) = ((max_ - min_) / (length - 1) * i) + min_;
    }
    return cvmat;
}

//"""Create a Filterbank matrix to combine FFT bins into Mel-frequency bins"""
cv::Mat_<double> mel_spectrogram_create(int nps, int n_fft, int n_mels) {
    double f_max = nps / 2.0;
    double f_min = 0;
    int n_fft_2 = 1 + n_fft / 2;
    // Initialize the weights
    //auto weights = nc::zeros<double>(nc::uint32(n_mels), nc::uint32(n_fft_2));
    auto weights = cv::Mat_<double>(n_mels, n_fft_2, 0.0);
    // Center freqs of each FFT bin
    //auto fftfreqs_ = nc::linspace<double>(f_min, f_max, nc::uint32(n_fft_2), true);
    auto fftfreqs = cvlinspace(f_min, f_max, n_fft_2);

    // 'Center freqs' of mel bands - uniformly spaced between limits
    double min_mel = hz_to_mel(f_min, false);
    double max_mel = hz_to_mel(f_max, false);
    //auto mels_ = nc::linspace(min_mel, max_mel, nc::uint32(n_mels + 2));
    auto mels = cvlinspace(min_mel, max_mel, n_mels + 2);
    auto mel_f = mel_to_hz(mels, false);

    //auto fdiff_ = nc::diff(mel_f_); //沿着指定轴计算第N维的离散差值(后一个元素减去前一个元素)
    cv::Mat_<double> d1(1, mel_f.cols * mel_f.rows - 1, (double *) (mel_f.data) + 1);
    cv::Mat_<double> d2(1, mel_f.cols * mel_f.rows - 1, (double *) (mel_f.data));
    cv::Mat_<double> fdiff = d1 - d2;

    //auto ramps = nc::subtract.outer(mel_f, fftfreqs); //nc没有subtract.outer
    //nc::NdArray<double> ramps = nc::zeros<double>(mel_f.cols, fftfreqs.cols);
    auto ramps = cv::Mat_<double>(mel_f.cols, fftfreqs.cols);
    for (int i = 0; i < mel_f.cols; i++) {
        for (int j = 0; j < fftfreqs.cols; j++) {
            ramps(i, j) = mel_f(0, i) - fftfreqs(0, j);
        }
    }

    for (int i = 0; i < n_mels; i++) {
        // lower and upper slopes for all bins
        //auto ramps_1 = nc::NdArray<double>(1, ramps.cols);
        auto ramps_1 = cv::Mat_<double>(1, ramps.cols);
        for (int j = 0; j < ramps.cols; j++) {
            ramps_1(0, j) = ramps(i, j);
        }
        //auto ramps_2 = nc::NdArray<double>(1, ramps.cols);
        auto ramps_2 = cv::Mat_<double>(1, ramps.cols);
        for (int j = 0; j < ramps.cols; j++) {
            ramps_2(0, j) = ramps(i + 2, j);
        }
        cv::Mat_<double> lower = ramps_1 * -1 / fdiff(0, i);
        cv::Mat_<double> upper = ramps_2 / fdiff(0, i + 1);
        // .. then intersect them with each other and zero
        //auto weights_1 = nc::maximum(nc::zeros<double>(1, ramps.cols), nc::minimum(lower, upper));
        cv::Mat c1 = lower;//(cv::Mat_<double>(1,5) << 1,2,-3,4,-5);
        cv::Mat c2 = upper;
        cv::Mat weights_1 = cv::Mat_<double>(1, lower.cols);
        cv::min(c1, c2, weights_1);
        cv::max(weights_1, 0, weights_1);
        for (int j = 0; j < n_fft_2; j++) {
            weights(i, j) = weights_1.at<double_t>(0, j);
        }
    }

    // Slaney-style mel is scaled to be approx constant energy per channel
    auto enorm = cv::Mat_<double>(1, n_mels);
    for (int j = 0; j < n_mels; j++) {
        enorm(0, j) = 2.0 / (mel_f(0, j + 2) - mel_f(0, j));
    }
    for (int j = 0; j < n_mels; j++) {
        for (int k = 0; k < n_fft_2; k++) {
            weights(j, k) *= enorm(0, j);
        }
    }
    return weights;
}

//"""Short-time Fourier transform (STFT)""": 默认center=True, window='hann', pad_mode='reflect'
cv::Mat_<double> MagnitudeSpectrogram(const cv::Mat_<float> *emphasis_data, int n_fft = 2048, int hop_length = 0,
                                      int win_length = 0) {
    if (win_length == 0) {
        win_length = n_fft;
    }
    if (hop_length == 0) {
        hop_length = win_length / 4;
    }

    // reflect对称填充
    int pad_lenght = n_fft / 2;
    // 使用opencv里的copyMakeBorder来完成reflect填充
    cv::Mat_<float> cv_padbuffer;
    cv::copyMakeBorder(*emphasis_data, cv_padbuffer, 0, 0, pad_lenght, pad_lenght, cv::BORDER_REFLECT_101);

    // windowing加窗：将每一帧乘以汉宁窗，以增加帧左端和右端的连续性。
    // 生成一个1600长度的hannWindow，并居中到2048长度的
    if (hannWindow.empty()) {
        hannWindow = cv::Mat_<float>(1, n_fft, 0.0f);
        int insert_cnt = 0;
        if (n_fft > win_length) {
            insert_cnt = (n_fft - win_length) / 2;
        } else {
            //std::cout << "\tn_fft:" << n_fft << " > win_length:" << n_fft << std::endl;
            return cv::Mat_<double>(0, 0);
        }
        for (int k = 1; k <= win_length; k++) {
            hannWindow(0, k - 1 + insert_cnt) = float(0.5 * (1 - cos(2 * pi * k / (win_length + 1))));
        }
    }
    // opencv虽然有Hann窗生成函数，但是必须要求width > 1，height > 1
    //cv::Mat_<double> cv_hannWindow;
    //cv::createHanningWindow(cv_hannWindow, cv::Size(1, win_length), CV_64FC1);

    int size = cv_padbuffer.rows * cv_padbuffer.cols;//padbuffer.size()
    int number_feature_vectors = (size - n_fft) / hop_length + 1;
    int number_coefficients = n_fft / 2 + 1;
    cv::Mat_<float> feature_vector(number_feature_vectors, number_coefficients, 0.0f);

    audiofft::AudioFFT fft; //将FFT初始化放在循环外，可达到最优速度
    fft.init(size_t(n_fft));
    for (int i = 0; i <= size - n_fft; i += hop_length) {
        // 每次取一段数据
        cv::Mat_<float> framef = cv::Mat_<float>(1, n_fft, (float *) (cv_padbuffer.data) + i).clone();
        // 加hann窗
        framef = framef.mul(hannWindow);

        // 复数：Xrf实数，Xif虚数。
        cv::Mat_<float> Xrf(1, number_coefficients);
        cv::Mat_<float> Xif(1, number_coefficients);
        fft.fft((float *) (framef.data), (float *) (Xrf.data), (float *) (Xif.data));

        // 求模
        cv::pow(Xrf, 2, Xrf);
        cv::pow(Xif, 2, Xif);
        cv::Mat_<float> cv_feature(1, number_coefficients, &(feature_vector[i / hop_length][0]));
        cv::sqrt(Xrf + Xif, cv_feature);
    }
    cv::Mat_<float> cv_mag;
    cv::transpose(feature_vector, cv_mag);
    cv::Mat_<double> mag;
    cv_mag.convertTo(mag, CV_64FC1);

    return mag;
}

/*********************************************
 * 名称：log_mel
 * 功能：传入音频数据，输出log-mel方式提取的特征数据。
 * 参数：@ifile_data        传入的音频数据
 *      @nSamples_per_sec  音频采样率
 * 返回：cv::Mat_<double>   特征数据
*********************************************/
//cv::Mat_<double> log_mel(std::vector<uint8_t> &ifile_data, int nSamples_per_sec) {
int log_mel(float* ifile_data, int ifile_length,int nSamples_per_sec,float* ofile_data) {
    if (nSamples_per_sec != nSamplesPerSec) {
        //std::cout << R"(the "nSamples_per_sec" is not 16000.)" << std::endl;
        return -1;//cv::Mat_<double>(0, 0);
    }
    //int ifile_length = int(ifile_data.size() / 4);

    // pre-emphasis 预加重 //高通滤波
    //cv::Mat_<float> d1(1, ifile_length - 1, (float *) (ifile_data.data()) + 1);
    //cv::Mat_<float> d2(1, ifile_length-1 , (float *) (ifile_data.data()));
    cv::Mat_<float> d1(1, ifile_length - 1, (float *) (ifile_data) + 1);
    cv::Mat_<float> d2(1, ifile_length-1 , (float *) (ifile_data));

    //std::cout<<ifile_length<<"====="<<d1[0][960000-1]<<std::endl;
    cv::Mat_<float> cv_emphasis_data;

    cv::hconcat(cv::Mat_<float>::zeros(1, 1), d1 - d2 * preemphasis, cv_emphasis_data);
    //cv::print(cv_emphasis_data);
    //std::cout<<ifile_length<<"====="<< cv_emphasis_data[0][960000-1]<<std::endl;

    // magnitude spectrogram 幅度谱图
    auto mag = MagnitudeSpectrogram(&cv_emphasis_data, length_DFT, hop_length, win_length);
    auto magb = cv::abs(mag);
    cv::pow(magb,2,mag);
    //tooken
    // 生成梅尔谱图 mel spectrogram       //3ms
    if (mel_basis.empty()) {
        mel_basis = mel_spectrogram_create(nSamplesPerSec, length_DFT, number_filterbanks);
    }

    //cv::print(mel_basis);
    //std::cout<<mel_basis.cols<<"=====cv_mel"<<mel_basis.rows<<std::endl;
    // doc
    cv::Mat cv_mel = mel_basis * mag;
    //cv::Mat cv_mel = mel_basis.dot( mag);

    // to decibel
    //mel = 20 * np.log10(np.maximum(1e-5, mel))
    //mag = 20 * np.log10(np.maximum(1e-5, mag))
    //由于后续没用用到mag了，所以不再对mag做运算。
    // 使用opencv来实现
    //cv::log(cv::max(cv_mel, 1e-5), cv_mel);

    //cv::log(cv::max(cv_mel, 1e-5), cv_mel);
    cv::log(cv_mel+ 1e-5, cv_mel);
    // opencv没有log10()，所以使用log(x)/log(10)来运算。
    cv_mel = cv_mel / 2.3025850929940459 * 10; // 2.3025850929940459=log(10)

    // normalize
    //mel = np.clip((mel - hp.ref_db + hp.max_db) / hp.max_db, 1e-8, 1)
    //mag = np.clip((mag - hp.ref_db + hp.max_db) / hp.max_db, 1e-8, 1)
    //cv::normalize(cv_mel, cv_mel, 1e-8, 1.0, cv::NORM_MINMAX); // cv::normalize无法实现
    //cv_mel = (cv_mel - ref_db + max_db) / max_db;
    //cv_mel = cv::max(cv::min(cv_mel, 1.0), 1e-8);
    cv_mel = cv_mel - ref_db;
    //cv::print(cv_mel);
    //std::cout<<cv_mel.cols<<"=====cv_mel"<<cv_mel.rows<<std::endl;
    //std::cout<<"=====cv_mel"<<std::endl;
        //getchar();

    // Transpose
    //mel = mel.T.astype(np.float32)
    //mag = mag.T.astype(np.float32)
    // 使用opencv的transpose
    cv::Mat cv_mel_r;//(cv_mel.cols,cv_mel.rows,CV_64FC1,ofile_data);
    cv::transpose(cv_mel, cv_mel_r);
    //cv::Mat rcv(cv_mel_r.cols,cv_mel_r.rows, CV_32FC1,ofile_data);
    cv::Mat rrr(cv_mel.cols,cv_mel.rows,CV_32FC1,ofile_data);
    cv_mel_r.convertTo(rrr, CV_32FC1);

    if (r == 1) {
        // 原计算公式是：
        // mel = mel[:len(mel) // hp.r * hp.r].reshape([len(mel) // hp.r, hp.r * hp.n_mels])
        // 当r=1的时候公式运算无任何数值改变。
    } else {
        //std::cout << R"(the "r" is not 1.)" << std::endl;
    }
    // 返回mel特征向量
    return 0;
}

/**--------------------------------- 以下是pcen运算方法 ---------------------------------**/

// scipy.signal.lfilter_zi()
cv::Mat_<double> cvlfilter_zi(cv::Mat_<double> b, cv::Mat_<double> a) {
    if ((b.rows != 1) || (a.rows != 1)) {
        //std::cout << "Numerator b and Denominator a must be 1-D." << std::endl;
    }
    if (a(0, 0) != 1) {
        // Normalize the coefficients so a[0] == 1.
        b = b / a(0, 0);
        a = a / a(0, 0);
    }
    int len_a = a.cols * a.rows;
    int len_b = b.cols * b.rows;
    int n = len_a > len_b ? len_a : len_b;
    if (len_a < n) {
        cv::hconcat(a, cv::Mat_<float>::zeros(1, n - len_a), a);
    } else if (len_b < n) {
        cv::hconcat(b, cv::Mat_<float>::zeros(1, n - len_b), b);
    }
    return cv::Mat_<double>(0, 0);
}
/*
// scipy.signal.lfilter()
// Filter data along one-dimension with an IIR or FIR filter.
cv::Mat_<double> cvlfilter(cv::Mat_<double> &b, cv::Mat_<double> &a, cv::Mat_<double> &x,
                           cv::Mat_<double> &zi, int axis = -1) {
    if (a.rows * a.cols == 1) {
        // This path only supports types fdgFDGO to mirror _linear_filter below.
        // Any of b, a, x, or zi can set the dtype, but there is no default
        // casting of other types; instead a NotImplementedError is raised.
        // 后续如果需要，则进行补充
    } else {
        // return sigtools._linear_filter(b, a, x, axis, zi)
        // sigtools._linear_filter()
        // (y,Vf) = _linear_filter(b,a,X,Dim=-1,Vi=None)  implemented using Direct Form II transposed flow diagram.
        // If Vi is not given, Vf is not returned.
        ;
    }
}
*/
/*********************************************
 * 名称：pcen
 * 功能：传入音频数据，输出pcen方式提取的特征数据。
 * 参数：@ifile_data        传入的音频数据
 *      @nSamples_per_sec  音频采样率
 * 返回：cv::Mat_<double>   特征数据
*********************************************/
cv::Mat_<double> pcen(std::vector<uint8_t> &ifile_data, int nSamples_per_sec) {
    //if (!(&ifile_data) || ifile_data.empty()) {
    if (ifile_data.empty()) {
        //std::cout << "error: invalid paramter: ifile_data" << std::endl;
        return cv::Mat_<double>(0, 0);
    }
    if (nSamples_per_sec != nSamplesPerSec) {
//        std::cout << R"(error: the "nSamples_per_sec" is not 16000.)" << std::endl;
        return cv::Mat_<double>(0, 0);
    }
    int ifile_length = int(ifile_data.size() / 4);
    cv::Mat_<float> cv_emphasis_data(1, ifile_length, (float *) (ifile_data.data()));
//    std::cout<<ifile_length<<"====="<<cv_emphasis_data[0][960000-1]<<std::endl;
    //getchar();

    // magnitude spectrogram 幅度谱图
    auto mag = MagnitudeSpectrogram(&cv_emphasis_data, length_DFT, hop_length, win_length);
    mag = cv::abs(mag) * std::pow(2, 31);

    // 生成梅尔谱图 mel spectrogram       //3ms
    if (mel_basis.empty()) {
        mel_basis = mel_spectrogram_create(nSamplesPerSec, length_DFT, number_filterbanks);
    }

    // doc
    cv::Mat_<double> mel = mel_basis * mag;

    // 计算pcen特征
//    double time_constant = 0.400;
//    int sr = 22050;
//    int hop_length = 512;
//    double t_frames = time_constant * sr / double(hop_length);
//    double b = (sqrt(1 + 4 * t_frames * t_frames) - 1) / (2 * t_frames * t_frames);
//    cv::Mat_<double> zi = (cv::Mat_<double>(1, 1) << 0.94361056);
//
//    cv::Mat_<double> in_b = (cv::Mat_<double>(1, 1) << b);
//    cv::Mat_<double> in_a = (cv::Mat_<double>(1, 2) << 1, b - 1);
//    cv::Mat_<double> zi = cvlfilter_zi(in_b, in_a);
    // 第二个公式计算
//    cv::Mat_<double> S_smooth = cvlfilter(in_b, in_a, mel, zi);

#if 1 // IIR滤波器
    if (!filter) {
        filter = std::make_shared<IIR_I>();
        double iir_b[1] = {0.05638943879134889};
        double iir_a[2] = {1.0, -0.9436105612086512};
        //filter.reset();
        filter->setPara(iir_b, 1, iir_a, 2);
    }
    cv::Mat_<double> S_smooth = cv::Mat_<double>(mel.rows, mel.cols);
    for (int i = 0; i < mel.rows; i++) {
        filter->filter(mel[i], S_smooth[i], mel.cols);
    }

#endif

    // 第一个公式计算
    double gain = 0.98;
    double bias = 2.0;
    double power = 0.5;
    double eps = 1e-6;
    //python: smooth = np.exp(-gain * (np.log(eps) + np.log1p(S_smooth / eps)))
    cv::Mat_<double> S_smooth_log1p;
    cv::log(S_smooth / eps + 1, S_smooth_log1p);
    cv::Mat_<double> smooth;
    cv::exp((S_smooth_log1p + cv::log(eps)) * (-gain), smooth);
    //python: S_out = (bias ** power) * np.expm1(power * np.log1p(ref * smooth / bias))
    cv::Mat_<double> smooth_log1p;
    cv::Mat_<double> smooth_log1p_exp;
    cv::log(mel.mul(smooth) / bias + 1, smooth_log1p);
    cv::exp(power * smooth_log1p, smooth_log1p_exp);
    cv::Mat_<double> S_out = (smooth_log1p_exp - 1) * pow(bias, power);
    // transpose
    cv::Mat_<double> pcen;
    cv::transpose(S_out, pcen);

    return pcen;
}
