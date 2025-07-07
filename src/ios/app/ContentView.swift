import SwiftUI

struct ContentView: View {
    @EnvironmentObject var player: MediaPlayerViewModel

    var body: some View {
        VStack {
            Text("Position: \(player.position, specifier: "%.1f")")
            HStack {
                Button("Play") { player.play() }
                Button("Pause") { player.pause() }
            }
        }
        .padding()
    }
}

struct ContentView_Previews: PreviewProvider {
    static var previews: some View {
        ContentView().environmentObject(MediaPlayerViewModel())
    }
}
