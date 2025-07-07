import SwiftUI

struct SettingsView: View {
    @AppStorage("darkMode") var darkMode: Bool = false
    @AppStorage("enableShake") var enableShake: Bool = true
    @AppStorage("enableSwipe") var enableSwipe: Bool = true

    var body: some View {
        Form {
            Toggle("Dark Mode", isOn: $darkMode)
            Toggle("Shake to Shuffle", isOn: $enableShake)
            Toggle("Swipe Gestures", isOn: $enableSwipe)
        }
    }
}
