package ai.guiji.duix.sdk.client.bean;

import android.text.TextUtils;
import android.util.Log;

import androidx.annotation.NonNull;

import com.btows.ncnntest.SCRFDNcnn;

import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;

import java.io.File;
import java.io.Serializable;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Comparator;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.Objects;
import java.util.Set;

import ai.guiji.duix.sdk.client.util.FileUtil;
import ai.guiji.duix.sdk.client.util.Logger;

/**
 * 模型信息
 */
public class ModelInfo implements Serializable {

    private static final Map<String, String> baseMD5Map = new HashMap<>();
    private static final Map<String, String> modelMD5Map = new HashMap<>();

    static {
        baseMD5Map.put("alpha_model.b", "ab");
        baseMD5Map.put("alpha_model.p", "ap");
        baseMD5Map.put("cacert.p", "cp");
        baseMD5Map.put("weight_168u.b", "wb");
        baseMD5Map.put("wenet.o", "wo");

        modelMD5Map.put("dh_model.b", "db");
        modelMD5Map.put("dh_model.p", "dp");
        modelMD5Map.put("bbox.j", "bj");
        modelMD5Map.put("config.j", "cj");
        modelMD5Map.put("weight_168u.b", "wb");     // 不一定有
    }

    private boolean hasMask = true;            // 是否有Mask配置
    private int width = 540;            // 数字人宽
    private int height = 960;           // 数字人高

    private List<Region> silenceRegions = new ArrayList<>();
    private List<Region> motionRegions = new ArrayList<>();


    private String ncnnConfig;      // 提交给NCNN的配置文件


    public String getNcnnConfig() {
        return ncnnConfig;
    }

    public List<Region> getSilenceRegions() {
        return silenceRegions;
    }

    public List<Region> getMotionRegions() {
        return motionRegions;
    }

    public int getWidth() {
        return width;
    }

    public int getHeight() {
        return height;
    }

