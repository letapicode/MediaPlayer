import SwiftUI

struct OpenURLView: View {
    @EnvironmentObject var player: MediaPlayerViewModel
    @State private var url: String = ""
    var body: some View {
        VStack(alignment: .leading) {
            TextField("Stream URL", text: $url)
                .textFieldStyle(RoundedBorderTextFieldStyle())
                .autocapitalization(.none)
                .disableAutocorrection(true)
            Button("Open") {
                guard !url.isEmpty else { return }
                _ = player.open(url)
                player.play()
            }
            Spacer()
        }
        .padding()
    }
}

struct OpenURLView_Previews: PreviewProvider {
    static var previews: some View {
        OpenURLView().environmentObject(MediaPlayerViewModel())
    }
}
