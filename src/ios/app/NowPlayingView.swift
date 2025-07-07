import SwiftUI

struct NowPlayingView: View {
    @EnvironmentObject var player: MediaPlayerViewModel
    @StateObject private var voice = VoiceControl()

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
                Button(action: { voice.start() }) {
                    Image(systemName: "mic.fill")
                }
            }
        }
        .gesture(DragGesture().onEnded { value in
            if value.translation.width < -50 { player.nextTrack() }
            if value.translation.width > 50 { player.previousTrack() }
        })
        .onAppear { voice.onCommand = { player.handleVoiceCommand($0) } }
    }
}
