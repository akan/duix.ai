package ai.guiji.duix.test.util;

import android.util.Log;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.util.Enumeration;
import java.util.zip.ZipEntry;
import java.util.zip.ZipFile;
import java.util.zip.ZipInputStream;

public class ZipUtil {

    //解压缩文件
    // 这里unzip需要使用canonicalPath做校验，但是该方法取的路径可能和getAbsolutePath()方式获取的不一致
    // getCanonicalPath()   /data/data/.......
    // getAbsolutePath      /data/user/0/......
    public static boolean unzip(String zipFilePath, String outOutPath, Callback callback) {
        try (FileInputStream fis = new FileInputStream(zipFilePath)) {
            long total = 0;
            if (callback != null){
                total = getZipSize(zipFilePath);
            }
            ZipInputStream inZip = new ZipInputStream(fis);
            long currentSize = 0;
            ZipEntry zipEntry;
            String szName = "";
            while ((zipEntry = inZip.getNextEntry()) != null) {
                szName = zipEntry.getName();
                if (zipEntry.isDirectory()) {
                    szName = szName.substring(0, szName.length() - 1);
                    File folder = new File(outOutPath + File.separator + szName);
                    String canonicalPath = folder.getCanonicalPath();
                    if (!canonicalPath.startsWith(outOutPath)) {
                        Log.e("123", "绝对值路径比较异常忽略该地址: " + folder.getAbsolutePath());
                    } else {
                        if (!folder.exists()) {
                            if (!folder.mkdirs()) {
                                return false;
                            }
                        }
                    }
                } else {
                    File file = new File(outOutPath + File.separator + szName);
                    String canonicalPath = file.getCanonicalPath();
                    if (!canonicalPath.startsWith(outOutPath)) {
                        Log.e("123", "绝对值路径比较异常忽略该地址: " + file.getAbsolutePath());
                    } else {
                        if (!file.exists()) {
                            if (!file.getParentFile().exists()) {
                                file.getParentFile().mkdirs();
                            }
                            if (!file.createNewFile()) {
                                return false;
                            }
                            FileOutputStream out = new FileOutputStream(file);
                            int len;
                            byte[] buffer = new byte[2048];
                            while ((len = inZip.read(buffer)) != -1) {
                                out.write(buffer, 0, len);
                                out.flush();
                                if (callback != null) {
                                    currentSize += len;
                                    int progress = (int)(currentSize * 100.0f / total);
                                    callback.onProgress(progress); // 通过回调函数更新进度
                                }
                            }
                            out.close();
                        }
                    }
                }
            }
            inZip.close();
            return true;
        } catch (Exception e) {
            e.printStackTrace();
        }
        return false;
    }

    private static long getZipSize(String filePath){
        long size = 0;
        ZipFile f;
        try {
            f = new ZipFile(filePath);
            Enumeration<? extends ZipEntry> en = f.entries();
            while (en.hasMoreElements()) {
                size += en.nextElement().getSize();
            }
        } catch (IOException e) {
            size = 0;
        }
        return size;
    }

    public interface Callback {
        void onProgress(int progress);
    }
}
