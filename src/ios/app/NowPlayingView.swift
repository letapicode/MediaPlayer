import SwiftUI
import MediaPlayer

struct NowPlayingView: View {
    @EnvironmentObject var player: MediaPlayerViewModel
    @StateObject private var voice = VoiceControl()
    @AppStorage("enableSwipe") private var enableSwipe: Bool = true
    @AppStorage("enableVerticalGestures") private var enableVerticalGestures: Bool = true
    private let systemPlayer = MPMusicPlayerController.systemMusicPlayer

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
        .optionalGesture(enableSwipe ? horizontalDrag : nil)
        .optionalGesture(enableVerticalGestures ? verticalDrag : nil)
        .onAppear { voice.onCommand = { player.handleVoiceCommand($0) } }
    }

    private var horizontalDrag: some Gesture {
        DragGesture().onEnded { value in
            if value.translation.width < -50 { player.nextTrack() }
            if value.translation.width > 50 { player.previousTrack() }
        }
    }

    private var verticalDrag: some Gesture {
        DragGesture().onEnded { value in
            if value.translation.height > 30 || value.translation.height < -30 {
                let delta = -Double(value.translation.height) / 500.0
                if player.artwork == nil {
                    let b = min(1.0, max(0.0, UIScreen.main.brightness + delta))
                    UIScreen.main.brightness = b
                } else {
                    let vol = max(0.0, min(1.0, Double(systemPlayer.volume) + delta))
                    systemPlayer.volume = Float(vol)
                }
            }
        }
    }
}
