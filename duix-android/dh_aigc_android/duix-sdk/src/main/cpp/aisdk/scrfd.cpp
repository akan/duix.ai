#include "scrfd.h"
#include "cpu.h"

static int drawface(cv::Mat& rgb, const std::vector<FaceObject>& faceobjects)
{
    int has_kps = 1;
    for (size_t i = 0; i < faceobjects.size(); i++)
    {
        const FaceObject& obj = faceobjects[i];

//         fprintf(stderr, "%.5f at %.2f %.2f %.2f x %.2f\n", obj.prob,
//                 obj.rect.x, obj.rect.y, obj.rect.width, obj.rect.height);

        cv::rectangle(rgb, obj.rect, cv::Scalar(0, 255, 0));

        if (has_kps)
        {
            cv::circle(rgb, obj.landmark[0], 2, cv::Scalar(255, 255, 0), -1);
            cv::circle(rgb, obj.landmark[1], 2, cv::Scalar(255, 255, 0), -1);
            cv::circle(rgb, obj.landmark[2], 2, cv::Scalar(255, 255, 0), -1);
            cv::circle(rgb, obj.landmark[3], 2, cv::Scalar(255, 255, 0), -1);
            cv::circle(rgb, obj.landmark[4], 2, cv::Scalar(255, 255, 0), -1);
        }

        char text[256];
        sprintf(text, "%.1f%%", obj.prob * 100);

        int baseLine = 0;
        cv::Size label_size = cv::getTextSize(text, cv::FONT_HERSHEY_SIMPLEX, 0.5, 1, &baseLine);

        int x = obj.rect.x;
        int y = obj.rect.y - label_size.height - baseLine;
        if (y < 0)
            y = 0;
        if (x + label_size.width > rgb.cols)
            x = rgb.cols - label_size.width;

        cv::rectangle(rgb, cv::Rect(cv::Point(x, y), cv::Size(label_size.width, label_size.height + baseLine)), cv::Scalar(255, 255, 255), -1);

        cv::putText(rgb, text, cv::Point(x, y + label_size.height), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 0, 0), 1);
    }

    return 0;
}

static inline float intersection_area(const FaceObject& a, const FaceObject& b)
{
    cv::Rect_<float> inter = a.rect & b.rect;
    return inter.area();
}

static void qsort_descent_inplace(std::vector<FaceObject>& faceobjects, int left, int right)
{
    int i = left;
    int j = right;
    float p = faceobjects[(left + right) / 2].prob;

    while (i <= j)
    {
        while (faceobjects[i].prob > p)
            i++;

        while (faceobjects[j].prob < p)
            j--;

        if (i <= j)
        {
            // swap
            std::swap(faceobjects[i], faceobjects[j]);

            i++;
            j--;
        }
    }

//     #pragma omp parallel sections
    {
//         #pragma omp section
        {
            if (left < j) qsort_descent_inplace(faceobjects, left, j);
        }
//         #pragma omp section
        {
            if (i < right) qsort_descent_inplace(faceobjects, i, right);
        }
    }
}

static void qsort_descent_inplace(std::vector<FaceObject>& faceobjects)
{
    if (faceobjects.empty())
        return;

    qsort_descent_inplace(faceobjects, 0, faceobjects.size() - 1);
}

static void nms_sorted_bboxes(const std::vector<FaceObject>& faceobjects, std::vector<int>& picked, float nms_threshold)
{
    picked.clear();

    const int n = faceobjects.size();

    std::vector<float> areas(n);
    for (int i = 0; i < n; i++)
    {
        areas[i] = faceobjects[i].rect.area();
    }

    for (int i = 0; i < n; i++)
    {
        const FaceObject& a = faceobjects[i];

        int keep = 1;
        for (int j = 0; j < (int)picked.size(); j++)
        {
            const FaceObject& b = faceobjects[picked[j]];

            // intersection over union
            float inter_area = intersection_area(a, b);
            float union_area = areas[i] + areas[picked[j]] - inter_area;
            //             float IoU = inter_area / union_area
            if (inter_area / union_area > nms_threshold)
                keep = 0;
        }

        if (keep)
            picked.push_back(i);
    }
}

// insightface/detection/scrfd/mmdet/core/anchor/anchor_generator.py gen_single_level_base_anchors()
static ncnn::Mat generate_anchors(int base_size, const ncnn::Mat& ratios, const ncnn::Mat& scales)
{
    int num_ratio = ratios.w;
    int num_scale = scales.w;

    ncnn::Mat anchors;
    anchors.create(4, num_ratio * num_scale);

    const float cx = 0;
    const float cy = 0;

    for (int i = 0; i < num_ratio; i++)
    {
        float ar = ratios[i];

        int r_w = round(base_size / sqrt(ar));
        int r_h = round(r_w * ar); //round(base_size * sqrt(ar));

        for (int j = 0; j < num_scale; j++)
        {
            float scale = scales[j];

            float rs_w = r_w * scale;
            float rs_h = r_h * scale;

            float* anchor = anchors.row(i * num_scale + j);

            anchor[0] = cx - rs_w * 0.5f;
            anchor[1] = cy - rs_h * 0.5f;
            anchor[2] = cx + rs_w * 0.5f;
            anchor[3] = cy + rs_h * 0.5f;
        }
    }

    return anchors;
}

