package com.example.mediaplayer

import android.os.Bundle
import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import androidx.fragment.app.Fragment
import androidx.recyclerview.widget.LinearLayoutManager
import androidx.recyclerview.widget.RecyclerView

class PlaylistsFragment : Fragment() {
    override fun onCreateView(
        inflater: LayoutInflater,
        container: ViewGroup?,
        savedInstanceState: Bundle?
    ): View {
        return inflater.inflate(R.layout.fragment_playlists, container, false)
    }

    override fun onViewCreated(view: View, savedInstanceState: Bundle?) {
        val list = view.findViewById<RecyclerView>(R.id.playlists)
        list.layoutManager = LinearLayoutManager(requireContext())
        list.adapter = PlaylistAdapter(emptyList())
    }
}

private class PlaylistAdapter(private val items: List<String>) :
    RecyclerView.Adapter<PlaylistViewHolder>() {
    override fun onCreateViewHolder(parent: ViewGroup, viewType: Int): PlaylistViewHolder {
        val v = LayoutInflater.from(parent.context)
            .inflate(android.R.layout.simple_list_item_1, parent, false)
        return PlaylistViewHolder(v)
    }

    override fun onBindViewHolder(holder: PlaylistViewHolder, position: Int) {
        (holder.itemView as android.widget.TextView).text = items[position]
    }

    override fun getItemCount(): Int = items.size
}

private class PlaylistViewHolder(view: View) : RecyclerView.ViewHolder(view)
