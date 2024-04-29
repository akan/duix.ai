package ai.guiji.duix.test.util;

import static java.lang.Character.UnicodeBlock.CJK_COMPATIBILITY_FORMS;
import static java.lang.Character.UnicodeBlock.CJK_COMPATIBILITY_IDEOGRAPHS;
import static java.lang.Character.UnicodeBlock.CJK_RADICALS_SUPPLEMENT;
import static java.lang.Character.UnicodeBlock.CJK_UNIFIED_IDEOGRAPHS;
import static java.lang.Character.UnicodeBlock.CJK_UNIFIED_IDEOGRAPHS_EXTENSION_A;
import static java.lang.Character.UnicodeBlock.CJK_UNIFIED_IDEOGRAPHS_EXTENSION_B;

import java.math.BigDecimal;
import java.net.URLEncoder;
import java.text.SimpleDateFormat;
import java.util.ArrayList;
import java.util.Date;
import java.util.List;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

public class StringUtils {

    /*
     * 秒数转时间
     * */
    public static String secondsToHMS(int seconds) {
        StringBuilder time = new StringBuilder();
        int min = 0;
        int hour = 0;
//        将毫秒转换成秒
//        seconds = seconds / 1000;
        if (seconds > 60) {
            min = seconds / 60;
            seconds = seconds % 60;
        }
        if (min > 60) {
            hour = min / 60;
            min = min % 60;
        }
        //拼接
        if (hour < 10)
            time.append("0");
        time.append(hour);
        time.append(":");
        if (min < 10)
            time.append("0");
        time.append(min);
        time.append(":");
        if (seconds < 10)
            time.append("0");
        time.append(seconds);
        return time.toString();
    }

    /*
     * 秒数转时间
     * */
    public static String secondsToMS(int seconds) {
        StringBuilder time = new StringBuilder();
        int min = 0;
//        将毫秒转换成秒
//        seconds = seconds / 1000;
        if (seconds > 60) {
            min = seconds / 60;
            seconds = seconds % 60;
        }
        /*if (min > 60) {
            min = min % 60;
        }*/
        if (min < 10)
            time.append("0");
        time.append(min);
        time.append(":");
        if (seconds < 10)
            time.append("0");
        time.append(seconds);
        return time.toString();
    }

    public static String formatDateTime(long mss) {
        String DateTimes;
        long days = mss / (60 * 60 * 24);
        long hours = (mss % (60 * 60 * 24)) / (60 * 60);
        long minutes = (mss % (60 * 60)) / 60;
        long seconds = mss % 60;
        if (days > 0) {
            DateTimes = days + "天" + hours + "时";
        } else if (hours > 0) {
            DateTimes = hours + "时" + minutes + "分钟";
        } else if (minutes > 0) {
            DateTimes = minutes + "分钟"
                    + seconds + "秒";
        } else {
            DateTimes = seconds + "秒";
        }

        return DateTimes;
    }

    //只展示秒，分钟
    public static String formatDateTime2(long mss) {
        long min = mss / 60;
        long second = mss % 60;
        String result = "";
        if (mss < 60) {
            result = mss + "秒";
        } else {
            if (0 == second) {
                result = min + "分钟";
            } else {
                result = min + "分钟" + second + "秒";
            }
        }
        return result;
    }

    //只展示秒或者分钟
    public static String formatDateTime3(long mss) {
        return mss < 60 ? (mss + "秒") : ((mss / 60) + "分钟");
    }

    //只展示秒，分
    public static String formatDateTime4(long mss) {
        return mss < 60 ? (mss + "秒") : ((mss / 60) + "分" + ((mss % 60) == 0 ? "" : (mss % 60) + "秒"));
    }

