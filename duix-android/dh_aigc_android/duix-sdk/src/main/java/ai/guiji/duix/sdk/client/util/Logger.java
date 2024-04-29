package ai.guiji.duix.sdk.client.util;

import android.util.Log;

import ai.guiji.duix.sdk.client.BuildConfig;

public class Logger {

    private static boolean logFlag = BuildConfig.DEBUG;

    private static String TAG = "DUIX-SDK";

    public static void debugEnable(boolean enable) {
        logFlag = enable;
    }

    public static void e(String text) {
        e("", text);
    }

    public static void e(String tag, String text) {
        if (logFlag) {
            Log.e(TAG, tag + "~" + text);
        }
    }

    public static void i(String text) {
        i("", text);
    }

    public static void i(String tag, String text) {
        if (logFlag) {
            Log.i(TAG, tag + "~" + text);
        }
    }

    public static void d(String text) {
        d("", text);
    }

    public static void d(String tag, String text) {
        if (logFlag) {
            Log.d(TAG, tag + "~" + text);
        }
    }

    public static void w(String text) {
        w("", text);
    }

    public static void w(String tag, String text) {
        if (logFlag) {
            Log.w(TAG, tag + "~" + text);
        }
    }
}
