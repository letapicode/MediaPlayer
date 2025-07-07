import XCTest

final class UITests: XCTestCase {
    func testPlayFirstLibraryItem() {
        let app = XCUIApplication()
        app.launch()

        let firstCell = app.tables.cells.element(boundBy: 0)
        XCTAssertTrue(firstCell.waitForExistence(timeout: 5))
        let title = firstCell.staticTexts.element(boundBy: 0).label
        firstCell.tap()

        let nowPlayingTitle = app.staticTexts[title]
        XCTAssertTrue(nowPlayingTitle.waitForExistence(timeout: 5))
    }
}
