package ai.guiji.duix.sdk.client.bean;


import java.nio.ByteBuffer;

public class ImageFrame {

    public ImageFrame(ByteBuffer rawBuffer, ByteBuffer maskBuffer, int width, int height) {
        this.rawBuffer = rawBuffer;
        this.maskBuffer = maskBuffer;
        this.width = width;
        this.height = height;
    }

    public int width;
    public int height;

    public ByteBuffer rawBuffer;
    public ByteBuffer maskBuffer;

}
