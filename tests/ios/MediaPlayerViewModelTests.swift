import XCTest
@testable import MediaPlayerApp

class DummyBridge: MediaPlayerBridge {
    var playCalled = false
    var shuffleEnabledFlag = false
    override func play() { playCalled = true }
    override func enableShuffle(_ enabled: Bool) { shuffleEnabledFlag = enabled }
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
}
