# DUIX-SDK
2D虚拟数字人SDK,可以通过语音完成对虚拟人实时驱动。

## 一、产品介绍

2D 数字人虚拟人SDK ,可以通过语音完成对虚拟人实时驱动。

### 1. 适用场景

部署成本低: 无需客户提供技术团队进行配合,支持低成本快速部署在多种终端及大屏;
网络依赖小:可落地在地铁、银行、政务等多种场景的虚拟助理自助服务上;
功能多样化:可根据客户需求满足视频、媒体、客服、金融、广电等多个行业的多样化需求

### 2. 核心功能

提供定制形象的 AI 主播,智能客服等多场景形象租赁,支持客户快速部署和低成本运营;
专属形象定制:支持定制专属的虚拟助理形象,可选低成本或深度形象生成;
播报内容定制:支持定制专属的播报内容,应用在培训、播报等多种场景上;
实时互动问答:支持实时对话,也可定制专属问答库,可满足咨询查询、语音闲聊、虚拟陪伴、垂类场景的客服问答等需求。

## 二、SDK集成

### 1. 支持的系统和硬件版本

| 项目     | 描述                                                               |
|--------|------------------------------------------------------------------|
| 系统     | 支持 Android 7.0+ ( API Level 24 )到 Android 13 ( API Level 33 )系统。 |
| CPU架构  | armeabi-v7a, arm64-v8a                                           |
| 硬件要求   | 要求设备 CPU4 核极以上,内存 4G 及以上。可用存储空间 500MB 及以上。                       |
| 网络     | 支持 WIF 及移动网络。如果使用云端问答库,设备带宽(用于数字人的实际带宽)期望 10mbps 及以上。            |
| 开发 IDE | Android Studio Giraffe \mid 2022.3.1 Patch 2                     |
| 内存要求   | 可用于数字人的内存 >= 400MB                                               |

##两个人物模型（切换男女形象，修改./duix-android/dh_aigc_android/test/src/main/java/ai/guiji/duix/test/ui/activity/MainActivity.kt的modelUrl变量即可）
```
  女：https://cdn.guiji.ai/duix/digital/model/1712034391673/bendi1_0329.zip
  男：https://digital-public.obs.cn-east-3.myhuaweicloud.com/duix/digital/model/1706009711636/liangwei_540s.zip 
```

### 2. SDK集成

引入 sdk aar 包: duix_client_sdk_release_${version}.aar
app 目录新建 libs 目录,放入 aar 包,在 build.gradle 中增加配置如下

```gradle
dependencies {
    // 使用阿里ASR需要将nuisdk-release.aar放到libs目录下(必选)
    implementation fileTree(include: ['*.jar', '*.aar'], dir: 'libs')
    // sdk 中使用到 exoplayer 处理音频(必选)
    implementation 'com.google.android.exoplayer:exoplayer:2.14.2'
    
    // 云端问答接口使用的SSE组件(非必选)
    implementation 'com.squareup.okhttp3:okhttp-sse:4.10.0'

    ...
}
```

权限要求, AndroidManifest.xml中,增加如下配置

```xml

<manifest xmlns:android="http://schemas.android.com/apk/res/android">
    <uses-permission android:name="android.permission.INTERNET" />
    <uses-permission android:name="android.permission.CHANGE_WIFI_STATE" />
    <uses-permission android:name="android.permission.ACCESS_NETWORK_STATE" />
    <uses-permission android:name="android.permission.ACCESS_WIFI_STATE" />
    <uses-permission android:name="android.permission.MODIFY_AUDIO_SETTINGS" />
    <uses-permission android:name="android.permission.WAKE_LOCK" />

</manifest>
```

## 三、SDK调用及API说明

### 1. 初始化SDK

在渲染页onCreate()阶段构建DUIX对象并添加回调事件

```kotlin
duix = DUIX(mContext, baseDir, modelDir, mDUIXRender) { event, msg, info ->
    when (event) {
        ai.guiji.duix.sdk.client.Constant.CALLBACK_EVENT_INIT_READY -> {
            initOK()
        }

        ai.guiji.duix.sdk.client.Constant.CALLBACK_EVENT_INIT_ERROR -> {

        }
        // ...

    }
}
// 异步回调结果
duix?.init()
```



