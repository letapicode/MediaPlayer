package com.example.mediaplayer

import android.os.Bundle
import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import androidx.fragment.app.Fragment
import androidx.recyclerview.widget.LinearLayoutManager
import androidx.recyclerview.widget.RecyclerView

class DevicesFragment : Fragment() {
    override fun onCreateView(inflater: LayoutInflater, container: ViewGroup?, savedInstanceState: Bundle?): View? {
        val view = inflater.inflate(R.layout.fragment_devices, container, false)
        val list = view.findViewById<RecyclerView>(R.id.device_list)
        list.layoutManager = LinearLayoutManager(context)
        // TODO bind to native SyncController via JNI
        return view
    }
}
