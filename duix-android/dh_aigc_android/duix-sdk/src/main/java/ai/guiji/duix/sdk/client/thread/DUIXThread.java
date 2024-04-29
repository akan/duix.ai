package ai.guiji.duix.sdk.client.thread;

import android.content.Context;
import android.os.Handler;
import android.os.Looper;
import android.os.Message;
import android.text.TextUtils;
import android.util.Log;

import androidx.annotation.NonNull;

import com.btows.ncnntest.SCRFDNcnn;
import com.google.android.exoplayer2.ExoPlaybackException;
import com.google.android.exoplayer2.MediaItem;
import com.google.android.exoplayer2.Player;
import com.google.android.exoplayer2.SimpleExoPlayer;
import com.google.android.exoplayer2.source.MediaSource;
import com.google.android.exoplayer2.source.ProgressiveMediaSource;
import com.google.android.exoplayer2.trackselection.AdaptiveTrackSelection;
import com.google.android.exoplayer2.trackselection.DefaultTrackSelector;
import com.google.android.exoplayer2.trackselection.ExoTrackSelection;
import com.google.android.exoplayer2.trackselection.TrackSelector;
import com.google.android.exoplayer2.upstream.BandwidthMeter;
import com.google.android.exoplayer2.upstream.DefaultBandwidthMeter;
import com.google.android.exoplayer2.upstream.DefaultDataSourceFactory;

import java.io.BufferedOutputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.InputStream;
import java.io.OutputStream;
import java.lang.ref.WeakReference;
import java.nio.ByteBuffer;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collections;
import java.util.List;
import java.util.Random;
import java.util.concurrent.ConcurrentLinkedQueue;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;

import ai.guiji.duix.sdk.client.Callback;
import ai.guiji.duix.sdk.client.Constant;
import ai.guiji.duix.sdk.client.bean.ImageFrame;
import ai.guiji.duix.sdk.client.bean.ModelInfo;
import ai.guiji.duix.sdk.client.render.RenderSink;
import ai.guiji.duix.sdk.client.util.Logger;
import ai.guiji.duix.sdk.client.util.MD5Util;

/**
 * DUIX绘制线程，负责绘制线程及音频播放控制
 */
public class DUIXThread implements Runnable {

    private static final int MSG_START_RENDER = 0;          // 启动渲染
    private static final int MSG_RENDER_STEP = 1;           // 请求下一帧渲染
    private static final int MSG_STOP_RENDER = 2;           // 停止渲染
    private static final int MSG_QUIT = 3;                  // 退出线程
    private static final int MSG_PREPARE_AUDIO = 4;         // 准备播放音频
    private static final int MSG_STOP_AUDIO = 5;            // 停止播放音频
    private static final int MSG_PLAY_AUDIO = 6;            // 音频的下载及ncnn计算完毕，准备播放
    private static final int MSG_REQUIRE_MOTION = 7;        // 请求播放动作区间

    private boolean isRendering = false;                     // 为false时终止线程
    RenderHandler mHandler;                                 // 使用该处理器来调度线程的事件

    private final Object mReadyFence = new Object();        // 给isReady加一个对象锁
    private boolean isReady;                                // handler等组件都创建完毕的标记

    private final Context mContext;
    private final SCRFDNcnn scrfdncnn;
    private final Callback callback;

    private ExecutorService commonExecutor;

    private ConcurrentLinkedQueue<ModelInfo.Frame> mPreviewQueue;       // 播放帧
    private SimpleExoPlayer mExoPlayer;                     // 音频播放器
    private int mTotalBnf = 0;                              // 播放音频的帧数
    private boolean requireMotion = false;                  // 请求播放动作

    private ModelInfo mModelInfo;                           // 模型的全部信息都放在这里面
    private ByteBuffer rawBuffer;
    private ByteBuffer maskBuffer;

    private final RenderSink renderSink;

    public DUIXThread(Context context, SCRFDNcnn scrfdncnn, RenderSink renderSink, Callback callback) {
        this.mContext = context;
        this.scrfdncnn = scrfdncnn;
        this.callback = callback;
        this.renderSink = renderSink;
    }

