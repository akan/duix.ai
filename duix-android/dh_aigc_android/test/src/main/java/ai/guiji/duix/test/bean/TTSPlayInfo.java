package ai.guiji.duix.test.bean;


import android.util.Log;

import org.json.JSONObject;

import java.io.Serializable;

/**
 * TTS播放信息
 */
public class TTSPlayInfo implements Serializable {


    public long nonce = (long) (Math.random() * Long.MAX_VALUE);

    public String answer;           // 可能为空的返回
    public String filePath;         // wav文件的URL地址
    public long duration;           // 音频时长

    public boolean isEnd;           // 是否为最后一个回复


    public static TTSPlayInfo parseJson(JSONObject data) {
        TTSPlayInfo ttsPlayInfo = new TTSPlayInfo();
        try {
            ttsPlayInfo.isEnd = data.getBoolean("isEnd");
            ttsPlayInfo.answer = data.getString("answer");
            ttsPlayInfo.filePath = data.getString("filePath");
            ttsPlayInfo.duration = data.optLong("duration", 0);
        } catch (Exception e) {
            Log.e("123", "TTSPlayInfo parseJson error: " + e);
        }
        return ttsPlayInfo;
    }

    @Override
    public String toString() {
        return "TTSPlayInfo{" +
                "answer='" + answer + '\'' +
                ", filePath='" + filePath + '\'' +
                ", duration=" + duration +
                ", isEnd=" + isEnd +
                '}';
    }
}
