package ai.guiji.duix.test.net;

import android.util.Log;

import java.io.File;
import java.io.FileOutputStream;
import java.io.InputStream;

import ai.guiji.duix.test.App;
import okhttp3.Call;
import okhttp3.Request;
import okhttp3.Response;
import okhttp3.ResponseBody;

/**
 * 同步下载小文件,不要再UI线程里面直接调用
 */
public class SyncDownloadFile {

    private String url;
    private String path;
    private Callback callback;

    public SyncDownloadFile(String url, String path, Callback callback) {
        this.url = url;
        this.path = path;
        this.callback = callback;
    }

    public boolean download() {
        try {
            Request request = new Request.Builder()
                    .url(url)
                    .build();
            Call call = App.getOkHttpClient().newCall(request);

            Response response = call.execute();
            if (response.code() == 200) {
                ResponseBody body = response.body();
                if (body != null) {
                    long contentLength = body.contentLength();
                    InputStream is = body.byteStream();
                    File tmpFile = new File(path + ".tmp");
                    File parent = tmpFile.getParentFile();
                    if (parent != null && !parent.exists()) {
                        if (!parent.mkdirs()) {
                            return false;
                        }
                    }
                    if (tmpFile.exists()) {
                        tmpFile.delete();
                    }
                    FileOutputStream fileOutputStream = new FileOutputStream(tmpFile);
                    long downloadLength = 0;
                    int len;
                    byte[] data = new byte[1024];
                    while ((len = is.read(data)) != -1) {
                        fileOutputStream.write(data, 0, len);
                        downloadLength += len;
                        if (callback != null){
                            callback.onProgress((int) (downloadLength * 100 / contentLength));
                        }
                    }
                    fileOutputStream.flush();
                    is.close();
                    fileOutputStream.close();
                    File target = new File(path);
                    if (tmpFile.renameTo(target)) {
                        return true;
                    }
                }
            }
        } catch (Exception e) {
            Log.e("123", "SyncDownloadFile error: " + e.getMessage());
        }
        return false;
    }

    public interface Callback {
        void onProgress(int progress);
    }

}
