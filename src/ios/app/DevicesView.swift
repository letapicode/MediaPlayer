import SwiftUI

struct DevicesView: View {
    @EnvironmentObject var player: MediaPlayerViewModel
    var devices: [Device] = []
    var body: some View {
        List(devices) { dev in
            HStack {
                Text(dev.name)
                Spacer()
                Button("Send") {
                    player.sendToDevice(address: dev.address, port: dev.port)
                }
            }
        }
    }
}

struct Device: Identifiable {
    let id = UUID()
    var name: String
    var address: String
    var port: UInt16
}
