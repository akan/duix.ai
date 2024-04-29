package ai.guiji.duix.sdk.client.util;

import android.annotation.SuppressLint;
import android.content.Context;
import android.content.SharedPreferences;
import android.content.pm.PackageInfo;
import android.content.pm.PackageManager;
import android.net.ConnectivityManager;
import android.net.NetworkInfo;
import android.provider.Settings;
import android.text.TextUtils;
import android.util.DisplayMetrics;
import android.util.Log;
import android.view.Display;
import android.view.WindowManager;

import java.util.UUID;

public class DeviceUtils {

    public static String getVersionName(Context context) {
        try {
            PackageInfo pi = context.getPackageManager().getPackageInfo(context.getPackageName(), 0);
            return pi.versionName;
        } catch (PackageManager.NameNotFoundException e) {
            e.printStackTrace();
            return "Unknown";
        }
    }

    public static int getVersionCode(Context context) {
        try {
            PackageInfo pi = context.getPackageManager().getPackageInfo(context.getPackageName(), 0);
            return pi.versionCode;
        } catch (PackageManager.NameNotFoundException e) {
            e.printStackTrace();
            return 0;
        }
    }

    @SuppressLint("MissingPermission")
    public static boolean isWifi(Context mContext) {
        ConnectivityManager connectivityManager = (ConnectivityManager) mContext
                .getSystemService(Context.CONNECTIVITY_SERVICE);
        NetworkInfo info = null;
        if (connectivityManager != null) {
            info = connectivityManager.getActiveNetworkInfo();
        }
        return info != null && info.getType() == ConnectivityManager.TYPE_WIFI;
    }

    private static String getAndroidID(Context context) {
        String id = Settings.System.getString(context.getContentResolver(), Settings.Secure.ANDROID_ID);
        return id;
    }

    private static String getDeviceUUid(Context context) {
        String androidId = DeviceUtils.getAndroidID(context);
        if (TextUtils.isEmpty(androidId)) {
            return null;
        }
        UUID deviceUuid = new UUID(androidId.hashCode(), ((long) androidId.hashCode() << 32));
        return deviceUuid.toString();
    }

    private static String getAppUUid(Context context) {
        SharedPreferences preferences = context.getSharedPreferences("device", Context.MODE_PRIVATE);
        String uuid = preferences.getString("UUID", null);
        if (TextUtils.isEmpty(uuid)) {
            uuid = UUID.randomUUID().toString();
            preferences.edit().putString("UUID", uuid).apply();
        }
        return uuid;
    }

    public static String getUUID(Context context) {
        Log.d("DeviceUtils", "getUUID: ");
        String uuid = getDeviceUUid(context);
        if (TextUtils.isEmpty(uuid)) {
            uuid = getAppUUid(context);
        }
        return uuid;
    }

    public static boolean isNetworkConnected(Context context) {
        if (context != null) {
            ConnectivityManager mConnectivityManager = (ConnectivityManager) context
                    .getSystemService(Context.CONNECTIVITY_SERVICE);
            @SuppressLint("MissingPermission") NetworkInfo mNetworkInfo = mConnectivityManager.getActiveNetworkInfo();
            if (mNetworkInfo != null) {
                return mNetworkInfo.isAvailable();
            }
        }
        return false;
    }

    /**
     * 获取屏幕信息
     *
     * @param context
     * @return
     */
    public static ScreenInfo getScreenInfo(Context context) {
        return new ScreenInfo(context);
    }

    public static class ScreenInfo {

        public final float screenWidthDp;//  dp单位
        public final float screenHeightDp;//  dp单位
        public final int screenWidthPx; //屏幕宽 px
        public final int screenHeightPx; //屏幕高 px
        public final float uiScale;//  density
        public final float fontScale;//  scaledDensity

        public ScreenInfo(Context context) {
            WindowManager windowManager = (WindowManager) context.getSystemService(Context.WINDOW_SERVICE);
            Display defaultDisplay = windowManager.getDefaultDisplay();
            DisplayMetrics displayMetrics = new DisplayMetrics();
            defaultDisplay.getMetrics(displayMetrics);
            screenWidthPx = displayMetrics.widthPixels;
            screenHeightPx = displayMetrics.heightPixels;
            uiScale = context.getResources().getDisplayMetrics().density;
            fontScale = context.getResources().getDisplayMetrics().scaledDensity;
            screenWidthDp = px2dp(screenWidthPx);
            screenHeightDp = px2dp(screenHeightPx);
        }

        public int dp2px(float dpValue) {
            return (int) (dpValue * uiScale + 0.5f);
        }

        public int px2dp(float pxValue) {
            return (int) (pxValue / uiScale + 0.5f);
        }

        public int sp2px(float spValue) {
            return (int) (spValue * fontScale + 0.5f);
        }

        public int px2sp(float pxValue) {
            return (int) (pxValue / fontScale + 0.5f);
        }


    }


}
