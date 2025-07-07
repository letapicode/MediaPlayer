import SwiftUI

struct LibraryView: View {
    @EnvironmentObject var player: MediaPlayerViewModel

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
    }
}
