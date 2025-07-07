import SwiftUI

struct ContentView: View {
    var body: some View {
        TabView {
            LibraryView()
                .tabItem { Label("Library", systemImage: "music.note.list") }
            NowPlayingView()
                .tabItem { Label("Now Playing", systemImage: "play.circle") }
            OpenURLView()
                .tabItem { Label("Stream", systemImage: "link") }
            SettingsView()
                .tabItem { Label("Settings", systemImage: "gear") }
        }
    }
}

struct ContentView_Previews: PreviewProvider {
    static var previews: some View {
        ContentView().environmentObject(MediaPlayerViewModel())
    }
}
