import Foundation
import Combine
import MediaPlayer
import UIKit

struct MediaItem: Identifiable {
    let id = UUID()
    let path: String
    let title: String
    let artist: String
}

class MediaPlayerViewModel: ObservableObject {
    private let bridge: MediaPlayerBridge
    private let nowPlayingCenter: MPNowPlayingInfoCenter
    private var observers: [NSObjectProtocol] = []

    @Published var isPlaying: Bool = false
    @Published var position: Double = 0
    @Published var currentTitle: String = ""
    @Published var currentArtist: String = ""
    @Published var library: [MediaItem] = []
    @Published var shuffleEnabled: Bool = false
    @Published var aiTaggingEnabled: Bool = UserDefaults.standard.bool(forKey: "enableAITagging") {
        didSet {
            UserDefaults.standard.set(aiTaggingEnabled, forKey: "enableAITagging")
        }
    }
    @Published var currentDuration: Double = 0
    @Published var artwork: UIImage? = nil
    @Published var volume: Double = 1.0
    @Published var videoMode: Bool = false

    init(bridge: MediaPlayerBridge = MediaPlayerBridge(), nowPlaying: MPNowPlayingInfoCenter = MPNowPlayingInfoCenter.default()) {
        self.bridge = bridge
        self.nowPlayingCenter = nowPlaying
        shuffleEnabled = bridge.shuffleEnabled()
        aiTaggingEnabled = UserDefaults.standard.bool(forKey: "enableAITagging")
        observers.append(NotificationCenter.default.addObserver(forName: .trackFinished, object: nil, queue: .main) { _ in
            self.isPlaying = false
        })
        observers.append(NotificationCenter.default.addObserver(forName: .positionChanged, object: nil, queue: .main) { n in
            if let pos = n.userInfo?["position"] as? Double {
                self.position = pos
                self.updateNowPlayingInfo()
            }
        })
        observers.append(NotificationCenter.default.addObserver(forName: .trackLoaded, object: nil, queue: .main) { n in
            self.currentTitle = n.userInfo?["title"] as? String ?? ""
            self.currentArtist = n.userInfo?["artist"] as? String ?? ""
            if let dur = n.userInfo?["duration"] as? Double {
                self.currentDuration = dur
            }
            if let artPath = n.userInfo?["artwork"] as? String {
                self.artwork = UIImage(contentsOfFile: artPath)
            } else {
                self.artwork = nil
            }
            self.updateNowPlayingInfo()
        })
    }

    deinit {
        for o in observers { NotificationCenter.default.removeObserver(o) }
    }

    func configureCallbacks() {
        bridge.setCallbacks()
        if let dir = FileManager.default.urls(for: .documentDirectory, in: .userDomainMask).first {
            let path = dir.appendingPathComponent("media_library.db").path
            bridge.setLibraryPath(path)
        }
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

    func search(_ query: String) {
        let items = bridge.search(query) as? [[String: Any]] ?? []
        library = items.map { dict in
            MediaItem(path: dict["path"] as? String ?? "",
                      title: dict["title"] as? String ?? "",
                      artist: dict["artist"] as? String ?? "")
        }
    }

    func nextTrack() { bridge.nextTrack() }
    func previousTrack() { bridge.previousTrack() }
    func enableShuffle(_ enabled: Bool) {
        bridge.enableShuffle(enabled)
        shuffleEnabled = enabled
    }

    func toggleShuffle() {
        enableShuffle(!shuffleEnabled)
    }

    func sendToDevice(address: String, port: UInt16) {
        let meta = bridge.currentMetadata() as? [String: Any] ?? [:]
        if let path = meta["path"] as? String {
            bridge.sendSyncTo(address, port: port, path: path, position: position)
        }
    }

    private func updateNowPlayingInfo() {
        var info: [String: Any] = [MPMediaItemPropertyTitle: currentTitle,
                                   MPMediaItemPropertyArtist: currentArtist]
        if let img = artwork {
            let art = MPMediaItemArtwork(boundsSize: img.size) { _ in img }
            info[MPMediaItemPropertyArtwork] = art
        }
        info[MPNowPlayingInfoPropertyElapsedPlaybackTime] = position
        info[MPMediaItemPropertyPlaybackDuration] = currentDuration
        nowPlayingCenter.nowPlayingInfo = info
    }

    func setupRemoteCommands() {
        let cc = MPRemoteCommandCenter.shared()
        cc.playCommand.addTarget { _ in self.play(); return .success }
        cc.pauseCommand.addTarget { _ in self.pause(); return .success }
        cc.nextTrackCommand.addTarget { _ in self.nextTrack(); return .success }
        cc.previousTrackCommand.addTarget { _ in self.previousTrack(); return .success }
    }

    func handleVoiceCommand(_ text: String) {
        let command = text.lowercased()
        if command.contains("play") && !command.contains("pause") {
            play()
            return
        }
        if command.contains("pause") {
            pause()
            return
        }
        if command.contains("next") {
            nextTrack()
            return
        }
        if command.contains("previous") || command.contains("back") {
            previousTrack()
            return
        }
        search(command)
        if let first = library.first {
            _ = open(first.path)
            play()
        }
    }
}

extension Notification.Name {
    static let trackFinished = Notification.Name("MediaPlayerTrackFinished")
    static let positionChanged = Notification.Name("MediaPlayerPositionChanged")
    static let trackLoaded = Notification.Name("MediaPlayerTrackLoaded")
}
