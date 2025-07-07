import SwiftUI

struct SettingsView: View {
    @AppStorage("darkMode") var darkMode: Bool = false

    var body: some View {
        Form {
            Toggle("Dark Mode", isOn: $darkMode)
        }
    }
}
