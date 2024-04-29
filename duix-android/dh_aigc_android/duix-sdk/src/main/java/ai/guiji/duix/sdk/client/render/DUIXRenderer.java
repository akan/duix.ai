package ai.guiji.duix.sdk.client.render;

import android.content.Context;
import android.opengl.GLES20;
import android.util.Log;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

import ai.guiji.duix.sdk.client.bean.ImageFrame;
import ai.guiji.duix.sdk.client.util.OpenGLUtil;

public final class DUIXRenderer implements DUIXTextureView.Renderer, RenderSink{

    private static final String TAG = "DUIXRenderer";

    DUIXTextureView glTextureView;
    private Context mContext;

    private int mViewWidth = 0;
    private int mViewHeight = 0;
    private int mVideoWidth = 0;
    private int mVideoHeight = 0;

    protected float[] mMvpMatrix = new float[16];           // 缩放的矩阵

    private ImageFrame pendingFrame;

    private ImageDrawer mImageDrawer;

    private static final int SCALE_TYPE_CROP = 0;
    private static final int SCALE_TYPE_INSIDE = 1;
    private int scaleType = SCALE_TYPE_CROP;

    public DUIXRenderer(Context context, DUIXTextureView glTextureView) {
        mContext = context;
        this.glTextureView = glTextureView;
    }

    @Override
    public void onVideoFrame(ImageFrame imageFrame) {
        pendingFrame = imageFrame;
        if (mVideoWidth != imageFrame.width || mVideoHeight != imageFrame.height) {
            mVideoWidth = imageFrame.width;
            mVideoHeight = imageFrame.height;
            tryChangeScale();
        }
        glTextureView.requestRender();
    }

    public void setScaleType(int scaleType) {
        this.scaleType = scaleType;
    }

    @Override
    public void onSurfaceCreated(GL10 gl, EGLConfig config) {
        Log.e(TAG, "onSurfaceCreated");
        mImageDrawer = new ImageDrawer();
    }

    @Override
    public void onSurfaceChanged(GL10 gl, int width, int height) {
        Log.e(TAG, "onSurfaceChanged size: " + " width: " + width + " height: " + height);
        GLES20.glViewport(0, 0, width, height);
        mViewWidth = width;
        mViewHeight = height;
        tryChangeScale();
    }

    private void tryChangeScale() {
        if (mViewWidth > 0 && mViewHeight > 0 && mVideoWidth > 0 && mVideoHeight > 0) {
            if (scaleType == SCALE_TYPE_CROP){
                mMvpMatrix = OpenGLUtil.changeMvpMatrixCrop(mViewWidth, mViewHeight, mVideoWidth, mVideoHeight);
            } else {
                mMvpMatrix = OpenGLUtil.changeMvpMatrixInside(mViewWidth, mViewHeight, mVideoWidth, mVideoHeight);
            }
        }
    }

    @Override
    public void onDrawFrame(GL10 gl) {
//        Log.e(TAG, "onDrawFrame");
        GLES20.glClear(GLES20.GL_COLOR_BUFFER_BIT | GLES20.GL_DEPTH_BUFFER_BIT);
        GLES20.glClearColor(0, 0, 0, 0);
        // 融合，不然透明会有问题
        GLES20.glEnable(GLES20.GL_BLEND);
        // 半透明会有白色边框
//        GLES20.glBlendFunc(GLES20.GL_ONE, GLES20.GL_ZERO);
        // 使用glBlendFuncSeparate为RGB和alpha通道分别设置不同的选项：照常设置了RGB分量，但让最终的alpha分量只被源颜色向量的alpha值影响。
        GLES20.glBlendFuncSeparate(GLES20.GL_SRC_ALPHA, GLES20.GL_ONE_MINUS_SRC_ALPHA, GLES20.GL_ONE, GLES20.GL_ZERO);

        if (pendingFrame != null) {
            mImageDrawer.draw(pendingFrame, mMvpMatrix);
        }
    }

    @Override
    public void onSurfaceDestroyed(GL10 gl) {
        Log.e(TAG, "onSurfaceDestroyed");
        if (mImageDrawer != null) {
            mImageDrawer.release();
        }
    }


    public void release() {

    }

}
