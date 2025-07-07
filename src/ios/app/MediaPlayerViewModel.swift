import Foundation
import Combine

class MediaPlayerViewModel: ObservableObject {
    private let bridge = MediaPlayerBridge()
    private var observers: [NSObjectProtocol] = []

    @Published var isPlaying: Bool = false
    @Published var position: Double = 0

    init() {
        observers.append(NotificationCenter.default.addObserver(forName: .trackFinished, object: nil, queue: .main) { _ in
            self.isPlaying = false
        })
        observers.append(NotificationCenter.default.addObserver(forName: .positionChanged, object: nil, queue: .main) { n in
            if let pos = n.userInfo?["position"] as? Double {
                self.position = pos
            }
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
}

extension Notification.Name {
    static let trackFinished = Notification.Name("MediaPlayerTrackFinished")
    static let positionChanged = Notification.Name("MediaPlayerPositionChanged")
}
