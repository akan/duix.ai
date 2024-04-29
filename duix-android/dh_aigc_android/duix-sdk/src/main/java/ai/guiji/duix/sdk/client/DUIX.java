package ai.guiji.duix.sdk.client;

import android.content.Context;
import android.os.Handler;
import android.os.HandlerThread;

import com.btows.ncnntest.SCRFDNcnn;

import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;

import ai.guiji.duix.sdk.client.bean.ModelInfo;
import ai.guiji.duix.sdk.client.render.RenderSink;
import ai.guiji.duix.sdk.client.thread.DUIXThread;
import ai.guiji.duix.sdk.client.util.Logger;

public class DUIX {

    private final SCRFDNcnn scrfdncnn;
    private final Callback mCallback;
    private final String baseDir;
    private final String modelDir;

    private ExecutorService commonExecutor = Executors.newSingleThreadExecutor();
    private final int sessionKey;
    private final DUIXThread duixRender;
    private final Thread renderThread;

    private final Handler mHandler;     // 处理心跳的异步线程

    private boolean isReady;            // 准备完成的标记

    public DUIX(Context context, String baseDir, String modelDir, RenderSink sink, Callback callback) {
        this.mCallback = callback;
        this.baseDir = baseDir;
        this.modelDir = modelDir;
        scrfdncnn = new SCRFDNcnn();

        HandlerThread handlerThread = new HandlerThread("DUIX");
        handlerThread.start();
        mHandler = new Handler(handlerThread.getLooper());

        sessionKey = (int) (System.currentTimeMillis() / 1000);
        scrfdncnn.createdigit(sessionKey, new SCRFDNcnn.Callback() {
            @Override
            public void onMessageCallback(int what, int arg1, long arg2, String msg1, String msg2, Object object) {
                Logger.d("onMessageCallback what" + what + " arg1: " + arg1 + " arg2: " + arg2 + " msg1: " + msg1 + " msg2: " + msg2);
            }
        });
        duixRender = new DUIXThread(context, scrfdncnn, sink, mCallback);
        renderThread = new Thread(duixRender);
        renderThread.setName("DUIXRender-Thread");
        renderThread.start();
    }

    public boolean isReady() {
        return isReady;
    }

    /**
     * 模型读取，需要异步操作
     */
    public void init() {
        if (commonExecutor != null) {
            commonExecutor.execute(this::loadModel);
        }
    }

    /**
     * 播放动作区间
     */
    public void motion() {
        if (isReady && duixRender != null) {
            duixRender.requireMotion();
        }
    }

    /**
     * 播放音频文件
     *
     * @param wavPath 音频文件路径或网络URL
     */
    public void playAudio(String wavPath) {
        if (isReady && duixRender != null) {
            duixRender.prepareAudio(wavPath);
        }
    }

    /**
     * 停止音频播放
     */
    public boolean stopAudio() {
        if (isReady && duixRender != null) {
            duixRender.stopAudio();
            return true;
        } else {
            return false;
        }
    }

    public void release() {
        isReady = false;
        if (mHandler != null) {
            mHandler.removeCallbacksAndMessages(null);
            mHandler.getLooper().quitSafely();
        }
        if (commonExecutor != null) {
            commonExecutor.shutdown();
            commonExecutor = null;
        }
        if (duixRender != null) {
            duixRender.stopPreview();
        }
        if (renderThread != null) {
            try {
                renderThread.join();
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
        }
        scrfdncnn.stop();
        scrfdncnn.reset();
        scrfdncnn.releasedigit(sessionKey);
    }

    private void loadModel() {
        ModelInfo info = ModelInfo.loadResource(scrfdncnn, baseDir, modelDir);
        if (info != null) {
            // 模型信息
            duixRender.startPreview(info);
            scrfdncnn.config(info.getNcnnConfig());
            scrfdncnn.start();
            isReady = true;
            mCallback.onEvent(Constant.CALLBACK_EVENT_INIT_READY, "init ok", null);
        } else {
            mCallback.onEvent(Constant.CALLBACK_EVENT_INIT_ERROR, "init error, load model file error!", null);
        }
    }
}
