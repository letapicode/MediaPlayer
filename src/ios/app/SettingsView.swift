import SwiftUI

struct SettingsView: View {
    @AppStorage("darkMode") var darkMode: Bool = false
    @AppStorage("enableShake") var enableShake: Bool = true
    @AppStorage("enableSwipe") var enableSwipe: Bool = true
    @AppStorage("enableVolumeSwipe") var enableVolumeSwipe: Bool = true
    @AppStorage("enableAITagging") var enableAITagging: Bool = false

    var body: some View {
        Form {
            Toggle("Dark Mode", isOn: $darkMode)
            Toggle("Shake to Shuffle", isOn: $enableShake)
            Toggle("Swipe Gestures", isOn: $enableSwipe)
            Toggle("Volume Swipes", isOn: $enableVolumeSwipe)
            Toggle("AI Tagging", isOn: $enableAITagging)
        }
    }
}