static void generate_proposals(const ncnn::Mat& anchors, int feat_stride, const ncnn::Mat& score_blob, const ncnn::Mat& bbox_blob, const ncnn::Mat& kps_blob, float prob_threshold, std::vector<FaceObject>& faceobjects)
{
    int w = score_blob.w;
    int h = score_blob.h;

    // generate face proposal from bbox deltas and shifted anchors
    const int num_anchors = anchors.h;

    for (int q = 0; q < num_anchors; q++)
    {
        const float* anchor = anchors.row(q);

        const ncnn::Mat score = score_blob.channel(q);
        const ncnn::Mat bbox = bbox_blob.channel_range(q * 4, 4);

        // shifted anchor
        float anchor_y = anchor[1];

        float anchor_w = anchor[2] - anchor[0];
        float anchor_h = anchor[3] - anchor[1];

        for (int i = 0; i < h; i++)
        {
            float anchor_x = anchor[0];

            for (int j = 0; j < w; j++)
            {
                int index = i * w + j;

                float prob = score[index];

                if (prob >= prob_threshold)
                {
                    // insightface/detection/scrfd/mmdet/models/dense_heads/scrfd_head.py _get_bboxes_single()
                    float dx = bbox.channel(0)[index] * feat_stride;
                    float dy = bbox.channel(1)[index] * feat_stride;
                    float dw = bbox.channel(2)[index] * feat_stride;
                    float dh = bbox.channel(3)[index] * feat_stride;

                    // insightface/detection/scrfd/mmdet/core/bbox/transforms.py distance2bbox()
                    float cx = anchor_x + anchor_w * 0.5f;
                    float cy = anchor_y + anchor_h * 0.5f;

                    float x0 = cx - dx;
                    float y0 = cy - dy;
                    float x1 = cx + dw;
                    float y1 = cy + dh;

                    FaceObject obj;
                    obj.rect.x = x0;
                    obj.rect.y = y0;
                    obj.rect.width = x1 - x0 + 1;
                    obj.rect.height = y1 - y0 + 1;
                    obj.prob = prob;

                    if (!kps_blob.empty())
                    {
                        const ncnn::Mat kps = kps_blob.channel_range(q * 10, 10);

                        obj.landmark[0].x = cx + kps.channel(0)[index] * feat_stride;
                        obj.landmark[0].y = cy + kps.channel(1)[index] * feat_stride;
                        obj.landmark[1].x = cx + kps.channel(2)[index] * feat_stride;
                        obj.landmark[1].y = cy + kps.channel(3)[index] * feat_stride;
                        obj.landmark[2].x = cx + kps.channel(4)[index] * feat_stride;
                        obj.landmark[2].y = cy + kps.channel(5)[index] * feat_stride;
                        obj.landmark[3].x = cx + kps.channel(6)[index] * feat_stride;
                        obj.landmark[3].y = cy + kps.channel(7)[index] * feat_stride;
                        obj.landmark[4].x = cx + kps.channel(8)[index] * feat_stride;
                        obj.landmark[4].y = cy + kps.channel(9)[index] * feat_stride;
                    }

                    faceobjects.push_back(obj);
                }

                anchor_x += feat_stride;
            }

            anchor_y += feat_stride;
        }
    }
}

