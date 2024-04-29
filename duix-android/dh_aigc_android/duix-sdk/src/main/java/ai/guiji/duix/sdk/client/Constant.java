package ai.guiji.duix.sdk.client;

public class Constant {

    public static final int VERSION_CODE = BuildConfig.VERSION_CODE;
    public static final String VERSION_NAME = BuildConfig.VERSION_NAME;


    // DUIX的回调事件
    public static final String CALLBACK_EVENT_INIT_READY = "init.ready";
    public static final String CALLBACK_EVENT_INIT_ERROR = "init.error";
    public static final String CALLBACK_EVENT_AUDIO_PLAY_START = "play.start";
    public static final String CALLBACK_EVENT_AUDIO_PLAY_END = "play.end";
    public static final String CALLBACK_EVENT_AUDIO_PLAY_ERROR = "play.error";
    public static final String CALLBACK_EVENT_AUDIO_PLAY_PROGRESS = "play.progress";

}
