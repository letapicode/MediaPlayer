import XCTest
@testable import MediaPlayerApp

class DummyBridge: MediaPlayerBridge {
    var playCalled = false
    var shuffleEnabledFlag = false
    var shuffleEnabledReturn = false
    var lastSearchQuery: String?
    override func play() { playCalled = true }
    override func enableShuffle(_ enabled: Bool) { shuffleEnabledFlag = enabled }
    override func shuffleEnabled() -> Bool { shuffleEnabledReturn }
    override func search(_ query: String) -> [NSDictionary] {
        lastSearchQuery = query
        return [["path": "p", "title": "t", "artist": "a"]]
    }
}

class MockNowPlayingInfoCenter: MPNowPlayingInfoCenter {
    var updated = false
    override var nowPlayingInfo: [String : Any]? {
        didSet { updated = true }
    }
}

final class MediaPlayerViewModelTests: XCTestCase {
    func testPlaySetsState() {
        let vm = MediaPlayerViewModel(bridge: DummyBridge(), nowPlaying: MockNowPlayingInfoCenter())
        vm.play()
        XCTAssertTrue(vm.isPlaying)
    }

    func testToggleShuffleCallsBridge() {
        let bridge = DummyBridge()
        let vm = MediaPlayerViewModel(bridge: bridge, nowPlaying: MockNowPlayingInfoCenter())
        vm.toggleShuffle()
        XCTAssertTrue(bridge.shuffleEnabledFlag)
    }

    func testSearchUpdatesLibrary() {
        let bridge = DummyBridge()
        let vm = MediaPlayerViewModel(bridge: bridge, nowPlaying: MockNowPlayingInfoCenter())
        vm.search("hello")
        XCTAssertEqual(bridge.lastSearchQuery, "hello")
        XCTAssertEqual(vm.library.first?.title, "t")
    }

    func testPositionNotificationUpdatesNowPlaying() {
        let nowPlaying = MockNowPlayingInfoCenter()
        let vm = MediaPlayerViewModel(bridge: DummyBridge(), nowPlaying: nowPlaying)
        NotificationCenter.default.post(name: .positionChanged, object: nil, userInfo: ["position": 10.0])
        XCTAssertTrue(nowPlaying.updated)
    }

    func testInitReadsShuffleState() {
        let bridge = DummyBridge()
        bridge.shuffleEnabledReturn = true
        let vm = MediaPlayerViewModel(bridge: bridge, nowPlaying: MockNowPlayingInfoCenter())
        XCTAssertTrue(vm.shuffleEnabled)
    }
}
