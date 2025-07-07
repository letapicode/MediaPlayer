import SwiftUI

struct NowPlayingView: View {
    @EnvironmentObject var player: MediaPlayerViewModel

    var body: some View {
        VStack {
            Text(player.currentTitle).font(.title)
            Text(player.currentArtist).font(.subheadline)
            HStack {
                Button("Prev") { player.previousTrack() }
                Button(player.isPlaying ? "Pause" : "Play") {
                    player.isPlaying ? player.pause() : player.play()
                }
                Button("Next") { player.nextTrack() }
            }
        }
        .gesture(DragGesture().onEnded { value in
            if value.translation.width < -50 { player.nextTrack() }
            if value.translation.width > 50 { player.previousTrack() }
        })
    }
}