int Scrfd::detect(JMat* pic,int* boxs){
    recal(pic->width(),pic->height());
    ncnn::Mat in = ncnn::Mat::from_pixels_resize(pic->udata(), ncnn::Mat::PIXEL_BGR2RGB, m_width, m_height, scale_w, scale_h);
    ncnn::Mat in_pad;
    ncnn::copy_make_border(in, in_pad, hpad / 2, hpad - hpad / 2, wpad / 2, wpad - wpad / 2, ncnn::BORDER_CONSTANT, 0.f);
    in_pad.substract_mean_normalize(mean_vals, norm_vals);
    int has_kps = 1;

    ncnn::Extractor ex = scrfd.create_extractor();
    ex.input("input.1", in_pad);

    std::vector<FaceObject> faceobjects;
    std::vector<FaceObject> faceproposals;
    //std::vector<FaceObject>& faceobjects, float prob_threshold, float nms_threshold)
    // stride 8
    {
        ncnn::Mat score_blob, bbox_blob, kps_blob;
        ex.extract("score_8", score_blob);
        ex.extract("bbox_8", bbox_blob);
        if (has_kps)
            ex.extract("kps_8", kps_blob);

        const int base_size = 16;
        const int feat_stride = 8;
        ncnn::Mat ratios(1);
        ratios[0] = 1.f;
        ncnn::Mat scales(2);
        scales[0] = 1.f;
        scales[1] = 2.f;
        ncnn::Mat anchors = generate_anchors(base_size, ratios, scales);

        std::vector<FaceObject> faceobjects32;
        generate_proposals(anchors, feat_stride, score_blob, bbox_blob, kps_blob, prob_threshold, faceobjects32);

        faceproposals.insert(faceproposals.end(), faceobjects32.begin(), faceobjects32.end());
    }

    // stride 16
    {
        ncnn::Mat score_blob, bbox_blob, kps_blob;
        ex.extract("score_16", score_blob);
        ex.extract("bbox_16", bbox_blob);
        if (has_kps)
            ex.extract("kps_16", kps_blob);

        const int base_size = 64;
        const int feat_stride = 16;
        ncnn::Mat ratios(1);
        ratios[0] = 1.f;
        ncnn::Mat scales(2);
        scales[0] = 1.f;
        scales[1] = 2.f;
        ncnn::Mat anchors = generate_anchors(base_size, ratios, scales);

        std::vector<FaceObject> faceobjects16;
        generate_proposals(anchors, feat_stride, score_blob, bbox_blob, kps_blob, prob_threshold, faceobjects16);

        faceproposals.insert(faceproposals.end(), faceobjects16.begin(), faceobjects16.end());
    }

    // stride 32
    {
        ncnn::Mat score_blob, bbox_blob, kps_blob;
        ex.extract("score_32", score_blob);
        ex.extract("bbox_32", bbox_blob);
        if (has_kps)
            ex.extract("kps_32", kps_blob);

        const int base_size = 256;
        const int feat_stride = 32;
        ncnn::Mat ratios(1);
        ratios[0] = 1.f;
        ncnn::Mat scales(2);
        scales[0] = 1.f;
        scales[1] = 2.f;
        ncnn::Mat anchors = generate_anchors(base_size, ratios, scales);

        std::vector<FaceObject> faceobjects8;
        generate_proposals(anchors, feat_stride, score_blob, bbox_blob, kps_blob, prob_threshold, faceobjects8);

        faceproposals.insert(faceproposals.end(), faceobjects8.begin(), faceobjects8.end());
    }

    // sort all proposals by score from highest to lowest
    qsort_descent_inplace(faceproposals);

    // apply nms with nms_threshold
    std::vector<int> picked;
    nms_sorted_bboxes(faceproposals, picked, nms_threshold);

    int face_count = picked.size();
    printf("====face_count %d\n",face_count);
    faceobjects.resize(face_count);
    for (int i = 0; i < face_count; i++)
    {
        faceobjects[i] = faceproposals[picked[i]];

        // adjust offset to original unpadded
        float x0 = (faceobjects[i].rect.x - (wpad / 2)) / scale;
        float y0 = (faceobjects[i].rect.y - (hpad / 2)) / scale;
        float x1 = (faceobjects[i].rect.x + faceobjects[i].rect.width - (wpad / 2)) / scale;
        float y1 = (faceobjects[i].rect.y + faceobjects[i].rect.height - (hpad / 2)) / scale;

        x0 = std::max(std::min(x0, (float)m_width - 1), 0.f);
        y0 = std::max(std::min(y0, (float)m_height - 1), 0.f);
        x1 = std::max(std::min(x1, (float)m_width - 1), 0.f);
        y1 = std::max(std::min(y1, (float)m_height - 1), 0.f);

        faceobjects[i].rect.x = x0;
        faceobjects[i].rect.y = y0;
        faceobjects[i].rect.width = x1 - x0;
        faceobjects[i].rect.height = y1 - y0;

        if (has_kps)
        {
            float x0 = (faceobjects[i].landmark[0].x - (wpad / 2)) / scale;
            float y0 = (faceobjects[i].landmark[0].y - (hpad / 2)) / scale;
            float x1 = (faceobjects[i].landmark[1].x - (wpad / 2)) / scale;
            float y1 = (faceobjects[i].landmark[1].y - (hpad / 2)) / scale;
            float x2 = (faceobjects[i].landmark[2].x - (wpad / 2)) / scale;
            float y2 = (faceobjects[i].landmark[2].y - (hpad / 2)) / scale;
            float x3 = (faceobjects[i].landmark[3].x - (wpad / 2)) / scale;
            float y3 = (faceobjects[i].landmark[3].y - (hpad / 2)) / scale;
            float x4 = (faceobjects[i].landmark[4].x - (wpad / 2)) / scale;
            float y4 = (faceobjects[i].landmark[4].y - (hpad / 2)) / scale;

            faceobjects[i].landmark[0].x = std::max(std::min(x0, (float)m_width - 1), 0.f);
            faceobjects[i].landmark[0].y = std::max(std::min(y0, (float)m_height - 1), 0.f);
            faceobjects[i].landmark[1].x = std::max(std::min(x1, (float)m_width - 1), 0.f);
            faceobjects[i].landmark[1].y = std::max(std::min(y1, (float)m_height - 1), 0.f);
            faceobjects[i].landmark[2].x = std::max(std::min(x2, (float)m_width - 1), 0.f);
            faceobjects[i].landmark[2].y = std::max(std::min(y2, (float)m_height - 1), 0.f);
            faceobjects[i].landmark[3].x = std::max(std::min(x3, (float)m_width - 1), 0.f);
            faceobjects[i].landmark[3].y = std::max(std::min(y3, (float)m_height - 1), 0.f);
            faceobjects[i].landmark[4].x = std::max(std::min(x4, (float)m_width - 1), 0.f);
            faceobjects[i].landmark[4].y = std::max(std::min(y4, (float)m_height - 1), 0.f);
        }
        break;
    }
    if(faceobjects.size()>0){
        FaceObject fo = faceobjects[0];
        cv::Rect_<float> rect = fo.rect;
        //std::cout<<fo.rect<<std::endl;
        int w = rect.width;
        int h = rect.height;
        int x1 = rect.x;
        int y1 = rect.y;
        int x2 = rect.x+w;
        int y2 = rect.y+h;
        printf("scrfd x1 %d y1 %d x2 %d y2 %d\n",x1,y1,x2,y2);
        int* tag = boxs;//pic->tagarr();
        int inx = 0;
        tag[inx++] = w;
        tag[inx++] = h;
        tag[inx++] = x1;
        tag[inx++] = y1;
        tag[inx++] = x2;
        tag[inx++] = y2;
        float adj = w*0.1f;
        x1 = x1 - adj;
        if(x1<0)x1=0;
        if(y1<0)y1=0;
        adj = (x2-x1)*0.1f;
        x2 = x2 + adj;
        if(x2>=m_width)x2=m_width-1;
        adj = (y2-y1)*0.1f;
        y2 = y2 + adj;
        if(y2>=m_height)y2=m_height-1;
        tag[inx++] = x1;
        tag[inx++] = y1;
        tag[inx++] = x2;
        tag[inx++] = y2;
        printf("adj x1 %d y1 %d x2 %d y2 %d\n",x1,y1,x2,y2);
    }
    //cv::Mat drawmat(m_height,m_width,CV_8UC3,pic->udata());
    //drawface(drawmat,faceobjects);
    //cv::imshow("aaa",drawmat);
    //cv::waitKey(0);
    return 0;
}

