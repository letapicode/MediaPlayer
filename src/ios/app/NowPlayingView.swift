import SwiftUI

struct NowPlayingView: View {
    @EnvironmentObject var player: MediaPlayerViewModel
    @StateObject private var voice = VoiceControl()
    @AppStorage("enableSwipe") private var enableSwipe: Bool = true
    @AppStorage("enableVerticalGestures") private var enableVerticalGestures: Bool = true

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
        DragGesture().onEnded { value in
            if enableSwipe {
                if value.translation.width < -50 {
                    player.nextTrack()
                }
                if value.translation.width > 50 {
                    player.previousTrack()
                }
            }
            if enableVerticalGestures {
                if value.translation.height < -40 {
                    adjust(by: 0.05)
                }
                if value.translation.height > 40 {
                    adjust(by: -0.05)
                }
            }
        }
    }

    private func adjust(by delta: Double) {
        if player.videoMode {
            let newValue = CGFloat(min(1.0, max(0.0, Double(UIScreen.main.brightness) + delta)))
            UIScreen.main.brightness = newValue
        } else {
            player.volume = min(1.0, max(0.0, player.volume + delta))
        }
    }
}