    public static String formatVideoDateTime(long mss) {
        String DateTimes;
        long hours = mss / (60 * 60);
        long minutes = (mss % (60 * 60)) / 60;
        long seconds = mss % 60;
        String hourStr = (hours < 10 ? "0" : "") + hours;
        String minuteStr = (minutes < 10 ? "0" : "") + minutes;
        String secondStr = (seconds < 10 ? "0" : "") + seconds;
        if (hours > 0) {
            DateTimes = hourStr + ":" + minuteStr + ":" + secondStr;
        } else {
            DateTimes = minuteStr + ":" + secondStr;
        }

        return DateTimes;
    }

    /**
     * 格式化文件大小
     *
     * @param size
     * @return
     */
    public static String formatSize(long size) {
        String sizeStr = "";
        String unit[] = {"byte", "kb", "mb", "g" };
        long lastValue = size;
        for (int i = 0; i < unit.length; i++) {
            size = size / 1024;
            if (0 == size) {
                sizeStr = lastValue + unit[i];
                break;
            } else {
                if (i == unit.length - 1) {
                    sizeStr = lastValue + unit[i];
                } else {
                    lastValue = size;
                }
            }
        }
        return sizeStr;
    }

    public static String createFileName(String prefix, String suffix) {
        Date dt = new Date(System.currentTimeMillis());
        SimpleDateFormat fmt = new SimpleDateFormat("yyyyMMddHHmmssSSS");
        String fileName = fmt.format(dt);
        fileName = prefix + fileName + suffix; //extension, you can change it.
        return fileName;
    }

    public static String dateToStringMS(long date) {
        SimpleDateFormat format = new SimpleDateFormat("yyyyMMddHHmmssSSS");
        Date dt = new Date(date);
        return format.format(dt);
    }

    public static String dateToStringMS2() {
        SimpleDateFormat format = new SimpleDateFormat("yyyy-MM");
        Date dt = new Date();
        return format.format(dt);
    }

    public static String dateToStringMS3() {
        SimpleDateFormat format = new SimpleDateFormat("MM-dd HH:mm");
        Date dt = new Date();
        return format.format(dt);
    }

    //去除数字结尾无用的0
    public static String filterUnUselessZero(String str) {
        String value = "";
        if (null != str) {
            String regEx = "\\.(0+)$";
            Pattern pattern = Pattern.compile(regEx);
            Matcher matcher = pattern.matcher(str);
            boolean rs = matcher.find();
            if (rs && null != matcher.group(0)) {
                value = str.replace(matcher.group(0), "");
            } else {
                value = str;
            }
        }
        return value;
    }

    //判断数字是否为0
    public static boolean isNullOrZero(String str) {
        boolean result = false;
        if (null == str || "".equals(str.trim())) {
            result = true;
        } else {
            try {
                double d = Double.parseDouble(str);
                result = d == 0;
            } catch (Exception e) {
            }
        }
        return result;
    }

    //判断是否为空
    public static boolean isEmpty(String str) {
        boolean result = false;
        if (null == str || "".equals(str.trim())) {
            result = true;
        }
        return result;
    }

    // encode
    public String urlEncoded(String paramString) {
        if (paramString == null || paramString == "") {
            return "";
        }
        try {
            String str = URLEncoder.encode(paramString, "UTF-8");
            return str;
        } catch (Exception e) {
            e.printStackTrace();
        }
        return "";
    }

    //判断大小
    public static Boolean isEnough(String v1, String v2) {
        Boolean result = null;
        try {
            Double d1 = Double.parseDouble(v1);
            Double d2 = Double.parseDouble(v2);
            result = d1 >= d2;
        } catch (Exception e) {
        }
        return result;
    }