    @Override
    public void run() {
        Looper.prepare();
        mHandler = new RenderHandler(this);
        mPreviewQueue = new ConcurrentLinkedQueue<>();

        commonExecutor = Executors.newSingleThreadExecutor();
        BandwidthMeter bandwidthMeter = new DefaultBandwidthMeter.Builder(mContext).build();
        ExoTrackSelection.Factory videoTrackSelectionFactory = new AdaptiveTrackSelection.Factory();
        TrackSelector trackSelector = new DefaultTrackSelector(mContext, videoTrackSelectionFactory);
        mExoPlayer = new SimpleExoPlayer.Builder(mContext)
                .setTrackSelector(trackSelector)
                .setBandwidthMeter(bandwidthMeter)
                .build();
        mExoPlayer.setPlayWhenReady(true);
        mExoPlayer.setRepeatMode(Player.REPEAT_MODE_OFF);
        mExoPlayer.addListener(new Player.Listener() {
            @Override
            public void onPlaybackStateChanged(int state) {
                Player.Listener.super.onPlaybackStateChanged(state);
//                Log.e("123", "onPlaybackStateChanged:" + state);
                if (state == Player.STATE_READY) {
                    callback.onEvent(Constant.CALLBACK_EVENT_AUDIO_PLAY_START, "play start", null);
                } else if (state == Player.STATE_ENDED) {
                    callback.onEvent(Constant.CALLBACK_EVENT_AUDIO_PLAY_END, "play end", null);
                }
            }

            @Override
            public void onPlayerError(@NonNull ExoPlaybackException error) {
                Player.Listener.super.onPlayerError(error);
                Log.e("123", "onPlayerError:" + error);
                callback.onEvent(Constant.CALLBACK_EVENT_AUDIO_PLAY_ERROR, "play error" + error.getMessage(), null);
            }
        });
        synchronized (mReadyFence) {
            isReady = true;
            mReadyFence.notify();
        }
        Looper.loop();
        synchronized (mReadyFence) {
            isReady = false;
            mHandler = null;
        }
    }

    public void startPreview(ModelInfo modelInfo) {
        synchronized (mReadyFence) {
            if (!isReady) {
                try {
                    mReadyFence.wait();
                } catch (InterruptedException e) {
                    e.printStackTrace();
                }
            }
            if (mHandler != null) {
                Message msg = new Message();
                msg.what = MSG_START_RENDER;
                msg.obj = modelInfo;
                mHandler.sendMessage(msg);
            }
        }
    }

    public void stopPreview() {
        if (mHandler != null) {
            mHandler.sendEmptyMessage(MSG_STOP_RENDER);
        }
    }

    public void prepareAudio(String wavPath) {
        if (mHandler != null) {
            Message msg = new Message();
            msg.what = MSG_PREPARE_AUDIO;
            msg.obj = wavPath;
            mHandler.sendMessage(msg);
        }
    }

    public void stopAudio() {
        if (mHandler != null) {
            mHandler.sendEmptyMessage(MSG_STOP_AUDIO);
        }
    }

    public void requireMotion() {
        if (mHandler != null) {
            mHandler.sendEmptyMessage(MSG_REQUIRE_MOTION);
        }
    }

    private void handleStartRender(ModelInfo modelInfo) {
        isRendering = true;
        mModelInfo = modelInfo;
        Logger.d("分辨率: " + mModelInfo.getWidth() + "x" + mModelInfo.getHeight());
        rawBuffer = ByteBuffer.allocate(mModelInfo.getWidth() * mModelInfo.getHeight() * 3);
        maskBuffer = ByteBuffer.allocate(mModelInfo.getWidth() * mModelInfo.getHeight() * 3);
        if (!mModelInfo.isHasMask()) {
            // 用纯白填充mask
            Arrays.fill(maskBuffer.array(), (byte) 255);
        }
        mHandler.sendEmptyMessage(MSG_RENDER_STEP);
    }

    private void handleAudioStep() {
        if (isRendering) {
            long startTime = System.currentTimeMillis();
            renderStep();
            long endTime = System.currentTimeMillis();
            long delay = 40 - (endTime - startTime);
            if (delay < 0) {
                Logger.w("渲染耗时过高: " + (endTime - startTime) + "(>40ms)");
                delay = 0;
            }
            if (mHandler != null) {
                mHandler.sendMessageDelayed(mHandler.obtainMessage(MSG_RENDER_STEP), delay);
            }
        } else {
            if (commonExecutor != null) {
                commonExecutor.shutdown();
            }
            if (mPreviewQueue != null) {
                mPreviewQueue.clear();
            }
            if (mExoPlayer != null) {
                if (mExoPlayer.isPlaying()) {
                    mExoPlayer.stop();
                }
                mExoPlayer.release();
            }
            if (mHandler != null) {
                mHandler.sendEmptyMessage(MSG_QUIT);
            }
        }
    }

