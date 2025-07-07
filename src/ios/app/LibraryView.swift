import SwiftUI

struct LibraryView: View {
    @EnvironmentObject var player: MediaPlayerViewModel
    @State private var searchText: String = ""

    var body: some View {
        List(player.library) { item in
            VStack(alignment: .leading) {
                Text(item.title)
                Text(item.artist).font(.subheadline).foregroundColor(.secondary)
            }
            .onTapGesture {
                _ = player.open(item.path)
                player.play()
            }
        }
        .onAppear { player.loadLibrary() }
        .searchable(text: $searchText)
        .onChange(of: searchText) { text in
            if text.isEmpty {
                player.loadLibrary()
            } else {
                player.search(text)
            }
        }
    }
}
