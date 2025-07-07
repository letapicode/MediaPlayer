import XCTest
@testable import MediaPlayerApp

class DummyBridge: MediaPlayerBridge {
    var playCalled = false
    override func play() { playCalled = true }
}

final class MediaPlayerViewModelTests: XCTestCase {
    func testPlaySetsState() {
        let vm = MediaPlayerViewModel(bridge: DummyBridge())
        vm.play()
        XCTAssertTrue(vm.isPlaying)
    }
}
