import SwiftUI

struct SettingsView: View {
    @AppStorage("darkMode") var darkMode: Bool = false
    @AppStorage("shakeEnabled") var shakeEnabled: Bool = true

    var body: some View {
        Form {
            Toggle("Dark Mode", isOn: $darkMode)
            Toggle("Shake to Shuffle", isOn: $shakeEnabled)
        }
    }
}