    public static ModelInfo loadResource(SCRFDNcnn scrfdncnn, String baseDir, String modelDir) {
        long startTime = System.currentTimeMillis();
        ModelInfo info = new ModelInfo();
        // md5检查
        Logger.i( "开始MD5文件检查...");
        Set<String> baseKeySet = baseMD5Map.keySet();
        for (String key : baseKeySet) {
            String value = baseMD5Map.get(key);
            if (value != null) {
                File targetFile = new File(baseDir, value);
                if (!targetFile.exists()) {
                    Logger.i("base需要的MD5文件不存在，尝试转换: " + targetFile.getAbsolutePath());
                    File srcFile = new File(baseDir, key);
                    if (!srcFile.exists()) {
                        Logger.e("base无法转换模型文件: " + srcFile.getAbsolutePath());
                        return null;
                    } else {
                        File tmpFile = new File(baseDir, value + ".tmp");
                        int rst = scrfdncnn.processmd5(0, srcFile.getAbsolutePath(), tmpFile.getAbsolutePath());
                        Logger.d( "base转换结果: " + rst);
                        if (!tmpFile.renameTo(targetFile)) {
                            Logger.e("base模型文件重命名失败: " + tmpFile.getAbsolutePath());
                        }
                    }
                } else {
                    Logger.d("base发现需要的模型文件: " + value);
                }
            }
        }

        Set<String> modelKeySet = modelMD5Map.keySet();
        for (String key : modelKeySet) {
            String value = modelMD5Map.get(key);
            if (value != null) {
                File targetFile = new File(modelDir, value);
                if (!targetFile.exists()) {
                    Logger.i("model需要的MD5文件不存在，尝试转换: " + targetFile.getAbsolutePath());
                    File srcFile = new File(modelDir, key);
                    if (!srcFile.exists()) {
                        if (!"weight_168u.b".equals(key)){
                            Logger.e("model无法转换模型文件: " + srcFile.getAbsolutePath());
                            return null;
                        } else {
                            Logger.d("model文件夹中未发现[" + key + "]，使用base中的配置");
                        }
                    } else {
                        File tmpFile = new File(modelDir, value + ".tmp");
                        int rst = scrfdncnn.processmd5(0, srcFile.getAbsolutePath(), tmpFile.getAbsolutePath());
                        Logger.d("model转换结果: " + rst);
                        if (!tmpFile.renameTo(targetFile)) {
                            Logger.e("model模型文件重命名失败: " + tmpFile.getAbsolutePath());
                        }
                    }
                } else {
                    Logger.d("model发现需要的模型文件: " + value);
                }
            }
        }

        File rawDir = new File(modelDir, "raw_jpgs");
        File sgDir = new File(modelDir, "raw_sg");
        File maskDir = new File(modelDir, "pha");

        // 以raw文件夹的数据为准
        try {
            JSONObject bBoxJson = new JSONObject(FileUtil.readFile(new File(modelDir, Objects.requireNonNull(modelMD5Map.get("bbox.j"))).getAbsolutePath()));
            Logger.d("load bBoxJson: " + bBoxJson);

            JSONObject config = new JSONObject(FileUtil.readFile(new File(modelDir, Objects.requireNonNull(modelMD5Map.get("config.j"))).getAbsolutePath()));
            Logger.d("load config: " + config);
            // 0是有绿幕的，1是没有绿幕的
            info.hasMask = config.optInt("need_png", 0) == 0 && sgDir.exists() && maskDir.exists();        // 这里判断都否有Mask，兼容之前的，默认有

            List<Frame> totalFrames = new ArrayList<>();
            File[] rawList = rawDir.listFiles();
            if (rawList != null) {
                for (File f : rawList) {
                    Frame frame = new Frame();
                    frame.index = Integer.parseInt(f.getName().toLowerCase().split("\\.")[0]);
                    frame.rawPath = f.getAbsolutePath();
                    File maskFile = new File(maskDir, f.getName());
                    if (info.hasMask && maskFile.exists()) {
                        frame.maskPath = maskFile.getAbsolutePath();
                    }
                    File sgFile = new File(sgDir, f.getName());
                    if (info.hasMask && sgFile.exists()) {
                        frame.sgPath = sgFile.getAbsolutePath();
                    }
                    JSONArray rectArray = bBoxJson.optJSONArray(String.valueOf(frame.index));
                    if (rectArray != null) {
                        frame.rect = new int[]{rectArray.optInt(0), rectArray.optInt(2), rectArray.optInt(1), rectArray.optInt(3)};
                    }
                    if (frame.check()) {
                        totalFrames.add(frame);
                    } else {
                        Logger.e(">>> find unusual frame: " + frame);
                    }
                }
            }

            if (totalFrames.size() == 0) {
                return null;
            }

            totalFrames.sort(Comparator.comparingInt(i -> i.index));
            // 拿到了所有的帧，我们根据配置文件来处理渲染尺寸和帧区间

            info.width = config.optInt("width", 540);
            info.height = config.optInt("height", 960);

            info.silenceRegions.clear();
            info.motionRegions.clear();

            JSONArray rangesArray = config.optJSONArray("ranges");
            if (rangesArray != null) {
                for (int i = 0; i < rangesArray.length(); i++) {
                    JSONObject range = rangesArray.getJSONObject(i);
                    int min = range.optInt("min");
                    int max = range.optInt("max");
                    int type = range.optInt("type", 0);

                    if (min >= 0 && max < totalFrames.get(totalFrames.size() - 1).index) {
                        Region region = new Region(type);
                        for (Frame item : totalFrames) {
                            if (item.index >= min && item.index <= max) {
                                region.frames.add(item);
                            }
                        }
                        if (region.frames.size() > 0) {
                            if (region.type == Region.TYPE_MOTION) {
                                info.motionRegions.add(region);
                            } else {
                                info.silenceRegions.add(region);
                            }
                        }
                    }
                }
            }
            Logger.i("发现静默区间个数: " + info.silenceRegions.size());
            Logger.i("发现动作区间个数: " + info.motionRegions.size());
            if (info.silenceRegions.size() == 0) {
                // 没有找到静默，所有的帧都作为静默
                Region region = new Region(Region.TYPE_SILENCE);
                region.frames.addAll(totalFrames);
                info.silenceRegions.add(region);
                Logger.w(">>> 未发现静默区间配置，将所有帧作为静默区间: " + info.silenceRegions.size());
            }

            JSONObject ncnnConfig = new JSONObject();
            ncnnConfig.put("action", 1);
            ncnnConfig.put("videowidth", info.width);
            ncnnConfig.put("videoheight", info.height);
            ncnnConfig.put("timeoutms", 5000);

            ncnnConfig.put("wenetfn", baseDir + File.separator + "wo");

            File modelMsk = new File(modelDir + File.separator + "wb");
            if (modelMsk.exists()){
                ncnnConfig.put("unetmsk", modelDir + File.separator + "wb");
                Logger.d("使用模型自带的weight");
            } else {
                ncnnConfig.put("unetmsk", baseDir + File.separator + "wb");
            }
            ncnnConfig.put("cacertfn", baseDir + File.separator + "cp");
            ncnnConfig.put("alphabin", baseDir + File.separator + "ab");
            ncnnConfig.put("alphaparam", baseDir + File.separator + "ap");

            ncnnConfig.put("unetbin", modelDir + File.separator + "db");
            ncnnConfig.put("unetparam", modelDir + File.separator + "dp");

            info.ncnnConfig = ncnnConfig.toString();
        } catch (JSONException e) {
            return null;
        }
        long endTime = System.currentTimeMillis();
        Logger.d("读取模型耗时: " + (endTime - startTime) + "ms");
        return info;
    }

    public static class Region implements Serializable {

        public static final int TYPE_SILENCE = 0;
        public static final int TYPE_MOTION = 1;       //motion

        public int type;
        public List<Frame> frames = new ArrayList<>();

        public Region(int type) {
            this.type = type;
        }

        @Override
        public String toString() {
            return "Region{" +
                    "type=" + type +
                    ", frames=" + frames.size() +
                    '}';
        }
    }

    /**
     * 帧信息,包含原图、绿幕、梦版图及人脸位置
     */
    public static class Frame implements Serializable {

        public int index;
        public String rawPath;
        public String maskPath;
        public String sgPath;
        public int[] rect;

        public boolean check() {
            return !TextUtils.isEmpty(rawPath) && rect != null;
        }

        @NonNull
        @Override
        public String toString() {
            return "Frame{" +
                    "index=" + index +
                    ", rawPath='" + rawPath + '\'' +
                    ", rect=" + Arrays.toString(rect) +
                    '}';
        }
    }

    public boolean isHasMask() {
        return hasMask;
    }
}
