import SwiftUI

extension View {
    @ViewBuilder
    func optionalGesture<T: Gesture>(_ gesture: T?) -> some View {
        if let gesture = gesture {
            self.gesture(gesture)
        } else {
            self
        }
    }
}
