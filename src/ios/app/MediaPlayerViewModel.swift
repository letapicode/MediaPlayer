import Foundation
import Combine
import MediaPlayer

struct MediaItem: Identifiable {
    let id = UUID()
    let path: String
    let title: String
    let artist: String
}

class MediaPlayerViewModel: ObservableObject {
    private let bridge: MediaPlayerBridge
    private var observers: [NSObjectProtocol] = []

    @Published var isPlaying: Bool = false
    @Published var position: Double = 0
    @Published var currentTitle: String = ""
    @Published var currentArtist: String = ""
    @Published var library: [MediaItem] = []

    init(bridge: MediaPlayerBridge = MediaPlayerBridge()) {
        self.bridge = bridge
        observers.append(NotificationCenter.default.addObserver(forName: .trackFinished, object: nil, queue: .main) { _ in
            self.isPlaying = false
        })
        observers.append(NotificationCenter.default.addObserver(forName: .positionChanged, object: nil, queue: .main) { n in
            if let pos = n.userInfo?["position"] as? Double {
                self.position = pos
            }
        })
        observers.append(NotificationCenter.default.addObserver(forName: .trackLoaded, object: nil, queue: .main) { n in
            self.currentTitle = n.userInfo?["title"] as? String ?? ""
            self.currentArtist = n.userInfo?["artist"] as? String ?? ""
            self.updateNowPlayingInfo()
        })
    }

    deinit {
        for o in observers { NotificationCenter.default.removeObserver(o) }
    }

    func configureCallbacks() {
        bridge.setCallbacks()
    }

    func open(_ path: String) -> Bool {
        bridge.open(path)
    }

    func play() {
        bridge.play()
        isPlaying = true
    }

    func pause() {
        bridge.pause()
        isPlaying = false
    }

    func stop() {
        bridge.stop()
        isPlaying = false
    }

    func seek(to pos: Double) {
        bridge.seek(pos)
    }

    func loadLibrary() {
        let items = bridge.allMedia() as? [[String: Any]] ?? []
        library = items.map { dict in
            MediaItem(path: dict["path"] as? String ?? "",
                      title: dict["title"] as? String ?? "",
                      artist: dict["artist"] as? String ?? "")
        }
    }

    func nextTrack() { bridge.nextTrack() }
    func previousTrack() { bridge.previousTrack() }

    private func updateNowPlayingInfo() {
        var info: [String: Any] = [MPMediaItemPropertyTitle: currentTitle,
                                   MPMediaItemPropertyArtist: currentArtist]
        info[MPNowPlayingInfoPropertyElapsedPlaybackTime] = position
        MPNowPlayingInfoCenter.default().nowPlayingInfo = info
    }

    func setupRemoteCommands() {
        let cc = MPRemoteCommandCenter.shared()
        cc.playCommand.addTarget { _ in self.play(); return .success }
        cc.pauseCommand.addTarget { _ in self.pause(); return .success }
        cc.nextTrackCommand.addTarget { _ in self.nextTrack(); return .success }
        cc.previousTrackCommand.addTarget { _ in self.previousTrack(); return .success }
    }
}

extension Notification.Name {
    static let trackFinished = Notification.Name("MediaPlayerTrackFinished")
    static let positionChanged = Notification.Name("MediaPlayerPositionChanged")
    static let trackLoaded = Notification.Name("MediaPlayerTrackLoaded")
}
