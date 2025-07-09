import SwiftUI

struct DevicesView: View {
    var devices: [Device] = []
    var body: some View {
        List(devices) { dev in
            HStack {
                Text(dev.name)
                Spacer()
                Button("Send") {
                    // TODO call native SyncController
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