void Scrfd::recal(int nw ,int nh){
    if((nw==m_width)&&(nh==m_height))return;
    m_width = nw;
    m_height = nh;
    int w = m_width;
    int h = m_height;
    scale = 1.f;
    if (w > h) {
        scale = (float)target_size / w;
        w = target_size;
        h = h * scale;
    } else {
        scale = (float)target_size / h;
        h = target_size;
        w = w * scale;
    }
    scale_h = h;
    scale_w = w;
    wpad = (w + 31) / 32 * 32 - w;
    hpad = (h + 31) / 32 * 32 - h;
}

Scrfd::Scrfd(const char* modeldir,const char* modelid,int cols,int rows){
    scrfd.clear();
    ncnn::set_cpu_powersave(2);
    ncnn::set_omp_num_threads(ncnn::get_big_cpu_count());
    scrfd.opt = ncnn::Option();
    //scrfd.opt.use_vulkan_compute = true;
    scrfd.opt.num_threads = ncnn::get_big_cpu_count();
    char filepath[1024];
    sprintf(filepath,"%s/%s.param",modeldir,modelid);
    scrfd.load_param(filepath);
    sprintf(filepath,"%s/%s.bin",modeldir,modelid);
    scrfd.load_model(filepath);
    //scrfd.load_param("model/scrfd_500m_kps-opt2.param");
    //scrfd.load_model("model/scrfd_500m_kps-opt2.bin");
    //scrfd.load_param("model/scrfd.param");
    //scrfd.load_model("model/scrfd.bin");
    recal(cols,rows);
}

Scrfd::~Scrfd(){

}

#ifdef _SCRFD_MAIN_
int main(int argc,char** argv){
    Scrfd* scrfd = new Scrfd(1080,1920);
    std::string picfile("1.jpg");
    JMat* pic = new JMat(picfile,1);
    scrfd->detect(pic,pic->tagarr());
    printf("precess to exit\n");
    getchar();
    return 0;
}
#endif
