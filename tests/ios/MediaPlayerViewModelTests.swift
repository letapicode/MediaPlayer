import XCTest
@testable import MediaPlayerApp

class DummyBridge: MediaPlayerBridge {
    var playCalled = false
    var shuffleEnabledFlag = false
    var lastSearchQuery: String?
    override func play() { playCalled = true }
    override func enableShuffle(_ enabled: Bool) { shuffleEnabledFlag = enabled }
    override func search(_ query: String) -> [NSDictionary] {
        lastSearchQuery = query
        return [["path": "p", "title": "t", "artist": "a"]]
    }
}

final class MediaPlayerViewModelTests: XCTestCase {
    func testPlaySetsState() {
        let vm = MediaPlayerViewModel(bridge: DummyBridge())
        vm.play()
        XCTAssertTrue(vm.isPlaying)
    }

    func testToggleShuffleCallsBridge() {
        let bridge = DummyBridge()
        let vm = MediaPlayerViewModel(bridge: bridge)
        vm.toggleShuffle()
        XCTAssertTrue(bridge.shuffleEnabledFlag)
    }

    func testSearchUpdatesLibrary() {
        let bridge = DummyBridge()
        let vm = MediaPlayerViewModel(bridge: bridge)
        vm.search("hello")
        XCTAssertEqual(bridge.lastSearchQuery, "hello")
        XCTAssertEqual(vm.library.first?.title, "t")
    }

    func testHandleVoiceCommandPlay() {
        let bridge = DummyBridge()
        let vm = MediaPlayerViewModel(bridge: bridge)
        vm.handleVoiceCommand("play the music")
        XCTAssertTrue(vm.isPlaying)
    }
}
