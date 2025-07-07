import SwiftUI
import AVFoundation

@main
struct MediaPlayerApp: App {
    @StateObject private var player = MediaPlayerViewModel()
    private let shakeDetector = ShakeDetector()
    @AppStorage("enableShake") private var enableShake: Bool = true

    init() {
        do {
            try AVAudioSession.sharedInstance().setCategory(.playback)
            try AVAudioSession.sharedInstance().setActive(true)
        } catch {
            print("AVAudioSession error: \(error)")
        }
        NotificationCenter.default.addObserver(forName: AVAudioSession.interruptionNotification,
                                               object: nil, queue: .main) { n in
            guard let info = n.userInfo,
                  let value = info[AVAudioSessionInterruptionTypeKey] as? UInt,
                  let type = AVAudioSession.InterruptionType(rawValue: value) else { return }
            if type == .began {
                player.pause()
            } else if type == .ended {
                if let optVal = info[AVAudioSessionInterruptionOptionKey] as? UInt,
                   AVAudioSession.InterruptionOptions(rawValue: optVal).contains(.shouldResume) {
                    player.play()
                }
            }
        }
    }

    var body: some Scene {
        WindowGroup {
            ContentView()
                .environmentObject(player)
                .onAppear {
                    player.configureCallbacks()
                    player.setupRemoteCommands()
                    shakeDetector.onShake = { player.toggleShuffle() }
                    if enableShake { shakeDetector.start() }
                }
                .onDisappear {
                    shakeDetector.stop()
                }
                .onChange(of: enableShake) { val in
                    if val { shakeDetector.start() } else { shakeDetector.stop() }
                }
        }
    }
}