    /**
     * 比较两个字符串不相等处，两个字符串去除标点之后，长度必须一致才会比对
     *
     * @param str1
     * @param str2
     * @return
     */
    public static List<Integer> compareTwoStr(String str1, String str2) {
        List<Integer> mark = new ArrayList<>();
        String str1WithoutSymbol = removeSymbol(str1);
        String str2WithoutSymbol = removeSymbol(str2);
        int symbolCount = 0;
        char[] str1Char = str1.toCharArray();
        char[] str2Char = str2WithoutSymbol.toCharArray();
        for (int i = 0; i < str1Char.length; i++) {
            if ("".equals(removeSymbol(String.valueOf(str1Char[i])))) {
                symbolCount++;
                continue;
            }
            if (str1Char[i] != str2Char[i - symbolCount]) {
                if (0 == mark.size() % 2) {
                    mark.add(i);
                }
            } else {
                if (0 != mark.size() % 2) {
                    mark.add(i);
                }
            }
            //提前结束
            if (i - symbolCount == str2Char.length - 1) {
                if (0 == mark.size() % 2 && i + 1 < str1Char.length) {
                    //存在非符号，才把后面标出来
                    if (!"".equals(removeSymbol(str1.substring(i + 1)))) {
                        mark.add(i + 1);
                    }
                }
                break;
            }
        }
        if (0 != mark.size() % 2) {
            mark.add(str1Char.length);
        }
        if (0 == mark.size() && str1WithoutSymbol.length() < str2WithoutSymbol.length()) {
            mark.add(0);
            mark.add(str1Char.length);
        }
        return mark;
    }

    /**
     * 去除字符串中的符号标点
     *
     * @param input
     * @return
     */
    public static String removeSymbol(String input) {
        return input.replaceAll("\\p{P}|\\p{S}", "");
    }

    public static double formatDouble(double value, int accuracy) {
        BigDecimal b = new BigDecimal(value);
        //保留2位小数
        double out = b.setScale(accuracy, BigDecimal.ROUND_HALF_UP).doubleValue();
        return out;
    }

    /**
     * 字符串中是否只包含汉字字母
     *
     * @return
     */
    public static boolean isCharOrLetter(String str) {
        String ruleString = "[\\u4e00-\\u9fa5a-zA-Z0-9]*";
        Pattern p = Pattern.compile(ruleString);
        char[] c = str.toCharArray();
        for (int i = 0; i < c.length; i++) {
            Matcher matcher = p.matcher("" + c[i]);
            boolean b = matcher.matches();
            if (b) {
                continue;
            } else {
                return false;
            }
        }
        return true;
    }

    /**
     * 判断是否含有中文
     *
     * @param checkChar
     * @return
     */
    public static boolean checkCharContainChinese(char checkChar) {
        Character.UnicodeBlock ub = Character.UnicodeBlock.of(checkChar);
        if (CJK_UNIFIED_IDEOGRAPHS == ub || CJK_COMPATIBILITY_IDEOGRAPHS == ub || CJK_COMPATIBILITY_FORMS == ub ||
                CJK_RADICALS_SUPPLEMENT == ub || CJK_UNIFIED_IDEOGRAPHS_EXTENSION_A == ub || CJK_UNIFIED_IDEOGRAPHS_EXTENSION_B == ub) {
            return true;
        }
        return false;
    }

    //格式化时间为年月日
    public static String formatDateString(String time) {
        SimpleDateFormat sdf = new SimpleDateFormat("yyyy-MM-dd");
        String formatString = "";
        try {
            Date date = sdf.parse(time);
            formatString = sdf.format(date);
        } catch (Exception e) {
            e.printStackTrace();
        }
        return formatString;
    }

    public static String subZeroAndDot(String s) {
        if (s.indexOf(".") > 0) {
            s = s.replaceAll("0+?$", "");//去掉多余的0
            s = s.replaceAll("[.]$", "");//如最后一位是.则去掉
        }
        return s;
    }
     // 是否包含特殊字符
    public static boolean isSpecialChar(String str) {
        String regEx = "[ _`~!@#$%^&*()+=|{}':;',\\[\\].<>/?~！@#￥%……&*（）——+|{}【】‘；：”“’。，、？]|\n|\r|\t";
        Pattern p = Pattern.compile(regEx);
        Matcher m = p.matcher(str);
        return m.find();
    }
}
