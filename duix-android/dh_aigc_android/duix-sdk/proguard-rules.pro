# Add project specific ProGuard rules here.
# You can control the set of applied configuration files using the
# proguardFiles setting in build.gradle.
#
# For more details, see
#   http://developer.android.com/guide/developing/tools/proguard.html

# If your project uses WebView with JS, uncomment the following
# and specify the fully qualified class name to the JavaScript interface
# class:
#-keepclassmembers class fqcn.of.javascript.interface.for.webview {
#   public *;
#}

# Uncomment this to preserve the line number information for
# debugging stack traces.
#-keepattributes SourceFile,LineNumberTable

# If you keep the line number information, uncomment this to
# hide the original source file name.
#-renamesourcefileattribute SourceFile

-optimizationpasses 5  #指定代码的压缩级别 0 - 7，一般都是5，无需改变
-dontusemixedcaseclassnames #不使用大小写混合
#告诉Proguard 不要跳过对非公开类的处理，默认是跳过
-dontskipnonpubliclibraryclasses #如果应用程序引入的有jar包，并且混淆jar包里面的class
-verbose #混淆时记录日志（混淆后生产映射文件 map 类名 -> 转化后类名的映射
#指定混淆时的算法，后面的参数是一个过滤器
#这个过滤器是谷歌推荐的算法，一般也不会改变
-optimizations !code/simplification/arithmetic,!field/*,!class/merging/*
#类型转换错误 添加如下代码以便过滤泛型（不写可能会出现类型转换错误，一般情况把这个加上就是了）,即避免泛型被混淆
-keepattributes Signature
#假如项目中有用到注解，应加入这行配置,对JSON实体映射也很重要,eg:fastjson
-keepattributes *Annotation*
#抛出异常时保留代码行数
-keepattributes SourceFile,LineNumberTable
#保持 native 的方法不去混淆
-keepclasseswithmembernames class * {
    native <methods>;
}

#保持指定规则的方法不被混淆（Android layout 布局文件中为控件配置的onClick方法不能混淆）
-keepclassmembers class * extends android.app.Activity {
    public void *(android.view.View);
}
#保持自定义控件指定规则的方法不被混淆
-keep public class * extends android.view.View {
    public <init>(android.content.Context);
    public <init>(android.content.Context, android.util.AttributeSet);
    public <init>(android.content.Context, android.util.AttributeSet, int);
    public void set*(...);
}
#保持枚举 enum 不被混淆
-keepclassmembers enum * {
    public static **[] values();
    public static ** valueOf(java.lang.String);
}
#保持 Parcelable 不被混淆（aidl文件不能去混淆）
-keep class * implements android.os.Parcelable {
    public static final android.os.Parcelable$Creator *;
}
#需要序列化和反序列化的类不能被混淆（注：Java反射用到的类也不能被混淆）
-keepnames class * implements java.io.Serializable
#保护实现接口Serializable的类中，指定规则的类成员不被混淆
-keepclassmembers class * implements java.io.Serializable {
    static final long serialVersionUID;
    private static final java.io.ObjectStreamField[] serialPersistentFields;
    !static !transient <fields>;
    private void writeObject(java.io.ObjectOutputStream);
    private void readObject(java.io.ObjectInputStream);
    java.lang.Object writeReplace();
    java.lang.Object readResolve();
}
#保持R文件不被混淆，否则，你的反射是获取不到资源id的
-keep class **.R$* { *; }

-keepclassmembers class * {
   public <init> (org.json.JSONObject);
}

-keepclassmembers enum * {
    public static **[] values();
    public static ** valueOf(java.lang.String);
}


#以下针对App本身设置


-keep class com.btows.ncnntest.**{*; }

-keep class ai.guiji.duix.sdk.client.render.** {*;}
-keep class ai.guiji.duix.sdk.client.render.**$* {*;}
-keep class ai.guiji.duix.sdk.client.bean.** {*;}
-keep class ai.guiji.duix.sdk.client.DUIX{*; }
-keep class ai.guiji.duix.sdk.client.DUIX$* {*;}
-keep class ai.guiji.duix.sdk.client.Constant{*; }
-keep class ai.guiji.duix.sdk.client.Constant* {*;}
-keep class ai.guiji.duix.sdk.client.DUIXOptions{*; }
-keep class ai.guiji.duix.sdk.client.DUIXOptions* {*;}
-keep class ai.guiji.duix.sdk.client.Callback{*; }
-keep class ai.guiji.duix.sdk.client.Callback* {*;}
-keep class ai.guiji.duix.sdk.client.render.DUIXTextureView{*; }
-keep class ai.guiji.duix.sdk.client.render.DUIXTextureView$* {*;}