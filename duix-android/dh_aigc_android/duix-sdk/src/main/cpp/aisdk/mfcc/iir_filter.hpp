#pragma once

#ifndef SERVICESUPERVISOR_IIR_FILTER_H
#define SERVICESUPERVISOR_IIR_FILTER_H

//E(t,f) is computed using a first-order in-finite impulse response (IIR) filter
#define UES_IIR_I
//#define UES_IIR_II

#ifdef UES_IIR_I

class IIR_I
{
private:
    double *m_pNum;
    double *m_pDen;
    double *m_px;
    double *m_py;
    int m_num_order;
    int m_den_order;
public:
    IIR_I();
    ~IIR_I();
    void reset();
    void setPara(double num[], int num_order, double den[], int den_order);
    void resp(double data_in[], int m, double data_out[], int n);
    void filter(double data_in[], double data_out[], int len);
};
/** \brief 将滤波器的内部状态清零，滤波器的系数保留
 * \return
 */
void IIR_I::reset()
{
    for(int i = 0; i <= m_num_order; i++)
    {
        m_pNum[i] = 0.0;
    }
    for(int i = 0; i <= m_den_order; i++)
    {
        m_pDen[i] = 0.0;
    }
}
IIR_I::IIR_I()
{
    m_pNum = NULL;
    m_pDen = NULL;
    m_px = NULL;
    m_py = NULL;
    m_num_order = -1;
    m_den_order = -1;
};
IIR_I::~IIR_I()
{
    delete[] m_pNum;
    delete[] m_pDen;
    delete[] m_px;
    delete[] m_py;
    m_pNum = NULL;
    m_pDen = NULL;
    m_px = NULL;
    m_py = NULL;
};

/** \brief
 *
 * \param num 分子多项式的系数，升序排列,num[0] 为常数项
 * \param m 分子多项式的阶数
 * \param den 分母多项式的系数，升序排列,den[0] 为常数项
 * \param m 分母多项式的阶数
 * \return
 */
void IIR_I::setPara(double num[], int num_order, double den[], int den_order)
{
    delete[] m_pNum;
    delete[] m_pDen;
    delete[] m_px;
    delete[] m_py;
    m_pNum = new double[num_order + 1];
    m_pDen = new double[den_order + 1];
    m_num_order = num_order;
    m_den_order = den_order;
    m_px = new double[num_order + 1];
    m_py = new double[den_order + 1];
    for(int i = 0; i < m_num_order; i++)
    {
        m_pNum[i] = num[i];
        m_px[i] = 0.0;
    }
    m_pNum[m_num_order] = 0.0;
    m_px[m_num_order] = 0.0;
    for(int i = 0; i < m_den_order; i++)
    {
        m_pDen[i] = den[i];
        m_py[i] = 0.0;
    }
    m_pDen[m_den_order] = 0.0;
    m_py[m_den_order] = 0.0;
}

/** \brief 计算 IIR 滤波器的时域响应，不影响滤波器的内部状态
 * \param data_in 为滤波器的输入，0 时刻之前的输入默认为 0，data_in[M] 及之后的输入默认为data_in[M-1]
 * \param data_out 滤波器的输出
 * \param M 输入数据的长度
 * \param N 输出数据的长度
 * \return
 */
void IIR_I::resp(double data_in[], int M, double data_out[], int N)
{
    int i, k, il;
    for(k = 0; k < N; k++)
    {
        data_out[k] = 0.0;
        for(i = 0; i <= m_num_order; i++)
        {
            if( k - i >= 0)
            {
                il = ((k - i) < M) ? (k - i) : (M - 1);
                data_out[k] = data_out[k] + m_pNum[i] * data_in[il];
            }
        }
        for(i = 1; i <= m_den_order; i++)
        {
            if( k - i >= 0)
            {
                data_out[k] = data_out[k] - m_pDen[i] * data_out[k - i];
            }
        }
    }
}

/** \brief 滤波函数，采用直接I型结构
 * 注：该函数内部修改过，移植librosa.pcen时参照scipy.signal.lfilter所做的设计。
 *
 * \param data_in[] 输入数据
 * \param data_out[] 保存滤波后的数据
 * \param len 数组的长度
 * \return
 */
void IIR_I::filter(double data_in[], double data_out[], int len)
{
    int i, k;
    m_py[1] = 1; //修改的地方，因为公式中y[n-k]，当为第一个元素时会出现y[-1]，pcen中y[-1]会被认为为1。
    for(k = 0; k < len; k++)
    {
        m_px[0] = data_in[k];
        m_py[0] = 0.0;
        for(i = 0; i <= m_num_order; i++)
        {
            m_py[0] = m_py[0] + m_pNum[i] * m_px[i];
        }
        for(i = 1; i <= m_den_order; i++)
        {
            m_py[0] = m_py[0] - m_pDen[i] * m_py[i];
        }
        for(i = m_num_order; i >= 1; i--)
        {
            m_px[i] = m_px[i-1];
        }
        for(i = m_den_order; i >= 1; i--)
        {
            m_py[i] = m_py[i-1];
        }
        data_out[k] = m_py[0];
    }
}

#endif

#ifdef UES_IIR_II

/**< IIR 滤波器直接II型实现 */
class IIR_II
{
public:
    IIR_II();
    void reset();
    void setPara(double num[], int num_order, double den[], int den_order);
    void resp(double data_in[], int m, double data_out[], int n);
    double filter(double data);
    void filter(double data[], int len);
    void filter(double data_in[], double data_out[], int len);
protected:
private:
    double *m_pNum;
    double *m_pDen;
    double *m_pW;
    int m_num_order;
    int m_den_order;
    int m_N;
};