DUIX对象构建说明:

| 参数     | 类型       | 描述                                                         |
| -------- | ---------- | ------------------------------------------------------------ |
| context  | Context    | 系统上下文                                                   |
| baseDir  | String     | 存放模型驱动的配置文件,需要自行管理. 可将压缩文件解压到外部存储并提供文件夹路径 |
| modelDir | String     | 存放模型文件的文件夹,需要自行管理. 可将压缩文件解压到外部存储并提供文件夹路径 |
| render   | RenderSink | 渲染数据接口，sdk提供了默认的渲染组件继承自该接口，也可以自己实现 |
| callback | Callback   | SDK处理的各种回调事件                                        |

参考demo LiveActivity示例

### 2. 获取SDK模型初始化状态

```kotlin
object : Callback {
    fun onEvent(event: String, msg: String, info: Object) {
        when (event) {
            "init.ready" -> {
                // SDK模型初始化成功
            }

            "init.error" -> {
                //初始化失败
                Log.e(TAG, "init error: $msg")
            }
            // ...

        }
    }
}
```

### 3. 数字人形象展示

使用RenderSink接口接受渲染帧数据，SDK中提供了该接口实现DUIXRenderer.java。也可以自己实现该接口自定义渲染。
其中RenderSink的定义如下:

```java
/**
 * 渲染管道，通过该接口返回渲染数据
 */
public interface RenderSink {

    // frame中的buffer数据以bgr顺序排列
    void onVideoFrame(ImageFrame imageFrame);

}
```

使用DUIXRenderer及DUIXTextureView控件简单实现渲染展示,该控件支持透明通道可以自由设置背景及前景：

```kotlin
override fun onCreate(savedInstanceState: Bundle?) {
    super.onCreate(savedInstanceState)
    // ...
    mDUIXRender =
        DUIXRenderer(
            mContext,
            binding.glTextureView
        )

    binding.glTextureView.setEGLContextClientVersion(GL_CONTEXT_VERSION)
    binding.glTextureView.setEGLConfigChooser(8, 8, 8, 8, 16, 0) // 透明
    binding.glTextureView.isOpaque = false           // 透明
    binding.glTextureView.setRenderer(mDUIXRender)
    binding.glTextureView.renderMode =
        GLSurfaceView.RENDERMODE_WHEN_DIRTY      // 一定要在设置完Render之后再调用

    duix = DUIX(mContext, duixOptions, mDUIXRender) { event, msg, _ ->
    }
    // ...
}
```

### 4. 启动数字人播报

在初始化成功后，可以播放音频以驱动形象

```kotlin
duix?.playAudio(wavPath)
```

参数说明:

| 参数      | 类型     | 描述                          |
|---------|--------|-----------------------------|
| wavPath | String | 16k采样率单通道的wav文件地址或https网络地址 |

音频播放状态及进度回调:

```kotlin
object : Callback {
    fun onEvent(event: String, msg: String, info: Object) {
        when (event) {
            // ...

            "play.start" -> {
                // 开始播放音频
            }

            "play.end" -> {
                // 完成播放音频
            }
            "play.error" -> {
                // 音频播放异常
            }
            "play.progress" -> {
                // 音频播放进度
            }

        }
    }
}

```

### 5. 终止当前播报

当数字人正在播报时调用该接口终止播报。

函数定义:

```
boolean stopAudio();
```

调用示例如下：

```kotlin
duix?.stopAudio()
```

### 6. 播放动作区间

当模型中支持播放动作区间时可使用该函数播放多做区间，多个时随机播放。

函数定义:

```
void motion();
```

调用示例如下：

```kotlin
duix?.motion()
```

### 

### 四. Proguard配置

如果代码使用了混淆，请在proguard-rules.pro中配置：

```
-keep class com.btows.ncnntest.**{*; }
-dontwarn com.squareup.okhttp3.**
-keep class com.squareup.okhttp3.** { *;}
```

## 五、注意事项

1. 驱动渲染必须正确的配置基础配置文件夹和对应模型文件夹的存储路径。
2. 播放的音频文件不宜过大，过大的音频导入会导致大量消耗CPU，从而造成绘制卡顿。



## 六、版本记录
