import SwiftUI
import AVFoundation

@main
struct MediaPlayerApp: App {
    @StateObject private var player = MediaPlayerViewModel()

    init() {
        do {
            try AVAudioSession.sharedInstance().setCategory(.playback)
            try AVAudioSession.sharedInstance().setActive(true)
        } catch {
            print("AVAudioSession error: \(error)")
        }
    }

    var body: some Scene {
        WindowGroup {
            ContentView()
                .environmentObject(player)
                .onAppear {
                    player.configureCallbacks()
                    player.setupRemoteCommands()
                }
        }
    }
}