class IIR_BODE
{
private:
    double *m_pNum;
    double *m_pDen;
    int m_num_order;
    int m_den_order;
    std::complex<double> poly_val(double p[], int order, double omega);
public:
    IIR_BODE();
    void setPara(double num[], int num_order, double den[], int den_order);
    std::complex<double> bode(double omega);
    void bode(double omega[], int n, std::complex<double> resp[]);
};

IIR_II::IIR_II()
{
//ctor
m_pNum = NULL;
m_pDen = NULL;
m_pW = NULL;
m_num_order = -1;
m_den_order = -1;
m_N = 0;
};

/** \brief 将滤波器的内部状态清零，滤波器的系数保留
 * \return
 */
void IIR_II::reset()
{
    for(int i = 0; i < m_N; i++)
    {
        m_pW[i] = 0.0;
    }
}
/** \brief
 *
 * \param num 分子多项式的系数，升序排列,num[0] 为常数项
 * \param m 分子多项式的阶数
 * \param den 分母多项式的系数，升序排列,den[0] 为常数项
 * \param m 分母多项式的阶数
 * \return
 */
void IIR_II::setPara(double num[], int num_order, double den[], int den_order)
{
    delete[] m_pNum;
    delete[] m_pDen;
    delete[] m_pW;
    m_num_order = num_order;
    m_den_order = den_order;
    m_N = fmax(num_order, den_order) + 1;
    m_pNum = new double[m_N];
    m_pDen = new double[m_N];
    m_pW = new double[m_N];
    for(int i = 0; i < m_N; i++)
    {
        m_pNum[i] = 0.0;
        m_pDen[i] = 0.0;
        m_pW[i] = 0.0;
    }
    for(int i = 0; i <= num_order; i++)
    {
        m_pNum[i] = num[i];
    }
    for(int i = 0; i <= den_order; i++)
    {
        m_pDen[i] = den[i];
    }
}
/** \brief 计算 IIR 滤波器的时域响应，不影响滤波器的内部状态
 * \param data_in 为滤波器的输入，0 时刻之前的输入默认为 0，data_in[M] 及之后的输入默认为data_in[M-1]
 * \param data_out 滤波器的输出
 * \param M 输入数据的长度
 * \param N 输出数据的长度
 * \return
 */
void IIR_II::resp(double data_in[], int M, double data_out[], int N)
{
    int i, k, il;
    for(k = 0; k < N; k++)
    {
        data_out[k] = 0.0;
        for(i = 0; i <= m_num_order; i++)
        {
            if( k - i >= 0)
            {
                il = ((k - i) < M) ? (k - i) : (M - 1);
                data_out[k] = data_out[k] + m_pNum[i] * data_in[il];
            }
        }
        for(i = 1; i <= m_den_order; i++)
        {
            if( k - i >= 0)
            {
                data_out[k] = data_out[k] - m_pDen[i] * data_out[k - i];
            }
        }
    }
}
/** \brief 滤波函数，采用直接II型结构
 *
 * \param data 输入数据
 * \return 滤波后的结果
 */
double IIR_II::filter(double data)
{
    m_pW[0] = data;
    for(int i = 1; i <= m_den_order; i++) // 先更新 w[n] 的状态
    {
        m_pW[0] = m_pW[0] - m_pDen[i] * m_pW[i];
    }
    data = 0.0;
    for(int i = 0; i <= m_num_order; i++)
    {
        data = data + m_pNum[i] * m_pW[i];
    }
    for(int i = m_N - 1; i >= 1; i--)
    {
        m_pW[i] = m_pW[i-1];
    }
    return data;
}
/** \brief 滤波函数，采用直接II型结构
 *
 * \param data[] 传入输入数据，返回时给出滤波后的结果
 * \param len data[] 数组的长度
 * \return
 */
void IIR_II::filter(double data[], int len)
{
    int i, k;
    for(k = 0; k < len; k++)
    {
        m_pW[0] = data[k];
        for(i = 1; i <= m_den_order; i++) // 先更新 w[n] 的状态
        {
            m_pW[0] = m_pW[0] - m_pDen[i] * m_pW[i];
        }
        data[k] = 0.0;
        for(i = 0; i <= m_num_order; i++)
        {
            data[k] = data[k] + m_pNum[i] * m_pW[i];
        }

        for(i = m_N - 1; i >= 1; i--)
        {
            m_pW[i] = m_pW[i-1];
        }
    }
}
/** \brief 滤波函数，采用直接II型结构
 *
 * \param data_in[] 输入数据
 * \param data_out[] 保存滤波后的数据
 * \param len 数组的长度
 * \return
 */
void IIR_II::filter(double data_in[], double data_out[], int len)
{
    int i, k;
    for(k = 0; k < len; k++)
    {
        m_pW[0] = data_in[k];
        for(i = 1; i <= m_den_order; i++) // 先更新 w[n] 的状态
        {
            m_pW[0] = m_pW[0] - m_pDen[i] * m_pW[i];
        }
        data_out[k] = 0.0;
        for(i = 0; i <= m_num_order; i++)
        {
            data_out[k] = data_out[k] + m_pNum[i] * m_pW[i];
        }

        for(i = m_N - 1; i >= 1; i--)
        {
            m_pW[i] = m_pW[i-1];
        }
    }
}

#endif

#endif //SERVICESUPERVISOR_IIR_FILTER_H
