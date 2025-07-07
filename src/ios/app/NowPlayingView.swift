import SwiftUI

struct NowPlayingView: View {
    @EnvironmentObject var player: MediaPlayerViewModel
    @StateObject private var voice = VoiceControl()
    @AppStorage("enableSwipe") private var enableSwipe: Bool = true

    var body: some View {
        VStack {
            if let art = player.artwork {
                Image(uiImage: art)
                    .resizable()
                    .aspectRatio(contentMode: .fit)
                    .frame(maxHeight: 200)
            } else {
                Image(systemName: "music.note")
                    .resizable()
                    .aspectRatio(contentMode: .fit)
                    .frame(maxHeight: 200)
                    .foregroundColor(.secondary)
            }
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
        .optionalGesture(enableSwipe ?
            DragGesture().onEnded { value in
                if value.translation.width < -50 { player.nextTrack() }
                if value.translation.width > 50 { player.previousTrack() }
            } : nil)
        .onAppear { voice.onCommand = { player.handleVoiceCommand($0) } }
    }
}