    private void renderStep() {
        ModelInfo.Frame frame;
        if (requireMotion) {
            // 收到动作的通知
            requireMotion = false;
            if (mModelInfo.getMotionRegions().size() > 0) {
                int index = new Random().nextInt(mModelInfo.getMotionRegions().size());
                mPreviewQueue.clear();
                ModelInfo.Region motionRegion = mModelInfo.getMotionRegions().get(index);
                Logger.d("发现想要动作区间index: " + index + " region: " + motionRegion);
                mPreviewQueue.addAll(motionRegion.frames);
            }
        }
        if (mPreviewQueue.isEmpty()) {
            // 先假设把静默的都加进来
            List<ModelInfo.Region> silenceRegions = mModelInfo.getSilenceRegions();
            mPreviewQueue.addAll(silenceRegions.get(0).frames);
            List<ModelInfo.Frame> copiedList = new ArrayList<>(silenceRegions.get(0).frames);
            // 反向的也加进来
            Collections.reverse(copiedList);
            mPreviewQueue.addAll(copiedList);
        }
        frame = mPreviewQueue.poll();
        if (frame != null) {
            int audioBnf = -1;
            if (mExoPlayer != null && mExoPlayer.isPlaying()) {
                float progress = mExoPlayer.getCurrentPosition() * 1.0F / mExoPlayer.getDuration();
                callback.onEvent(Constant.CALLBACK_EVENT_AUDIO_PLAY_PROGRESS, "audio play progress", progress);
                float curr = mTotalBnf * progress;
                audioBnf = (int) curr;
            }
            if (audioBnf > -1 && audioBnf <= mTotalBnf) {
                if (mModelInfo.isHasMask()) {
                    int rst = scrfdncnn.mskrstbuf(frame.rawPath, frame.maskPath, frame.rect, audioBnf, frame.sgPath, rawBuffer.array(), maskBuffer.array(), mModelInfo.getWidth() * mModelInfo.getHeight() * 3);
                    //                Log.e("123", "mskrstbuf rst: " + rst);
                } else {
                    int rst = scrfdncnn.onerstbuf(frame.rawPath, frame.rect, audioBnf, rawBuffer.array(), mModelInfo.getWidth() * mModelInfo.getHeight() * 3);
                }
            } else {
                if (mModelInfo.isHasMask()) {
                    int rst = scrfdncnn.drawmskbuf(frame.sgPath, frame.maskPath, rawBuffer.array(), maskBuffer.array(), mModelInfo.getWidth() * mModelInfo.getHeight() * 3);
//                Log.e("123", "drawmskbuf rst: " + rst);
                } else {
                    int rst = scrfdncnn.drawonebuf(frame.rawPath, rawBuffer.array(), mModelInfo.getWidth() * mModelInfo.getHeight() * 3);
                }
            }
            if (renderSink != null) {
                renderSink.onVideoFrame(new ImageFrame(rawBuffer, maskBuffer, mModelInfo.getWidth(), mModelInfo.getHeight()));
            }
        }
    }

    private void handleStopRender() {
        if (isRendering) {
            isRendering = false;
        } else {
            mHandler.sendEmptyMessage(MSG_QUIT);
        }
    }

    private void handlePrepareAudio(String wavPath) {
        if (commonExecutor != null) {
            commonExecutor.execute(() -> {
                if (!TextUtils.isEmpty(wavPath)) {
                    loadAudio(wavPath);
                } else {
                    callback.onEvent(Constant.CALLBACK_EVENT_AUDIO_PLAY_ERROR, "音频路径不能为空!", null);
                }
            });
        }
    }

    private void handleStopAudio() {
        if (mExoPlayer != null) {
            if (mExoPlayer.isPlaying()) {
                mExoPlayer.stop();
            }
        }
    }

    private void handlePlayAudio(int all_bnf, String path) {
        mTotalBnf = all_bnf;
        Logger.d("收到所有嘴型信息 size: " + all_bnf);
        if (mExoPlayer != null) {
            if (mExoPlayer.isPlaying()) {
                mExoPlayer.stop();
            }
            MediaSource videoSource = new ProgressiveMediaSource.Factory(new DefaultDataSourceFactory(mContext))
                    .createMediaSource(new MediaItem.Builder().setUri(path).build());
            mExoPlayer.setMediaSource(videoSource, true);
            mExoPlayer.setPlayWhenReady(true);
            mExoPlayer.setRepeatMode(Player.REPEAT_MODE_OFF);
            mExoPlayer.setVolume(1f);
            mExoPlayer.prepare();
        }
    }

    private void loadAudio(String path) {
        if (isRendering) {
            int all_bnf = scrfdncnn.onewav(path, "");
            Message msg = new Message();
            msg.what = MSG_PLAY_AUDIO;
            msg.arg1 = all_bnf;
            msg.obj = path;
            if (mHandler != null) {
                mHandler.sendMessage(msg);
            }
        }
    }

    private void handleRequireMotion() {
        requireMotion = true;
    }

    static class RenderHandler extends Handler {

        private final WeakReference<DUIXThread> encoderWeakReference;

        public RenderHandler(DUIXThread render) {
            encoderWeakReference = new WeakReference<>(render);
        }

        @Override
        public void handleMessage(Message msg) {
            int what = msg.what;
            DUIXThread render = encoderWeakReference.get();
            if (render == null) {
                return;
            }
            switch (what) {
                case MSG_START_RENDER:
                    render.handleStartRender((ModelInfo) msg.obj);
                    break;
                case MSG_RENDER_STEP:
                    render.handleAudioStep();
                    break;
                case MSG_STOP_RENDER:
                    render.handleStopRender();
                    break;
                case MSG_PREPARE_AUDIO:
                    render.handlePrepareAudio((String) msg.obj);
                    break;
                case MSG_STOP_AUDIO:
                    render.handleStopAudio();
                    break;
                case MSG_PLAY_AUDIO:
                    render.handlePlayAudio(msg.arg1, (String) msg.obj);
                    break;
                case MSG_REQUIRE_MOTION:
                    render.handleRequireMotion();
                    break;
                case MSG_QUIT:
                    Logger.i("duix thread quit!");
                    Looper myLooper = Looper.myLooper();
                    if (myLooper != null) {
                        myLooper.quit();
                    }
                    break;
            }
        }

    }
}
