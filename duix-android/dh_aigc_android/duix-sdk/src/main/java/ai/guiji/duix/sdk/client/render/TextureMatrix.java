package ai.guiji.duix.sdk.client.render;

public class TextureMatrix {


    public static final float[] DINGDING_TEXTURE_MATRIX = {
            1.0f, 0.0f, 0.0f, 0.0f,
            0.0f, -1.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 1.0f, 0.0f,
            0.0f, 1.0f, 0.0f, 1.0f
    };
    //    private static final float[] TEXTURE_MATRIX = {
//            0.0f, -1.0f, 0.0f, 0.0f,
//            1.0f, 0.0f, 0.0f, 0.0f,
//            0.0f, 0.0f, 1.0f, 0.0f,
//            0.0f, 1.0f, 0.0f, 1.0f
//    };
    //X 轴旋转 180 // 中心点需要在加一下//
    public static final float[] TEXTURE_MATRIX = {
            0.0f, 1.0f, 0.0f, 0.0f,
            -1.0f, 0.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 1.0f, 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f
    };
    //Y 轴旋转
     /*public static final float[] TEXTURE_MATRIX = {
             1.0f, 0.0f, 0.0f, 0.0f,
             0.0f, 0.0f, 1.0f, 0.0f,
             0.0f, 1.0f, 0.0f, 0.0f,
             0.0f, 0.0f, 0.0f, 1.0f
     };*/

    public static final float[] TEXTURE_MATRIX_0 = {
            1.0f, 0.0f, 0.0f, 0.0f,
            0.0f, 1.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 1.0f, 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f
    };
    // X轴旋转 90度
    public static final float[] TEXTURE_MATRIX_90 = {
            //X轴旋转 90度
//            0.0f, 1.0f, 0.0f, 0.0f,
//            1.0f, 0.0f, 0.0f, 0.0f,
//            0.0f, 0.0f, 1.0f, 0.0f,
//            0.0f, 0.0f, 0.0f, 1.0f

//            0.0f, -1.0f, 0.0f, 0.0f,
//            1.0f, 0.0f, 0.0f, 0.0f,
//            0.0f, 0.0f, 1.0f, 0.0f,
//            0.0f, 1.0f, 0.0f, 1.0f
            //90度水平翻转
            0.0f, -1.0f, 0.0f, 0.0f,
            1.0f, 0.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 1.0f, 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f

    };
    public static final float[] TEXTURE_MATRIX_90_V = {
            //X轴旋转 90度
            0.0f, 1.0f, 0.0f, 0.0f,
            1.0f, 0.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 1.0f, 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f

//            0.0f, -1.0f, 0.0f, 0.0f,
//            1.0f, 0.0f, 0.0f, 0.0f,
//            0.0f, 0.0f, 1.0f, 0.0f,
//            0.0f, 1.0f, 0.0f, 1.0f
            //90度水平翻转
//            0.0f, -1.0f, 0.0f, 0.0f,
//            1.0f, 0.0f, 0.0f, 0.0f,
//            0.0f, 0.0f, 1.0f, 0.0f,
//            0.0f, 0.0f, 0.0f, 1.0f

    };
    // X轴旋转 180度
    public static final float[] TEXTURE_MATRIX_180 = {
            -1.0f, 0.0f, 0.0f, 0.0f,
            0.0f, -1.0f, 0.0f, 0.0f,
            0.0f, 0.0f, -1.0f, 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f

//            -1.0f, 0.0f, 0.0f, 0.0f,
//            0.0f, 1.0f, 0.0f, 0.0f,
//            0.0f, 0.0f, -1.0f, 0.0f,
//            0.0f, 0.0f, 0.0f, 1.0f

//            1.0f, 0.0f, 0.0f, 0.0f,
//            0.0f, -1.0f, 0.0f, 0.0f,
//            0.0f, 0.0f, 1.0f, 0.0f,
//            0.0f, 1.0f, 0.0f, 1.0f

    };
    // X轴旋转 270度
    public static final float[] TEXTURE_MATRIX_270 = {
            0.0f, 1.0f, 0.0f, 0.0f,
            -1.0f, 0.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 1.0f, 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f
    };

    public static float[] getRotation(final int rotation, boolean flipHorizontal,
                                      boolean flipVertical) {
        float[] rotatedTex;
        boolean temp;
        switch (rotation) {
            case 90:
                rotatedTex = TEXTURE_MATRIX_90;
                temp = flipHorizontal;
                flipHorizontal = flipVertical;
                flipVertical = temp;
                break;
            case 180:
                rotatedTex = TEXTURE_MATRIX_180;
                break;
            case 270:
                rotatedTex = TEXTURE_MATRIX_270;
                temp = flipHorizontal;
                flipHorizontal = flipVertical;
                flipVertical = temp;
                break;
            case 0:
            case 360:
            default:
                rotatedTex = TEXTURE_MATRIX_0;
                break;
        }
        if (flipHorizontal) {
            rotatedTex = new float[]{
                    flip(rotatedTex[0]), rotatedTex[1],
                    flip(rotatedTex[2]), rotatedTex[3],
                    flip(rotatedTex[4]), rotatedTex[5],
                    flip(rotatedTex[6]), rotatedTex[7],
                    flip(rotatedTex[8]), rotatedTex[9],
                    flip(rotatedTex[10]), rotatedTex[11],
                    flip(rotatedTex[12]), rotatedTex[13],
                    flip(rotatedTex[14]), rotatedTex[15],
            };
        }
        if (flipVertical) {
            rotatedTex = new float[]{
                    rotatedTex[0], flip(rotatedTex[1]),
                    rotatedTex[2], flip(rotatedTex[3]),
                    rotatedTex[4], flip(rotatedTex[5]),
                    rotatedTex[6], flip(rotatedTex[7]),
                    rotatedTex[8], flip(rotatedTex[9]),
                    rotatedTex[10], flip(rotatedTex[11]),
                    rotatedTex[12], flip(rotatedTex[13]),
                    rotatedTex[14], flip(rotatedTex[15]),
            };
        }
        return rotatedTex;
    }

    private static float flip(final float i) {
        if (i == 0.0f) {
            return 1.0f;
        }
        return 0.0f;
    }
}
