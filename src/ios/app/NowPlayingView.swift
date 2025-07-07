import SwiftUI

struct NowPlayingView: View {
    @EnvironmentObject var player: MediaPlayerViewModel
    @StateObject private var voice = VoiceControl()
    @State private var listening = false

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
                Button(listening ? "Stop" : "Voice") {
                    if listening {
                        voice.stop()
                        listening = false
                    } else {
                        voice.onCommand = { cmd in
                            player.handleVoiceCommand(cmd)
                        }
                        try? voice.start()
                        listening = true
                    }
                }
            }
        }
        .gesture(DragGesture().onEnded { value in
            if value.translation.width < -50 { player.nextTrack() }
            if value.translation.width > 50 { player.previousTrack() }
        })
    }
}
