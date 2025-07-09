package com.example.mediaplayer

import android.os.Bundle
import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import androidx.fragment.app.Fragment
import androidx.fragment.app.FragmentActivity
import androidx.lifecycle.lifecycleScope
import androidx.recyclerview.widget.LinearLayoutManager
import androidx.recyclerview.widget.RecyclerView
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.launch

class DevicesFragment : Fragment() {
    override fun onCreateView(inflater: LayoutInflater, container: ViewGroup?, savedInstanceState: Bundle?): View? {
        val view = inflater.inflate(R.layout.fragment_devices, container, false)
        val list = view.findViewById<RecyclerView>(R.id.device_list)
        list.layoutManager = LinearLayoutManager(context)
        val devices = MediaPlayerNative.nativeDiscoverDevices().map { str ->
            val parts = str.split("|")
            Device(parts[0], parts[1], parts.getOrNull(2)?.toInt() ?: 0)
        }
        list.adapter = DeviceAdapter(devices)
        return view
    }
}

private data class Device(val name: String, val address: String, val port: Int)

private class DeviceAdapter(private val items: List<Device>) :
    RecyclerView.Adapter<DeviceViewHolder>() {
    override fun onCreateViewHolder(parent: ViewGroup, viewType: Int): DeviceViewHolder {
        val v = LayoutInflater.from(parent.context)
            .inflate(android.R.layout.simple_list_item_1, parent, false)
        return DeviceViewHolder(v)
    }

    override fun onBindViewHolder(holder: DeviceViewHolder, position: Int) {
        (holder.itemView as android.widget.TextView).text = items[position].name
        holder.itemView.setOnClickListener { view ->
            (view.context as? FragmentActivity)?.lifecycleScope?.launch(Dispatchers.IO) {
                val path = MediaPlayerNative.nativeCurrentPath() ?: return@launch
                val pos = MediaPlayerNative.nativeCurrentPosition()
                val d = items[position]
                MediaPlayerNative.nativeSendSync(d.address, d.port, path, pos)
            }
        }
    }

    override fun getItemCount(): Int = items.size
}

private class DeviceViewHolder(view: View) : RecyclerView.ViewHolder(view)
