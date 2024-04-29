package ai.guiji.duix.test.ui.activity

import ai.guiji.duix.sdk.client.Constant
import ai.guiji.duix.sdk.client.DUIX
import ai.guiji.duix.sdk.client.render.DUIXRenderer
import ai.guiji.duix.test.databinding.ActivityCallBinding
import android.opengl.GLSurfaceView
import android.os.Bundle
import android.util.Log
import android.view.View
import android.widget.Toast
import com.bumptech.glide.Glide
import java.io.File
import java.io.FileOutputStream
import java.io.OutputStream
import java.util.concurrent.Executors


class CallActivity : BaseActivity() {

    companion object {
        const val GL_CONTEXT_VERSION = 2
    }

    private var baseDir = ""
    private var modelDir = ""


    private lateinit var binding: ActivityCallBinding
    private var duix: DUIX? = null
    private var mDUIXRender: DUIXRenderer? = null

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        keepScreenOn()
        binding = ActivityCallBinding.inflate(layoutInflater)
        setContentView(binding.root)

        baseDir = intent.getStringExtra("baseDir") ?: ""
        modelDir = intent.getStringExtra("modelDir") ?: ""

        Log.e("123", "baseDir: $baseDir")
        Log.e("123", "modelDir: $modelDir")

        binding.btnPlay.setOnClickListener {
            playWav()
        }

        Glide.with(mContext).load("file:///android_asset/bg/bg1.png").into(binding.ivBg)

        binding.glTextureView.setEGLContextClientVersion(GL_CONTEXT_VERSION)
        binding.glTextureView.setEGLConfigChooser(8, 8, 8, 8, 16, 0) // 透明
//        binding.glTextureView.preserveEGLContextOnPause = true      // 后台运行不要释放上下文
        binding.glTextureView.isOpaque = false           // 透明

        mDUIXRender =
            DUIXRenderer(
                mContext,
                binding.glTextureView
            )
        binding.glTextureView.setRenderer(mDUIXRender)
        binding.glTextureView.renderMode =
            GLSurfaceView.RENDERMODE_WHEN_DIRTY      // 一定要在设置完Render之后再调用

        duix = DUIX(mContext, baseDir, modelDir, mDUIXRender) { event, msg, info ->
            when (event) {
                Constant.CALLBACK_EVENT_INIT_READY -> {
                    initOk()
                }

                Constant.CALLBACK_EVENT_INIT_ERROR -> {
                    runOnUiThread {
                        Toast.makeText(mContext, "初始化异常: $msg", Toast.LENGTH_SHORT).show()
                    }
                }

                Constant.CALLBACK_EVENT_AUDIO_PLAY_START -> {
                    runOnUiThread {

                    }
                }

                Constant.CALLBACK_EVENT_AUDIO_PLAY_END -> {
                    Log.e(TAG, "CALLBACK_EVENT_PLAY_END: $msg")
                }

                Constant.CALLBACK_EVENT_AUDIO_PLAY_ERROR -> {
                    Log.e(TAG, "CALLBACK_EVENT_PLAY_ERROR: $msg")
                }

                Constant.CALLBACK_EVENT_AUDIO_PLAY_PROGRESS -> {
//                    Log.e(TAG, "audio play progress: $info")

                }
            }
        }
        // 异步回调结果
        duix?.init()
    }

    private fun initOk() {
        Log.e(TAG, "init ok")
        runOnUiThread {
            binding.btnPlay.visibility = View.VISIBLE
        }
    }


    override fun onDestroy() {
        super.onDestroy()
        duix?.release()
        mDUIXRender?.release()
    }

    private fun playWav() {
        val wavName = "help.wav"
        val wavDir = File(mContext.getExternalFilesDir("duix"), "wav")
        if (!wavDir.exists()) {
            wavDir.mkdirs()
        }
        val wavFile = File(wavDir, wavName)
        if (!wavFile.exists()) {
            // 拷贝到sdcard
            val executor = Executors.newSingleThreadExecutor()
            executor.execute {
                val input = mContext.assets.open("wav/${wavName}")
                val out: OutputStream = FileOutputStream("${wavFile.absolutePath}.tmp")
                val buffer = ByteArray(1024)
                var read: Int
                while (input.read(buffer).also { read = it } != -1) {
                    out.write(buffer, 0, read)
                }
                input.close()
                out.close()
                File("${wavFile.absolutePath}.tmp").renameTo(wavFile)
                duix?.playAudio(wavFile.absolutePath)
            }
        } else {
            duix?.playAudio(wavFile.absolutePath)
        }
    }

}
