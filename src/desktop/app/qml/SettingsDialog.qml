import QtQuick 2.15
import QtQuick.Controls 2.15

Dialog {
    id: settings
    title: qsTr("Settings")
    standardButtons: Dialog.Close
    Column {
        spacing: 8
        CheckBox { text: qsTr("Dark theme"); onToggled: Qt.application.theme = checked ? "dark" : "light" }
        Row {
            spacing: 4
            Label { text: qsTr("Audio device") }
            ComboBox { id: deviceBox; model: audioDevicesModel; textRole: "name" }
            Button { text: qsTr("Refresh"); onClicked: audioDevicesModel.refresh() }
        }
        Row {
            spacing: 4
            Label { text: qsTr("Language") }
            ComboBox {
                id: lang
                model: [ { text: "English", value: "en" }, { text: "Español", value: "es" } ]
                textRole: "text"
                onActivated: translation.switchLanguage(model[currentIndex].value)
            }
        }
        Button {
            text: qsTr("Discover Devices")
            onClicked: discoveredDevices = sync.discoverDevices()
        }
        ListView {
            id: deviceList
            height: 100
            width: parent.width
            model: discoveredDevices
            delegate: Row {
                spacing: 4
                Text { text: name }
                Button {
                    text: qsTr("Send")
                    onClicked: sync.sendSync(address, port, win.currentFile, player.position())
                }
            }
        }
    }

    property var discoveredDevices: []
}
