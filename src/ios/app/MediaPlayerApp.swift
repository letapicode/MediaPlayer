import SwiftUI

@main
struct MediaPlayerApp: App {
    @StateObject private var player = MediaPlayerViewModel()

    var body: some Scene {
        WindowGroup {
            ContentView()
                .environmentObject(player)
                .onAppear { player.configureCallbacks() }
        }
    }
}
