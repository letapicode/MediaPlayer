import SwiftUI

struct NowPlayingView: View {
    @EnvironmentObject var player: MediaPlayerViewModel
    @StateObject private var voice = VoiceControl()
    @AppStorage("enableSwipe") private var enableSwipe: Bool = true
    @State private var toastMessage: String? = nil
    @State private var showToast: Bool = false

    var body: some View {
        ZStack {
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
            if showToast, let msg = toastMessage {
                Text(msg)
                    .padding(8)
                    .background(Color.black.opacity(0.8))
                    .foregroundColor(.white)
                    .cornerRadius(8)
                    .transition(.opacity)
                    .zIndex(1)
                    .onAppear {
                        DispatchQueue.main.asyncAfter(deadline: .now() + 2) {
                            withAnimation { showToast = false }
                        }
                    }
            }
        }
        .optionalGesture(enableSwipe ?
            DragGesture().onEnded { value in
                if value.translation.width < -50 { player.nextTrack() }
                if value.translation.width > 50 { player.previousTrack() }
            } : nil)
        .onAppear {
            voice.onCommand = { text in
                if !player.handleVoiceCommand(text) {
                    toastMessage = "Command not understood."
                    withAnimation { showToast = true }
                }
            }
            voice.onError = { message in
                toastMessage = message
                withAnimation { showToast = true }
            }
        }
    }
}
