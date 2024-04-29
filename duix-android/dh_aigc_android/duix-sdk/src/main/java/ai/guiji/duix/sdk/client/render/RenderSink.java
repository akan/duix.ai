package ai.guiji.duix.sdk.client.render;

import ai.guiji.duix.sdk.client.bean.ImageFrame;

/**
 * 渲染管道，通过该接口返回渲染数据
 */
public interface RenderSink {

    void onVideoFrame(ImageFrame imageFrame);

}
