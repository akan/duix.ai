package ai.guiji.duix.test.service;

import android.content.Context;
import android.content.res.AssetManager;
import android.os.Environment;
import android.util.Log;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.OutputStream;
import java.util.concurrent.Executor;
import java.util.concurrent.Executors;

import ai.guiji.duix.sdk.client.util.MD5Util;
import ai.guiji.duix.test.net.SyncDownloadFile;
import ai.guiji.duix.test.util.StringUtils;
import ai.guiji.duix.test.util.ZipUtil;


public class StorageService {

    public interface Callback {

        void onDownloadProgress(int progress);

        void onUnzipProgress(int progress);

        void onComplete(String path);

        void onError(String msg);
    }

    /**
     * 下载zip文件并解压
     *
     * @param context
     * @param url
     * @param targetPath
     * @param uuid
     * @param callback
     */
    public static void downloadAndUnzip(Context context, String url, String targetPath, String uuid, Callback callback, boolean deleteZip) {
        Executor executor = Executors.newSingleThreadExecutor();
        executor.execute(() -> {
            File cacheDir = context.getExternalCacheDir();
            if (!cacheDir.exists()) {
                cacheDir.mkdirs();
            }
            File zipFile = new File(cacheDir, MD5Util.string2MD5(url));
            boolean result = true;
            if (!zipFile.exists()) {
                Log.d("123", "zip not found, try download.");
                result = new SyncDownloadFile(url, zipFile.getAbsolutePath(), callback::onDownloadProgress).download();
                Log.d("123", "download file success.");
            } else {
                Log.d("123", "found cache zip file.");
            }
            if (result) {
                Log.e("123", "try unzip file.");
                File targetDirFile = new File(targetPath);
                if (targetDirFile.exists()) {
                    Log.e("123", "delete old files.");
                    deleteContents(targetDirFile);
                }
                // 拿到目标路径的父级
                File targetParentDir = new File(targetPath).getParentFile();
                if (!targetParentDir.exists()) {
                    targetParentDir.mkdirs();
                }
                result = ZipUtil.unzip(zipFile.getAbsolutePath(), targetParentDir.getAbsolutePath(), callback::onUnzipProgress);
                if (result) {
                    Log.d("123", "unzip file complete.");
                    // 这里时候targetDirFile应该是存在的
                    if (targetDirFile.exists()) {
                        File uuidFile = new File(targetDirFile, "uuid");
                        try {
                            OutputStream out = new FileOutputStream(uuidFile);
                            byte[] uuidBytes = uuid.getBytes();
                            out.write(uuidBytes, 0, uuidBytes.length);
                            out.flush();
                            out.close();
                            if (deleteZip && zipFile.exists()){
                                zipFile.delete();
                            }
                            callback.onComplete(targetPath);
                        } catch (Exception e) {
                            callback.onError("touch uuid file error!");
                        }
                    } else {
                        callback.onError("unzip dir not found!");
                    }
                } else {
                    callback.onError("unzip file error!");
                }
            } else {
                callback.onError("zip file download error");
            }
        });
    }

    /**
     * 从assets拷贝文件到sdcard
     *
     * @param context
     * @param sourcePath 在assets目录的路径
     * @param targetPath 在sd卡的路径
     */
    public static void unpack(Context context, String sourcePath, final String targetPath, final Callback callback) {
        Executor executor = Executors.newSingleThreadExecutor();
        executor.execute(() -> {
            try {
                final String outputPath = sync(context, sourcePath, targetPath);
                callback.onComplete(outputPath);
            } catch (final IOException e) {
                callback.onError("拷贝文件异常: " + e);
            }
        });
    }

    public static boolean deleteContents(File dir) {
        File[] files = dir.listFiles();
        boolean success = true;
        if (files != null) {
            for (File file : files) {
                if (file.isDirectory()) {
                    success &= deleteContents(file);
                }
                if (!file.delete()) {
                    success = false;
                }
            }
        }
        return success;
    }

    private static String sync(Context context, String sourcePath, String targetPath) throws IOException {

        AssetManager assetManager = context.getAssets();

        File externalFilesDir = context.getExternalFilesDir(null);
        if (externalFilesDir == null) {
            throw new IOException("cannot get external files dir, "
                    + "external storage state is " + Environment.getExternalStorageState());
        }

        File targetDir = new File(externalFilesDir, targetPath);
        String resultPath = new File(targetDir, sourcePath).getAbsolutePath();
        String sourceUUID = readLine(assetManager.open(sourcePath + "/uuid"));
        try {
            String targetUUID = readLine(new FileInputStream(new File(targetDir, sourcePath + "/uuid")));
            if (targetUUID.equals(sourceUUID)) return resultPath;
        } catch (FileNotFoundException e) {
            // ignore
        }
        deleteContents(targetDir);

        copyAssets(assetManager, sourcePath, targetDir);

        // Copy uuid
        copyFile(assetManager, sourcePath + "/uuid", targetDir);

        return resultPath;
    }

    private static String readLine(InputStream is) throws IOException {
        return new BufferedReader(new InputStreamReader(is)).readLine();
    }

    private static void copyAssets(AssetManager assetManager, String path, File outPath) throws IOException {
        String[] assets = assetManager.list(path);
        if (assets == null) {
            return;
        }
        if (assets.length == 0) {
            if (!path.endsWith("uuid"))
                copyFile(assetManager, path, outPath);
        } else {
            File dir = new File(outPath, path);
            if (!dir.exists()) {
                Log.d("123", "Making directory " + dir.getAbsolutePath());
                if (!dir.mkdirs()) {
                    Log.d("123", "Failed to create directory " + dir.getAbsolutePath());
                }
            }
            for (String asset : assets) {
                copyAssets(assetManager, path + "/" + asset, outPath);
            }
        }
    }

    private static void copyFile(AssetManager assetManager, String fileName, File outPath) throws IOException {
        InputStream in;

        Log.d("123", "Copy " + fileName + " to " + outPath);
        in = assetManager.open(fileName);
        OutputStream out = new FileOutputStream(outPath + "/" + fileName);

        byte[] buffer = new byte[4000];
        int read;
        while ((read = in.read(buffer)) != -1) {
            out.write(buffer, 0, read);
        }
        in.close();
        out.close();
    }

}
