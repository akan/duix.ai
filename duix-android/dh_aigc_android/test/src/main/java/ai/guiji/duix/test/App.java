package ai.guiji.duix.test;

import android.app.Application;
import android.text.TextUtils;

import java.util.concurrent.TimeUnit;

import okhttp3.OkHttpClient;

public class App extends Application {

    public static App mApp;
    private static OkHttpClient mOkHttpClient;

    @Override
    public void onCreate() {
        super.onCreate();
        mApp = this;
    }

    public static OkHttpClient getOkHttpClient() {
        if (mOkHttpClient == null) {
            mOkHttpClient = new OkHttpClient.Builder()
                    .connectTimeout(15, TimeUnit.SECONDS)
                    .writeTimeout(15, TimeUnit.SECONDS)
                    .readTimeout(15, TimeUnit.SECONDS)
                    .build();
        }
        return mOkHttpClient;
    }

    public static String addBaseUrl(String url, String baseUrl){
        String u = url;
        if (!TextUtils.isEmpty(u) && !u.startsWith("http")){
            u = baseUrl + u;
        }
        return u;
    }
}
