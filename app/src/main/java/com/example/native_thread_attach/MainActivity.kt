package com.example.native_thread_attach

import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import android.os.Debug
import android.util.Log
import android.widget.TextView
import com.example.native_thread_attach.databinding.ActivityMainBinding
import kotlin.concurrent.thread

class MainActivity : AppCompatActivity() {

    private lateinit var binding: ActivityMainBinding

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)

        binding = ActivityMainBinding.inflate(layoutInflater)
        setContentView(binding.root)

        // start method-trace sampling in the background
        Debug.startMethodTracingSampling(null, 3_000_000, 30_000);


        binding.sampleText.setOnClickListener {
            binding.sampleText.text = attachThreadWithoutDetach()
        }
    }

    private external fun attachThreadWithoutDetach(): String

    companion object {
        // Used to load the 'native_thread_attach' library on application startup.
        init {
            System.loadLibrary("native_thread_attach")
        }
    }
}