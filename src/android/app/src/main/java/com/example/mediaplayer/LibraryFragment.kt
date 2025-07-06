package com.example.mediaplayer

import android.os.Bundle
import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import androidx.fragment.app.Fragment
import androidx.lifecycle.lifecycleScope
import androidx.recyclerview.widget.LinearLayoutManager
import androidx.recyclerview.widget.RecyclerView
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.launch

class LibraryFragment : Fragment() {

    override fun onCreateView(
        inflater: LayoutInflater,
        container: ViewGroup?,
        savedInstanceState: Bundle?
    ): View {
        return inflater.inflate(R.layout.fragment_library, container, false)
    }

    override fun onViewCreated(view: View, savedInstanceState: Bundle?) {
        val list = view.findViewById<RecyclerView>(R.id.list)
        list.layoutManager = LinearLayoutManager(requireContext())
        lifecycleScope.launch(Dispatchers.IO) {
            val items = MediaPlayerNative.nativeListMedia().toList()
            launch(Dispatchers.Main) {
                list.adapter = MediaAdapter(items)
            }
        }
    }
}

private class MediaAdapter(private val items: List<String>) :
    RecyclerView.Adapter<MediaViewHolder>() {

    override fun onCreateViewHolder(parent: ViewGroup, viewType: Int): MediaViewHolder {
        val v = LayoutInflater.from(parent.context)
            .inflate(android.R.layout.simple_list_item_1, parent, false)
        return MediaViewHolder(v)
    }

    override fun onBindViewHolder(holder: MediaViewHolder, position: Int) {
        (holder.itemView as android.widget.TextView).text = items[position]
        holder.itemView.setOnClickListener {
            holder.itemView.context.let { ctx ->
                (ctx as? androidx.fragment.app.FragmentActivity)?.lifecycleScope?.launch(Dispatchers.IO) {
                    MediaPlayerNative.nativeOpen(items[position])
                }
            }
        }
    }

    override fun getItemCount(): Int = items.size
}

private class MediaViewHolder(view: View) : RecyclerView.ViewHolder(view)
