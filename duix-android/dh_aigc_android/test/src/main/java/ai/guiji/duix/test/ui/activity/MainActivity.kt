package ai.guiji.duix.test.ui.activity

import ai.guiji.duix.sdk.client.BuildConfig
import ai.guiji.duix.test.R
import ai.guiji.duix.test.databinding.ActivityMainBinding
import ai.guiji.duix.test.service.StorageService
import android.Manifest
import android.annotation.SuppressLint
import android.content.Intent
import android.os.Bundle
import android.view.View
import android.widget.AdapterView
import android.widget.AdapterView.OnItemSelectedListener
import android.widget.Toast
import java.io.BufferedReader
import java.io.File
import java.io.FileInputStream
import java.io.InputStreamReader


class MainActivity : BaseActivity() {

    private lateinit var binding: ActivityMainBinding

    private val baseConfigUrl =
        "https://cdn.guiji.ai/duix/location/gj_dh_res.zip"
    private lateinit var baseDir: File
    private val baseConfigUUID = "d39caddd-488b-4682-b6d1-13549b135dd1"     // 可以用来控制模型文件版本
    private var baseConfigReady = false

    // https://cdn.guiji.ai/duix/location/Valentin_540.zip
    private val modelUrl =
        "https://cdn.guiji.ai/duix/location/Valentin_540.zip"
    private lateinit var modelDir: File
    private val liangweiUUID = "d39caddd-488b-4682-b6d1-13549b135dd1"       // 可以用来控制模型文件版本
    private var modelReady = false


    @SuppressLint("SetTextI18n")
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        binding = ActivityMainBinding.inflate(layoutInflater)
        setContentView(binding.root)

        binding.tvSdkVersion.text = "SDK Version: ${BuildConfig.VERSION_NAME}"

        val duixDir = mContext.getExternalFilesDir("duix")
        if (!duixDir!!.exists()) {
            duixDir.mkdirs()
        }
        baseDir = File(
            duixDir,
            baseConfigUrl.substring(baseConfigUrl.lastIndexOf("/") + 1).replace(".zip", "")
        )
        modelDir = File(
            duixDir,
            modelUrl.substring(modelUrl.lastIndexOf("/") + 1).replace(".zip", "")
        )        // 这里要求存放模型的文件夹的名字和下载的zip文件的一致以对应解压的文件夹路径

        binding.btnBaseConfigDownload.setOnClickListener {
            downloadBaseConfig()
        }
        binding.btnModelPlay.setOnClickListener {
            tryPlay()
        }

        checkFile()
    }

    private fun tryPlay() {
        if (!modelReady) {
            downloadModel()
        } else if (!baseConfigReady) {
            Toast.makeText(mContext, "您必须正确的安装基础配置文件", Toast.LENGTH_SHORT).show()
        } else {
            val intent = Intent(mContext, CallActivity::class.java)
            intent.putExtra("baseDir", baseDir.absolutePath)
            intent.putExtra("modelDir", modelDir.absolutePath)
            startActivity(intent)
        }
    }

    private fun downloadBaseConfig() {
        binding.btnBaseConfigDownload.isEnabled = false
        binding.progressBaseConfig.progress = 0
        StorageService.downloadAndUnzip(
            mContext,
            baseConfigUrl,
            baseDir.absolutePath,
            baseConfigUUID,
            object : StorageService.Callback {
                override fun onDownloadProgress(progress: Int) {
                    runOnUiThread {
                        binding.progressBaseConfig.progress = progress / 2
                    }
                }

                override fun onUnzipProgress(progress: Int) {
                    runOnUiThread {
                        binding.progressBaseConfig.progress = 50 + progress / 2
                    }
                }

                override fun onComplete(path: String?) {
                    runOnUiThread {
                        binding.btnBaseConfigDownload.isEnabled = false
                        binding.btnBaseConfigDownload.text = getString(R.string.ready)
                        binding.progressBaseConfig.progress = 100
                        baseConfigReady = true
                    }
                }

                override fun onError(msg: String?) {
                    runOnUiThread {
                        binding.btnBaseConfigDownload.isEnabled = true
                        binding.progressBaseConfig.progress = 0
                        Toast.makeText(mContext, "文件下载异常: $msg", Toast.LENGTH_SHORT).show()
                    }
                }
            }, true
        )
    }

    private fun downloadModel() {
        binding.btnModelPlay.isEnabled = false
        binding.btnModelPlay.text = getString(R.string.download)
        binding.progressModel.progress = 0
        StorageService.downloadAndUnzip(
            mContext,
            modelUrl,
            modelDir.absolutePath,
            liangweiUUID,
            object : StorageService.Callback {
                override fun onDownloadProgress(progress: Int) {
                    runOnUiThread {
                        binding.progressModel.progress = progress / 2
                    }
                }

                override fun onUnzipProgress(progress: Int) {
                    runOnUiThread {
                        binding.progressModel.progress = 50 + progress / 2;
                    }
                }

                override fun onComplete(path: String?) {
                    runOnUiThread {
                        binding.btnModelPlay.isEnabled = true
                        binding.btnModelPlay.text = getString(R.string.play)
                        binding.progressModel.progress = 100
                        modelReady = true
                    }
                }

                override fun onError(msg: String?) {
                    runOnUiThread {
                        binding.btnModelPlay.isEnabled = true
                        binding.btnModelPlay.text = getString(R.string.download)
                        binding.progressModel.progress = 0
                        Toast.makeText(mContext, "文件下载异常: $msg", Toast.LENGTH_SHORT).show()
                    }
                }
            }, false        // for debug
        )
    }

    private fun checkFile() {
        if (baseDir.exists() && File(baseDir, "/uuid").exists() && baseConfigUUID == BufferedReader(
                InputStreamReader(
                    FileInputStream(
                        File(baseDir, "/uuid")
                    )
                )
            ).readLine()
        ) {
            binding.btnBaseConfigDownload.isEnabled = false
            binding.btnBaseConfigDownload.text = getString(R.string.ready)
            binding.progressBaseConfig.progress = 100
            baseConfigReady = true
        } else {
            binding.btnBaseConfigDownload.isEnabled = true
            binding.progressBaseConfig.progress = 0
        }
        if (modelDir.exists() && File(modelDir, "/uuid").exists() && liangweiUUID == BufferedReader(
                InputStreamReader(
                    FileInputStream(
                        File(modelDir, "/uuid")
                    )
                )
            ).readLine()
        ) {
            binding.btnModelPlay.isEnabled = true
            binding.btnModelPlay.text = getString(R.string.play)
            binding.progressModel.progress = 100
            modelReady = true
        } else {
            binding.btnModelPlay.isEnabled = true
            binding.btnModelPlay.text = getString(R.string.download)
            binding.progressModel.progress = 0
        }
    }


}
