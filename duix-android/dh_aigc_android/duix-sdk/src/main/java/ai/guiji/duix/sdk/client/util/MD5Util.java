package ai.guiji.duix.sdk.client.util;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.InputStream;
import java.security.MessageDigest;
import java.security.NoSuchAlgorithmException;

/**
 * 采用MD5加密解密
 *
 * @author tfq
 * @datetime 2011-10-13
 */
public class MD5Util {
    private static final char hexDigits[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
            'a', 'b', 'c', 'd', 'e', 'f'};

    public final static String md5Encode(final String s) {
        try {
            final byte[] strTemp = s.getBytes();
            final MessageDigest mdTemp = MessageDigest.getInstance("MD5");
            mdTemp.update(strTemp);
            final byte[] md = mdTemp.digest();
            final int j = md.length;
            final char str[] = new char[j * 2];
            int k = 0;
            for (int i = 0; i < j; i++) {
                final byte byte0 = md[i];
                str[k++] = hexDigits[byte0 >>> 4 & 0xf];
                str[k++] = hexDigits[byte0 & 0xf];
            }
            return new String(str);
        } catch (Exception e) {
            return null;
        }
    }

    public final static String md5Encode(final byte[] md) {
        try {
            final int j = md.length;
            final char str[] = new char[j * 2];
            int k = 0;
            for (int i = 0; i < j; i++) {
                final byte byte0 = md[i];
                str[k++] = hexDigits[byte0 >>> 4 & 0xf];
                str[k++] = hexDigits[byte0 & 0xf];
            }
            return new String(str);
        } catch (Exception e) {
            return null;
        }
    }

    private static String toHexString(final byte[] b) {
        final StringBuilder sb = new StringBuilder(b.length * 2);
        for (int i = 0; i < b.length; i++) {
            final byte byte0 = b[i];
            sb.append(hexDigits[byte0 >>> 4 & 0xf]);
            sb.append(hexDigits[byte0 & 0x0f]);
        }
        return sb.toString();
    }

    /**
     * 求一个文件的MD5签名
     *
     * @return
     */
    public final static String md5EncodeFile(final File file) {
        try {
            return md5EncodeFile(new FileInputStream(file));
        } catch (FileNotFoundException e) {
            e.printStackTrace();
            return null;
        }
    }

    public final static String md5EncodeFile(InputStream inputStream) {
        final byte[] buffer = new byte[1024];
        int bytes;
        MessageDigest md5;
        try {
            final InputStream is = inputStream;
            md5 = MessageDigest.getInstance("MD5");
            while ((bytes = is.read(buffer)) > 0) {
                md5.update(buffer, 0, bytes);
            }
            is.close();
            return toHexString(md5.digest());
        } catch (Exception e) {
            return null;
        }
    }

    public final static String md5EncodeFile(final String fileName) {
        try {
            return md5EncodeFile(new FileInputStream(fileName));
        } catch (FileNotFoundException e) {
            e.printStackTrace();
            return null;
        }
    }


    public final static boolean verifyMD5File(File file, String md5) {
        String md5File = md5EncodeFile(file);
        if (md5File != null && md5 != null) {
            return md5File.toLowerCase().trim().equals(md5.toLowerCase().trim());
        }
        return false;
    }


    /**
     * MD5加码 生成32位md5码
     */
    public static byte[] MD5(String val) {
        MessageDigest md5 = null;
        try {
            md5 = MessageDigest.getInstance("MD5");
            md5.update(val.getBytes());
            byte[] m = md5.digest();//加密  MessageDigest md5 = null;
            return m;
        } catch (NoSuchAlgorithmException e) {
            e.printStackTrace();
            return null;
        }
    }

    public static String string2MD5(String inStr) {
        MessageDigest md5 = null;
        try {
            md5 = MessageDigest.getInstance("MD5");
        } catch (Exception e) {
            System.out.println(e.toString());
            e.printStackTrace();
            return "";
        }
        char[] charArray = inStr.toCharArray();
        byte[] byteArray = new byte[charArray.length];

        for (int i = 0; i < charArray.length; i++)
            byteArray[i] = (byte) charArray[i];
        byte[] md5Bytes = md5.digest(byteArray);
        StringBuffer hexValue = new StringBuffer();
        for (int i = 0; i < md5Bytes.length; i++) {
            int val = ((int) md5Bytes[i]) & 0xff;
            if (val < 16)
                hexValue.append("0");
            hexValue.append(Integer.toHexString(val));
        }
        return hexValue.toString();

    }

    /**
     * 加密解密算法 执行一次加密，两次解密
     */
    public static String convertMD5(String inStr) {

        char[] a = inStr.toCharArray();
        for (int i = 0; i < a.length; i++) {
            a[i] = (char) (a[i] ^ 't');
        }
        String s = new String(a);
        return s;

    }

}
