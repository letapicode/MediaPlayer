import SwiftUI

struct NowPlayingView: View {
    @EnvironmentObject var player: MediaPlayerViewModel
    @StateObject private var voice = VoiceControl()
    @AppStorage("enableSwipe") private var enableSwipe: Bool = true
    @AppStorage("enableVolumeSwipe") private var enableVolumeSwipe: Bool = true

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
        .optionalGesture(gesture)
        .onAppear { voice.onCommand = { player.handleVoiceCommand($0) } }
    }

    private var gesture: some Gesture? {
        if !enableSwipe && !enableVolumeSwipe { return nil }
        return DragGesture().onEnded { value in
            if abs(value.translation.width) > abs(value.translation.height) {
                if enableSwipe {
                    if value.translation.width < -50 { player.nextTrack() }
                    if value.translation.width > 50 { player.previousTrack() }
                }
            } else if enableVolumeSwipe {
                if value.translation.height < -50 {
                    player.volume = min(player.volume + 0.1, 1.0)
                }
                if value.translation.height > 50 {
                    player.volume = max(player.volume - 0.1, 0.0)
                }
            }
        }
    }
}
